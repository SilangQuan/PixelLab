#version 330 core

out vec4 FragColor;

in vec4 wPosition;
in vec3 wNormal;
in vec2 TexCoords;


// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicRoughnessMap;
uniform sampler2D aoMap;
uniform sampler2D emissiveMap;

uniform sampler2D opacityMap;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform float metallicFactor;
uniform float roughnessFactor;
uniform vec4 baseColorFactor;
uniform vec4 emissiveFactor;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 viewPos;

uniform vec3 dirLightDir;
uniform vec3 dirLightColor;


const float M_PI = 3.14159265359;

// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct PBRInfo
{
	float NdotL;                  // cos angle between normal and light direction
	float NdotV;                  // cos angle between normal and view direction
	float NdotH;                  // cos angle between normal and half vector
	float LdotH;                  // cos angle between light direction and half vector
	float VdotH;                  // cos angle between view direction and half vector
	float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
	float metalness;              // metallic value at the surface
	vec3 reflectance0;            // full reflectance color (normal incidence angle)
	vec3 reflectance90;           // reflectance color at grazing angle
	float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
	vec3 diffuseColor;            // color contribution from diffuse lighting
	vec3 specularColor;           // color contribution from specular lighting
};



// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(wPosition.xyz);
    vec3 Q2  = dFdy(wPosition.xyz);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(wNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}



// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------


vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
    float val = 1.0 - cosTheta;
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * (val*val*val*val*val); //Faster than pow
}

// Calculation of the lighting contribution from an optional Image Based Light source.
// Precomputed Environment Maps are required uniform inputs and are computed as outlined in [1].
// See our README.md on Environment Maps [3] for additional discussion.
vec3 getIBLContribution(PBRInfo pbrInputs, vec3 n, vec3 reflection)
{
	int maxMip = 8;

	float lod = (pbrInputs.perceptualRoughness * maxMip);
	// retrieve a scale and bias to F0. See [1], Figure 3

	vec3  kS = fresnelSchlickRoughness(max(pbrInputs.NdotV, 0.0), pbrInputs.specularColor, pbrInputs.perceptualRoughness);
        
	vec3 brdf = (texture(brdfLUT, vec2(pbrInputs.NdotV, pbrInputs.perceptualRoughness))).rgb;
	vec3 diffuseLight = texture(irradianceMap, n).rgb;

	vec3 specularLight = textureLod(prefilterMap, reflection, lod).rgb;

	vec3 diffuse = diffuseLight * pbrInputs.diffuseColor;
	vec3 specular = specularLight * (kS * brdf.x + brdf.y);

	// For presentation, this allows us to disable IBL terms
	// For presentation, this allows us to disable IBL terms
	//diffuse *= uboParams.scaleIBLAmbient;
	//specular *= uboParams.scaleIBLAmbient;

	//return vec3(pbrInputs.specularColor * brdf.x + brdf.y);
	//return vec3(pbrInputs.NdotV,0,0);
	
	//return vec3(brdf.x, brdf.y,0);
	///return vec3(pbrInputs.perceptualRoughness, 0,0);
	//return specular;
	return diffuse;
	//return texture(prefilterMap, wNormal).rgb + 0.00001 * brdf;
	//return texture(irradianceMap, wNormal).rgb + 0.00001 * brdf;
}

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog6
// See also [1], Equation 1
vec3 diffuse(PBRInfo pbrInputs)
{
	return pbrInputs.diffuseColor / M_PI;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 specularReflection(PBRInfo pbrInputs)
{  
	return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
float geometricOcclusion(PBRInfo pbrInputs)
{
	float NdotL = pbrInputs.NdotL;
	float NdotV = pbrInputs.NdotV;
	float r = pbrInputs.alphaRoughness;

	float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
	float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
	return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
float microfacetDistribution(PBRInfo pbrInputs)
{
	float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
	float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
	return roughnessSq / (M_PI * f * f);
}




void main()
{		
    vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2)) * baseColorFactor.rgb;
    vec3 emissive = pow(texture(emissiveMap, TexCoords).rgb, vec3(2.2)) * emissiveFactor.rgb;
    vec3 mrValue = texture(metallicRoughnessMap, TexCoords).rgb;
    //float metallic  = mrValue.r * metallicFactor;
    //float roughness = (1-mrValue.g) * roughnessFactor;

	float metallic  = mrValue.b * metallicFactor;
    float roughness = mrValue.g * roughnessFactor;
    float ao        = texture(aoMap, TexCoords).r;

	//metallic  = 1;
    //roughness = 0;
    //albedo.rgb = vec3(1,1,1);


    vec3 N = getNormalFromMap();
    vec3 V = normalize(viewPos - wPosition.xyz);
    vec3 R = reflect(-V, N);
    vec3 L = normalize(dirLightDir);
	vec3 H = normalize(L+V);   

    float NdotL = clamp(dot(N, L), 0.001, 1.0);
	float NdotV = clamp(abs(dot(N, V)), 0.001, 1.0);
	float NdotH = clamp(dot(N, H), 0.0, 1.0);
	float LdotH = clamp(dot(L, H), 0.0, 1.0);
	float VdotH = clamp(dot(V, H), 0.0, 1.0);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 

    float alphaRoughness = roughness * roughness;

    vec3 diffuseColor = albedo.rgb * (vec3(1.0) - F0);
	diffuseColor *= 1.0 - metallic;
    vec3 specularColor  = mix(F0, albedo, metallic);

    // Compute reflectance.
	float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

	// For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
	// For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflecance to 0%.
	float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
	vec3 specularEnvironmentR0 = specularColor.rgb;
	vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;
		
   
    PBRInfo pbrInputs = PBRInfo(
		NdotL,
		NdotV,
		NdotH,
		LdotH,
		VdotH,
		roughness,
		metallic,
		specularEnvironmentR0,
		specularEnvironmentR90,
		alphaRoughness,
		diffuseColor,
		specularColor
	);
    
    // Calculate the shading terms for the microfacet specular shading model
	vec3 F = specularReflection(pbrInputs);
	float G = geometricOcclusion(pbrInputs);
	float D = microfacetDistribution(pbrInputs);
	
	vec3 color= vec3(0,0,0);
	color += getIBLContribution(pbrInputs, N, R);

    vec3 prefilterSpec = textureLod(prefilterMap, R, 5).rgb;
    vec3 irradiance = texture(irradianceMap, R).rgb;

	color += emissive;



    //FragColor = vec4(ao,0,0, 1.0);
    //FragColor = vec4(prefilterSpec, 1.0);
	FragColor= vec4(color,  1.0);
    //FragColor = vec4(roughness,0,0,  1.0);
    //FragColor = vec4(irradiance, 1.0);
}
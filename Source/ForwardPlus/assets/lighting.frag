#version 430 core
#define TILE_SIZE 16
#define MAX_LIGHTS_PER_TILE 4


struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct PointLight {
    vec3 position;
    vec4 color;
	float intensity;
};



#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

// Function prototypes
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


uniform int workgroup_x;
uniform int workgroup_y;

struct Light {
	vec3 position;
	float radius;
	vec3 color;
	float intensity;
};
layout (std140, binding = 0) readonly buffer lights_data { 
    Light lights[];
};

layout(std430, binding = 1) readonly buffer visible_lights_indices {
    int lights_indices[];
};


void main()
{    
    ivec2 loc = ivec2(gl_FragCoord.xy);
    ivec2 tileID = loc / ivec2(16, 16);
    uint index = tileID.y * workgroup_x + tileID.x;
    uint offset = index * MAX_LIGHTS_PER_TILE;

    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0,0,0);

    for (uint i = 0; i < MAX_LIGHTS_PER_TILE; i++) 
    {
	    if (lights_indices[offset + i] != -1) 
        {
            int indices = lights_indices[offset + i];
	        //vec3 ts_light_pos = vs_in.TBN * lights[indices].position;
            vec3 light_pos = lights[indices].position;
            float radius = lights[indices].radius;
            vec3 lightDir = normalize(light_pos - FragPos);
            // Diffuse shadingcolor
            float diff = max(dot(norm, lightDir), 0.0);
            // Attenuation
            float distance = length(light_pos - FragPos);
            float attenuation = max(0,(radius - distance )/radius);    
            vec3 diffuse = attenuation * lights[indices].color.rgb * diff * vec3(texture(material.diffuse, TexCoords).rgb);
            result += diffuse;
        }
    }

    //// Phase 2: Point lights
   // for(int i = 0; i < NR_POINT_LIGHTS; i++)
   //     result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    
    //color = texture(material.diffuse, TexCoords);
    color = vec4(result,1);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shadingcolor
    float diff = max(dot(normal, lightDir), 0.0);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 3.0f /  (distance );    
    //float attenuation = 1f / distance ;    
    // Combine results
    vec3 diffuse = light.color.rgb * diff * vec3(texture(material.diffuse, TexCoords).rgb);

    diffuse *= attenuation;
    //return vec3(0.1,0,0);
    return diffuse;

   // vec3 diffuse = light.color
   //  return diffuse;
}

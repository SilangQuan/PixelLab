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
    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Phase 1: Directional lighting
    vec3 result = vec3(0,0,0);

    //// Phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    
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

#version 330 core

struct Material {
    sampler2D texture_diffuse0;
    //sampler2D diffuse;
    sampler2D texture_specular0;
    float shininess;
}; 


struct DirLight {
    vec3 direction;
    vec4 color;
	float intensity;
};

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;

uniform Material material;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
//uniform sampler2D texture_diffuse0;
//uniform sampler2D texture_diffuse1;

void main()
{
 	vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	//color = mix(texture(texture_diffuse0, TexCoord), texture(texture_diffuse1, TexCoord), 0.9);
	//color = texture(material.texture_diffuse0, TexCoords);
	color =  vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
  //  vec3 reflectDir = reflect(-lightDir, normal);
  //  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Combine results
   // vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse0, TexCoords));
    vec3 diffuse = 0.5 * light.color.rgb * (diff*0.5 + 0.5) * vec3(texture(material.texture_diffuse0, TexCoords));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
   // return (ambient + diffuse + specular);
    return (diffuse);
}
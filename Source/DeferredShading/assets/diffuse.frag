#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;

void main()
{    
    //vec3 norm = normalize(Normal);
   // color = vec4(norm, 1.0);
    color.rgb = Normal;
    color.a = 1.0;
}

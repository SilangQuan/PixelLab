#version 400

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sourceRT;

void main()
{             
    vec3 color = texture(sourceRT, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}
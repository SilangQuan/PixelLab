#version 330 core

uniform sampler2D ourTexture1;

in vec2 TexCoord;
in vec3 Normal;

out vec4 color;

void main()
{
   // color = vec4(1.0f, 0.5f, 0f, 1.0f);
    color = texture(ourTexture1, TexCoord);
}
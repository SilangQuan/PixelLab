#version 460 core

uniform vec3 lampColor;
out vec4 color;

void main()
{
    color = vec4(lampColor, 1.0f); // Set alle 4 vector values to 1.0f
    //color = vec4(1,0,0, 1.0f); // Set alle 4 vector values to 1.0f
}
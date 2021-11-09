#version 330 core

uniform vec4 lampColor;
out vec4 color;

void main()
{
    color = lampColor; // Set alle 4 vector values to 1.0f
}
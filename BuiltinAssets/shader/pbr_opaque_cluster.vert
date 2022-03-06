#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

out vec4 wPosition;
out vec3 wNormal;
out vec2 TexCoords;

void main()
{
	wPosition = model * vec4(position, 1.0);
    wNormal = normalize(mat3(model) * normal);
    TexCoords = texCoords;
    gl_Position = projection * view * wPosition;
}
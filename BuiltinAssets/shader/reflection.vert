#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;



out vec4 wPosition;
out vec3 wNormal;
out vec3 IncidentVector;

void main()
{
	//ourColor = normal;
	//TexCoords = texCoords;
    //gl_Position = projection * view * model * vec4(position, 1.0f);

	vec4 P = model * vec4(position, 1.0);
    vec3 N = normalize(mat3(model) * normal);
    //vec3 N = normalize(normal);
    vec3 I = P.xyz - viewPos;
    wPosition = P;
    wNormal = N;
    IncidentVector = I;

    gl_Position = projection * view * P;
}
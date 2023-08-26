#version 400
layout (location = 0) in vec3 VertexPosition;  
layout (location = 1) in vec2 VertexUv;  
layout (location = 2) in vec3 VertexNormal;  

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;


uniform mat4 MVP;

out vec2 texCoord;

void main()
{ 
	vec4 wPosition = model * vec4(VertexPosition, 1.0);
	texCoord = VertexUv;
    gl_Position = projection * view * wPosition;
}

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;
out vec3 ourColor;
out vec2 TexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	ourColor = color.rgb;
	TexCoords = texCoords;
    gl_Position = projection * view * model * vec4(position, 1.0f);
	 
   gl_Position = vec4(2*position.xy,0.0,1.0);
    //gl_Position = vec4(position, 1.0f);
}
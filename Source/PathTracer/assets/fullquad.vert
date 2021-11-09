#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
out vec3 ourColor;
out vec2 TexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	ourColor = normal;
	TexCoords = texCoords;
	 //vec2 vertices[3]=vec2[3](vec2(-1,-1), vec2(3,-1), vec2(-1, 3));
       // gl_Position = vec4(vertices[gl_VertexID],0,1);

		//gl_Position = vec4(position.xy,0.0,1.0);
    //gl_Position = projection * view * model * vec4(position, 1.0f);
    //gl_Position = vec4(position, 1.0f);


	//float x = float(((uint(gl_VertexID) + 2u) / 3u)%2u); 
    //float y = float(((uint(gl_VertexID) + 1u) / 3u)%2u); 

  //  gl_Position = vec4(-1.0f + x*2.0f, -1.0f+y*2.0f, 0.0f, 1.0f);
	gl_Position = vec4(position.xy * 2.0, position.z, 1.0);
}
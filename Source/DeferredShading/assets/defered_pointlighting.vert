#version 330
layout(location = 0) in vec3 vsPos;
out vec4 fsPos;
uniform float uLightRadius;
uniform vec3 uLightPosition;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    vec4 pos =  projection * view * model  * vec4(vsPos * uLightRadius + uLightPosition, 1.0);
    gl_Position = pos;
    fsPos = pos;
}
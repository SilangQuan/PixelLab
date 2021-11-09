#version 330
layout(location = 0) in vec3 vsPos;
layout(location = 1) in vec2 vsUv;
layout(location = 2) in vec3 vsNormal;
out vec3 fsPos;
out vec3 fsNormal;
out vec2 fsUv;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    fsPos = vsPos;
    fsNormal = vsNormal;
    fsUv = vsUv;
    gl_Position = projection * view * model  * vec4(vsPos, 1.0);
}
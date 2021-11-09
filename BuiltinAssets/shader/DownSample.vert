#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoord0;
out vec2 texCoord1;
out vec2 texCoord2;
out vec2 texCoord3;

uniform vec2 texelSize;

void main()
{
    texCoord0 = aTexCoords + vec2(texelSize.x, 0.0);
	texCoord1 = aTexCoords - vec2(texelSize.x, 0.0);
	texCoord2 = aTexCoords + vec2(0.0, texelSize.y);
	texCoord3 = aTexCoords - vec2(0.0, texelSize.y);

    gl_Position = vec4(aPos, 1.0);
}
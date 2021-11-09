#version 400
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 TexCoord0;

out vec2 texCoord0;
out vec2 texCoord1;
out vec2 texCoord2;
out vec2 texCoord3;
out vec2 texCoord4;

uniform vec2 texelSize;
uniform float isVertical;

void main()
{
	vec2 gaussianBlurTexcoordOffset = vec2(0.0);

	if (isVertical > 0)
	{
		gaussianBlurTexcoordOffset = vec2(0.0, texelSize.y);
	}else
	{
		gaussianBlurTexcoordOffset = vec2(texelSize.x, 0);
	}
	texCoord0 = TexCoord0;
	texCoord1 = TexCoord0 + gaussianBlurTexcoordOffset * 1.384615421;
	texCoord2 = TexCoord0 - gaussianBlurTexcoordOffset * 1.384615421;
	texCoord3 = TexCoord0 + gaussianBlurTexcoordOffset * 3.230769157;
	texCoord4 = TexCoord0 - gaussianBlurTexcoordOffset * 3.230769157;

    gl_Position = vec4(aPos, 1.0);
}
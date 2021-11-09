#version 400

uniform sampler2D sourceRT;

in vec2 texCoord0;
in vec2 texCoord1;
in vec2 texCoord2;
in vec2 texCoord3;
in vec2 texCoord4;

out vec4 fragColor;

void main()
{
	vec4 color = texture(sourceRT, texCoord0) * 0.227027029;
	color += texture(sourceRT, texCoord1) * 0.31621623;
	color += texture(sourceRT, texCoord2) * 0.31621623;
	color += texture(sourceRT, texCoord3) * 0.07027027;
	color += texture(sourceRT, texCoord4) * 0.07027027;

	fragColor = color;
}
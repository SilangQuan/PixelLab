#version 400

uniform sampler2D sourceRT;

in vec2 texCoord0;
in vec2 texCoord1;
in vec2 texCoord2;
in vec2 texCoord3;

out vec4 fragColor;

void main()
{
	vec4 downscaledColor = texture(sourceRT, texCoord0) * 0.25;
	downscaledColor += texture(sourceRT, texCoord1) * 0.25;
	downscaledColor += texture(sourceRT, texCoord2) * 0.25;
	downscaledColor += texture(sourceRT, texCoord3) * 0.25;
	fragColor = downscaledColor;
}
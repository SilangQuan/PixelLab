#version 400

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D bloomBuffer0;
uniform sampler2D bloomBuffer1;
uniform sampler2D bloomBuffer2;
uniform sampler2D bloomBuffer3;
uniform sampler2D bloomBuffer4;

void main()
{
    vec4 finalBloom = vec4(0.0);

	finalBloom = texture(bloomBuffer0, TexCoords) * 0.150000006;
	finalBloom += texture(bloomBuffer1, TexCoords) * 0.150000006;
	finalBloom += texture(bloomBuffer2, TexCoords) * 0.200000003;
	finalBloom += texture(bloomBuffer3, TexCoords) * 0.200000003;
	finalBloom += texture(bloomBuffer4, TexCoords) * 0.300000012;

	FragColor = finalBloom;
}
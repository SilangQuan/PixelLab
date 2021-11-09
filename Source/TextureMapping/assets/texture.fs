#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

// Texture samplers
uniform sampler2D ourTexture1;

void main()
{
	color = texture(ourTexture1, TexCoord);
	//color = texture(ourTexture1, TexCoord) * vec4(ourColor, 1.0f);
	//color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, 1-TexCoord), 0.2);
}
#pragma once
#include "Texture.h"

class ShaderProgram;

class TextureCubemap : public Texture
{
public:
	TextureCubemap();
	~TextureCubemap();

	static TextureCubemap* GeneratePrefilterMap(int size, ShaderProgram* prefilterShader, TextureCubemap* originCube);
	static TextureCubemap* GenerateConvolutionMap(int size, ShaderProgram* convolveShader, TextureCubemap* originCube);

private:
	//Useful in the Specular IBL component
	//unsigned int maxMipLevels;
	//unsigned int frameBufferID;
	//unsigned int depthBuffer;
};
#pragma once
#include "Texture.h"
#include "Mesh.h"
#include <vector>

class ShaderProgram;

class TextureCubemap : public Texture
{
public:
	TextureCubemap();
	~TextureCubemap();
	TextureCubemap(const std::string& file);
	bool LoadSixFaceCube();

	bool GeneratePrefilterMap(int size, ShaderProgram* prefilterShader, TextureCubemap* originCube);
	bool GenerateConvolutionMap(int size, ShaderProgram* convolveShader, TextureCubemap* originCube);

private:
	//Useful in the Specular IBL component
	unsigned int maxMipLevels;
	unsigned int frameBufferID;
	unsigned int depthBuffer;
};
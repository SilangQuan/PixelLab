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

	int GetSize() { return mSize; };
	void SetSize(int size) { mSize = size; };
private:
	int mSize;
};
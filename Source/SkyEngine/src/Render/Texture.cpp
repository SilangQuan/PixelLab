#include "Render/Texture.h"

Texture::~Texture()
{
	glDeleteTextures(1, &textureID);
}

Texture::Texture()
{
}



Texture::Texture(const std::string &file) :Resource(file)
{
	filePath = file;
	isLoaded = false;
	surface = NULL;
}



// Sets magnification and minification texture filter.

void Texture::SetFiltering(ETextureSampleFilter _magnification, ETextureSampleFilter _minification)
{
	mMagnifiFilter = _magnification;
	mMinifiFilter = _minification;
}

int Texture::GetWidth()
{
	return mWidth;
}

int Texture::GetHeight()
{
	return mHeight;
}

int Texture::GetBPP()
{
	return mBpp;
}

std::string Texture::GetPath()
{
	return filePath;
}


bool Texture::HasLoaded()
{
	return isLoaded;
}

void Texture::SetWrapMode(ETextureWrapMode _wrapS, ETextureWrapMode _wrapT)
{
	mWrapH = _wrapS;
	mWrapV = _wrapT;
}

void Texture::SetMipMapActive(bool enable)
{
	mGenerateMipMap = enable;
}

GLuint Texture::GetTextureID()
{
	return textureID;
}

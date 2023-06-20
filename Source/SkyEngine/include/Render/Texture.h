#pragma once


#include "../../3rdParty/include/SDL/SDL_image.h"
#include "EngineBase.h"
#include "Core/Resource.h"

enum ETextureFiltering
{
	TEXTURE_FILTER_MAG_NEAREST = 0, // Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR, // Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST, // Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR, // Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP, // Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR, // Bilinear criterion for minification on two closest mipmaps, then averaged
};

/********************************
Class:		Texture
Purpose:	Wraps OpenGL texture object and performs their loading.
********************************/

class Texture : public Resource
{
public:
	~Texture();
	Texture();
	Texture(const std::string &file);
	Texture(const std::string &file, std::string type);

	bool Init(const std::string* path);
	bool Init(int width, int height, GLuint format);
/*	void Bind(GLenum TextureUnit = 0);*/

	void SetFiltering(int magnification, int minification);
	void SetWrap(int wrapS, int wrapT);
	void SetMipMapActive(bool enable);
	//void SetSamplerParameter(GLenum parameter, GLenum value);

	int GetWidth();
	int GetHeight();
	int GetBPP();
	std::string GetPath();
	void SetPath(std::string& path);
	void SetPath(const char * path);


	void DeleteTexture();
	bool HasLoaded();

	std::string type;

	virtual GLuint GetTextureID();
	//void Bind();

protected:
	SDL_Surface* surface;
	int mWidth, mHeight, bpp; // Texture width, height, and bytes per pixel

	std::string filePath;
	GLuint textureID;

	int mode;
	bool isLoaded;
	int wrapHorizontal;
	int wrapVertical;
	int magnifiFilter;
	int minifiFilter;
	bool generateMipMap;
};
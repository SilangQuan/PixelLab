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


struct TextureInfo
{
public:
	ColorType ColorType = ColorType::RGB;
	ETextureDataType DataType = ETextureDataType::TDY_UnsignedByte;
	ETextureWrapMode WrapH = ETextureWrapMode::WM_Repeat;
	ETextureWrapMode WrapV = ETextureWrapMode::WM_Repeat;
	ETextureSampleFilter MagnifiFilter = ETextureSampleFilter::SF_Linear;
	ETextureSampleFilter MinifiFilter = ETextureSampleFilter::SF_Linear_Mip_Linear;
	bool GenerateMipMap = true;
	int Width, Height, Bpp; // Texture width, height, and bytes per pixel
};

class Texture : public Resource
{
public:
	~Texture();
	Texture();
	Texture(const std::string &file);

	bool Init(const std::string* path);

	void SetFiltering(ETextureSampleFilter magnification, ETextureSampleFilter minification);
	void SetWrapMode(ETextureWrapMode wrapS, ETextureWrapMode wrapT);
	void SetMipMapActive(bool enable);
	//void SetSamplerParameter(GLenum parameter, GLenum value);

	int GetWidth();
	int GetHeight();

	void SetWidth(int width) { mWidth = width; };
	void SetHeight(int height) { mHeight = height; };

	int GetBPP();
	std::string GetPath();
	ColorType GetColorType() { return mColorType; };
	void SetColorType(ColorType type) { mColorType = type; };

	bool HasLoaded();

	virtual uint32 GetTextureID();
	virtual void SetTextureID(uint32 id) { textureID = id; };

	bool HasMips() { return mGenerateMipMap; };
protected:
	SDL_Surface* surface;

	std::string filePath;
	uint32 textureID;

	ColorType mColorType;
	bool isLoaded;


	ETextureWrapMode mWrapH = ETextureWrapMode::WM_Repeat;
	ETextureWrapMode mWrapV = ETextureWrapMode::WM_Repeat;
	ETextureSampleFilter mMagnifiFilter = ETextureSampleFilter::SF_Linear;
	ETextureSampleFilter mMinifiFilter = ETextureSampleFilter::SF_Linear_Mip_Linear;
	bool mGenerateMipMap = true;
	int mWidth, mHeight, mBpp; // Texture width, height, and bytes per pixel

	friend class RenderDeviceGL;
};
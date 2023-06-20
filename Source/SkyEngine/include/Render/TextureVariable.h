#pragma once
#include "../../3rdParty/include/SDL/SDL_image.h"
#include "EngineBase.h"

/********************************
Class:		Texture
Purpose:	Wraps OpenGL texture object and performs their loading.
********************************/
class Texture;

class TextureVariable
{
public:
	TextureVariable();
	~TextureVariable();
	TextureVariable(Texture* texture, GLenum textureUnit, std::string uniformRefname, ETextureVariableType type = TV_2D);

	void SetTextureUnit(GLenum textureUnit);
	GLenum GetTextureUnit() const;
	GLuint GetTextureID() const;
	std::string GetUniformName() const;
	Texture* GetTexture();
	void SetTexture(Texture* inTexture) { texture = inTexture; }
	const void bind();

private:
	GLenum textureUnit;
	Texture* texture;
	std::string uniformRefname;
	ETextureVariableType type;
};
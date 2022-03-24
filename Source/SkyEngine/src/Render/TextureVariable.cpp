#include "Render/Texture.h"
#include "Render/TextureVariable.h"

TextureVariable::TextureVariable()
{

}

TextureVariable::TextureVariable(Texture* _texture, GLenum _textureUnit, string _uniformRefname, ETextureVariableType inType)
{
	textureUnit = _textureUnit;
	texture = _texture;
	uniformRefname = _uniformRefname;
	type = inType;
	//qDebug() << "new TextureVariable" << (int)_texture->GetTextureID();
}

TextureVariable::~TextureVariable()
{

}


void  TextureVariable::SetTextureUnit(GLenum _textureUnit)
{
	textureUnit = _textureUnit;
}

GLenum TextureVariable::GetTextureUnit() const
{
	return textureUnit;
}

GLuint TextureVariable::GetTextureID() const
{
	return texture->GetTextureID();
}

std::string TextureVariable::GetUniformName() const
{
	return uniformRefname;
}

Texture* TextureVariable::GetTexture()
{
	return texture;
}

const void TextureVariable::bind()
{
	if (texture == NULL)
		return;
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	switch (type)
	{
	case TV_2D:
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		break;
	case TV_3D:
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		break;
	case TV_CUBE:
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->GetTextureID());
		break;
	}
}

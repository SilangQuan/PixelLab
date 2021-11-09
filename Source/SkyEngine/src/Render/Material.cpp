#include "Render/Material.h"
#include "Render/ShaderProgram.h"
#include "Render/Texture.h"
#include "Render/TextureVariable.h"
#include "Lighting/Light.h"

Material::Material(ShaderProgram* _shaderProgram):CullMode(ECullMode::CM_Back),FillMode(EFillMode::FM_Solid),ZWriteMode(EZWriteMode::WM_ON),ZTestMode(EZTestMode::TM_LEQUAL)
{
	shaderProgram = new ShaderProgram(*_shaderProgram);
}

void Material::Bind(RenderContext* renderContext)
{
	if (shaderProgram != 0)
	{
		shaderProgram->Bind(renderContext);
	}
}

void Material::Bind()
{
	if (shaderProgram != 0)
	{
		shaderProgram->Bind();
	}
}


ShaderProgram* Material::GetShaderProgram()
{
	return shaderProgram;
}

/*
Texture& Material::GetTexture(int index)
{
	return textures[index];
}
*/

void Material::SetShader(ShaderProgram* shader)
{
	shader = shader;
}

void Material::AddTextureVariable(TextureVariable* textureVariable)
{
	if (shaderProgram->HasUniform(textureVariable->GetUniformName()))
	{
		shaderProgram->SetUniform(textureVariable->GetUniformName(), *textureVariable);
	}
	else
	{
		qDebug() << "No such shaderRefName " << textureVariable->GetUniformName();
	}
}

void Material::AddTextureVariable(string shaderRefName, Texture* texture, ETextureVariableType tvType, int textureUnit)
{
	if (shaderProgram->HasUniform(shaderRefName))
	{
		TextureVariable* tmpTextureVariable = new TextureVariable(texture, textureUnit, shaderRefName, tvType);
		shaderProgram->SetUniform(shaderRefName, *tmpTextureVariable);
	}else
	{
		qDebug() << "No such shaderRefName " << shaderRefName;
	}
}

void Material::SetFloat(string shaderRefName, float value)
{
	if (shaderProgram->HasUniform(shaderRefName))
	{
		shaderProgram->SetUniform(shaderRefName, value);
	}
	else
	{
		qDebug() << "No such shaderRefName " << shaderRefName;
	}
}
void Material::SetVector3(string shaderRefName, Vector3 value)
{
	if (shaderProgram->HasUniform(shaderRefName))
	{
		shaderProgram->SetUniform(shaderRefName, value);
	}
	else
	{
		qDebug() << "No such shaderRefName " << shaderRefName;
	}
}

void Material::SetVector4(string shaderRefName, Vector4 value)
{
	if (shaderProgram->HasUniform(shaderRefName))
	{
		shaderProgram->SetUniform(shaderRefName, value);
	}
	else
	{
		qDebug() << "No such shaderRefName " << shaderRefName;
	}
}

void Material::SetColor(string shaderRefName, Color& c)
{
	if (shaderProgram->HasUniform(shaderRefName))
	{
		shaderProgram->SetUniform(shaderRefName, Vector4(c.r, c.g, c.b, c.a));
	}
	else
	{
		qDebug() << "No such shaderRefName " << shaderRefName;
	}
}


void Material::SetDirectionLight(Light& light)
{
	if (light.type != kLightDirectional)
	{
		qDebug() << "Cannot Set DirectionLight. Wrong light type. ";
	}

	if (shaderProgram->HasUniform("dirLight.direction"))
	{
		shaderProgram->SetUniform("dirLight.direction", light.direction);
	}
	else
	{
		qDebug() << "No such shaderRefName dirLight.direction";
	}

	if (shaderProgram->HasUniform("dirLight.color"))
	{
		shaderProgram->SetUniform("dirLight.color", Vector4(light.color.r, light.color.g, light.color.b, light.color.a));
	}
	else
	{
		qDebug() << "No such shaderRefName dirLight.color";
	}

}

void Material::SetPointLight(vector<Light*>& lights)
{
	int count = 0;

	bool isArray = false;
	if (lights.size() > 1)
	{
		isArray = true;
	}

	int maxLightNum = 10;
	int lightIndex = 0;
	for (vector<Light*>::const_iterator lightIter = lights.begin(); lightIter != lights.end(); lightIter++)
	{
		lightIndex++;
		if (lightIndex > maxLightNum)
		{
			return;
		}

		if ((*lightIter)->type != kLightPoint)
		{
			qDebug() << "Cannot Set DirectionLight. Wrong light type. ";
		}
		string lightName = "pointLights";

		if(isArray)
		{
			lightName += "[" + std::to_string(count++) + "]";
		}

		if (shaderProgram->HasUniform(lightName + ".position"))
		{
			shaderProgram->SetUniform(lightName + ".position", (*lightIter)->position);
		}
		else
		{
			qDebug() << "No such position "<< lightName;
		}

		if (shaderProgram->HasUniform(lightName + ".color"))
		{
			shaderProgram->SetUniform(lightName + ".color", Vector4((*lightIter)->color.r, (*lightIter)->color.g, (*lightIter)->color.b, (*lightIter)->color.a));
		}
		else
		{
			qDebug() << "No such color" << lightName;
		} 
	}
}

void Material::SetSpotLight(vector<Light*>& lights)
{
	bool isArray = false;
	if(lights.size() > 1)
	{
		isArray = true;
	}

	int count = 0;

	for (vector<Light*>::const_iterator lightIter = lights.begin(); lightIter != lights.end(); lightIter++)
	{
		if ((*lightIter)->type != kLightSpot)
		{
			qDebug() << "Cannot Set DirectionLight. Wrong light type. ";
		}
		string lightName = "spotLight";

		if(isArray)
			lightName += "[" + std::to_string(count++) +"]";


		if (shaderProgram->HasUniform(lightName + ".position"))
		{
			shaderProgram->SetUniform(lightName + ".position", (*lightIter)->position);
		}
		else
		{
			qDebug() << "No such position " << lightName;
		}

		if (shaderProgram->HasUniform(lightName + ".direction"))
		{
			shaderProgram->SetUniform(lightName + ".direction", (*lightIter)->position);
		}
		else
		{
			qDebug() << "No such direction " << lightName;
		}

		if (shaderProgram->HasUniform(lightName + ".color"))
		{
			shaderProgram->SetUniform(lightName + ".color", (*lightIter)->color);
		}
		else
		{
			qDebug() << "No such color" << lightName;
		}


		if (shaderProgram->HasUniform(lightName + ".cutOff"))
		{
			shaderProgram->SetUniform(lightName + ".cutOff", (*lightIter)->cutOff);
		}
		else
		{
			qDebug() << "No such cutOff" << lightName;
		}

		if (shaderProgram->HasUniform(lightName + ".outerCutOff"))
		{
			shaderProgram->SetUniform(lightName + ".outerCutOff", (*lightIter)->outerCutOff);
		}
		else
		{
			qDebug() << "No such outerCutOff" << lightName;
		}

		if (shaderProgram->HasUniform(lightName + ".constant"))
		{
			shaderProgram->SetUniform(lightName + ".constant", (*lightIter)->constant);
		}
		else
		{
			qDebug() << "No such constant" << lightName;
		}

		if (shaderProgram->HasUniform(lightName + ".linear"))
		{
			shaderProgram->SetUniform(lightName + ".linear", (*lightIter)->linear);
		}
		else
		{
			qDebug() << "No such linear" << lightName;
		}

		if (shaderProgram->HasUniform(lightName + ".quadratic"))
		{
			shaderProgram->SetUniform(lightName + ".quadratic", (*lightIter)->quadratic);
		}
		else
		{
			qDebug() << "No such quadratic" << lightName;
		}
	}
}

Material::~Material() {}


void Material::SetCullMode(ECullMode mode)
{
	CullMode = mode;
}

void Material::SetFillMode(EFillMode mode)
{
	FillMode = mode;
}
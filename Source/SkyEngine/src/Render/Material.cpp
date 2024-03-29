#include "Render/Material.h"
#include "Render/ShaderProgram.h"
#include "Render/Texture.h"
#include "Render/TextureCubemap.h"
#include "Render/TextureVariable.h"
#include "Render/RenderContext.h"
#include "Lighting/Light.h"
#include "Core/ResourceManager.h"

Material::Material(ShaderProgram* _shaderProgram):CullMode(ECullMode::CM_Back),FillMode(EFillMode::FM_Solid),ZWriteMode(EZWriteMode::WM_ON),ZTestMode(EZTestMode::TM_LEQUAL)
{
	mShaderProgram = new ShaderProgram(*_shaderProgram);
}

Material::Material(const MaterialDescription& description, std::string sceneName)
{
}


Material::Material(MaterialDescription* description, ShaderProgram* shaderProgram, string sceneName)
{
	mShaderProgram = shaderProgram;
	mMatDescription = description;


	//Load from library folder.
	string texPath = CLibraryPath + sceneName + "/Texture/";
	Texture* texture = NULL;
	TextureVariable* textureVariable = NULL;
	uint32 texunit = 0;

	Texture* whiteDummyTex = ResourceManager::GetInstance()->FindTexture("WhiteDummy");
	Texture* DefaultNormalTex = ResourceManager::GetInstance()->FindTexture("DefaultNormal");
	Texture* BlackDummyTex = ResourceManager::GetInstance()->FindTexture("BlackDummy");

	texture = description->albedoMap.length() == 0 ? whiteDummyTex : ResourceManager::GetInstance()->LoadTex(texPath + description->albedoMap);
	textureVariable = new TextureVariable(texture, texunit, "albedoMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description->metallicRoughnessMap.length() == 0 ? whiteDummyTex : ResourceManager::GetInstance()->LoadTex(texPath + description->metallicRoughnessMap);
	textureVariable = new TextureVariable(texture, texunit, "metallicRoughnessMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description->normalMap.length() == 0 ? DefaultNormalTex : ResourceManager::GetInstance()->LoadTex(texPath + description->normalMap);
	textureVariable = new TextureVariable(texture, texunit, "normalMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description->metallicRoughnessMap.length() == 0 ? whiteDummyTex : ResourceManager::GetInstance()->LoadTex(texPath + description->metallicRoughnessMap);
	textureVariable = new TextureVariable(texture, texunit, "metallicRoughnessMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description->ambientOcclusionMap.length() == 0 ? whiteDummyTex : ResourceManager::GetInstance()->LoadTex(texPath + description->ambientOcclusionMap);
	textureVariable = new TextureVariable(texture, texunit, "aoMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description->emissiveMap.length() == 0 ? BlackDummyTex : ResourceManager::GetInstance()->LoadTex(texPath + description->emissiveMap);
	textureVariable = new TextureVariable(texture, texunit, "emissiveMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description->opacityMap.length() == 0 ? whiteDummyTex : ResourceManager::GetInstance()->LoadTex(texPath + description->opacityMap);
	textureVariable = new TextureVariable(texture, texunit, "opacityMap");
	mTextures.push_back(textureVariable);

	textureVariable = new TextureVariable(NULL, ++texunit, "irradianceMap", ETextureVariableType::TV_CUBE);
	mTextures.push_back(textureVariable);

	textureVariable = new TextureVariable(NULL, ++texunit, "prefilterMap", ETextureVariableType::TV_CUBE);
	mTextures.push_back(textureVariable);

	textureVariable = new TextureVariable(NULL, ++texunit, "brdfLUT");
	mTextures.push_back(textureVariable);


	CullMode = (ECullMode)description->cullMode;
	ZTestMode = (EZTestMode)description->zTest;
	FillMode = (EFillMode)description->fillMode;
	ZWriteMode = (EZWriteMode)description->zWrite;
}

void Material::Bind(const RenderDevice* device, const RenderContext* renderContext)
{
	if (mShaderProgram != 0)
	{
		glUseProgram(mShaderProgram->GetProgramID());

		SetProperty(device, "viewPos", renderContext->viewPos);
		SetProperty(device, "view", renderContext->viewMatrix);
		SetProperty(device, "model", renderContext->modelMatrix);
		SetProperty(device, "projection", renderContext->projectionMatrix);
		SetProperty(device, "viewProjection", renderContext->viewProjectionMatrix);

		SetProperty(device, "baseColorFactor", mMatDescription->albedoColor);
		SetProperty(device, "emissiveFactor", mMatDescription->emissiveColor);
		SetProperty(device, "roughnessFactor", mMatDescription->roughness.x);
		SetProperty(device, "metallicFactor", mMatDescription->metallicFactor);

		if (mShaderProgram->HasUniform("alphaCutoff"))
		{
			SetProperty(device, "alphaCutoff", mMatDescription->alphaCutoff);
		}

		for (int i = 0; i < mTextures.size(); i++)
		{
			if (mShaderProgram->HasUniform(mTextures[i]->GetUniformName()))
			{
				mShaderProgram->BindTextureVariable(mTextures[i]);
			}

			if (mTextures[i]->GetUniformName().compare("irradianceMap") == 0)
			{
				if (mShaderProgram->HasUniform("irradianceMap"))
				{
					mTextures[i]->SetTexture(renderContext->DiffuseCubeMap);
					mShaderProgram->BindTextureVariable(mTextures[i]);
				}
			}

			if (mTextures[i]->GetUniformName().compare("prefilterMap") == 0)
			{
				if (mShaderProgram->HasUniform("prefilterMap"))
				{
					mTextures[i]->SetTexture(renderContext->SpecCubeMap);
					mShaderProgram->BindTextureVariable(mTextures[i]);
				}
			}

			if (mTextures[i]->GetUniformName().compare("brdfLUT") == 0)
			{
				if (mShaderProgram->HasUniform("brdfLUT"))
				{
					mTextures[i]->SetTexture(renderContext->BrdfLut);
					mShaderProgram->BindTextureVariable(mTextures[i]);
				}
			}
		}
	}
}

void Material::Bind(const RenderDevice* device)
{
	if (mShaderProgram != 0)
	{
		mShaderProgram->Bind(device);
	}
}


ShaderProgram* Material::GetShaderProgram()
{
	return mShaderProgram;
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
/*
template<typename T> inline void SetProperty(const RenderDevice* device, const string& uniformName, const T& uniformData)
{

}
*/

template<typename T> inline void Material::SetProperty(const RenderDevice* device, const string& uniformName, const T& uniformData)
{
	if (mShaderProgram->HasUniform(uniformName))
	{
		mShaderProgram->SetUniformAndBind(device, uniformName, uniformData);
	}
	else
	{
		//qDebug() << "No such shaderRefName " << uniformName;
	}
}

void Material::SetDirectionLight(Light& light)
{
	if (light.type != kLightDirectional)
	{
		qDebug() << "Cannot Set DirectionLight. Wrong light type. ";
	}

	if (mShaderProgram->HasUniform("dirLight.direction"))
	{
		mShaderProgram->SetUniform("dirLight.direction", light.direction);
	}
	else
	{
		qDebug() << "No such shaderRefName dirLight.direction";
	}

	if (mShaderProgram->HasUniform("dirLight.color"))
	{
		mShaderProgram->SetUniform("dirLight.color", Vector4(light.color.r, light.color.g, light.color.b, light.color.a));
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

		if (mShaderProgram->HasUniform(lightName + ".position"))
		{
			mShaderProgram->SetUniform(lightName + ".position", (*lightIter)->position);
		}
		else
		{
			qDebug() << "No such position "<< lightName;
		}

		if (mShaderProgram->HasUniform(lightName + ".color"))
		{
			mShaderProgram->SetUniform(lightName + ".color", Vector4((*lightIter)->color.r, (*lightIter)->color.g, (*lightIter)->color.b, (*lightIter)->color.a));
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


		if (mShaderProgram->HasUniform(lightName + ".position"))
		{
			mShaderProgram->SetUniform(lightName + ".position", (*lightIter)->position);
		}
		else
		{
			qDebug() << "No such position " << lightName;
		}

		if (mShaderProgram->HasUniform(lightName + ".direction"))
		{
			mShaderProgram->SetUniform(lightName + ".direction", (*lightIter)->position);
		}
		else
		{
			qDebug() << "No such direction " << lightName;
		}

		if (mShaderProgram->HasUniform(lightName + ".color"))
		{
			mShaderProgram->SetUniform(lightName + ".color", (*lightIter)->color);
		}
		else
		{
			qDebug() << "No such color" << lightName;
		}


		if (mShaderProgram->HasUniform(lightName + ".cutOff"))
		{
			mShaderProgram->SetUniform(lightName + ".cutOff", (*lightIter)->cutOff);
		}
		else
		{
			qDebug() << "No such cutOff" << lightName;
		}

		if (mShaderProgram->HasUniform(lightName + ".outerCutOff"))
		{
			mShaderProgram->SetUniform(lightName + ".outerCutOff", (*lightIter)->outerCutOff);
		}
		else
		{
			qDebug() << "No such outerCutOff" << lightName;
		}

		if (mShaderProgram->HasUniform(lightName + ".constant"))
		{
			mShaderProgram->SetUniform(lightName + ".constant", (*lightIter)->constant);
		}
		else
		{
			qDebug() << "No such constant" << lightName;
		}

		if (mShaderProgram->HasUniform(lightName + ".linear"))
		{
			mShaderProgram->SetUniform(lightName + ".linear", (*lightIter)->linear);
		}
		else
		{
			qDebug() << "No such linear" << lightName;
		}

		if (mShaderProgram->HasUniform(lightName + ".quadratic"))
		{
			mShaderProgram->SetUniform(lightName + ".quadratic", (*lightIter)->quadratic);
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
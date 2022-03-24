#include "Render/Material.h"
#include "Render/ShaderProgram.h"
#include "Render/Texture.h"
#include "Render/TextureVariable.h"
#include "Lighting/Light.h"

const static string CShaderPath = "../../BuiltinAssets/shader/";
const static string CLibraryPath = "../../Library/";


Material::Material(ShaderProgram* _shaderProgram):CullMode(ECullMode::CM_Back),FillMode(EFillMode::FM_Solid),ZWriteMode(EZWriteMode::WM_ON),ZTestMode(EZTestMode::TM_LEQUAL)
{
	mShaderProgram = new ShaderProgram(*_shaderProgram);
}

Material::Material(const MaterialDescription& description, string sceneName)
{
	name = description.name;

	mShaderProgram = new ShaderProgram(CShaderPath + description.shader +".vert", CShaderPath + description.shader + ".frag");
	SetVector4("baseColorFactor", description.albedoColor);
	SetVector4("emissiveFactor", description.emissiveColor);
	SetFloat("roughnessFactor", description.roughness.x);
	SetFloat("metallicFactor", description.metallicFactor);

	if (mShaderProgram->HasUniform("alphaCutoff"))
	{
		SetFloat("alphaCutoff", description.alphaCutoff);
	}

	//Load from library folder.
	string texPath = CLibraryPath  + sceneName + "/Texture/";
	Texture* texture = NULL;
	TextureVariable* textureVariable = NULL;
	uint32 texunit = 0;

	Texture* whiteDummyTex = ResourceManager::GetInstance()->FindTexture("WhiteDummy");
	Texture* DefaultNormalTex = ResourceManager::GetInstance()->FindTexture("DefaultNormal");
	Texture* BlackDummyTex = ResourceManager::GetInstance()->FindTexture("BlackDummy");

	texture = description.albedoMap.length() == 0 ? whiteDummyTex : ResourceManager::GetInstance()->TryGetResource<Texture>(texPath + description.albedoMap);
	textureVariable = new TextureVariable(texture, texunit, "albedoMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description.metallicRoughnessMap.length() == 0 ? whiteDummyTex : ResourceManager::GetInstance()->TryGetResource<Texture>(texPath + description.metallicRoughnessMap);
	textureVariable = new TextureVariable(texture, texunit, "metallicRoughnessMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description.normalMap.length()== 0 ? DefaultNormalTex: ResourceManager::GetInstance()->TryGetResource<Texture>(texPath + description.normalMap);
	textureVariable = new TextureVariable(texture, texunit, "normalMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description.metallicRoughnessMap.length() == 0 ? whiteDummyTex : ResourceManager::GetInstance()->TryGetResource<Texture>(texPath + description.metallicRoughnessMap);
	textureVariable = new TextureVariable(texture, texunit, "metallicRoughnessMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description.ambientOcclusionMap.length() == 0 ? whiteDummyTex : ResourceManager::GetInstance()->TryGetResource<Texture>(texPath + description.ambientOcclusionMap);
	textureVariable = new TextureVariable(texture, texunit, "aoMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description.emissiveMap.length() == 0 ? BlackDummyTex : ResourceManager::GetInstance()->TryGetResource<Texture>(texPath + description.emissiveMap);
	textureVariable = new TextureVariable(texture, texunit, "emissiveMap");
	mTextures.push_back(textureVariable);
	texunit += 1;

	texture = description.opacityMap.length() == 0 ? whiteDummyTex : ResourceManager::GetInstance()->TryGetResource<Texture>(texPath + description.opacityMap);
	textureVariable = new TextureVariable(texture, texunit, "opacityMap");
	mTextures.push_back(textureVariable);

	textureVariable = new TextureVariable(NULL, ++texunit, "irradianceMap", ETextureVariableType::TV_CUBE);
	mTextures.push_back(textureVariable);

	textureVariable = new TextureVariable(NULL, ++texunit, "prefilterMap", ETextureVariableType::TV_CUBE);
	mTextures.push_back(textureVariable);

	textureVariable = new TextureVariable(NULL, ++texunit, "brdfLUT");
	mTextures.push_back(textureVariable);

	//AddTextureVariable("irradianceMap", NULL, ETextureVariableType::TV_CUBE, ++texunit);
	//AddTextureVariable("prefilterMap", NULL, ETextureVariableType::TV_CUBE, ++texunit);
	//AddTextureVariable("brdfLUT", NULL, ETextureVariableType::TV_2D, texunit);
	
	for (int i = 0; i < mTextures.size(); i++)
	{
		if (mShaderProgram->HasUniform(mTextures[i]->GetUniformName()))
		{
			mShaderProgram->SetUniformHandle(mTextures[i]->GetUniformName(), mTextures[i]);
		} 
	}

	CullMode = (ECullMode)description.cullMode;
	ZTestMode =	(EZTestMode)description.zTest;
	FillMode =	(EFillMode)description.fillMode;
	ZWriteMode = (EZWriteMode)description.zWrite;
}


void Material::Bind(RenderContext* renderContext)
{
	if (mShaderProgram != 0)
	{
		mShaderProgram->Bind(renderContext);
	}
}

void Material::Bind()
{
	if (mShaderProgram != 0)
	{
		mShaderProgram->Bind();
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


void Material::SetTexture(string shaderRefName, Texture* texture)
{
	for (int i = 0; i < mTextures.size(); i++)
	{
		if (mTextures[i]->GetUniformName().compare(shaderRefName) == 0)
		{
			mTextures[i]->SetTexture(texture);
			if (mShaderProgram->HasUniform(mTextures[i]->GetUniformName()))
			{
				mShaderProgram->SetUniformHandle(mTextures[i]->GetUniformName(), mTextures[i]);
			}
		}
	}
}

void Material::AddTextureVariable(TextureVariable* textureVariable)
{
	if (mShaderProgram->HasUniform(textureVariable->GetUniformName()))
	{
		mShaderProgram->SetUniform(textureVariable->GetUniformName(), *textureVariable);
	}
	else
	{
		qDebug() << "No such shaderRefName " << textureVariable->GetUniformName();
	}
}

void Material::AddTextureVariable(string shaderRefName, Texture* texture, ETextureVariableType tvType, int textureUnit)
{
	if (mShaderProgram->HasUniform(shaderRefName))
	{
		TextureVariable* tmpTextureVariable = new TextureVariable(texture, textureUnit, shaderRefName, tvType);
		mShaderProgram->SetUniform(shaderRefName, *tmpTextureVariable);
	}else
	{
		qDebug() << "No such shaderRefName " << shaderRefName;
	}
}

void Material::SetFloat(string shaderRefName, float value)
{
	if (mShaderProgram->HasUniform(shaderRefName))
	{
		mShaderProgram->SetUniform(shaderRefName, value);
	}
	else
	{
		qDebug() << "No such shaderRefName " << shaderRefName;
	}
}
void Material::SetVector3(string shaderRefName, Vector3 value)
{
	if (mShaderProgram->HasUniform(shaderRefName))
	{
		mShaderProgram->SetUniform(shaderRefName, value);
	}
	else
	{
		qDebug() << "No such shaderRefName " << shaderRefName;
	}
}

void Material::SetVector4(string shaderRefName, Vector4 value)
{
	if (mShaderProgram->HasUniform(shaderRefName))
	{
		mShaderProgram->SetUniform(shaderRefName, value);
	}
	else
	{
		qDebug() << "No such shaderRefName " << shaderRefName;
	}
}

void Material::SetColor(string shaderRefName, Color& c)
{
	if (mShaderProgram->HasUniform(shaderRefName))
	{
		mShaderProgram->SetUniform(shaderRefName, Vector4(c.r, c.g, c.b, c.a));
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
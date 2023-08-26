#pragma once

#include "EngineBase.h"

class Material;
class ShaderProgram;
class Texture;
class Resource;
class TextureCubemap;

// =================================================================================================
// Resource
// =================================================================================================

struct ResourceTypes
{
	enum List
	{
		Undefined = 0,
		SceneGraph,
		Geometry,
		Animation,
		Mat,
		Code,
		Shader,
		Texture,
		ParticleEffect,
		Pipeline
	};
};



// =================================================================================================
// Resource Manager
// =================================================================================================

// =================================================================================================


const static std::string CShaderPath = "../../../BuiltinAssets/shader/";
const static std::string CLibraryPath = "../../../Library/";
const static std::string CBuiltinAssetsPath = "../../../BuiltinAssets/";


class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	
	const static std::string& GetLibraryPath()
	{
		return CLibraryPath;
	}

	const static std::string& GetShaderPath()
	{
		return CShaderPath;
	}

	const static std::string& GetBuiltinAssetsPath()
	{
		return CBuiltinAssetsPath;
	}

	static ResourceManager* GetInstance()
	{
		if (instance == NULL)
			instance = new ResourceManager();
		return instance;
	}

	ShaderProgram* LoadShader(std::string filename);
	ShaderProgram* LoadShader(std::string vsfilename, std::string psfilename);

	void LoadBuitinShaders();
	void LoadBuiltinTexs();

	Texture* LoadTex(std::string filename);
	TextureCubemap* LoadTexCube(std::string filepath);

	//Resource *GetResource(const std::string &name);

	template <typename T>
	T *LoadResource(const std::string &filePath)
	{
		T* resource = new T(filePath);
		resources[filePath] = dynamic_cast<Resource*>(resource);
		return resource;
	}

	template <typename T>
	T *TryGetResource(const std::string &filePath)
	{
		auto iter = resources.find(filePath); 
		if (iter == resources.end())
		{
			return LoadResource<T>(filePath);
		}
		else
		{
			return dynamic_cast<T*>(iter->second);
		}
	}

	void RemoveResource(Resource* resource);
	void Clear();

	std::map <const std::string, Resource*> &GetResources() { return resources; }

	void AddMaterial(Material* mat);

	Material* FindMaterial(const std::string name)
	{
		auto iter = mMaterialResources.find(name);
		if (iter == mMaterialResources.end())
		{
			qDebug() << "Error:Can't load material with name " << name;
			return NULL;
		}
		else
		{
			return mMaterialResources[name];
		}
	}


	Texture* FindTexture(const std::string name)
	{
		auto iter = mTextureResources.find(name);
		if (iter == mTextureResources.end())
		{
			qDebug() << "Error:Can't find texture with name " << name;
			return NULL;
		}
		else
		{
			return mTextureResources[name];
		}
	}

	ShaderProgram* FindShader(const std::string name)
	{
		auto iter = mShaderResources.find(name);
		if (iter == mShaderResources.end())
		{
			qDebug() << "Error:Can't load shader with name " << name;
			return NULL;
		}
		else
		{
			return mShaderResources[name];
		}
	}


private:
	std::map <const std::string, Resource*> resources;
	static ResourceManager * instance;

	//Need to improve.
	std::map <const std::string, Material*> mMaterialResources;
	std::map <const std::string, ShaderProgram*> mShaderResources;
	std::map <const std::string, Texture*> mTextureResources;

};


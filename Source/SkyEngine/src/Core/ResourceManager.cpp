
#include "Core/ResourceManager.h"
#include "Core/Resource.h"
#include "../../SkyEngine/include/SkyEngine.h"

using namespace std;

// **********************************************************************************
// Class ResourceManager
// **********************************************************************************
ResourceManager* ResourceManager::instance = NULL;


ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
}

/*
Resource* ResourceManager::GetResource(const std::string &name)
{
	auto iter = resources.find(name);
	if (iter == resources.end())
	{
		return NULL;
	}else
	{
		return iter->second;
	}
}
*/
void ResourceManager::LoadBuitinShaders()
{
	//ShaderProgram* tmpShader = new ShaderProgram("../../BuiltinAssets/shader/pbr_opaque.vert", "../../BuiltinAssets/shader/pbr_opaque.frag");
	ShaderProgram* pbr_opaque_cluster = new ShaderProgram("../../BuiltinAssets/shader/pbr_opaque_cluster.vert", "../../BuiltinAssets/shader/pbr_opaque_cluster.frag");
	mShaderResources.insert(std::pair<string, ShaderProgram*>("pbr_opaque_cluster", pbr_opaque_cluster));

	ShaderProgram* pbr_opaque = new ShaderProgram("../../BuiltinAssets/shader/pbr_opaque.vert", "../../BuiltinAssets/shader/pbr_opaque.frag");
	mShaderResources.insert(std::pair<string, ShaderProgram*>("pbr_opaque", pbr_opaque));


	ShaderProgram* pbr_opaque_alphatest = new ShaderProgram("../../BuiltinAssets/shader/pbr_opaque_alphatest.vert", "../../BuiltinAssets/shader/pbr_opaque_alphatest.frag");
	mShaderResources.insert(std::pair<string, ShaderProgram*>("pbr_opaque_alphatest", pbr_opaque_alphatest));

	ShaderProgram* ConvolveShader = new ShaderProgram("../../BuiltinAssets/shader/cubeMapShader.vert", "../../BuiltinAssets/shader/convolveCubemapShader.frag");
	ShaderProgram* PrefilterShader = new ShaderProgram("../../BuiltinAssets/shader/cubeMapShader.vert", "../../BuiltinAssets/shader/preFilteringShader.frag");
	
	mShaderResources.insert(std::pair<string, ShaderProgram*>("ConvolveShader", ConvolveShader));
	mShaderResources.insert(std::pair<string, ShaderProgram*>("PrefilterShader", PrefilterShader));

	Texture* whiteDummy = new Texture("../../BuiltinAssets/texture/WhiteDummy.png");
	Texture* blackDummy = new Texture("../../BuiltinAssets/texture/BlackDummy.png");
	Texture* defaultNormal = new Texture("../../BuiltinAssets/texture/DefaultNormal.png");

	Texture* brdfLut = new Texture("../../BuiltinAssets/texture/BrdfLUT.hdr");

	mTextureResources.insert(std::pair<string, Texture*>("WhiteDummy", whiteDummy));
	mTextureResources.insert(std::pair<string, Texture*>("BlackDummy", blackDummy));
	mTextureResources.insert(std::pair<string, Texture*>("DefaultNormal", defaultNormal));

	mTextureResources.insert(std::pair<string, Texture*>("BrdfLut", brdfLut));
}

void ResourceManager::RemoveResource(Resource* resource)
{
	/*std::map <const std::string &, Resource*>::iterator iter = resources.find(name);
	if (iter != resources.end())
	{
		iter->second;
	}*/
}

void ResourceManager::Clear()
{

}
void ResourceManager::AddMaterial(Material* mat) { mMaterialResources[mat->name] = mat; }

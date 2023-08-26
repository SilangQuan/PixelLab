
#include "Core/ResourceManager.h"
#include "Core/Resource.h"
#include "../../SkyEngine/include/SkyEngine.h"
#include "stb/stb_image.h"

using namespace std;

// **********************************************************************************
// Class ResourceManager
// **********************************************************************************
ResourceManager* ResourceManager::instance = NULL;

bool EndWith(const std::string& str, const std::string& suffix)
{
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
}


ShaderProgram* ResourceManager::LoadShader(std::string filename)
{
	RenderDevice* renderDevice = GetRenderDevice();
	ShaderProgram* tmpShader = renderDevice->CreateGPUProgram(GetShaderPath() + filename + ".vert", GetShaderPath() + filename + ".frag");
	tmpShader->DetectViewInfoUniforms();
	//ShaderProgram* tmpShader = new ShaderProgram(GetShaderPath() + filename +".vert", GetShaderPath() + filename + ".frag");
	mShaderResources.insert(std::pair<string, ShaderProgram*>(filename, tmpShader));
	return tmpShader;
}

ShaderProgram* ResourceManager::LoadShader(std::string vsfilename, std::string psfilename)
{
	RenderDevice* renderDevice = GetRenderDevice();
	ShaderProgram* tmpShader = renderDevice->CreateGPUProgram(GetShaderPath() + vsfilename + ".vert", GetShaderPath() + psfilename + ".frag");
	tmpShader->DetectViewInfoUniforms();
	//ShaderProgram* tmpShader = new ShaderProgram(GetShaderPath() + filename +".vert", GetShaderPath() + filename + ".frag");
	mShaderResources.insert(std::pair<string, ShaderProgram*>(psfilename, tmpShader));
	return tmpShader;
}

void ResourceManager::LoadBuitinShaders()
{
	//ShaderProgram* tmpShader = new ShaderProgram("../../BuiltinAssets/shader/pbr_opaque.vert", "../../BuiltinAssets/shader/pbr_opaque.frag");
	ShaderProgram* pbr_opaque_cluster = new ShaderProgram(ResourceManager::GetShaderPath() + "pbr_opaque_cluster.vert", ResourceManager::GetShaderPath() + "pbr_opaque_cluster.frag");
	mShaderResources.insert(std::pair<string, ShaderProgram*>("pbr_opaque_cluster", pbr_opaque_cluster));

	ShaderProgram* pbr_opaque = new ShaderProgram(ResourceManager::GetShaderPath() + "pbr_opaque.vert", ResourceManager::GetShaderPath() + "pbr_opaque.frag");
	mShaderResources.insert(std::pair<string, ShaderProgram*>("pbr_opaque", pbr_opaque));


	ShaderProgram* pbr_opaque_alphatest = new ShaderProgram(ResourceManager::GetShaderPath() + "pbr_opaque_alphatest.vert", ResourceManager::GetShaderPath() + "pbr_opaque_alphatest.frag");
	mShaderResources.insert(std::pair<string, ShaderProgram*>("pbr_opaque_alphatest", pbr_opaque_alphatest));

	ShaderProgram* ConvolveShader = new ShaderProgram(ResourceManager::GetShaderPath() + "cubeMapShader.vert", ResourceManager::GetShaderPath() + "convolveCubemapShader.frag");
	ShaderProgram* PrefilterShader = new ShaderProgram(ResourceManager::GetShaderPath() + "cubeMapShader.vert", ResourceManager::GetShaderPath() + "preFilteringShader.frag");
	
	mShaderResources.insert(std::pair<string, ShaderProgram*>("ConvolveShader", ConvolveShader));
	mShaderResources.insert(std::pair<string, ShaderProgram*>("PrefilterShader", PrefilterShader));
}

void ResourceManager::LoadBuiltinTexs()
{
	Texture* whiteDummy = LoadTex(GetBuiltinAssetsPath() + "texture/WhiteDummy.png");
	Texture* blackDummy = LoadTex(GetBuiltinAssetsPath() + "texture/BlackDummy.png");
	Texture* defaultNormal = LoadTex(GetBuiltinAssetsPath() + "texture/DefaultNormal.png");

	Texture* brdfLut = LoadTex(GetBuiltinAssetsPath() + "texture/BrdfLUT.hdr");

	mTextureResources.insert(std::pair<string, Texture*>("WhiteDummy", whiteDummy));
	mTextureResources.insert(std::pair<string, Texture*>("BlackDummy", blackDummy));
	mTextureResources.insert(std::pair<string, Texture*>("DefaultNormal", defaultNormal));

	mTextureResources.insert(std::pair<string, Texture*>("BrdfLut", brdfLut));
}

TextureCubemap* ResourceManager::LoadTexCube(std::string filepath)
{
	RenderDevice* renderDevice = GetRenderDevice();
	TextureCubemap* cubemap = nullptr;
	TextureInfo tInfo;

	int nrChannels;
	vector<std::string> faces
	{
		"/px.hdr",
		"/nx.hdr",
		"/py.hdr",
		"/ny.hdr",
		"/pz.hdr",
		"/nz.hdr"
	};

	void* data[6];

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		std::string fullpath = filepath + faces[i];
		data[i] = stbi_loadf(fullpath.c_str(), &tInfo.Width, &tInfo.Height, &nrChannels, 0);
	}

	cubemap = renderDevice->CreateTextureCube(tInfo, data);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		stbi_image_free(data[i]);
	}

	return cubemap;
}


Texture* ResourceManager::LoadTex(std::string filepath)
{
	RenderDevice* renderDevice = GetRenderDevice();
	TextureInfo tInfo;
	int nrChannels;

	Texture* tex = nullptr;
	if (EndWith(filepath, "hdr"))
	{
		tInfo.ColorType = RGB32F;
		tInfo.DataType = ETextureDataType::TDY_Float;
		float* data = stbi_loadf(filepath.c_str(), &tInfo.Width, &tInfo.Height, &nrChannels, 0);
		if (data)
		{ 
			tex = renderDevice->CreateTexture2D(tInfo, data);
		}

		stbi_image_free(data);
	}
	else
	{
		SDL_Surface* surface = IMG_Load(filepath.c_str());

		if (surface == NULL) { //If it failed, say why and don't continue loading the texture
			qDebug() << "Error:" << SDL_GetError();
			return 0;
		}

		tInfo.Width = surface->w;
		tInfo.Height = surface->h;
		tInfo.Bpp = surface->pitch;

		// work out what format to tell glTexImage2D to use...
		if (surface->format->BytesPerPixel == 3) { // RGB 24bit
			tInfo.ColorType = RGB;
		}
		else if (surface->format->BytesPerPixel == 4) { // RGBA 32bit
			tInfo.ColorType = RGBA;
		}
		else {
			SDL_FreeSurface(surface);
			return false;
		}
		if (surface->pixels)
		{
			tex = renderDevice->CreateTexture2D(tInfo, surface->pixels);
		}
		if (surface) SDL_FreeSurface(surface);
	}
	return tex;
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

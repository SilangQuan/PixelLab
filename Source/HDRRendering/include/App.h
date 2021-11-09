#pragma once
#include "../../LibAppFramework/include/application.h"

class GLShader;
class PostProcessor;

class App : public Application
{
public:
	//bool Initialize();
	bool CreateWorld();
	void DestroyWorld();

	void FrameMove();
	void RenderWorld();

	void RenderUI();


	void BakeIBL();


private:
	TextureCubemap* skyCubeBarcelona;
	TextureCubemap* skyCubeTokyo;
	TextureCubemap* skyCubeCatwalk;



	ShaderProgram* mConvolveShader;
	ShaderProgram* mPrefilterShader;
	//Diffuse IBL
	TextureCubemap* mSpecCubeMap;
	//Specular IBL
	TextureCubemap* mDiffuseCubeMap;

	Texture* mBrdfLut;

	Scene* scene;
	GameObject* quad;
	GameObject* light;
	Material* mat;
	Material* metalMat;
	Material* pbrMat;
	Mesh* mesh;

	ShaderProgram* refractionShader;
	ShaderProgram* reflectionShader;
	ShaderProgram* pbrOpaqueShader;
	Camera* camera;
	PostProcessor* postProcessor;
	RenderTexture* mHdrRT;
	RenderTexture* mHdrRTMSAA;
	CubeMesh* skyBoxMesh;
	ShaderProgram* skyBoxShader;
	Material* skyBoxMat;

	Model* centerModel;
	GLuint skyboxID;
	
	int iblIndex;
	int materialIndex;
	float exposure;

	float bloomThreshold = 1;
	float bloomIntensity = 1;

	bool mEnableMsaa = false;

};


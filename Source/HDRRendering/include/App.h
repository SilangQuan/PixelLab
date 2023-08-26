#pragma once
#include "../../LibAppFramework/include/application.h"
#include "../../LibAppFramework/include/OrbitCameraController.h"
#include "../../LibAppFramework/include/FPSCameraController.h"

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

	OrbitCameraController* mOrbitCameraController;
	FPSCameraController* mFPSCameraController;

	TextureCubemap* skyCubeBarcelona;
	TextureCubemap* skyCubeTokyo;
	TextureCubemap* skyCubeCatwalk;



	ShaderProgram* mConvolveShader;
	ShaderProgram* mPrefilterShader;
	//Diffuse IBL
	TextureCubemap* mSpecCubeMap = nullptr;
	//Specular IBL
	TextureCubemap* mDiffuseCubeMap = nullptr;

	Texture* mBrdfLut = nullptr;

	Scene* mScene;
	GameObject* quad;
	GameObject* light;
	Material* mat;
	Material* metalMat;
	Material* pbrMat;
	Mesh* mesh;

	ShaderProgram* refractionShader;
	ShaderProgram* reflectionShader;
	ShaderProgram* pbrOpaqueShader;
	ShaderProgram* pbrAlphaTestShader;
	
	Camera* camera;
	Transform* mCamTrans;
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


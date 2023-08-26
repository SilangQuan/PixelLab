#pragma once
#include "../../LibAppFramework/include/application.h"
#include "../../LibAppFramework/include/OrbitCameraController.h"
#include "../../LibAppFramework/include/FPSCameraController.h"

class PostProcessor;

class App : public Application
{
public:
	void InitScene();
	bool CreateWorld();
	void DestroyWorld();

	void FrameMove();
	void RenderWorld();

	void RenderUI();


	void BakeIBL();


private:
	OrbitCameraController* mOrbitCameraController;
	FPSCameraController* mFPSCameraController;

	Scene* mScene;
	GameObject* quad;
	GameObject* light;
	Mesh* mesh;

	ShaderProgram* mDiffuseShader;
	Material* mDiffuseMat;

	Camera* camera;
	Transform* mCamTrans;
	
	int iblIndex;
	int materialIndex;
};


#pragma comment( lib, "..\\..\\Build\\lib\\Debug\\LibAppFramework.lib" )

#include "../include/App.h"
#include "../../SkyEngine/include/SkyEngine.h"
#include "../../3rdParty/include/GLM/glm.hpp"

bool App::CreateWorld()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	mDiffuseShader = ResourceManager::GetInstance()->LoadShader("diffuse");

	ResourceManager::GetInstance()->LoadBuitinShaders();
	ResourceManager::GetInstance()->LoadBuiltinTexs();

	mDiffuseMat = new Material(mDiffuseShader);
	mDiffuseMat->name = "Diffuse";
	mDiffuseMat->SetCullMode(CM_None);

	ResourceManager::GetInstance()->AddMaterial(mDiffuseMat);
	ForwardSceneRenderer* pForwardRenderer = dynamic_cast<ForwardSceneRenderer*>(pRenderer);

	//pForwardRenderer->SetRenderTarget(mHdrRT);
	mScene = new Scene();

	mScene->Init("../../../Library/HDR_Alucy/HDR_Alucy.json");
	
	quad = mScene->GetRoot()->GetChild(0);

	camera = mScene->GetActiveCamera();
	float sens = 20;
	float speed = 10;
	mOrbitCameraController = new OrbitCameraController(camera, pInput, sens, GetWindow()->GetHeight(), GetWindow()->GetWidth());
	mFPSCameraController = new FPSCameraController(camera, pInput, sens, speed, GetWindow()->GetHeight(), GetWindow()->GetWidth());

	mScene->Start();

	mesh = new SphereMesh(10, 10);
	InitScene();
	return true;
}

void App::InitScene()
{
	int count = 2000;
	for (int i = 0; i < count; i++)
	{
		GameObject* childObj = new GameObject();
		childObj->name = "_child_" + to_string(i);

		childObj->transform.rotation = Quaternion::identity;
		childObj->transform.position = 100 * Vector3(Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f), Random::Range(0.0f, 1.0f));
		childObj->transform.scale = Vector3::one;

		MeshRenderer* mr = new MeshRenderer(mesh, mDiffuseMat);
		childObj->AddComponent(mr);
		mScene->AddGameObject(childObj);
	}
}

void App::RenderWorld()
{
	PushGroupMarker("SceneRender");
	pRenderer->Render(mScene, mScene->GetActiveCamera());

	PopGroupMarker();
}


void App::FrameMove()
{
	mOrbitCameraController->Update();
}

void App::RenderUI()
{
	RenderDevice* renderDevice = GetRenderDevice();

	PushGroupMarker("IMGui");
	pImGuiRenderer->BeginFrame(renderDevice);
	int uiWidth = 250;
	int uiHeight = 300;

	// position the controls widget in the top-right corner with some margin
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	// here we set the calculated width and also make the height to be
	// be the height of the main window also with some margin
	ImGui::SetNextWindowSize(
		ImVec2(static_cast<float>(uiWidth), static_cast<float>(uiHeight - 20)),
		ImGuiCond_Always
	);

	ImGui::Begin("Options", NULL, ImGuiWindowFlags_NoResize);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	bool bTestCheckBox = false;

	
	ImGui::End();
	pImGuiRenderer->EndFrame(renderDevice);
	PopGroupMarker();
}

void App::DestroyWorld()
{
	SAFE_DELETE(mOrbitCameraController);
	SAFE_DELETE(mFPSCameraController);
}


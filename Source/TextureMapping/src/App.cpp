#ifdef _WIN64
#ifdef _DEBUG
#pragma comment( lib, "..\\..\\Lib\\x64\\Debug\\AppFramework.lib" )
#else
#pragma comment( lib, "..\\..\\Lib\\x64\\Release\\AppFramework.lib" )
#endif
#else
#ifdef _DEBUG
#pragma comment( lib, "..\\..\\Lib\\Win32\\Debug\\AppFramework.lib" )
#else
#pragma comment( lib, "..\\..\\Lib\\Win32\\Release\\AppFramework.lib" )
#endif
#endif


#include "../include/App.h"
#include "../../SkyEngine/include/SkyEngine.h"

bool App::CreateWorld()
{
	//glDisable(GL_CULL_FACE);
	scene = new Scene();
	quad = new GameObject();
	quad->transform.rotation = Quaternion::Euler(25, 5, 45);
	//quad->transform.position =  Vector3(1,1,0);
	scene->AddGameObject(quad);

	refractionShader = new ShaderProgram("./assets/basic4.vert", "./assets/basic4.frag");
	//shaderProgram->SetUniform("tex", texture1);
	texture1 = ResourceManager::GetInstance()->TryGetResource<Texture>("./assets/crate.jpg");
	mat = new Material(refractionShader);
	mat->AddTextureVariable("tex", texture1);

	//mesh = new Mesh();
	mesh = new CubeMesh();
	//mesh = new QuadMesh();
	
	mat->SetCullMode(ECullMode::CM_None);
	MeshRenderer* mr = new MeshRenderer(mesh, mat);
	quad->AddComponent(mr);

	camera = new Camera(Vector3(0, 0, -2.5f), Vector3::zero, Vector3::up);
	//camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 10, true);
	camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 100, true);
	scene->SetActiveCamera(camera);
	return true;

}

void App::RenderWorld()
{
	pRenderer->Render(scene, scene->GetActiveCamera());
}

void App::DestroyWorld()
{
}

void App::FrameMove()
{

}


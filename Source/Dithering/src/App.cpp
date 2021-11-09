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
	glDisable(GL_CULL_FACE);
	scene = new Scene();
	quad = new GameObject();
	quad->transform.rotation = Quaternion::Euler(15, 5, 45);
	scene->AddGameObject(quad);

	refractionShader = new ShaderProgram("./assets/fullscreen.vert", "./assets/basic3.frag");
	//shaderProgram->SetUniform("tex", texture1);
	texture1 = ResourceManager::GetInstance()->TryGetResource<Texture>("./assets/crate.jpg");
	mat = new Material(refractionShader);
	mat->AddTextureVariable("tex", texture1);

	mesh = new Mesh();

	vector<Vector3> vertices;
	vertices.push_back(Vector3(0.5f, 0.5f, -1.0f));
	vertices.push_back(Vector3(0.5f, -0.5f, -1.0f));
	vertices.push_back(Vector3(-0.5f, -0.5f, -1.0f));
	vertices.push_back(Vector3(-0.5f, 0.5f, -1.0f));

	vector<Vector3> normals;
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

	vector<Color> colors;
	colors.push_back(Color(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(Color(0.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(Color(0.0f, 0.0f, 1.0f, 1.0f));
	colors.push_back(Color(1.0f, 1.0f, 0.0f, 1.0f));

	vector<Vector2> uvs;
	uvs.push_back(Vector2(1.0f, 1.0f));
	uvs.push_back(Vector2(1.0f, 0.0f));
	uvs.push_back(Vector2(0.0f, 0.0f));
	uvs.push_back(Vector2(0.0f, 1.0f));

	vector<uint32> triangles;
	triangles.push_back(0);
	triangles.push_back(1);
	triangles.push_back(2);
	triangles.push_back(0);
	triangles.push_back(2);
	triangles.push_back(3);

	mesh->positions = vertices;
	mesh->colors = colors;
	mesh->uvs = uvs;
	mesh->positions = vertices;
	mesh->normals = normals;
	mesh->triangles = triangles;
	mesh->CreateBufferData();

	MeshRenderer* mr = new MeshRenderer(mesh, mat);
	quad->AddComponent(mr);

	camera = new Camera(Vector3(0, 0, -2.5f), Vector3::zero, Vector3::up);
	//camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 10, true);
	camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 10, true);
	scene->SetActiveCamera(camera);


	mRT = new RenderTexture();
	//mRT->Init(GetWindowWidth(), GetWindowHeight(), ColorType::RGBA16F, DepthType::Depth24S8, 0);
	mRT->Init(GetWindowWidth(), GetWindowHeight(), ColorType::RGB565, DepthType::Depth24S8, 0);

	ForwardSceneRenderer* pForwardRenderer = dynamic_cast<ForwardSceneRenderer*>(pRenderer);
	if (pForwardRenderer != NULL)
	{
		pForwardRenderer->SetRenderTarget(mRT);
	}

	mPostProcessor = new PostProcessor();
	mPostProcessor->InitRenderData();
	return true;

}

void App::RenderWorld()
{
	pRenderer->Render(scene, scene->GetActiveCamera());

	mPostProcessor->BlitToBackBuffer(mRT, 0, window->GetWidth(), window->GetHeight());
}

void App::DestroyWorld()
{
}

void App::FrameMove()
{

}


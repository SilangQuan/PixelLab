#pragma comment( lib, "..\\..\\Lib\\Win32\\Debug\\AppFramework.lib" )

#include "../include/App.h"
#include "../../SkyEngine/include/SkyEngine.h"

bool App::CreateWorld()
{
	glDisable(GL_CULL_FACE);
	scene = new Scene();
	quad = new GameObject();
	quad->transform.rotation = Quaternion::Euler(15, 5, 45);
	scene->AddGameObject(quad);

	refractionShader = new ShaderProgram("./assets/basic3.vert", "./assets/basic3.frag");
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

	mat->SetCullMode(ECullMode::CM_None);
	MeshRenderer* mr = new MeshRenderer(mesh, mat);
	quad->AddComponent(mr);

	camera = new Camera(Vector3(0, 0, -2.5f), Vector3::zero, Vector3::up);
	//camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 10, true);
	camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 10, true);
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


#pragma comment( lib, "..\\..\\Lib\\Win32\\Debug\\AppFramework.lib" )

#include "../include/App.h"
#include "../../SkyEngine/include/SkyEngine.h"
#include "../../3rdParty/include/GLM/glm.hpp"

void App::FrameMove()
{
	if (filterIndex == 0)
	{
		mat->AddTextureVariable("tex", texture1);
	}
	else if (filterIndex == 1)
	{
		RunComputeFilter(texture1, targetTex, computeInvertShader);
		mat->AddTextureVariable("tex", targetTex);
	}
	else if (filterIndex == 2)
	{
		RunComputeFilter(texture1, targetTex, computeBlurShader);
		mat->AddTextureVariable("tex", targetTex);
	}
}

void App::RenderUI()
{
	PushGroupMarker("IMGui");

	pImGuiRenderer->BeginFrame(window);
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

	bool bTestCheckBox = false;
	ImGui::Text("Filter:");
	ImGui::RadioButton("Origin", &filterIndex, 0); ImGui::SameLine();
	ImGui::RadioButton("Invert", &filterIndex, 1); ImGui::SameLine();
	ImGui::RadioButton("Blur", &filterIndex, 2);
	ImGui::End();
	pImGuiRenderer->EndFrame(window);
	PopGroupMarker();
}

void App::DestroyWorld()
{

}


bool App::CreateWorld()
{
	scene = new Scene();
	quad = new GameObject();
	//quad->transform.rotation = Quaternion::Euler(15, 5, 45);
	scene->AddGameObject(quad);

	unlitShader = new ShaderProgram("./assets/basic3.vert", "./assets/basic3.frag");

	computeInvertShader = new ShaderProgram("./assets/invert.compute");

	computeBlurShader = new ShaderProgram("./assets/blur.compute");

	//shaderProgram->SetUniform("tex", texture1);
	texture1 = ResourceManager::GetInstance()->TryGetResource<Texture>("./assets/spacecraft.png");

	targetTex = new Texture();
	targetTex->Init(256, 256, GL_RGBA8);

	mat = new Material(unlitShader);
	mat->AddTextureVariable("tex", texture1);
	mat->SetCullMode(CM_None);
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

	camera = new Camera(Vector3(0, 0, -1.1f), Vector3::zero, Vector3::up);
	camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 0.1f, 100, true);
	scene->SetActiveCamera(camera);
	return true;

}

void App::RenderWorld()
{
	glDisable(GL_CULL_FACE);
	pRenderer->Render(scene, scene->GetActiveCamera());
}

#define WORKGROUP_SIZE 16
void App::RunComputeFilter(Texture* srcTex, Texture* outputTex, ShaderProgram* filterProgram)
{
	filterProgram->Use();
	glBindImageTexture(0, srcTex->GetTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, outputTex->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glDispatchCompute(srcTex->GetWidth() / WORKGROUP_SIZE, srcTex->GetHeight() / WORKGROUP_SIZE, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
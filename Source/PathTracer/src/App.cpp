#pragma comment( lib, "..\\..\\Lib\\Win32\\Debug\\AppFramework.lib" )

#include "../include/App.h"
#include "../../SkyEngine/include/SkyEngine.h"
#include "../../3rdParty/include/GLM/glm.hpp"

void App::FrameMove()
{
	if (filterIndex == 0)
	{
		TraceScene(targetTex, computePathTracing);
		mat->AddTextureVariable("tex", targetTex);
	}
}

void App::RenderUI()
{
    return;
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

	unlitShader = new ShaderProgram("./assets/fullquad.vert", "./assets/basic3.frag");

	//computePathTracing = new ShaderProgram("./assets/pathtracer.compute");
    //computePathTracing = new ShaderProgram("./assets/pathtracer_lambert.compute");
    computePathTracing = new ShaderProgram("./assets/pathtracer_spherescene.compute");

	//shaderProgram->SetUniform("tex", texture1);
	texture1 = ResourceManager::GetInstance()->TryGetResource<Texture>("./assets/ShapeMask2.png");

	targetTex = new Texture();
	targetTex->Init(512, 288, GL_RGBA32F);

	mat = new Material(unlitShader);
	mat->AddTextureVariable("tex", texture1);
	mat->SetCullMode(CM_None);
	mesh = new QuadMesh();

	MeshRenderer* mr = new MeshRenderer(mesh, mat);
	quad->AddComponent(mr);

	//camera = new Camera(Vector3(0, 0, -2.1f), 60, 16.0f / 9.0f, 0.1f, 100);
	camera = new Camera(Vector3(0, 0, 100), 60, 16.0f/9.0f, 0.1f, 100);

	Vector4 cameraOrigin = camera->viewMatrix.invert()* Vector4(0.0, 0.0, 0.0, 1.0);
	//camera = new Camera(Vector3(0, 0, -1.1f), Vector3::zero, Vector3::up);
	//camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 0.1f, 100, true);
	scene->SetActiveCamera(camera);

    InitGPUScene();

	mNumFrames = 0;
	return true;
}

void App::RenderWorld()
{
	glDisable(GL_CULL_FACE);
	pRenderer->Render(scene, scene->GetActiveCamera());
}

#define WORKGROUP_SIZE 16

void  App::TraceScene(Texture* targetTex, ShaderProgram* tracerProgram)
{
	tracerProgram->Use();
	glBindImageTexture(0, targetTex->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//glBindImageTexture(0, targetTex->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

/*	computePathTracing->SetUniform("u_FOV", glm::radians(camera->fov));

	computePathTracing->SetUniform("u_NumFrames", 1);
	computePathTracing->SetUniform("u_Accum", float(mNumFrames) / float(mNumFrames + 1));
	//computePathTracing->SetUniform("u_AspectRatio", float(targetTex->GetWidth())/ float(targetTex->GetHeight()));
	computePathTracing->SetUniform("u_Resolution", Vector2(targetTex->GetWidth(), targetTex->GetHeight()));
	computePathTracing->SetUniform("u_InvViewMat", camera->viewMatrix.invert());
	computePathTracing->SetUniform("u_InvProjectionMat", camera->projectionMaxtrix.invert());
	*/
	glDispatchCompute(targetTex->GetWidth() / WORKGROUP_SIZE, targetTex->GetHeight() / WORKGROUP_SIZE, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	mNumFrames++;
}

void  App::InitGPUScene()
{
    mGPUscene.clear();

    SphereData data;

    data.Center = Vector3(0, -1000.0f, 0.0f);
    data.Radius = 1000.0f;
    data.MaterialType = (int)MaterialType::MAT_LAMBERTIAN;
    data.MaterialAlbedo = Vector3(0.5f, 0.5f, 0.5f);
    data.MaterialData = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    mGPUscene.push_back(data);

    // mGPUscene.push_back(data);
   // //mGPUscene.push_back(data); = Random::Range(0.0, 100.0f);
    

    data.Center = Vector3(0, 1.0f, 0.0f);
    data.Radius = 1.0f;
    data.MaterialType = (int)MaterialType::MAT_DIELECTRIC;
    data.MaterialAlbedo = Vector3(0.1f, 0.2f, 0.5f);
    data.MaterialData = Vector4(1.5f, 0.0f, 0.0f, 0.0f);
     mGPUscene.push_back(data);
    //mGPUscene.push_back(data); = Random::Range(0.0, 100.0f);
    

    data.Center = Vector3(-4.0f, 1.0f, 0.0f);
    data.Radius = 1.0f;
    data.MaterialType = (int)MaterialType::MAT_LAMBERTIAN;
    data.MaterialAlbedo = Vector3(0.4f, 0.2f, 0.1f);
    data.MaterialData = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
     mGPUscene.push_back(data);
    //mGPUscene.push_back(data); = Random::Range(0.0, 100.0f);
    

    data.Center = Vector3(4.0f, 1.0f, 0.0f);
    data.Radius = 1.0f;
    data.MaterialType = (int)MaterialType::MAT_METAL;
    data.MaterialAlbedo = Vector3(0.7f, 0.6f, 0.5f);
    data.MaterialData = Vector4(0.5f, 0.0f, 0.0f, 0.0f);
     mGPUscene.push_back(data);
    //mGPUscene.push_back(data); = Random::Range(0.0, 100.0f);
    

    for (int a = -4; a < 5; a++)
    {
        for (int b = -4; b < 4; b++)
        {
            float Choose_Mat = Random::Range(0.0f, 1.0f);
            Vector3 Center = Vector3(a * 1.5f + 1.5f * Random::Range(0.0, 1.0f), 0.2f, b * 1.0f + 1.0f * Random::Range(0.0, 1.0f));
            Vector3 Dist = Center - Vector3(4, 0.2f, 0);
            if (Dist.magnitude() > 0.9f)
            {
                if (Choose_Mat < 0.5f)
                {
                    // diffuse
                    Vector3 Albedo = Vector3(Random::Range(0.0, 1.0f), Random::Range(0.0, 1.0f), Random::Range(0.0, 1.0f));
                    data.Center = Center;
                    data.Radius = 0.2f;
                    data.MaterialType = (int)MaterialType::MAT_LAMBERTIAN;
                    data.MaterialAlbedo = Albedo;
                    data.MaterialData = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
                }
                else if (Choose_Mat < 0.8f)
                {
                    // metal
                    Vector3 Albedo = Vector3(Random::Range(0.0, 1.0f), Random::Range(0.0, 1.0f), Random::Range(0.0, 1.0f));
                    float Roughness = Random::Range(0.0, 1.0f);
                    data.Center = Center;
                    data.Radius = 0.2f;
                    data.MaterialType = (int)MaterialType::MAT_METAL;
                    data.MaterialAlbedo = Albedo;
                    data.MaterialData = Vector4(Roughness, 0.0f, 0.0f, 0.0f);
                }
                else
                {
                    data.Center = Center;
                    data.Radius = 0.2f;
                    data.MaterialType = (int)MaterialType::MAT_DIELECTRIC;
                    data.MaterialData = Vector4(0.0f, 1.5f, 0.0f, 0.0f);
                }
                 mGPUscene.push_back(data);
                //mGPUscene.push_back(data); = Random::Range(0.0, 100.0f);
            }
        }
    }

    glGenBuffers(1, &mGPUSceneSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mGPUSceneSSBO);
    //We generate the buffer but don't populate it yet.
    glBufferData(GL_SHADER_STORAGE_BUFFER, mGPUscene.size() * sizeof(struct SphereData), NULL, GL_DYNAMIC_DRAW);

    GLint bufMask = GL_READ_WRITE;
    //GLint bufMask = GL_READ_BUFFER;
    struct SphereData* spheres = (struct SphereData*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, bufMask);

    for (unsigned int i = 0; i < mGPUscene.size(); ++i) {
        //Fetching the data from the current scene
        spheres[i].Center = mGPUscene[i].Center;
        spheres[i].Radius = mGPUscene[i].Radius;
        spheres[i].MaterialType = mGPUscene[i].MaterialType;
        spheres[i].MaterialAlbedo = mGPUscene[i].MaterialAlbedo;
        spheres[i].MaterialData = mGPUscene[i].MaterialData;
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mGPUSceneSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
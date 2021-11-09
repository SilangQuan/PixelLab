
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
#include "../../SkyEngine/include/Render/Graphics.h"
#include "../../SkyEngine/include/Render/Model.h"
#include "../../SkyEngine/include/Render/Mesh.h"
#include "../../SkyEngine/include/Render/DeferredSceneRenderer.h"
#include <vector>


bool App::CreateWorld()
{
	scene = new Scene();
	sponza = new GameObject();
	sponza->transform.rotation = Quaternion::Euler(15, 5, 45);
	scene->AddGameObject(sponza);

	mGeometryPass = new ShaderProgram("./assets/deferred_geo.vert", "./assets/deferred_geo.frag");
	ShaderProgram* dirLightingPass = new ShaderProgram("./assets/defered_dirlighting.vert", "./assets/defered_dirlighting.frag");
	ShaderProgram* pointLightingPass = new ShaderProgram("./assets/defered_pointlighting.vert", "./assets/defered_pointlighting.frag");

	DeferredSceneRenderer *pDeferredRenderer = dynamic_cast<DeferredSceneRenderer*>(pRenderer);
	if (pDeferredRenderer != NULL)
	{
		pDeferredRenderer->SetShaderProgram(mGeometryPass, dirLightingPass, pointLightingPass);
	}

	//shaderProgram->SetUniform("tex", texture1);
	diffuseMap2 = ResourceManager::GetInstance()->TryGetResource<Texture>("./assets/Model/rock/rock.png");
	
	mat = new Material(mGeometryPass);
	mat->AddTextureVariable("uDiffTex", diffuseMap2);

	//bunny = new Model("./assets/Model/rock/rock.obj", true);
	//bunny = new Model("./assets/Model/sponza2/sponza.obj", true);
	//bunny = new Model("../../Assets/model/sponza/sponza.obj", true);
	bunny = new Model("./assets/Model/sponza2/sponza.obj", true);

	bunny->CreateBufferData();
	vector<Mesh>* meshes = bunny->GetMeshes();

	for (int i = 0; i < meshes->size(); i++)
	{
		GameObject *tmp = new GameObject();
		//tmp->transform.scale = Vector3(0.5, 0.5, 0.5);

		mat2 = new Material(mGeometryPass);
		mat2->AddTextureVariable("uDiffTex", diffuseMap2);
		for (int j = 0; j < (*meshes)[i].textures.size(); j++)
		{
			mat = new Material(mGeometryPass);
			//mat->AddTextureVariable((*meshes)[i].textures[j]->GetUniformName(), (*meshes)[i].textures[j]->GetTexture());
			mat->AddTextureVariable("uDiffTex", (*meshes)[i].textures[j]->GetTexture());
		}
		

		MeshRenderer* meshRenderer = new MeshRenderer(&(*meshes)[i], mat);
		tmp->AddComponent(meshRenderer);
		sponza->AddChild(tmp);
	}

	scene = new Scene();
	scene->AddGameObject(sponza);

	camera = new Camera(Vector3(100, 100, 0), Vector3(1, 0, 0), Vector3::up);
	camera->transform.Rotate(-20, 90,0);
	camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 2120, true);
	scene->SetActiveCamera(camera);

	CreatePointLights();
	return true;
}

void  App::CreatePointLights()
{
	Light *dirlight = new Light(kLightDirectional);
	dirlight->position = Vector3::zero;
	dirlight->color = Color(0.3, 0.3, 0.3,1);
	dirlight->direction = Vector3(-0.7, 0.3, 0.1);
	dirlight->AddToManager();

	int pointLightCount = 2200;
	const int PS = 14;
	Color palette[PS] = {
		Color(1.0f, 0.0f, 0.0f, 1.0),
		Color(0.0f, 1.0f, 0.0f, 1.0),
		Color(0.0f, 0.0f, 1.0f, 1.0),
		Color(1.0f, 1.0f, 0.0f, 1.0),
		Color(1.0f, 0.0f, 1.0f, 1.0),
		Color(0.0f, 1.0f, 1.0f, 1.0),
		Color(1.0f, 1.0f, 1.0f, 1.0),
		Color(0.9f, 0.5f, +.3f, 1.0),
		Color(0.5f, 0.5f, 0.5f, 1.0),
		Color(0.3f, 0.6f, 0.9f, 1.0),
		Color(0.4f, 0.8f, 0.4f, 1.0),
		Color(0.3f, 0.3f, 1.0f, 1.0),
		Color(1.0f, 0.5f, 0.8f, 1.0),
		Color(0.5f, 0.9f, 0.2f, 1.0),
	};

	for (int i = 0; i < pointLightCount; i++)
	{
		GameObject *tmp = new GameObject();
		Vector3 randomPos(Random::Range(-800.0f, 800.0f), Random::Range(0.0f, 800.0f), Random::Range(-800.0f, 800.0f));
		tmp->transform.position = randomPos;

		Light *light = new Light(kLightPoint);
		light->range = 20.0f + 130.0f * Random::Range(0.1f, 1.0f);
		light->color = palette[(int)Random::Range(0, 13)];
		
		light->AddToManager();

		tmp->AddComponent(light);
		mLightObjs.push_back(tmp);
	}
}

void  App::UpdateLights()
{
	for (int i = 0; i < mLightObjs.size(); i++)
	{
		Quaternion q = Quaternion::AxisAngleToQuaternionSafe(Vector3::up, 1 * Time::deltaTime);
		Vector3 dif = mLightObjs[i]->transform.position - Vector3::zero;
		dif = q * dif;
		mLightObjs[i]->transform.position = Vector3::zero + dif;
		//mLightObjs[i]->transform.RotateAround(Vector3::zero, Vector3::up, 1);
	}
}



void  App::UpdateCamera()
{
	float moveSpeed = 10000.0f * Time::deltaTime;


	if (pInput->GetMouseButton(MOUSE_LEFT))
	{
		float xoffset = pInput->GetMouseDeltaX();
		float yoffset = pInput->GetMouseDeltaY();

		float sensitivity = 100;

		xoffset *= Time::deltaTime * sensitivity;
		yoffset *= Time::deltaTime * sensitivity;

		camera->transform.Rotate(0, xoffset, 0);
		camera->transform.Rotate(yoffset, 0, 0);
	}
	

	if (pInput->GetKeyDown(KEY_W))
	{
		camera->transform.position += moveSpeed * camera->transform.GetForward();
	}

	if (pInput->GetKeyDown(KEY_S))
	{
		camera->transform.position += -moveSpeed * camera->transform.GetForward();
	}

	if (pInput->GetKeyDown(KEY_A))
	{
		camera->transform.position += -moveSpeed * camera->transform.GetRight();
	}

	if (pInput->GetKeyDown(KEY_D))
	{
		camera->transform.position += moveSpeed * camera->transform.GetRight();
	}


	if (pInput->GetKeyDown(KEY_0))
	{
		DeferredSceneRenderer *pDeferredSceneRenderer = dynamic_cast<DeferredSceneRenderer*>(pRenderer);
		if (pDeferredSceneRenderer != NULL)
		{
			pDeferredSceneRenderer->SwitchRenderType(RenderContentType::Result);
		}
	}

	if (pInput->GetKeyDown(KEY_1))
	{
		DeferredSceneRenderer *pDeferredSceneRenderer = dynamic_cast<DeferredSceneRenderer*>(pRenderer);
		if (pDeferredSceneRenderer != NULL)
		{
			pDeferredSceneRenderer->SwitchRenderType(RenderContentType::Albedo);
		}
	}
	if (pInput->GetKeyDown(KEY_2))
	{
		DeferredSceneRenderer *pDeferredSceneRenderer = dynamic_cast<DeferredSceneRenderer*>(pRenderer);
		if (pDeferredSceneRenderer != NULL)
		{
			pDeferredSceneRenderer->SwitchRenderType(RenderContentType::Normal);
		}
	}
	if (pInput->GetKeyDown(KEY_3))
	{
		DeferredSceneRenderer *pDeferredSceneRenderer = dynamic_cast<DeferredSceneRenderer*>(pRenderer);
		if (pDeferredSceneRenderer != NULL)
		{
			pDeferredSceneRenderer->SwitchRenderType(RenderContentType::PosWorld);
		}
	}
	if (pInput->GetKeyDown(KEY_4))
	{
		DeferredSceneRenderer *pDeferredSceneRenderer = dynamic_cast<DeferredSceneRenderer*>(pRenderer);
		if (pDeferredSceneRenderer != NULL)
		{
			pDeferredSceneRenderer->SwitchRenderType(RenderContentType::Depth);
		}
	}
}

void App::RenderWorld()
{
	UpdateLights();
	UpdateCamera();
	pRenderer->Render(scene, scene->GetActiveCamera());



	return;
}


void App::DestroyWorld()
{
}

void App::FrameMove()
{

}

App::~App()
{
	delete mGBuffer;
}

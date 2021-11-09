#pragma comment( lib, "..\\..\\Lib\\Win32\\Debug\\AppFramework.lib" )

#include "../include/App.h"
#include "../../SkyEngine/include/SkyEngine.h"


void App::InitLights()
{
	light = new Light(kLightDirectional);
	light->direction = Vector3(-0.2f, -1.0f, -0.3f);
	light->color = Color(1, 1, 1, 1);
	light->AddToManager();
	delete(light);

	//lightObjs = new GameObject();
	//MeshRenderer * lampMeshRenderer = new MeshRenderer(boxMesh, lampMat);
	//lightObjs->AddComponent(lampMeshRenderer);
	//Vector3 randomPos(Random::Range(-3.0f, 3.0f), Random::Range(-3.0f, 3.0f), Random::Range(-3.0f, 3.0f));
	//lightObjs->transform.Translate(randomPos);
	//scene->AddGameObject(lightObjs);


	int pointLightCount = 4;

	lightObjs = new GameObject[pointLightCount];

	Color pointColors[]{ Color::red, Color::green, Color::white, Color::blue };

	for (int i = 0; i < pointLightCount; i++)
	{
		//MeshRenderer * lampMeshRenderer = new MeshRenderer(boxMesh, lampMat);
		//lampMat = new Material(lampProgram);
		//lightObjs[i].AddComponent(lampMeshRenderer);
		Vector3 randomPos(Random::Range(-3.0f, 3.0f), Random::Range(-3.0f, 3.0f), Random::Range(-3.0f, 3.0f));
		//lightObjs[i].transform.Translate(randomPos);
		//scene->AddGameObject(&lightObjs[i]);
		light = new Light(kLightPoint);
		light->position = randomPos;
		light->constant = 1.0f;
		light->linear = 0.09f;
		light->quadratic = 0.032;
		light->color = pointColors[i];
		light->AddToManager();
	}
}
GLuint FramebufferName = 0;

bool App::CreateWorld()
{
	Random::SetSeed(5);
	mixValue = 0.5f;
	lampProgram = new ShaderProgram("./assets/lamp.vert", "./assets/lamp.frag");
	lightingProgram = new ShaderProgram("./assets/lighting.vert", "./assets/lighting.frag");

	diffuseMap = ResourceManager::GetInstance()->TryGetResource<Texture>("./assets/crate.jpg");
	specularMap = ResourceManager::GetInstance()->TryGetResource<Texture>("./assets/crate_specular.jpg");

	scene = new Scene();
	box = new GameObject();

	boxMat = new Material(lightingProgram);
	boxMat->AddTextureVariable("material.diffuse", diffuseMap);
	boxMat->AddTextureVariable("material.specular", specularMap);
	boxMat->SetFloat("material.shininess", 32.0f);
	//boxMat->SetCullMode(ECullMode::CM_Front);

	cubeMesh = new CubeMesh();

	MeshRenderer* boxMeshRenderer = new MeshRenderer(cubeMesh, boxMat);

	box->AddComponent(boxMeshRenderer);

	scene->AddGameObject(box);

	boxes = new GameObject[10];

	for (int i = 0; i < 10; i++)
	{
		MeshRenderer* box2MeshRenderer = new MeshRenderer(cubeMesh, boxMat);
		boxes[i].transform.Translate(Vector3(Random::Range(-8.0f, 8.0f), Random::Range(-6.0f, 6.0f), Random::Range(-1.0f, 1.0f)));
		boxes[i].transform.Scale(Random::Range(0.1f, 2.0f) * Vector3::one);
		boxes[i].transform.Rotate(Random::Range(0, 360), Random::Range(0, 360), Random::Range(0, 360));
		boxes[i].AddComponent(box2MeshRenderer);
		scene->AddGameObject(&boxes[i]);
	} 
	InitLights();

	boxMat->SetDirectionLight(*GetLightManager().GetDirectionLight());
	boxMat->SetPointLight(GetLightManager().GetPointLights());

	//MeshRenderer* box2MeshRenderer = new MeshRenderer(boxMesh, boxMat);
	////boxes[0].transform.Translate(Vector3(0,0,0));
	////boxes[0].transform.Scale(Random::Range(3.1f, 6.5f) * Vector3::one);
	////boxes[0].transform.Rotate(Random::Range(0, 360), Random::Range(0, 360), Random::Range(0, 360));
	//boxes[0].AddComponent(box2MeshRenderer);
	//scene->AddGameObject(&boxes[0]);


	//camera = new Camera(Vector3(-1, 1, 1.1f), Vector3::zero, Vector3::up);
	//camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 20, true);

	//camera = new Camera(Vector3(0, 0, -4), Vector3::zero, Vector3::up);
	//camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 0.1, 100, true);
	camera = new Camera(Vector3(0, 0, -8), Vector3::zero, Vector3::up);
	camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 2000, true);
	scene->SetActiveCamera(camera);

	return true;
}

void  App::UpdateCamera()
{
	return;
	float moveSpeed = 40.0f;
	float move = Time::deltaTime * moveSpeed;
	float xoffset =  pInput->GetMouseDeltaX();
	float yoffset = pInput->GetMouseDeltaY();

	float sensitivity = 1;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	float currentYaw = camera->transform.rotation.EulerAngle().y;
	float currentPitch = camera->transform.rotation.EulerAngle().x;

	currentYaw += yoffset;
	currentPitch += xoffset;

	currentPitch = Mathf::Clamp(-89, 89, currentPitch);

	camera->transform.Rotate(yoffset, xoffset, 0);

	//camera->transform.rotation = Quaternion::Euler(-currentYaw, currentPitch, 0);

	if (pInput->GetKeyDown(KEY_W))
	{
		camera->transform.position += move * camera->transform.GetForward();
	}

	if (pInput->GetKeyDown(KEY_S))
	{
		camera->transform.position += -move * camera->transform.GetForward();
	}

	if (pInput->GetKeyDown(KEY_A))
	{
		camera->transform.position += -move * camera->transform.GetRight();
	}

	if (pInput->GetKeyDown(KEY_D))
	{
		camera->transform.position += move * camera->transform.GetRight();
	}

}

void App::RenderWorld()
{
	UpdateCamera();
	pRenderer->Render(scene, scene->GetActiveCamera());
}

void App::DestroyWorld()
{
	glDeleteVertexArrays(1, &boxVAO);
	glDeleteBuffers(1, &boxVBO);

	delete cubeMesh;
}

void App::FrameMove()
{

}

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
		light->position = 5 * randomPos;
		light->constant = 1.0f;
		light->linear = 0.9f;
		light->quadratic = 0.32;
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
	boxMat->SetCullMode(ECullMode::CM_Front);

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
	camera = new Camera(Vector3(0, 0, -9), Vector3::zero, Vector3::up);
	camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 2000, true);
	scene->SetActiveCamera(camera);

	return true;
}

void  App::UpdateCamera()
{
	float sensitivity = 20;
	if (pInput->GetMouseButton(MOUSE_LEFT))
	{
		// step 1 : Calculate the amount of rotation given the mouse movement.
		float deltaAngleX = (2 * M_PI / GetWindow()->GetWidth()); // a movement from left to right = 2*PI = 360 deg
		float deltaAngleY = (M_PI / GetWindow()->GetHeight());  // a movement from top to bottom = PI = 180 deg
		float xAngle = pInput->GetMouseDeltaX() * deltaAngleX * sensitivity;
		float yAngle = pInput->GetMouseDeltaY() * deltaAngleY * sensitivity;

		// Extra step to handle the problem when the camera direction is the same as the up vector
		//float cosAngle = dot(app->m_camera.GetViewDir(), app->m_upVector);
		//if (cosAngle * sgn(yAngle) > 0.99f)
		//	yAngle = 0;

		// step 2: Rotate the camera around the pivot point on the first axis.
		Vector3 pivot = Vector3::zero;
		Matrix4x4 rotationMatrixX;
		rotationMatrixX.rotateY(xAngle);
		//rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, app->m_upVector);
		Vector3 newPos = (rotationMatrixX * (camera->transform.position - pivot)) + pivot;
		camera->transform.position = newPos;

		//// step 3: Rotate the camera around the pivot point on the second axis.
		Matrix4x4 rotationMatrixY;
		rotationMatrixY.rotate(yAngle, camera->transform.GetRight());
		newPos = (rotationMatrixY * (camera->transform.position - pivot)) + pivot;
		camera->transform.position = newPos;

		Matrix4x4 rotMatrix = Matrix4x4::LookAt(camera->transform.position, pivot, Vector3::up);
		camera->viewMatrix = rotMatrix;
	}

	return;


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

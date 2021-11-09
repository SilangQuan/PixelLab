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
		Vector3 randomPos(Random::Range(-3.0f, 3.0f), Random::Range(-3.0f, 3.0f), Random::Range(-3.0f, 3.0f));
	
		light = new Light(kLightPoint);
		light->position = 3 * randomPos;
		light->constant = 1.0f;
		light->linear = 0.9f;
		light->quadratic = 0.32;
		light->color = pointColors[i%4];
		light->AddToManager();
		Material* tmp = new Material(lampProgram);
		MeshRenderer * lampMeshRenderer = new MeshRenderer(sphereMesh, tmp);
		tmp->SetColor("lampColor", light->color);
		lightObjs[i].AddComponent(lampMeshRenderer);
		lightObjs[i].transform.Translate(light->position);
		lightObjs[i].transform.scale = 0.1f * Vector3::one;
		scene->AddGameObject(&lightObjs[i]);
	}
}
GLuint FramebufferName = 0;

void  App::ClusterSetUp()
{
	clusterSetupShader->Use();
	glUniform1f(glGetUniformLocation(clusterSetupShader->GetProgramID(), "zNear"), camera->zNear);
	glUniform1f(glGetUniformLocation(clusterSetupShader->GetProgramID(), "zFar"), camera->zFar);
	clusterSetupShader->Dispatch(gridSizeX, gridSizeY, gridSizeZ);
}

void App::InitSSBOs()
{
	int windowWidth = window->GetWidth();
	int windowHeight = window->GetHeight();
	//Setting up tile size on both X and Y 
	sizeX = (unsigned int)std::ceilf(windowWidth / (float)gridSizeX);

	float zFar = camera->zFar;
	float zNear = camera->zNear;

	//Buffer containing all the clusters
	{
		glGenBuffers(1, &AABBvolumeGridSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, AABBvolumeGridSSBO);

		//We generate the buffer but don't populate it yet.
		glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * sizeof(struct VolumeTileAABB), NULL, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, AABBvolumeGridSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	//Setting up screen2View ssbo
	{
		glGenBuffers(1, &screenToViewSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, screenToViewSSBO);
		ScreenToView screen2View;
		//Setting up contents of buffer
		screen2View.inverseProjectionMat = camera->projectionMaxtrix.inverted();
		screen2View.tileSizes[0] = gridSizeX;
		screen2View.tileSizes[1] = gridSizeY;
		screen2View.tileSizes[2] = gridSizeZ;
		screen2View.tileSizes[3] = sizeX;
		screen2View.screenWidth = windowWidth;
		screen2View.screenHeight = windowHeight;
		//Basically reduced a log function into a simple multiplication an addition by pre-calculating these
		screen2View.sliceScalingFactor = (float)gridSizeZ / std::log2f(zFar / zNear);
		screen2View.sliceBiasFactor = -((float)gridSizeZ * std::log2f(zNear) / std::log2f(zFar / zNear));

		//Generating and copying data to memory in GPU
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(struct ScreenToView), &screen2View, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, screenToViewSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	//Setting up lights buffer that contains all the lights in the scene
	{
		glGenBuffers(1, &lightSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, maxLights * sizeof(struct GPULight), NULL, GL_DYNAMIC_DRAW);

		GLint bufMask = GL_READ_WRITE;

		struct GPULight* lights = (struct GPULight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, bufMask);


		vector<Light*>& pointLights = GetLightManager().GetPointLights();
		for (unsigned int i = 0; i < pointLights.size(); ++i) {
			//Fetching the light from the current scene
			Light* light = pointLights[i];
			lights[i].position = Vector4(light->position.x, light->position.y, light->position.z, 1.0f);
			lights[i].color = Vector4(light->color.r, light->color.g, light->color.b, 1.0f);
			lights[i].enabled = 1;
			lights[i].intensity = 1.0f;
			lights[i].range = 65.0f;
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, lightSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	//A list of indices to the lights that are active and intersect with a cluster
	{
		unsigned int totalNumLights = numClusters * maxLightsPerTile; //50 lights per tile max
		glGenBuffers(1, &lightIndexListSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndexListSSBO);

		//We generate the buffer but don't populate it yet.
		glBufferData(GL_SHADER_STORAGE_BUFFER, totalNumLights * sizeof(unsigned int), NULL, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, lightIndexListSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	//Every tile takes two unsigned ints one to represent the number of lights in that grid
	//Another to represent the offset to the light index list from where to begin reading light indexes from
	//This implementation is straight up from Olsson paper
	{
		glGenBuffers(1, &lightGridSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightGridSSBO);

		glBufferData(GL_SHADER_STORAGE_BUFFER, numClusters * 2 * sizeof(unsigned int), NULL, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, lightGridSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	//Setting up simplest ssbo in the world
	{
		glGenBuffers(1, &lightIndexGlobalCountSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndexGlobalCountSSBO);

		//Every tile takes two unsigned ints one to represent the number of lights in that grid
		//Another to represent the offset 
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), NULL, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, lightIndexGlobalCountSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

}

bool App::CreateWorld()
{
	Random::SetSeed(2);
	mixValue = 0.5f;
	lampProgram = new ShaderProgram("./assets/lamp.vert", "./assets/lamp.frag");
	lampMat = new Material(lampProgram);
	lightingProgram = new ShaderProgram("./assets/lighting.vert", "./assets/lighting.frag");

	clusterSetupShader = new ShaderProgram("./assets/clusterShader.compute");
	clusterCullLightShader = new ShaderProgram("./assets/clusterCullLightShader.compute");

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
	sphereMesh = new SphereMesh(10, 20);

	MeshRenderer* boxMeshRenderer = new MeshRenderer(cubeMesh, boxMat);

	box->AddComponent(boxMeshRenderer);

	scene->AddGameObject(box);

	boxes = new GameObject[10];

	for (int i = 0; i < 10; i++)
	{
		MeshRenderer* box2MeshRenderer = new MeshRenderer(cubeMesh, boxMat);
		boxes[i].transform.Translate(Vector3(Random::Range(-6.0f, 6.0f), Random::Range(-6.0f, 6.0f), Random::Range(-6.0f, 6.0f)));
		boxes[i].transform.Scale(Random::Range(0.1f, 2.0f) * Vector3::one);
		boxes[i].transform.Rotate(Random::Range(0, 360), Random::Range(0, 360), Random::Range(0, 360));
		boxes[i].AddComponent(box2MeshRenderer);
		scene->AddGameObject(&boxes[i]);
	} 
	InitLights();

	boxMat->SetDirectionLight(*GetLightManager().GetDirectionLight());
	boxMat->SetPointLight(GetLightManager().GetPointLights());

	camera = new Camera(Vector3(0, 0, -9), Vector3::zero, Vector3::up);
	camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 2000, true);
	scene->SetActiveCamera(camera);

	InitSSBOs();
	ClusterSetUp();
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
	//ClusterSetUp();
	clusterCullLightShader->Use();
	glUniformMatrix4fv(glGetUniformLocation(clusterCullLightShader->GetProgramID(), "viewMatrix"), 1, GL_FALSE, &(camera->viewMatrix[0]));
	//glUniform1f(glGetUniformLocation(clusterCullLightShader->GetProgramID(), "viewMatrix"), camera->viewMatrix);
	clusterCullLightShader->Dispatch(1, 1, 6);


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

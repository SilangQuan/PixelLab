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
	ResourceManager::GetInstance()->LoadBuitinShaders();
	mSpecCubeMap = new TextureCubemap();
	mDiffuseCubeMap = new TextureCubemap();

	skyCubeBarcelona = new TextureCubemap("../../BuiltinAssets/texture/skyboxes/barcelona");
	skyCubeTokyo = new TextureCubemap("../../BuiltinAssets/texture/skyboxes/tokyo");
	skyCubeCatwalk = new TextureCubemap("../../BuiltinAssets/texture/skyboxes/catwalk");

	glDisable(GL_CULL_FACE);
	mDiffuseCubeMap->GenerateConvolutionMap(64, ResourceManager::GetInstance()->FindShader("ConvolveShader"), skyCubeBarcelona);
	mSpecCubeMap->GeneratePrefilterMap(256, ResourceManager::GetInstance()->FindShader("PrefilterShader"), skyCubeBarcelona);

	Random::SetSeed(2);
	mixValue = 0.5f;
	lampProgram = new ShaderProgram("./assets/lamp.vert", "./assets/lamp.frag");
	lampMat = new Material(lampProgram);
	lightingProgram = new ShaderProgram("./assets/lighting.vert", "./assets/lighting.frag");

	clusterSetupShader = new ShaderProgram("./assets/clusterShader.compute");
	clusterCullLightShader = new ShaderProgram("./assets/clusterCullLightShader.compute");

	diffuseMap = ResourceManager::GetInstance()->TryGetResource<Texture>("./assets/crate.jpg");
	specularMap = ResourceManager::GetInstance()->TryGetResource<Texture>("./assets/crate_specular.jpg");


	ForwardSceneRenderer* pForwardRenderer = dynamic_cast<ForwardSceneRenderer*>(pRenderer);
	pForwardRenderer->GetRenderContext()->BrdfLut = ResourceManager::GetInstance()->FindTexture("BrdfLut");
	pForwardRenderer->GetRenderContext()->DiffuseCubeMap = mDiffuseCubeMap;
	pForwardRenderer->GetRenderContext()->SpecCubeMap = mSpecCubeMap;

	mHdrRT = new RenderTexture();
	mHdrRT->Init(GetWindowWidth(), GetWindowHeight(), ColorType::RGBA16F, DepthType::Depth24S8, 0);

	mPostProcessor = new PostProcessor();
	mPostProcessor->InitRenderData();

	if (mEnableMsaa)
	{
		int msaaX = 16;
		mHdrRTMSAA = new RenderTexture();
		//mHdrRTMSAA->Init(GetWindowWidth(), GetWindowHeight(), ColorType::RGB565, DepthType::Depth24S8, msaaX);
		mHdrRTMSAA->Init(GetWindowWidth(), GetWindowHeight(), ColorType::RGBA16F, DepthType::Depth24S8, msaaX);

		if (pForwardRenderer != NULL)
		{
			pForwardRenderer->SetRenderTarget(mHdrRTMSAA);
		}
	}
	else
	{
		if (pForwardRenderer != NULL)
		{
			pForwardRenderer->SetRenderTarget(mHdrRT);
		}
	}

	scene = new Scene();

	scene->Init("./assets/Scenes/Sponza_PBR.json");
	//quad = scene->GetRoot()->GetChild(0)->GetChild(0);

	//scene->Init("./Assets/Scenes/HDR_Sphere.json");
	//quad = scene->FindByName("Center_child_0");
	camera = scene->GetActiveCamera();


	box = new GameObject();

	boxMat = new Material(lightingProgram);
	boxMat->AddTextureVariable("material.diffuse", diffuseMap);
	boxMat->AddTextureVariable("material.specular", specularMap);
	boxMat->SetFloat("material.shininess", 32.0f);
	boxMat->SetCullMode(ECullMode::CM_Back);

	cubeMesh = new CubeMesh();
	sphereMesh = new SphereMesh(10, 20);

	MeshRenderer* boxMeshRenderer = new MeshRenderer(cubeMesh, boxMat);

	box->AddComponent(boxMeshRenderer);

	scene->AddGameObject(box);

	InitLights();

	boxMat->SetDirectionLight(*GetLightManager().GetDirectionLight());
	boxMat->SetPointLight(GetLightManager().GetPointLights());
	//camera = scene->GetActiveCamera()
	camera = new Camera(Vector3(0, 0, -9), Vector3::zero, Vector3::up);
	//camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 2000, true);
	camera->fov = 90;
	camera->aspect = 16.0 / 9;
	camera->zNear = 0.1;
	camera->zFar = 2000;
	camera->projectionMaxtrix = Transform::Perspective(camera->fov, camera->aspect, camera->zNear, camera->zFar);

	scene->SetActiveCamera(camera);

	InitSSBOs();
	ClusterSetUp();
	return true;
}

void  App::UpdateCamera()
{
	float moveSpeed = 10.0f * Time::deltaTime;

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


	if (pInput->GetKey(KEY_W))
	{
		camera->transform.position += moveSpeed * camera->transform.GetForward();
	}

	if (pInput->GetKey(KEY_S))
	{
		camera->transform.position += -moveSpeed * camera->transform.GetForward();
	}

	if (pInput->GetKey(KEY_A))
	{
		camera->transform.position += -moveSpeed * camera->transform.GetRight();
	}

	if (pInput->GetKey(KEY_D))
	{
		camera->transform.position += moveSpeed * camera->transform.GetRight();
	}

	camera->viewMatrix = Matrix4x4::LookAt(camera->transform.position, camera->transform.position + camera->transform.GetForward(), Vector3::up);
	//camera->projectionMaxtrix = Transform::Perspective(camera->fov, camera->aspect, camera->zNear, camera->zFar);
	return;
}

void App::RenderWorld()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//Need to Change to GL_CCW. Bug need to be fixed 2022.2.9  
	glFrontFace(GL_CW);
	UpdateCamera();

	//ClusterSetUp();
	clusterCullLightShader->Use();
	glUniformMatrix4fv(glGetUniformLocation(clusterCullLightShader->GetProgramID(), "viewMatrix"), 1, GL_FALSE, &(camera->viewMatrix[0]));
	//glUniform1f(glGetUniformLocation(clusterCullLightShader->GetProgramID(), "viewMatrix"), camera->viewMatrix);
	clusterCullLightShader->Dispatch(1, 1, 6);

	pRenderer->Render(scene, scene->GetActiveCamera());

	if (mEnableMsaa)
	{
		mHdrRTMSAA->BindForRead();
		mHdrRT->BindForWrite();
		glBlitFramebuffer(0, 0, mHdrRT->GetWidth(), mHdrRT->GetHeight(), 0, 0, mHdrRT->GetWidth(), mHdrRT->GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBlitFramebuffer(0, 0, mHdrRT->GetWidth(), mHdrRT->GetHeight(), 0, 0, mHdrRT->GetWidth(), mHdrRT->GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	PushGroupMarker("Post Processing");
	PostProcessingInputsForward ppInputs;
	ppInputs.BackBufferFBO = 0;
	ppInputs.SceneColorTex = mHdrRT;
	ppInputs.Width = mHdrRT->GetWidth();
	ppInputs.Height = mHdrRT->GetHeight();
	ppInputs.Exposure = exposure;
	ppInputs.BloomActive = true;
	ppInputs.BloomThreshold = bloomThreshold;
	ppInputs.BloomIntensity = bloomIntensity;

	mPostProcessor->AddPostProcessingPasses(ppInputs);
	PopGroupMarker();

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

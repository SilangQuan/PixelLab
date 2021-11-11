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

#define TILE_SIZE 16
#define MAX_LIGHTS_PER_TILE 4


void App::InitLights()
{
	light = new Light(kLightDirectional);
	light->direction = Vector3(-0.2f, -1.0f, -0.3f);
	light->color = Color(1, 1, 1, 1);
	light->AddToManager();
	delete(light);

	int pointLightCount = MAX_LIGHTS_PER_TILE;

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

		//Material* tmp = new Material(lampProgram);
		//MeshRenderer * lampMeshRenderer = new MeshRenderer(sphereMesh, tmp);
		//tmp->SetColor("lampColor", light->color);
		//lightObjs[i].AddComponent(lampMeshRenderer);
		//lightObjs[i].transform.Translate(light->position);
		//lightObjs[i].transform.scale = 0.1f * Vector3::one;
		//scene->AddGameObject(&lightObjs[i]);
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

	mWorkGroupsX = (windowWidth + (windowWidth % TILE_SIZE)) / TILE_SIZE;
	mWorkGroupsY = (windowHeight + (windowHeight % TILE_SIZE)) / TILE_SIZE;

	// Lights SSBO
	//glGenBuffers(1, &ssbo_lights);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_lights);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Lights), &uniforms.lights, GL_DYNAMIC_DRAW);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_lights);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &lightSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, maxLights * sizeof(struct GPULight), NULL, GL_DYNAMIC_DRAW);

	GLint bufMask = GL_READ_WRITE;

	struct GPULight* lights = (struct GPULight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, bufMask);

	vector<Light*>& pointLights = GetLightManager().GetPointLights();
	for (unsigned int i = 0; i < pointLights.size(); ++i) {
		//Fetching the light from the current scene
		Light* light = pointLights[i];
		lights[i].position = Vector3(light->position.x, light->position.y, light->position.z);
		lights[i].color = Vector3(light->color.r, light->color.g, light->color.b);
		lights[i].intensity = 1.0f;
		lights[i].radius = 65.0f;
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	// Visible light indices SSBO
	glGenBuffers(1, &ssbo_visible_lights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_visible_lights);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * mWorkGroupsX * mWorkGroupsY * MAX_LIGHTS_PER_TILE, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_visible_lights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

bool App::CreateWorld()
{
	Random::SetSeed(2);
	mixValue = 0.5f;
	lampProgram = new ShaderProgram("./assets/lamp.vert", "./assets/lamp.frag");
	lampMat = new Material(lampProgram);
	lightingProgram = new ShaderProgram("./assets/lighting.vert", "./assets/lighting.frag");

	mDepthPreShader = new ShaderProgram("./assets/lamp.vert", "./assets/depthprepass.frag");
	mDepthPreMat = new Material(mDepthPreShader);

	clusterSetupShader = new ShaderProgram("./assets/clusterShader.compute");
	clusterCullLightShader = new ShaderProgram("./assets/clusterCullLightShader.compute");
	mCullLightShader = new ShaderProgram("./assets/lightculling.compute");
	

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

	mDepthPrePassTarget = new RenderTexture();
	//mDepthPrePassTarget->Init(GetWindowWidth(), GetWindowHeight(), ColorType::RGBA16F, DepthType::Depth24S8, 0);
	mDepthPrePassTarget->InitForDepthOnly(GetWindowWidth(), GetWindowHeight(), DepthType::Depth16);
	mForwardRenderer = dynamic_cast<ForwardSceneRenderer*>(pRenderer);

	mPostProcessor = new PostProcessor();
	mPostProcessor->InitRenderData();

	InitSSBOs();
	//ClusterSetUp();
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

	// Depth prepass
	{
		mForwardRenderer->SetRenderTarget(mDepthPrePassTarget);
		mForwardRenderer->SetReplacementShader(mDepthPreShader);
		mForwardRenderer->Render(scene, scene->GetActiveCamera());
		mForwardRenderer->SetReplacementShader(nullptr);
	}

	//LightCulling
	glDepthFunc(GL_EQUAL);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(mCullLightShader->GetProgramID());
	int LightCout = GetLightManager().GetPointLights().size();
	Matrix4x4 VP = camera->GetViewMatrix() * camera->GetProjectionMatrix();
	Matrix4x4 InvP =  camera->GetProjectionMatrix().inverted();
	Vector4 ScreenSizeAndInv(GetWindowWidth(), GetWindowHeight(), 1.0f / GetWindowWidth(), 1.0f / GetWindowHeight());
	glUniformMatrix4fv(glGetUniformLocation(mCullLightShader->GetProgramID(), "ViewInfo.Projection"), 1, GL_FALSE, &(camera->projectionMaxtrix[0]));
	glUniformMatrix4fv(glGetUniformLocation(mCullLightShader->GetProgramID(), "ViewInfo.View"), 1, GL_FALSE, &(camera->viewMatrix[0]));
	glUniformMatrix4fv(glGetUniformLocation(mCullLightShader->GetProgramID(), "ViewInfo.InvProjection"), 1, GL_FALSE, &(InvP[0]));
	glUniformMatrix4fv(glGetUniformLocation(mCullLightShader->GetProgramID(), "ViewInfo.ViewProjection"), 1, GL_FALSE, &(VP[0]));
	glUniform4f(glGetUniformLocation(mCullLightShader->GetProgramID(), "ViewInfo.ScreenSizeAndInv"),ScreenSizeAndInv.x,ScreenSizeAndInv.y,ScreenSizeAndInv.z,ScreenSizeAndInv.w);
	glUniform1i(glGetUniformLocation(mCullLightShader->GetProgramID(), "num_lights"), LightCout);
	mCullLightShader->SetUniform("num_lights", LightCout);
	mCullLightShader->Bind();

	// Bind shader storage buffer objects for the light and indice buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_lights);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_visible_lights);

	// Bind depth map texture
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(mCullLightShader->GetProgramID(), "depthMap"), 0);
	glBindTexture(GL_TEXTURE_2D, mDepthPrePassTarget->GetDepthID());

	// Dispatch the compute shader, using the workgroup values calculated earlier
	mCullLightShader->Dispatch(mWorkGroupsX, mWorkGroupsY, 1);

	// Unbind the depth map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	PushGroupMarker("Post Processing");
	PostProcessingInputsForward ppInputs;
	ppInputs.BackBufferFBO = 0;
	ppInputs.WorkGroupX = mWorkGroupsX;
	ppInputs.WorkGroupY = mWorkGroupsY;
	ppInputs.EnableForwardPlusDebug = true;
	ppInputs.SSBOVisibleLight = ssbo_visible_lights;
	mPostProcessor->AddPostProcessingPasses(ppInputs);
	PopGroupMarker();


	//pRenderer->Render(scene, scene->GetActiveCamera());
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

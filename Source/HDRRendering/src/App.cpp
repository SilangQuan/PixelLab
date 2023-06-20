#pragma comment( lib, "..\\..\\Build\\lib\\Debug\\LibAppFramework.lib" )

#include "../include/App.h"
#include "../../SkyEngine/include/SkyEngine.h"
#include "../../3rdParty/include/GLM/glm.hpp"

bool App::CreateWorld()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	refractionShader = new ShaderProgram("../../BuiltinAssets/shader/refraction.vert", "../../BuiltinAssets/shader/refraction.frag");
	reflectionShader = new ShaderProgram("../../BuiltinAssets/shader/reflection.vert", "../../BuiltinAssets/shader/reflection.frag");
	pbrOpaqueShader = new ShaderProgram("../../BuiltinAssets/shader/pbr_opaque.vert", "../../BuiltinAssets/shader/pbr_opaque.frag");
	pbrAlphaTestShader = new ShaderProgram("../../BuiltinAssets/shader/pbr_opaque.vert", "../../BuiltinAssets/shader/pbr_opaque_alphatest.frag");
	
	mConvolveShader = new ShaderProgram("../../BuiltinAssets/shader/cubeMapShader.vert", "../../BuiltinAssets/shader/convolveCubemapShader.frag");
	mPrefilterShader = new ShaderProgram("../../BuiltinAssets/shader/cubeMapShader.vert", "../../BuiltinAssets/shader/preFilteringShader.frag");

	skyCubeBarcelona = new TextureCubemap("../../BuiltinAssets/texture/skyboxes/barcelona");
	//skyCubeBarcelona = new TextureCubemap("../../BuiltinAssets/texture/skyboxes/forest");
	skyCubeTokyo = new TextureCubemap("../../BuiltinAssets/texture/skyboxes/tokyo");
	skyCubeCatwalk = new TextureCubemap("../../BuiltinAssets/texture/skyboxes/catwalk");

	mBrdfLut = ResourceManager::GetInstance()->TryGetResource<Texture>("../../BuiltinAssets/texture/BrdfLUT.hdr");

	ResourceManager::GetInstance()->LoadBuitinShaders();

	mat = new Material(refractionShader);
	mat->name = "refraction";
	//mat->SetCullMode(CM_Back);
	mat->SetCullMode(CM_None);

	metalMat = new Material(reflectionShader);
	metalMat->name = "reflection";
	//metalMat->SetCullMode(CM_Back);
	metalMat->SetCullMode(CM_None);
	
	ResourceManager::GetInstance()->AddMaterial(mat);
	ResourceManager::GetInstance()->AddMaterial(metalMat);

	postProcessor = new PostProcessor();
	postProcessor->InitRenderData();

	mEnableMsaa = true;
	
	ForwardSceneRenderer* pForwardRenderer = dynamic_cast<ForwardSceneRenderer*>(pRenderer);

	mHdrRT = new RenderTexture();
	mHdrRT->Init(GetWindowWidth(), GetWindowHeight(), ColorType::RGBA16F, DepthType::Depth24S8, 0);

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


	skyBoxMesh = new CubeMesh();
	skyBoxShader = new ShaderProgram("../../BuiltinAssets/shader/skyboxShader.vert", "../../BuiltinAssets/shader/skyboxShader.frag");
	skyBoxMat = new Material(skyBoxShader);
	skyBoxMat->SetCullMode(ECullMode::CM_None);
	//skyBoxMat->ZWriteMode = EZWriteMode::WM_OFF;
	//skyBoxMat->ZTestMode = EZTestMode::TM_LEQUAL;
	//skyBoxMat->ZTestMode = EZTestMode::TM_GEQUAL;

	exposure = 3;

	//pbrMat = ResourceManager::GetInstance()->FindMaterial("Mat");
	//pbrMat->AddTextureVariable("irradianceMap", mDiffuseCubeMap, ETextureV  ariableType::TV_CUBE, 5);
	//pbrMat->AddTextureVariable("prefilterMap", mSpecCubeMap, ETextureVariableType::TV_CUBE, 6);
	//pbrMat->AddTextureVariable("brdfLUT", mBrdfLut, ETextureVariableType::TV_2D, 7);
	

	scene = new Scene();
	//scene->Init("./Assets/Scenes/HDR_DamagedHelmet.json");
	//scene->Init("./Assets/Scenes/HDR_Bistro_Motor.json");
	//scene->Init("./Assets/Scenes/HDR_Bistro.json"); 
	//scene->Init("./Assets/Scenes/HDR_Bistro_RoadLight.json");

	//scene->Init("../../Library/Sponza/Sponza.json");
	//scene->Init("../../Library/Bistro/Bistro.json");
	//scene->Init("../../Library/PBRValidation/PBRValidation.json");
	scene->Init("../../../Library/HDR_Alucy/HDR_Alucy.json");
	//scene->Init("../../Library/DamagedHelmet/DamagedHelmet.json");
	
	quad = scene->GetRoot()->GetChild(0);

	//scene->Init("./Assets/Scenes/HDR_Sphere.json");
	//quad = scene->FindByName("Center_child_0");
	camera = scene->GetActiveCamera();
	float sens = 20;
	float speed = 10;
	mOrbitCameraController = new OrbitCameraController(camera, pInput, sens, GetWindow()->GetHeight(), GetWindow()->GetWidth());
	mFPSCameraController = new FPSCameraController(camera, pInput, sens, speed, GetWindow()->GetHeight(), GetWindow()->GetWidth());

	scene->Start();

	BakeIBL();

	//MeshRenderer* rc = quad->GetComponent<MeshRenderer>();
	//
	//if (rc != 0)
	//{
	//	Material* mat = rc->GetMaterial();
	//	if (mat != NULL && mat->GetShaderProgram()->HasUniform("brdfLUT"))
	//	{
	//		mat->SetTexture("irradianceMap", mDiffuseCubeMap);
	//		mat->SetTexture("prefilterMap", mSpecCubeMap);
	//		mat->SetTexture("brdfLUT", mBrdfLut);
	//	}
	//}
	//BakeIBL();
	pForwardRenderer->GetRenderContext()->BrdfLut = mBrdfLut;
	pForwardRenderer->GetRenderContext()->DiffuseCubeMap = mDiffuseCubeMap;
	pForwardRenderer->GetRenderContext()->SpecCubeMap = mSpecCubeMap;
	return true;
}


void App::RenderWorld()
{
	//BakeIBL();
	//ForwardSceneRenderer* pForwardRenderer = dynamic_cast<ForwardSceneRenderer*>(pRenderer);
	//pForwardRenderer->GetRenderContext()->BrdfLut = mBrdfLut;
	//pForwardRenderer->GetRenderContext()->DiffuseCubeMap = mDiffuseCubeMap;
	//pForwardRenderer->GetRenderContext()->SpecCubeMap = mSpecCubeMap;

	PushGroupMarker("SceneRender");
	pRenderer->Render(scene, scene->GetActiveCamera());

	Matrix4x4 transMatrix(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	Matrix4x4 scaleMatrix(1300, 0, 0, 0,
		0, 1300, 0, 0,
		0, 0, 1300, 0,
		0, 0, 0, 1);

	Matrix4x4 skyBoxModel = transMatrix * Quaternion::identity.GetRotMatrix() * scaleMatrix;

	glFrontFace(GL_CW);
	//Matrix4x4 skyBoxModel = Matrix4x4::identity.scale(2);
	pRenderer->DrawMesh(skyBoxMesh, &skyBoxModel, skyBoxMat, camera);

	PopGroupMarker();

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

	postProcessor->AddPostProcessingPasses(ppInputs);
	PopGroupMarker();
}

static Quaternion QuaternionFromMatrix(Matrix4x4 m) 
{
	// Adapted from: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
	Quaternion q ;
	q.w = Mathf::Sqrt(Mathf::Max(0, 1 + m[0, 0] + m[1, 1] + m[2, 2])) / 2;
	q.x = Mathf::Sqrt(Mathf::Max(0, 1 + m[0, 0] - m[1, 1] - m[2, 2])) / 2;
	q.y = Mathf::Sqrt(Mathf::Max(0, 1 - m[0, 0] + m[1, 1] - m[2, 2])) / 2;
	q.z = Mathf::Sqrt(Mathf::Max(0, 1 - m[0, 0] - m[1, 1] + m[2, 2])) / 2;
	q.x *= Mathf::Sign(q.x * (m[2, 1] - m[1, 2]));
	q.y *= Mathf::Sign(q.y * (m[0, 2] - m[2, 0]));
	q.z *= Mathf::Sign(q.z * (m[1, 0] - m[0, 1]));
	return q;
}


void App::FrameMove()
{
	//if (pInput->GetMouseButton(MOUSE_LEFT))
	//{
	//	qDebug() << "Rotation:" << quad->transform.rotation.EulerAngle();
	//	qDebug() << "Position:" << quad->transform.position;
	//	quad->transform.Rotate(0,3,0);
	//	quad->transform.position = quad->transform.rotation * (5 * Vector3::forward);
	//	quad->transform.SetDirty(true);
	//}

	//mOrbitCameraController->Update();
	mFPSCameraController->Update();
	//float rotSpeed = 10;
	//quad->transform.Rotate(0, Time::deltaTime * rotSpeed, 0);
	
	//BakeIBL();
	/*
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
	*/
	/*
	if (iblIndex == 0)
	{
		mat->AddTextureVariable("envMap", skyCubeBarcelona, TV_CUBE);
		skyBoxMat->AddTextureVariable("skybox", skyCubeBarcelona, TV_CUBE);
		metalMat->AddTextureVariable("envMap", skyCubeBarcelona, TV_CUBE);
	}
	else if (iblIndex == 1)
	{
		mat->AddTextureVariable("envMap", skyCubeTokyo, TV_CUBE);
		//skyBoxMat->AddTextureVariable("skybox", skyCubeTokyo, TV_CUBE);
		//metalMat->AddTextureVariable("envMap", skyCubeTokyo, TV_CUBE);
		
		metalMat->AddTextureVariable("envMap", mSpecCubeMap, TV_CUBE);
		skyBoxMat->AddTextureVariable("skybox", mDiffuseCubeMap, TV_CUBE);
	}
	else if (iblIndex == 2)
	{
		mat->AddTextureVariable("envMap", skyCubeCatwalk, TV_CUBE);
		metalMat->AddTextureVariable("envMap", skyCubeCatwalk, TV_CUBE);
		skyBoxMat->AddTextureVariable("skybox", skyCubeCatwalk, TV_CUBE);
	}
	
	if (materialIndex == 0)
	{
	}
	else if (materialIndex == 1)
	{
		quad->GetComponent<MeshRenderer>()->SetMaterial(mat);
	}
	else if (materialIndex == 2)
	{
		quad->GetComponent<MeshRenderer>()->SetMaterial(metalMat);
	}
	
	if (mEnableMsaa)
	{
		ForwardSceneRenderer* pForwardRenderer = dynamic_cast<ForwardSceneRenderer*>(pRenderer);

		if (pForwardRenderer != NULL)
		{
			pForwardRenderer->SetRenderTarget(mHdrRTMSAA);
		}
	}
	else
	{
		ForwardSceneRenderer* pForwardRenderer = dynamic_cast<ForwardSceneRenderer*>(pRenderer);

		if (pForwardRenderer != NULL)
		{
			pForwardRenderer->SetRenderTarget(mHdrRT);
		}
	}*/
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
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	bool bTestCheckBox = false;

	ImGui::Text("Material:");
	ImGui::RadioButton("Origin", &materialIndex, 0); ImGui::SameLine();
	ImGui::RadioButton("Refraction", &materialIndex, 1); ImGui::SameLine();
	ImGui::RadioButton("Reflection", &materialIndex, 2); ImGui::SameLine();

	ImGui::NewLine();
	ImGui::Text("IBL:");
	ImGui::RadioButton("Barcelona", &iblIndex, 0); ImGui::SameLine();
	ImGui::RadioButton("Tokyo", &iblIndex, 1); ImGui::SameLine();
	ImGui::RadioButton("Catwalk", &iblIndex, 2);
	ImGui::NewLine();

	ImGui::Text("Tonemap:");
	ImGui::SliderFloat("Exposure", &exposure, 0.0f, 10.0f);
	ImGui::NewLine();

	ImGui::Text("Bloom:");
	ImGui::SliderFloat("Threshold", &bloomThreshold, 0.0f, 5.0f);
	ImGui::SliderFloat("Intensity", &bloomIntensity, 0.0f, 6.0f);
	ImGui::NewLine();
	
	ImGui::Text("MSAA:");
	ImGui::Checkbox("MSAA", &mEnableMsaa);
	ImGui::NewLine();
	
	ImGui::End();
	pImGuiRenderer->EndFrame(window);
	PopGroupMarker();
}

void App::DestroyWorld()
{

}

void App::BakeIBL()
{
	mSpecCubeMap = new TextureCubemap();
	mDiffuseCubeMap = new TextureCubemap();

	mSpecCubeMap->setName("SpecCube");
	mDiffuseCubeMap->setName("DiffuseCube");
	mDiffuseCubeMap->GenerateConvolutionMap(64, mConvolveShader, skyCubeBarcelona);
	mSpecCubeMap->GeneratePrefilterMap(256, mPrefilterShader, skyCubeBarcelona);
}
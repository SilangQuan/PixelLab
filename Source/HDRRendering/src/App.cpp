#pragma comment( lib, "..\\..\\Build\\lib\\Debug\\LibAppFramework.lib" )

#include "../include/App.h"
#include "../../SkyEngine/include/SkyEngine.h"
#include "../../3rdParty/include/GLM/glm.hpp"

bool App::CreateWorld()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	refractionShader = ResourceManager::GetInstance()->LoadShader("refraction");
	reflectionShader = ResourceManager::GetInstance()->LoadShader("reflection");
	
	//refractionShader = new ShaderProgram(ResourceManager::GetShaderPath() + "refraction.vert", ResourceManager::GetShaderPath() + "refraction.frag");
	//reflectionShader = new ShaderProgram(ResourceManager::GetShaderPath() + "reflection.vert", ResourceManager::GetShaderPath() + "reflection.frag");
	pbrOpaqueShader = new ShaderProgram(ResourceManager::GetShaderPath() + "pbr_opaque.vert", ResourceManager::GetShaderPath() + "pbr_opaque.frag");
	pbrAlphaTestShader = new ShaderProgram(ResourceManager::GetShaderPath() + "pbr_opaque.vert", ResourceManager::GetShaderPath() + "pbr_opaque_alphatest.frag");
	
	mConvolveShader = new ShaderProgram(ResourceManager::GetShaderPath() + "cubeMapShader.vert", ResourceManager::GetShaderPath() + "convolveCubemapShader.frag");
	mPrefilterShader = new ShaderProgram(ResourceManager::GetShaderPath() + "cubeMapShader.vert", ResourceManager::GetShaderPath() + "preFilteringShader.frag");

	//skyCubeBarcelona = new TextureCubemap(ResourceManager::GetBuiltinAssetsPath() + "texture/skyboxes/barcelona");
	//skyCubeTokyo = new TextureCubemap(ResourceManager::GetBuiltinAssetsPath() + "texture/skyboxes/tokyo");
	//skyCubeCatwalk = new TextureCubemap(ResourceManager::GetBuiltinAssetsPath() + "texture/skyboxes/catwalk");

	skyCubeBarcelona = ResourceManager::GetInstance()->LoadTexCube(ResourceManager::GetBuiltinAssetsPath() + "texture/skyboxes/barcelona");
	skyCubeTokyo = ResourceManager::GetInstance()->LoadTexCube(ResourceManager::GetBuiltinAssetsPath() + "texture/skyboxes/tokyo");
	skyCubeCatwalk = ResourceManager::GetInstance()->LoadTexCube(ResourceManager::GetBuiltinAssetsPath() + "texture/skyboxes/catwalk");

	//mBrdfLut = ResourceManager::GetInstance()->TryGetResource<Texture>(ResourceManager::GetBuiltinAssetsPath() + "texture/BrdfLUT.hdr");
	mBrdfLut = ResourceManager::GetInstance()->LoadTex(ResourceManager::GetBuiltinAssetsPath() + "texture/BrdfLUT.hdr");

	ResourceManager::GetInstance()->LoadBuitinShaders();
	ResourceManager::GetInstance()->LoadBuiltinTexs();

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
	skyBoxShader = new ShaderProgram(ResourceManager::GetShaderPath() + "skyboxShader.vert", ResourceManager::GetShaderPath() + "skyboxShader.frag");
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
	

	mScene = new Scene();
	//scene->Init(ResourceManager::GetLibraryPath() +"DamagedHelmet/DamagedHelmet.json");
	//scene->Init("./Assets/Scenes/HDR_Bistro_Motor.json");
	//scene->Init("./Assets/Scenes/HDR_Bistro.json"); 
	//scene->Init("./Assets/Scenes/HDR_Bistro_RoadLight.json");

	mScene->Init("../../../Library/Sponza/Sponza.json");
	//scene->Init("../../Library/Bistro/Bistro.json");
	//scene->Init("../../Library/PBRValidation/PBRValidation.json");
	//scene->Init(ResourceManager::GetLibraryPath() +"HDR_Alucy/HDR_Alucy.json");
	//scene->Init("../../Library/DamagedHelmet/DamagedHelmet.json");
	
	quad = mScene->GetRoot()->GetChild(0);

	//scene->Init("./Assets/Scenes/HDR_Sphere.json");
	//quad = scene->FindByName("Center_child_0");
	camera = mScene->GetActiveCamera();
	float sens = 20;
	float speed = 10;
	mOrbitCameraController = new OrbitCameraController(camera, pInput, sens, GetWindow()->GetHeight(), GetWindow()->GetWidth());
	mFPSCameraController = new FPSCameraController(camera, pInput, sens, speed, GetWindow()->GetHeight(), GetWindow()->GetWidth());

	mScene->Start();

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
	pRenderer->Render(mScene, mScene->GetActiveCamera());

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
	mOrbitCameraController->Update();
	//mFPSCameraController->Update();

	//BakeIBL();
	/*

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
	RenderDevice* renderDevice = GetRenderDevice();

	PushGroupMarker("IMGui");
	pImGuiRenderer->BeginFrame(renderDevice);
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
	pImGuiRenderer->EndFrame(renderDevice);
	PopGroupMarker();
}

void App::DestroyWorld()
{
	SAFE_DELETE(mSpecCubeMap);
	SAFE_DELETE(mDiffuseCubeMap);
}

void App::BakeIBL()
{
	mDiffuseCubeMap = TextureCubemap::GenerateConvolutionMap(64, mConvolveShader, skyCubeBarcelona);
	mSpecCubeMap = TextureCubemap::GeneratePrefilterMap(256, mPrefilterShader, skyCubeBarcelona);
	mSpecCubeMap->setName("SpecCube");
	mDiffuseCubeMap->setName("DiffuseCube");

}
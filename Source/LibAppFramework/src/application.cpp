#include "../../SkyEngine/include/SkyEngine.h"
#include "../include/application.h"


Application::Application()
{
	fFPS = 0.0f;
	fInvFPS = 0.0f;
	fElapsedTime = 0.0f;

	bWindowed = true;
	bActive = false;

	frameIdent = 0;

	pAppData = 0;

	pInput = 0;
	//pFileIO = 0;
	pRenderer = 0;
	pScene = 0;
	pResManager = 0;
	pImGuiRenderer = 0;

	AllocConsole();
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONIN$", "r+t", stdin);
}

Application::~Application()
{
	SAFE_DELETE_ARRAY(pAppData);

	SAFE_DELETE(pScene);
	SAFE_DELETE(pResManager);
	//SAFE_DELETE(pGraphics);
	//SAFE_DELETE(pFileIO);
	SAFE_DELETE(pInput);
	SAFE_DELETE(pImGuiRenderer);
}

bool Application::CreateSubSystems(const CreationFlags &creationFlags)
{
	pInput = new Input(this);
	// NOTE: should handle possible failing! maybe bad_alloc...
	time = new Time();
	time->Start();

	if (!pInput->Initialize())
		return false;

	LightManager::InitializeClass();


	switch (creationFlags.renderPath)
	{
	case RenderingPath::Forward:
		pRenderer = new ForwardSceneRenderer();
		if (!pRenderer->Initialize(creationFlags.width, creationFlags.height))
			return false;
		break;

	case RenderingPath::Deferred:
		pRenderer = new DeferredSceneRenderer();
		if (!pRenderer->Initialize(creationFlags.width, creationFlags.height))
			return false;
		break;
	default:
		break;
	}

	pResManager = new ResourceManager();

	if (!CreateWorld())
	{
		DestroyWorld();
		return false;
	}

	pImGuiRenderer = new ImGuiRenderer();
	pImGuiRenderer->Init(window);

	return true;
}

// ----------------------------------------------------------------------------


bool Application::Initialize(const CreationFlags &creationFlags)
{
	bWindowed = creationFlags.isWindowed;

	window = new SdlWindow(creationFlags.title, creationFlags.width, creationFlags.height, creationFlags.graphicsAPI);

	if (creationFlags.graphicsAPI == GraphicsAPI::OpenGL)
	{
		if (!window->InitGL())
			return false;
	}
	else if (creationFlags.graphicsAPI == GraphicsAPI::Vulkan)
	{
		if (!window->InitVulkan())
			return false;
	}

	// Initialize the timer ---------------------------------------------------

	if (!QueryPerformanceFrequency(&ticksPerSecond))
		return false;

	return CreateSubSystems(creationFlags); 


}

int Application::Run()
{
	while (!pInput->QuitRequested())
	{
		time->Update();
		pInput->Update();
		BeginFrame();
		FrameMove();
		RenderWorld();
		RenderUI();

		Present();
	

		EndFrame();
	}
		
	window->Quit(0);
	DestroyWorld();

	return 0;
}

void Application::BeginFrame()
{

	if (!startTime.QuadPart)
	{
		QueryPerformanceCounter(&startTime);
		lastTime.QuadPart = startTime.QuadPart;
	}

	++frameIdent;

}

void Application::Present()
{
	window->Present();
}

void Application::RenderUI()
{
	if(window->GetCurGraphicsAPI() == GraphicsAPI::OpenGL)
		PushGroupMarker("IMGui");

	pImGuiRenderer->BeginFrame(window);
	int uiWidth = 200;
	int uiHeight = 300;

	// position the controls widget in the top-right corner with some margin
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
	// here we set the calculated width and also make the height to be
	// be the height of the main window also with some margin
	ImGui::SetNextWindowSize(
		ImVec2(static_cast<float>(uiWidth), static_cast<float>(uiHeight - 20)),
		ImGuiCond_Always
	);
	ImGui::Begin("SystemInfo", NULL, ImGuiWindowFlags_NoResize);
	ImGui::Dummy(ImVec2(0.0f, 1.0f));
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
	ImGui::Text("%s", SDL_GetPlatform());
	ImGui::Text("CPU cores: %d", SDL_GetCPUCount());
	ImGui::Text("RAM: %.2f GB", SDL_GetSystemRAM() / 1024.0f);
	//const GLubyte* vendor = glGetString​(GL_VENDOR); // Returns the vendor
	ImGui::End();
	
	pImGuiRenderer->EndFrame(window);
	
	if (window->GetCurGraphicsAPI() == GraphicsAPI::OpenGL)
		PopGroupMarker();

}

bool Application::CheckMessages()
{
	return true;
}

void Application::EndFrame()
{
	SDL_GL_SwapWindow(window->GetSDLWindow());


	LARGE_INTEGER iCurrentTime;
	QueryPerformanceCounter(&iCurrentTime);

	float32 fTimeDifference = (float32)(iCurrentTime.QuadPart - lastTime.QuadPart);
	fElapsedTime += fTimeDifference / (float32)ticksPerSecond.QuadPart;
	fFPS = (float32)ticksPerSecond.QuadPart / fTimeDifference;

	fInvFPS = 1.0f / fFPS;
	lastTime.QuadPart = iCurrentTime.QuadPart;
}

float32 Application::GetFPS() 
{
	return time->fps;
}


SdlWindow* Application::GetWindow()
{
	return window;
}


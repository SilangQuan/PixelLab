#include "ImGui/imgui_renderer.h"
#include "Core/SdlWindow.h"

ImGuiRenderer::ImGuiRenderer()
{
	
}

ImGuiRenderer::~ImGuiRenderer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiRenderer::Init(SdlWindow* window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	

	if (window->GetCurGraphicsAPI() == GraphicsAPI::OpenGL)
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		////io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		////io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		//
		//// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//
		const char* glsl_version = "#version 130";
		//// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForOpenGL(window->GetSDLWindow(), window->GetGLContext());
		ImGui_ImplOpenGL3_Init(glsl_version);
	}
	else if (window->GetCurGraphicsAPI() == GraphicsAPI::Vulkan)
	{
		
	}
}

void ImGuiRenderer::BeginFrame(SdlWindow* window)
{
	if (window->GetCurGraphicsAPI() == GraphicsAPI::OpenGL)
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
	}
	else if (window->GetCurGraphicsAPI() == GraphicsAPI::Vulkan)
	{
		//ImGui_ImplVulkan_NewFrame();
	}
	ImGui_ImplSDL2_NewFrame(window->GetSDLWindow());
	ImGui::NewFrame();
}

void ImGuiRenderer::EndFrame(SdlWindow* window)
{
	// Rendering
	ImGui::Render();
	if (window->GetCurGraphicsAPI() == GraphicsAPI::OpenGL)
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());	
	}
	else if (window->GetCurGraphicsAPI() == GraphicsAPI::Vulkan)
	{
		//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *(window->GetVulkanContext()->CurCommandBuffer));
	}
}


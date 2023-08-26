#include "ImGui/imgui_renderer.h"
#include "Core/SdlWindow.h"
#include "Render/RenderDevice.h"

ImGuiRenderer::ImGuiRenderer()
{
	
}

ImGuiRenderer::~ImGuiRenderer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiRenderer::Init(RenderDevice* device)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	

	if (device->GetCurGraphicsAPI() == GraphicsAPI::OpenGL)
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
		SdlWindow* window = device->GetWindow();
		ImGui_ImplSDL2_InitForOpenGL(window->GetSDLWindow(), window->GetGLContext());
		ImGui_ImplOpenGL3_Init(glsl_version);
	}
	else if (device->GetCurGraphicsAPI() == GraphicsAPI::Vulkan)
	{
		
	}
}

void ImGuiRenderer::BeginFrame(RenderDevice* device)
{
	if (device->GetCurGraphicsAPI() == GraphicsAPI::OpenGL)
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
	}
	else if (device->GetCurGraphicsAPI() == GraphicsAPI::Vulkan)
	{
		//ImGui_ImplVulkan_NewFrame();
	}
	ImGui_ImplSDL2_NewFrame(device->GetWindow()->GetSDLWindow());
	ImGui::NewFrame();
}

void ImGuiRenderer::EndFrame(RenderDevice* device)
{
	// Rendering
	ImGui::Render();
	if (device->GetCurGraphicsAPI() == GraphicsAPI::OpenGL)
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());	
	}
	else if (device->GetCurGraphicsAPI() == GraphicsAPI::Vulkan)
	{
		//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *(window->GetVulkanContext()->CurCommandBuffer));
	}
}


#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
//#include "imgui_impl_vulkan.h"

class SdlWindow;

class ImGuiRenderer
{
public:

	ImGuiRenderer();
	~ImGuiRenderer();

	void BeginFrame(SdlWindow* window);
	void EndFrame(SdlWindow* window);

	void Init(SdlWindow* window);

};
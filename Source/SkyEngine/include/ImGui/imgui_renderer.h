#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
//#include "imgui_impl_vulkan.h"
class RenderDevice;

class SdlWindow;

class ImGuiRenderer
{
public:

	ImGuiRenderer();
	~ImGuiRenderer();

	void BeginFrame(RenderDevice* device);
	void EndFrame(RenderDevice* device);

	void Init(RenderDevice* device);

};
#pragma once
#include "../../SkyEngine/include/Base.h"

#include "../../3rdParty/include/SDL/SDL.h"
#include "../../3rdParty/include/SDL/SDL_vulkan.h"
#include "../../3rdParty/include/SDL/SDL_opengl.h"

#include <windows.h>

class SdlWindow
{
public:
	SdlWindow();
	SdlWindow(std::string title, int width, int height, GraphicsAPI api = GraphicsAPI::OpenGL);
	~SdlWindow();
	void Quit(int code);
	void ToggleFullscreen();
	void HandleKeyEvent(SDL_Keysym* keysym);
	void HandleEvents();
	void SdlDie(const char *msg);
	void CheckSDLError(int line = -1);
	std::string GetTitle();

	bool InitGL();
	bool InitVulkan();

	void Present();

	SDL_Window * GetSDLWindow();
	SDL_GLContext* GetGLContext();

	bool IsActive();
	int GetWidth() { return width; };
	int GetHeight() { return height; };

	GraphicsAPI GetCurGraphicsAPI() { return mGAPI; }

protected:
private:
	SDL_Window *mainwindow;
	SDL_GLContext maincontext;
	//Whether the window is windowed or not
	bool windowed;
	//Whether the window is fine
	bool windowOK;
	std::string title;
	int width;
	int height;
	bool isActive;

	GraphicsAPI mGAPI;
};
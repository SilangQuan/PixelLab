#pragma once
#include "../../SkyEngine/include/Base.h"

#include "../../3rdParty/include/SDL/SDL.h"
#include "../../3rdParty/include/SDL/SDL_opengl.h"

#include <windows.h>

class RenderWindow
{
public:
	RenderWindow();
	RenderWindow(std::string title, int width, int height, GraphicsAPI api = GraphicsAPI::OpenGL);
	~RenderWindow();
	void Quit(int code);
	void ToggleFullscreen();
	void HandleKeyEvent(SDL_Keysym* keysym);
	void HandleEvents();
	void SdlDie(const char *msg);
	void CheckSDLError(int line = -1);
	std::string GetTitle();

	virtual bool InitRHI() = 0;
	virtual void Present();

	SDL_Window* GetSDLWindow() { return mSDLWindow; };
	GraphicsAPI GetCurGraphicsAPI() { return mGAPI; }

	bool IsActive();
	int GetWidth() { return width; };
	int GetHeight() { return height; };

protected:
	SDL_Window *mSDLWindow;
	bool windowed;
	bool windowOK;
	std::string mTitle;
	int width;
	int height;
	bool isActive;
	GraphicsAPI mGAPI;
};
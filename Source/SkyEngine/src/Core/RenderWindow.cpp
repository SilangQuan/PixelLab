#include "Core/RenderWindow.h"
#include "Log/Log.h"
#include <stdio.h>

RenderWindow::RenderWindow()
{
	isActive = true;
}

RenderWindow::RenderWindow(std::string _title, int _width, int _height, GraphicsAPI api)
{
	mTitle = _title;
	height = _height;
	width = _width;

	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		SdlDie("Unable to initialize SDL"); /* Or die on error */
	}

	if (api == GraphicsAPI::Vulkan)
	{
		// Setup window
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		mSDLWindow = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, window_flags);
		if (!mSDLWindow)
		{
			printf("Error: %s\n", SDL_GetError());
		}
	}
	else if (api = GraphicsAPI::OpenGL)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		/* Request opengl 4.4 context. Because Intel HD4600 only support ver 4.4.
		* SDL doesn't have the ability to choose which profile at this time of writing,
		* but it should default to the core profile */
		// Request OpenGL context
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		/* Turn on double buffering with a 24bit Z buffer.
		* You may need to change this to 16 or 32 for your system */

		int bpp = 16;
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 0);
		/*
		int bpp = 32;
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		*/
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, bpp);

		//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);


		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

		//SDLTest_CommonCreateState

		SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

		/* Create our window centered at 512x512 resolution */
		mSDLWindow = SDL_CreateWindow(mTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	}

	
	if (!mSDLWindow) /* Die if creation failed */
	{
		SdlDie("Unable to create window");
		printf("SDL_Init failed: %s\n", SDL_GetError());
	}
	else windowed = true;

	InitRHI();

	CheckSDLError(__LINE__);

	isActive = true;

}

RenderWindow::~RenderWindow()
{

}

void RenderWindow::Quit(int code)
{
	SDL_Quit();
	exit(code);
}

void RenderWindow::SdlDie(const char *msg)
{
	printf("%s: %s\n", msg, SDL_GetError());

	SDL_Quit();
	exit(1);
}

std::string RenderWindow::GetTitle()
{
	return mTitle;
}



void RenderWindow::ToggleFullscreen()
{

}

void RenderWindow::Present()
{

}


void RenderWindow::HandleKeyEvent(SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	case SDLK_ESCAPE:
		//this->quit(0);
		break;
	case SDLK_SPACE:
		std::cout << "Space" << std::endl;
		break;
	case SDLK_F1:
		this->ToggleFullscreen();
		break;
	case SDLK_r:
		std::cout << "Fuck" << std::endl;
		break;

	default:
		break;
	}
}

bool RenderWindow::IsActive()
{
	return isActive;
}


void RenderWindow::HandleEvents()
{
	// Our SDL event placeholder.
	SDL_Event event;
	//Grab all the events off the queue.
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			// Handle key Event
			HandleKeyEvent(&event.key.keysym);
			break;
		case SDL_QUIT:
			isActive = false;
			// Handle quit requests (like Ctrl-c).
			//quit(0);
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				if (mSDLWindow)
				{
					int tmpX, tmpY;
					SDL_GetWindowSize(mSDLWindow, &tmpX, &tmpY);
					//gl->resizeGL(tmpX, tmpY);

				}
			}
			SDL_GL_SwapWindow(mSDLWindow);
			break;
		}
	}
}

void RenderWindow::CheckSDLError(int line)
{

}


#include "Core/SdlWindow.h"
#include "Log/Log.h"
#include <stdio.h>

SdlWindow::SdlWindow()
{
	isActive = true;
}

void SDLDebug()
{
	int i, j, m, n, w, h;
	SDL_Rect bounds, usablebounds;
	float hdpi = 0;
	float vdpi = 0;
	SDL_DisplayMode mode;
	int bpp;
	Uint32 Rmask, Gmask, Bmask, Amask;
#if SDL_VIDEO_DRIVER_WINDOWS
	int adapterIndex = 0;
	int outputIndex = 0;
#endif
	n = SDL_GetNumVideoDisplays();
	SDL_Log("Number of displays: %d\n", n);
	for (i = 0; i < n; ++i)
	{
		SDL_Log("Display %d: %s\n", i, SDL_GetDisplayName(i));

		SDL_zero(bounds);
		SDL_GetDisplayBounds(i, &bounds);

		SDL_zero(usablebounds);
		//SDL_GetDisplayUsableBounds(i, &usablebounds);

		SDL_GetDisplayDPI(i, NULL, &hdpi, &vdpi);

		SDL_Log("Bounds: %dx%d at %d,%d\n", bounds.w, bounds.h, bounds.x, bounds.y);
		//SDL_Log("Usable bounds: %dx%d at %d,%d\n", usablebounds.w, usablebounds.h, usablebounds.x, usablebounds.y);
		SDL_Log("DPI: %fx%f\n", hdpi, vdpi);

		SDL_GetDesktopDisplayMode(i, &mode);
		SDL_PixelFormatEnumToMasks(mode.format, &bpp, &Rmask, &Gmask,
			&Bmask, &Amask);
		SDL_Log("  Current mode: %dx%d@%dHz, %d bits-per-pixel (%s)\n",
			mode.w, mode.h, mode.refresh_rate, bpp,
			SDL_GetPixelFormatName(mode.format));
		if (Rmask || Gmask || Bmask) {
			SDL_Log("      Red Mask   = 0x%.8x\n", Rmask);
			SDL_Log("      Green Mask = 0x%.8x\n", Gmask);
			SDL_Log("      Blue Mask  = 0x%.8x\n", Bmask);
			if (Amask)
				SDL_Log("      Alpha Mask = 0x%.8x\n", Amask);
		}

		/* Print available fullscreen video modes */
		m = SDL_GetNumDisplayModes(i);
		if (m == 0) {
			SDL_Log("No available fullscreen video modes\n");
		}
		else {
			SDL_Log("  Fullscreen video modes:\n");
			for (j = 0; j < m; ++j) {
				SDL_GetDisplayMode(i, j, &mode);
				SDL_PixelFormatEnumToMasks(mode.format, &bpp, &Rmask,
					&Gmask, &Bmask, &Amask);
				SDL_Log("    Mode %d: %dx%d@%dHz, %d bits-per-pixel (%s)\n",
					j, mode.w, mode.h, mode.refresh_rate, bpp,
					SDL_GetPixelFormatName(mode.format));
				if (Rmask || Gmask || Bmask) {
					SDL_Log("        Red Mask   = 0x%.8x\n",
						Rmask);
					SDL_Log("        Green Mask = 0x%.8x\n",
						Gmask);
					SDL_Log("        Blue Mask  = 0x%.8x\n",
						Bmask);
					if (Amask)
						SDL_Log("        Alpha Mask = 0x%.8x\n",
							Amask);
				}
			}
		}
	}
}

SdlWindow::SdlWindow(std::string _title, int _width, int _height, GraphicsAPI api)
{
	title = _title;
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
		mainwindow = SDL_CreateWindow(_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, window_flags);
		if (!mainwindow)
		{
			printf("Error: %s\n", SDL_GetError());
		}
		//mVulkanContext = new VulkanContext();
	}
	else if (api == GraphicsAPI::OpenGL)
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
		mainwindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	}

	
	if (!mainwindow) /* Die if creation failed */
	{
		SdlDie("Unable to create window");
		printf("SDL_Init failed: %s\n", SDL_GetError());
	}
	else windowed = true;

	CheckSDLError(__LINE__);

	isActive = true;

}

SdlWindow::~SdlWindow()
{

}

void SdlWindow::Quit(int code)
{
	SDL_Quit();
	exit(code);
}

void SdlWindow::SdlDie(const char *msg)
{
	printf("%s: %s\n", msg, SDL_GetError());

	SDL_Quit();
	exit(1);
}

std::string SdlWindow::GetTitle()
{
	return title;
}

bool SdlWindow::InitGL()
{
	mGAPI = GraphicsAPI::OpenGL;

	maincontext = SDL_GL_CreateContext(mainwindow);


	int r, g, b;
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &r);
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &g);
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &b);

	printf("Red size: %d, Green size: %d, Blue size: %d\n", r, g, b);
	//Reset View
	glViewport(0, 0, (GLint)width, (GLint)height);
	//Choose the Matrix mode
	glMatrixMode(GL_PROJECTION);
	//reset projection
	glLoadIdentity();
	//set perspection
	gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 1000.0);
	//choose Matrix mode
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_DITHER);
	glLoadIdentity();

	if (glewInit() != GLEW_OK)
	{
		cout << "GLEW init Failed!" << endl;
		exit(EXIT_FAILURE);
	}
	else cout << "GLEW init successful!" << endl;

	/* This makes our buffer swap syncronized with the monitor's vertical refresh */
	SDL_GL_SetSwapInterval(0);

	qDebug() << "----------Renderer Initialize----------";

	/* Enable smooth shading */
	glShadeModel(GL_SMOOTH);
	/* Set the background black */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	/* Depth buffer setup */
	glClearDepth(1.0f);
	/* Enables Depth Testing */
	glEnable(GL_DEPTH_TEST);
	/* The Type Of Depth Test To Do */
	glDepthFunc(GL_LEQUAL);
	/* Really Nice Perspective Calculations */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glFrontFace(GL_CCW);
	//glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("OpenGL error Renderer Init Error\n");
	}

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("GL Vendor : %s\n", vendor);
	printf("GL Renderer : %s\n", renderer);
	printf("GL Version (string) : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glslVersion);


	return true;

}


bool SdlWindow::InitVulkan()
{
	mGAPI = GraphicsAPI::Vulkan;
	return true;

	/*
	// Setup Vulkan
	uint32_t extensions_count = 0;
	SDL_Vulkan_GetInstanceExtensions(mainwindow, &extensions_count, NULL);
	const char** extensions = new const char* [extensions_count];
	SDL_Vulkan_GetInstanceExtensions(mainwindow, &extensions_count, extensions);
	SetupVulkan(extensions, extensions_count, &mVulkanContext);
	delete[] extensions;

	// Create Window Surface
	
	VkResult err;
	if (SDL_Vulkan_CreateSurface(mainwindow, mVulkanContext.Instance, &(mVulkanContext.Surface)) == 0)
	{
		printf("Failed to create Vulkan surface.\n");
		return 1;
	}*/
}


SDL_Window*  SdlWindow::GetSDLWindow()
{
	return mainwindow;
}

SDL_GLContext* SdlWindow::GetGLContext()
{
	return &maincontext;
}

void SdlWindow::ToggleFullscreen()
{

}

void SdlWindow::Present()
{

}


void SdlWindow::HandleKeyEvent(SDL_Keysym* keysym)
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

bool SdlWindow::IsActive()
{
	return isActive;
}


void SdlWindow::HandleEvents()
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
				if (mainwindow)
				{
					int tmpX, tmpY;
					SDL_GetWindowSize(mainwindow, &tmpX, &tmpY);
					//gl->resizeGL(tmpX, tmpY);

				}
			}
			SDL_GL_SwapWindow(mainwindow);
			break;
		}
	}
}

void SdlWindow::CheckSDLError(int line)
{

}


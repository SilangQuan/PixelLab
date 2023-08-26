#include <iostream>
#include "../include/App.h"
#include "Render/Threaded/RenderDeviceClient.h"

// Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 color;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"ourColor = color;\n"
"gl_Position = vec4(position.x, -position.y, position.z, 1.0);\n"
"}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
"in vec3 ourColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(ourColor, 1.0);\n"
"}\n\0";

const GLchar* fragmentShader2Source = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(0.2, 1, 0, 1.0);\n"
"}\n\0";

GLuint shaderProgram;
GLuint shaderProgram2;
GLuint VBOs[2], VAOs[2], EBO;

bool bRuning = true;

void ProcessEvent()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			bRuning = false;
			break;
		case SDL_KEYDOWN:
		{
			int index = event.key.keysym.scancode;
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				bRuning = false;
			}
		}
		break;
		}
	}

}



void InitRenderResource()
{

	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
	}
	else
	{
		qDebug() << "Vertex Shader compile success!";
	}


	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog;
	}
	else
	{
		qDebug() << "Fragment Shader compile success!";
	}

	// Fragment shader2
	GLuint fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fragmentShader2Source, NULL);
	glCompileShader(fragmentShader2);
	// Check for compile time errors
	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::FRAGMENT 2::COMPILATION_FAILED\n" << infoLog;
	}
	else
	{
		qDebug() << "Fragment Shader 2 compile success!";
	}

	// Link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
	}
	else
	{
		qDebug() << "Link Program success!";
	}

	// Link shaders 2
	shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);
	// Check for linking errors
	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::PROGRAM 2::LINKING_FAILED\n" << infoLog;
	}
	else
	{
		qDebug() << "Link Program2 success!";
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(fragmentShader2);


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		0.0f, -0.9,	1.0,	1.0f, 0.0f, 0.0f,
		0.9f, -0.9,	-1.0,	 0.0f, 1.0f, 0.0f,
		0.45f, 0.5f,	-1.0,   0.0f, 0.0f, 1.0f,
		-0.5,		0.5,		-1.0,	1.0f, 1.0f, 1.0f,
	};

	GLuint indices[] = {
		0,  3,1,
		1,  3,2
	};

	GLfloat secondTriangle[] = {
		0.9f, -0.5f, -1.0,  // Right
		0.0f, -0.5f, -1.0,  // Left
		0.45f, 0.5f, -1.0   // Top 
	};

	glGenVertexArrays(2, VAOs);
	glGenBuffers(2, VBOs);
	glGenBuffers(1, &EBO);

	//First: Square
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	//Second: Triangle
	glBindVertexArray(VAOs[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

}



void DrawScene()
{
	glCullFace(GL_NONE);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_ALWAYS);

	int n = 1000;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(shaderProgram);
	for (int i = 0; i < n; i++)
	{
		glBindVertexArray(VAOs[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(shaderProgram2);
	glBindVertexArray(VAOs[1]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

RenderDeviceClient* GRenderDeviceClient;

int main(int argc, char* argv[])
{
	CreationFlags creationFlags;
	int32 width = 640;
	int32 height = 360;

	creationFlags.width = width;
	creationFlags.height = height;
	creationFlags.title = "MultithreadedRendering";
	creationFlags.isWindowed = true;
	creationFlags.renderPath = RenderingPath::Forward;
	creationFlags.graphicsAPI == GraphicsAPI::OpenGL;

	SdlWindow* window = new SdlWindow(creationFlags.title, creationFlags.width, creationFlags.height, creationFlags.graphicsAPI);

	//GRenderDeviceClient = new RenderDeviceClient(GraphicsAPI::OpenGL, RenderThreadMode::ThreadModeThreaded, window);
	GRenderDeviceClient = new RenderDeviceClient(GraphicsAPI::OpenGL, RenderThreadMode::ThreadModeThreaded, window);

	//window->InitGL();

	InitRenderResource();

	while (bRuning)
	{
		ProcessEvent();
		DrawScene();
		SDL_GL_SwapWindow(window->GetSDLWindow());
	}

	//Quit SDL
	SDL_Quit();
	//getchar();
	qDebug() << "Ready to exit";
	exit(0);
	//exist();
	return 0;
}
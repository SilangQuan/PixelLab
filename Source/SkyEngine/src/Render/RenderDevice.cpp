#include "Render/RenderDevice.h"
#include "Core/SdlWindow.h"
#include "Core/ResourceManager.h"
#include "Render/RenderTexture.h"

GLenum GetGLDepthFunc(EZTestMode ztestMode)
{
	switch (ztestMode)
	{
	case EZTestMode::TM_ALWAYS:
		return GL_ALWAYS;
	case EZTestMode::TM_NEVER:
		return GL_NEVER;
	case EZTestMode::TM_LESS:
		return GL_LESS;
	case EZTestMode::TM_EQUAL:
		return GL_EQUAL;
	case EZTestMode::TM_LEQUAL:
		return GL_LEQUAL;
	case EZTestMode::TM_GREATER:
		return GL_GREATER;
	case EZTestMode::TM_NOTEQUAL:
		return GL_NOTEQUAL;
	case EZTestMode::TM_GEQUAL:
		return GL_GEQUAL;
	}
	return GL_LEQUAL;
}


static RenderDevice* GMainRenderDeviceDevice = nullptr;




RenderDevice* GetRenderDevice()
{
    assert(GMainRenderDeviceDevice);
    return GMainRenderDeviceDevice;
}

RenderDevice* CreateClientDevice(GraphicsAPI api, RenderThreadMode threadmode, SdlWindow* mainwindow)
{
	switch (api)
	{
	case GraphicsAPI::OpenGL:
		GMainRenderDeviceDevice = new RenderDeviceGL(mainwindow);
		return GMainRenderDeviceDevice;
	}
}


// helper macro that checks for GL errors.
#define GL_C(stmt) do {					\
	stmt;						\
	CheckOpenGLError(#stmt, __FILE__, __LINE__);	\
    } while (0)

inline void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("OpenGL error %08x, at %s:%i - for %s.\n", err, fname, line, stmt);
		//exit(1);
	}
}

int GetGLColorType(ColorType type)
{
	switch (type)
	{
	case RGB:
		return GL_RGB;
		break;
	case RGBA:
		break;
	case RGB565:
		break;
	case RGB888:
		break;
	case RGBA8888:
		break;
	case RGB16F:
		return GL_RGB16F;
		break;
	case RGBA16F:
		return GL_RGBA16F;
		break;
	case RGB32F:
		return GL_RGB32F;
		break;
	case R11G11B10:
		break;
	default:
		break;
	}
}


RenderDeviceGL::RenderDeviceGL(SdlWindow* mainwindow)
{
	window = mainwindow;
	maincontext = SDL_GL_CreateContext(mainwindow->GetSDLWindow());

	int r, g, b;
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &r);
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &g);
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &b);

	printf("Red size: %d, Green size: %d, Blue size: %d\n", r, g, b);
	//Reset View
	glViewport(0, 0, (GLint)mainwindow->GetWidth(), (GLint)mainwindow->GetHeight());

	glEnable(GL_DEPTH_TEST);

	if (glewInit() != GLEW_OK)
	{
		cout << "GLEW init Failed!" << endl;
		exit(EXIT_FAILURE);
	}
	else cout << "GLEW init successful!" << endl;

	/* This makes our buffer swap syncronized with the monitor's vertical refresh */
	SDL_GL_SetSwapInterval(0);
	qDebug() << "----------Renderer Initialize----------";

	/* Set the background black */
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	/* Depth buffer setup */
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);

	GLenum err1 = glGetError();
	if (err1 != GL_NO_ERROR) {
		printf("1111OpenGL error Renderer Init Error\n");
	}

	glEnable(GL_CULL_FACE);
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

}

void RenderDeviceGL::Init()
{
}

void RenderDeviceGL::ResizeWindow(int width, int height)
{
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
	glLoadIdentity();
}


void RenderDeviceGL::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderDeviceGL::GenGPUTexture(Texture* tex)
{
	glGenTextures(1, &tex->textureID);
}


Texture* RenderDeviceGL::CreateTexture2D(const TextureInfo& Info, void* data)
{
	Texture* tex = new Texture();
	tex->mWidth = Info.Width;
	tex->mHeight = Info.Height;
	tex->mBpp = Info.Bpp;
	tex->mWrapH = Info.WrapH;
	tex->mWrapV = Info.WrapV;
	tex->mMagnifiFilter = Info.MagnifiFilter;
	tex->mMinifiFilter = Info.MinifiFilter;


	glGenTextures(1, &tex->textureID);
	glBindTexture(GL_TEXTURE_2D, tex->textureID);
	if (Info.DataType == ETextureDataType::TDY_Float)
	{
		if (Info.ColorType == RGB32F)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Info.Width, Info.Height, 0, GL_RGB, GL_FLOAT, data);
			tex->mColorType = ColorType::RGB32F;
		}
	}
	else if(Info.DataType == ETextureDataType::TDY_UnsignedByte)
	{
		if (Info.ColorType == RGB)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Info.Width, Info.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			tex->mColorType = ColorType::RGB;
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Info.Width, Info.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			tex->mColorType = ColorType::RGBA;
		}
	}

	if (Info.GenerateMipMap)
		glGenerateMipmap(GL_TEXTURE_2D);

	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Info.WrapH);    // Note that we set our container wrapping method to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Info.WrapV);    // Note that we set our container wrapping method to GL_CLAMP_TO_EDGE

	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Info.MinifiFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Info.MagnifiFilter);
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}



void RenderDeviceGL::UpdateTextureCubeMip(TextureCubemap* tex, void* data[], int mip)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex->GetTextureID());
	int faceCount = 6;
	for (unsigned int i = 0; i < faceCount; i++)
	{
		if (data[i])
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, GetGLColorType(tex->GetColorType()), tex->GetWidth(), tex->GetHeight(), 0, GL_RGB, GL_FLOAT, data[i]);
		}
	}

}

TextureCubemap* RenderDeviceGL::CreateTextureCube(const TextureInfo& Info, void* data[])
{
	TextureCubemap* tex = new TextureCubemap();
	tex->mWidth = Info.Width;
	tex->mHeight = Info.Height;
	tex->mBpp = Info.Bpp;
	tex->mWrapH = Info.WrapH;
	tex->mWrapV = Info.WrapV;
	tex->mMagnifiFilter = Info.MagnifiFilter;
	tex->mMinifiFilter = Info.MinifiFilter;
	tex->SetColorType(Info.ColorType);

	glGenTextures(1, &tex->textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex->textureID);
	int faceCount = 6;
	for (unsigned int i = 0; i < faceCount; i++)
	{
		if (data[i])
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GetGLColorType(Info.ColorType), Info.Width, Info.Height, 0, GL_RGB, GL_FLOAT, data[i]);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return tex;
}

void RenderDeviceGL::DeleteTexture2D(Texture* texture)
{
	glDeleteTextures(1, &texture->textureID);
}

void RenderDeviceGL::UseTexture2D(Texture* texture, unsigned int index)
{
}
void RenderDeviceGL::SetClearColor(float r, float g, float b, float alpha)
{
	glClearColor(r, g, b, alpha);
}
void RenderDeviceGL::DrawTriangle(std::vector<Vector3>& vertices)
{
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, &vertices[0][0]);
	//glEnableVertexAttribArray(0);

	//glDrawArrays(GL_LINES, 0, 2);
}
void RenderDeviceGL::SetViewPort(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}
void RenderDeviceGL::AcqiureThreadOwnerShip()
{
	SDL_GL_MakeCurrent(window->GetSDLWindow(), maincontext);

}
void RenderDeviceGL::ReleaseThreadOwnership()
{
	SDL_GL_MakeCurrent(nullptr, nullptr);
}
void RenderDeviceGL::Present()
{
	SDL_GL_SwapWindow(window->GetSDLWindow());
}
void RenderDeviceGL::Draw2DPoint(const Vector2& pos)
{
}
void RenderDeviceGL::DrawLine(const std::vector<Vector3>& line)
{
}
Vector3 RenderDeviceGL::Project(const Vector3& coord, const Matrix4x4& transMat)
{
	return Vector3();
}
VBO* RenderDeviceGL::CreateVBO()
{
	return nullptr;
}
void RenderDeviceGL::UpdateVBO(VBO* vbo, const VBOData::Ptr& vboData)
{
}
void RenderDeviceGL::DeleteVBO(VBO* vbo)
{
}
void RenderDeviceGL::DrawVBO(VBO* vbo)
{
}
void RenderDeviceGL::DrawMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* mat, Camera* cam)
{
}
int RenderDeviceGL::GetScreenWidth()
{
	return 0;
}
int RenderDeviceGL::GetScreenHeigt()
{
	return 0;
}
void RenderDeviceGL::UseGPUProgram(ShaderProgram* program)
{
	glUseProgram(program->GetProgramID());
}


ShaderProgram* RenderDeviceGL::CreateGPUProgram(const std::string& vsFile, const std::string& fsFile)
{
	ShaderProgram* program = new ShaderProgram();
	uint32 programID = glCreateProgram();
	program->SetProgramID(programID);
	Shader* vsShader = ResourceManager::GetInstance()->TryGetResource<Shader>(vsFile);
	Shader* fsShader = ResourceManager::GetInstance()->TryGetResource<Shader>(fsFile);

	program->addShader(vsShader);
	program->addShader(fsShader);

	//Attach all of our shaders to the program object
	for (uint32 i = 0; i < Shader::NUM_SHADER_TYPES; i++)
	{
		Shader* shader = program->GetShader((Shader::ShaderType)i);
		if (shader != 0)
		{
			glAttachShader(programID, shader->shaderID);
		}
	}

	//And link
	glLinkProgram(programID);

	//Throw exception if failed
	GLint linkStatus;
	glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);

	if (linkStatus != GL_TRUE)
	{
		qDebug() << "ShaderProgram Link error: " << vsFile;
	}

	glValidateProgram(programID);

	GLint validateStatus;
	glGetProgramiv(programID, GL_LINK_STATUS, &validateStatus);

	if (validateStatus != GL_TRUE)
	{
		qDebug() << "ShaderProgram validate error: " << vsFile;
	}

	DetectUniforms(program);

	return program;
}

void RenderDeviceGL::DetectUniforms(ShaderProgram* program)
{
	uint32 programID = program->GetProgramID();
	GLint numActiveUniforms;

	glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

	for (uint32 i = 0; i < (uint32)numActiveUniforms; i++)
	{
		GLint nameLength;
		glGetActiveUniformsiv(programID, 1, &i, GL_UNIFORM_NAME_LENGTH, &nameLength);

		GLchar* uniformName = new GLchar[nameLength];
		glGetActiveUniformName(programID, i, nameLength, 0, uniformName);

		GLint uniformType;
		glGetActiveUniformsiv(programID, 1, &i, GL_UNIFORM_TYPE, &uniformType);

		GLenum type = (GLenum)uniformType;

		switch (type)
		{
		case GL_FLOAT:
			program->AddUniform<float>(uniformName);
			break;
		case GL_FLOAT_VEC2:
			program->AddUniform<Vector2>(uniformName);
			break;
		case GL_FLOAT_VEC3:
			program->AddUniform<Vector3>(uniformName);
			break;
		case GL_FLOAT_VEC4:
			program->AddUniform<Vector4>(uniformName);
			break;
		case GL_DOUBLE:
			program->AddUniform<double>(uniformName);
			break;
		case GL_INT:
			program->AddUniform<int>(uniformName);
			break;
		case GL_UNSIGNED_INT:
			program->AddUniform<uint32>(uniformName);
			break;
		case GL_BOOL:
			program->AddUniform<bool>(uniformName);
			break;
		case GL_FLOAT_MAT2:
			program->AddUniform<Matrix2>(uniformName);
			break;
		case GL_FLOAT_MAT3:
			program->AddUniform<Matrix3>(uniformName);
			break;
		case GL_FLOAT_MAT4:
			program->AddUniform<Matrix4x4>(uniformName);
			break;
		case GL_SAMPLER_1D:
			break;
		case GL_SAMPLER_2D:
			program->AddUniform<TextureVariable>(uniformName);
			break;
		case GL_SAMPLER_3D:
			program->AddUniform<TextureVariable>(uniformName);
			break;
		case GL_SAMPLER_CUBE:
			program->AddUniform<TextureVariable>(uniformName);
			break;
		default:
			break;
		}

		delete uniformName;
	}
}

void RenderDeviceGL::DeletGPUProgram(ShaderProgram* program)
{
}
GPUProgramParam* RenderDeviceGL::GetGPUProgramParam(ShaderProgram* program, const std::string& name)
{
	return nullptr;
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, int value) const
{
	glUniform1i(location, value);
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, float value) const
{
	glUniform1f(location, value);
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, double value) const
{
	glUniform1d(location, value);
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, unsigned int value) const
{
	glUniform1ui(location, value);
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, bool value) const
{
	glUniform1i(location, value);
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, Vector2 value) const
{
	glUniform2f(location,
		value.x,
		value.y);
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, Vector3 value) const
{
	glUniform3f(location,
		value.x,
		value.y,
		value.z);
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, Vector4 value) const
{
	glUniform4f(location,
		value.x,
		value.y,
		value.z,
		value.w);
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, Color value) const
{
	glUniform4f(location,
		value.r,
		value.g,
		value.b,
		value.a);
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, Matrix2 m) const
{
	glUniformMatrix2fv(location,
		1, false,
		&(m[0]));
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, Matrix3 m) const
{
	glUniformMatrix3fv(location,
		1, false,
		&(m[0]));
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, Matrix4x4 m) const
{
	glUniformMatrix4fv(location,
		1, false,
		&(m[0]));
}

void RenderDeviceGL::BindShaderProgramParam(uint32 location, TextureVariable tVariable) const
{

	glUniform1i(location, tVariable.GetTextureUnit());

	if (tVariable.GetTexture() == NULL)
		return;
	glActiveTexture(GL_TEXTURE0 + tVariable.textureUnit);
	switch (tVariable.type)
	{
	case TV_2D:
		glBindTexture(GL_TEXTURE_2D, tVariable.texture->GetTextureID());
		break;
	case TV_3D:
		glBindTexture(GL_TEXTURE_2D, tVariable.texture->GetTextureID());
		break;
	case TV_CUBE:
		glBindTexture(GL_TEXTURE_CUBE_MAP, tVariable.texture->GetTextureID());
		break;
	}

}


void RenderDeviceGL::SetDepthFunc(EZTestMode mode)
{
	glDepthFunc(GetGLDepthFunc(mode));
}

bool RenderDeviceGL::InitDepthOnlyRenderTexture(RenderTexture* rt, int inWidth, int inHeight, DepthType depth)
{
	GLenum err;
	rt->mWidth = inWidth;
	rt->mHeight = inHeight;
	glGenFramebuffers(1, &rt->mFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, rt->mFboId);
	err = glGetError();

	glGenTextures(1, &rt->mDepthBufferId);
	glBindTexture(GL_TEXTURE_2D, rt->mDepthBufferId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, inWidth, inHeight, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		rt->mDepthBufferId, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Could not validate framebuffer" << std::endl;
		return false;
	}
	return true;
}


bool RenderDeviceGL::InitRenderTexture(RenderTexture* rt, int inWidth, int inHeight, ColorType color, DepthType depth, int msaaSamples)
{
	GLenum err;
	rt->mWidth = inWidth;
	rt->mHeight = inHeight;
	glGenFramebuffers(1, &rt->mFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, rt->mFboId);
	err = glGetError();

	rt->mColorFormat = color;
	rt->mDepthFormat = depth;

	uint32 colorFormat;
	uint32 depthFormat;

	if (color >= RGB16F) {
		// Create the HDR render target
		switch (color) {
		case RGB16F: colorFormat = GL_RGB16F;   break;
		case RGBA16F: colorFormat = GL_RGBA16F; break;
		case R11G11B10: colorFormat = GL_R11F_G11F_B10F; break;

		default:  colorFormat = GL_R11F_G11F_B10F;break;
			break;
		}
	}
	else {
		// Create the LDR render target
		switch (color) {
		case RGB565: colorFormat = GL_RGB565; break;
		case RGBA8888: colorFormat = GL_RGBA; break;
		default:
		case RGB888: colorFormat = GL_RGB; break;
		}
	}

	if (depth != NoDepth) {
		switch (depth) {
		case Depth32: depthFormat = GL_DEPTH_COMPONENT32; break;
		case Depth24S8: depthFormat = GL_DEPTH24_STENCIL8; break;
		default:
		case Depth16: depthFormat = GL_DEPTH_COMPONENT16; break;
		}
	}

	glGenTextures(1, &rt->mColorBufferId);
	if (msaaSamples == 0)
	{
		glBindTexture(GL_TEXTURE_2D, rt->mColorBufferId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, rt->ms_useFiltering ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, rt->ms_useFiltering ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexStorage2D(GL_TEXTURE_2D, 1, colorFormat, rt->mWidth, rt->mHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->mColorBufferId, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, rt->mColorBufferId);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, rt->ms_useFiltering ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, rt->ms_useFiltering ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaSamples, colorFormat, rt->mWidth, rt->mHeight, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, rt->mColorBufferId, 0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}


	// Create depth buffer
	if (depth != NoDepth) {
		glGenRenderbuffers(1, &rt->mDepthBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, rt->mDepthBufferId);

		if (msaaSamples == 0)
		{
			glRenderbufferStorage(GL_RENDERBUFFER, depthFormat, rt->mWidth, rt->mHeight);
		}
		else
		{
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, depthFormat, rt->mWidth, rt->mHeight);
		}
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rt->mDepthBufferId);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return false;
	}
	return true;
}

void RenderDeviceGL::DeleteRenderTexture(RenderTexture* rt)
{
	if (rt->mColorBufferId) {
		glDeleteTextures(1, &rt->mColorBufferId);
		rt->mColorBufferId = 0;
	}

	if (rt->mDepthBufferId) {
		glDeleteRenderbuffers(1, &rt->mDepthBufferId);
		rt->mDepthBufferId = 0;
	}
	if (rt->mFboId) {
		glDeleteFramebuffers(1, &rt->mFboId);
		rt->mFboId = 0;
	}
}

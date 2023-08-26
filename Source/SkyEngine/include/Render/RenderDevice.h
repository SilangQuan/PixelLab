#pragma once
#include "EngineBase.h"
#include "RenderContext.h"
#include "ShaderProgram.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "TextureCubemap.h"

class RenderDeviceGL;
class SdlWindow;
class RenderTexture;




class RenderDevice
{
public:

	virtual void Init() = 0;
	virtual void ResizeWindow(int width, int height) = 0;

	virtual void SetCullFace(ECullMode cullMode) = 0;
	virtual void SetFrontFace(EFrontFace frontFace) = 0;

	//virtual void SetFillMode(EZWriteMode mode) = 0;
	virtual void SetDepthMask(EZWriteMode mode) = 0;
	virtual void SetDepthFunc(EZTestMode mode) = 0;
	virtual void BindVAO(uint32 vao) = 0;
	virtual void BindFrameBuffer(uint32 fbo) = 0;

	virtual void Clear() = 0;
	virtual GraphicsAPI GetCurGraphicsAPI() = 0;
	virtual SdlWindow* GetWindow() = 0;
	virtual Texture* CreateTexture2D(const TextureInfo& Info, void* data) = 0;
	virtual void DeleteTexture2D(Texture* texture) = 0;

	virtual TextureCubemap* CreateTextureCube(const TextureInfo& Info, void* data[]) = 0;

	virtual bool InitRenderTexture(RenderTexture* rt, int width, int height, ColorType color, DepthType depth, int msaaSamples = 0) = 0;
	virtual bool InitDepthOnlyRenderTexture(RenderTexture* rt, int inWidth, int inHeight, DepthType depth) = 0;
	virtual void DeleteRenderTexture(RenderTexture* rt) = 0;

	virtual void UseTexture2D(Texture* texture, unsigned int index) = 0;
	virtual void SetClearColor(float r, float g, float b, float alpha) = 0;
	virtual void DrawTriangle(std::vector<Vector3>& vertices) = 0;
	virtual void SetViewPort(int x, int y, int width, int height) = 0;
	virtual void AcqiureThreadOwnerShip() = 0;
	virtual void ReleaseThreadOwnership() = 0;
	virtual void BeginFrame() = 0;
	virtual void Present() = 0;
	virtual VBO* CreateVBO() = 0;
	virtual void UpdateVBO(VBO* vbo, const VBOData::Ptr& vboData) = 0;
	virtual void DeleteVBO(VBO* vbo) = 0;
	virtual void DrawVBO(VBO* vbo) = 0;
	virtual void Cleanup() = 0;

	virtual void UseGPUProgram(ShaderProgram* program) = 0;
	virtual ShaderProgram* CreateGPUProgram(const std::string& vertexShader, const std::string& fragmentShader) = 0;
	virtual void DetectUniforms(ShaderProgram* program) = 0;

	virtual void DeletGPUProgram(ShaderProgram* program) = 0;

	virtual uint32 GenFullscreenQuadVAO() = 0;

	virtual GPUProgramParam* GetGPUProgramParam(ShaderProgram* program, const std::string& name) = 0;

	virtual void BindShaderProgramParam(uint32 location, int value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, float value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, double value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, unsigned int value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, bool value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, Vector2 value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, Vector3 value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, Vector4 value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, Color value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, Matrix2 value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, Matrix3 value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, Matrix4x4 value) const = 0;
	virtual void BindShaderProgramParam(uint32 location, TextureVariable value) const = 0;

	virtual int GetScreenWidth() = 0;
	virtual int GetScreenHeigt() = 0;

	virtual void DrawMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* mat, Camera* cam) = 0;

	virtual void DrawElements(uint32 indiceCount) = 0;
	virtual void DrawArrays(uint32 indiceCount) = 0;
};


class RenderDeviceGL : public RenderDevice
{
private:
	RenderContext* mRenderContext;
public:
	RenderDeviceGL(SdlWindow* mainwindow);

	~RenderDeviceGL() {
	}

	void Init();
	void ResizeWindow(int width, int height);
	virtual void Cleanup() {}
	virtual void Clear();

	virtual GraphicsAPI GetCurGraphicsAPI() { return GraphicsAPI::OpenGL; }
	virtual SdlWindow* GetWindow() { return window; };

	virtual void SetCullFace(ECullMode cullMode)
	{
		if (cullMode == ECullMode::CM_None)
		{
			glCullFace(GL_NONE);
		}
		else
		{
			glCullFace(cullMode == ECullMode::CM_Front ? GL_FRONT : GL_BACK);
		}
	}

	virtual void SetFrontFace(EFrontFace frontFace)
	{
		glFrontFace(frontFace == EFrontFace::FF_CCW ? GL_CCW : GL_CW);
	}

	virtual void SetDepthMask(EZWriteMode mode)
	{
		glDepthMask(mode == EZWriteMode::WM_ON ? GL_TRUE : GL_FALSE);
	}

	virtual void SetDepthFunc(EZTestMode mode);

	virtual void BindVAO(uint32 vao)
	{
		glBindVertexArray(vao);
	}

	virtual void BindFrameBuffer(uint32 fbo)
	{
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}


	virtual void DrawElements(uint32 indiceCount)
	{
		glDrawElements(GL_TRIANGLES, indiceCount, GL_UNSIGNED_INT, 0);
	}

	virtual void DrawArrays(uint32 indiceCount)
	{
		glDrawArrays(GL_TRIANGLES, 0, indiceCount);
	}

	virtual uint32 GenFullscreenQuadVAO()
	{
		uint32 VAO;
		// configure VAO/VBO
		unsigned int VBO;
		float vertices[] = {
			// pos        // tex
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f,

			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(VAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
		return VAO;
	}


	virtual bool InitRenderTexture(RenderTexture* rt, int width, int height, ColorType color, DepthType depth, int msaaSamples = 0);
	virtual bool InitDepthOnlyRenderTexture(RenderTexture* rt, int inWidth, int inHeight, DepthType depth);
	virtual void DeleteRenderTexture(RenderTexture* rt);

	virtual Texture* CreateTexture2D(const TextureInfo& Info, void* data);
	virtual TextureCubemap* CreateTextureCube(const TextureInfo& Info, void* data[]);
	virtual void DeleteTexture2D(Texture* texture);
	virtual void UseTexture2D(Texture* texture, unsigned int index);
	virtual void SetClearColor(float r, float g, float b, float alpha);
	virtual void DrawTriangle(std::vector<Vector3>& vertices);
	virtual void SetViewPort(int x, int y, int width, int height);
	virtual void AcqiureThreadOwnerShip();
	virtual void ReleaseThreadOwnership();
	virtual void BeginFrame() {};
	virtual void Present();
	virtual void Draw2DPoint(const Vector2& pos);
	virtual void DrawLine(const std::vector<Vector3>& line);
	virtual Vector3 Project(const Vector3& coord, const Matrix4x4& transMat);
	virtual VBO* CreateVBO();
	virtual void UpdateVBO(VBO* vbo, const VBOData::Ptr& vboData);
	virtual void DeleteVBO(VBO* vbo);
	virtual void DrawVBO(VBO* vbo);
	virtual void DrawMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* mat, Camera* cam);

	virtual int GetScreenWidth();
	virtual int GetScreenHeigt();
	//gpu program
	virtual void UseGPUProgram(ShaderProgram* program);

	virtual ShaderProgram* CreateGPUProgram(const std::string& vertexShader, const std::string& fragmentShader);
	virtual void DetectUniforms(ShaderProgram* program);

	virtual void DeletGPUProgram(ShaderProgram* program);

	virtual GPUProgramParam* GetGPUProgramParam(ShaderProgram* program, const std::string& name);

	virtual void BindShaderProgramParam(uint32 location, int value) const;
	virtual void BindShaderProgramParam(uint32 location, float value) const;
	virtual void BindShaderProgramParam(uint32 location, double value) const;
	virtual void BindShaderProgramParam(uint32 location, unsigned int value) const;
	virtual void BindShaderProgramParam(uint32 location, bool value) const;
	virtual void BindShaderProgramParam(uint32 location, Vector2 value) const;
	virtual void BindShaderProgramParam(uint32 location, Vector3 value) const;
	virtual void BindShaderProgramParam(uint32 location, Vector4 value) const;
	virtual void BindShaderProgramParam(uint32 location, Color value) const;
	virtual void BindShaderProgramParam(uint32 location, Matrix2 value) const;
	virtual void BindShaderProgramParam(uint32 location, Matrix3 value) const;
	virtual void BindShaderProgramParam(uint32 location, Matrix4x4 value) const;
	virtual void BindShaderProgramParam(uint32 location, TextureVariable value) const;


private:
	SDL_GLContext maincontext;
	SdlWindow* window;
};

RenderDevice* CreateClientDevice(GraphicsAPI api, RenderThreadMode threadmode, SdlWindow* mainwindow);

RenderDevice* GetRenderDevice();
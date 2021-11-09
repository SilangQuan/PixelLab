#pragma once
#include <stack>
#include <map>
#include "SceneRendererBase.h"
#include "Mesh.h"

class SkyEngine;
class RenderContext;
class RenderTexture;
class ShaderProgram;

class ForwardSceneRenderer : public SceneRendererBase
{
public:
	ForwardSceneRenderer();
	~ForwardSceneRenderer();
	bool Initialize(int width, int height);
	void ResizeGL(int width, int height);
	void Render(Scene* scene, Camera* camera);
	void DrawMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* mat, Camera* cam);

	void RenderGameObject(GameObject* gameObject, RenderContext* renderContext);

	void SetRenderTarget(RenderTexture* RT);
	void SetReplacementShader(ShaderProgram* shader) { mReplaceShader = shader; };
	RenderContext* GetRenderContext() { return renderContext; };

private:
	SkyEngine* engine;
	RenderContext* renderContext;
	RenderTexture* mRenderTarget;
	ShaderProgram* mReplaceShader;
};

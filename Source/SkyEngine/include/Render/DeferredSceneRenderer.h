#pragma once
#include <stack>
#include <map>
#include "SceneRendererBase.h"
#include "Mesh.h"
#include "ShaderProgram.h"

class Application;
class SkyEngine;
class RenderContext;
class GBuffer;

enum RenderContentType
{
	Albedo,
	Normal,
	PosWorld,
	Depth,
	Result,
};

class DeferredSceneRenderer : public SceneRendererBase
{
public:
	DeferredSceneRenderer();
	~DeferredSceneRenderer();
	bool Initialize(int inWindowWidth, int inWindowHeight);

	void ResizeGL(int width, int height);
	void Render(Scene* scene, Camera* camera);
	void RenderGameObjectGeo(GameObject* gameObject, RenderContext* renderContext);
	void RenderGameObject(GameObject* gameObject, RenderContext* renderContext);
	void DrawMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* mat, Camera* cam);

	void SetShaderProgram(ShaderProgram* geoPass, ShaderProgram* dirLightingPass, ShaderProgram* pointLightingPass);

	void CreateSphere();

	void RenderPointLight(float radius, const Vector3& position, const Color& color);
	void GeoPass(Scene* scene);
	void DirLightPass(Camera* camera);
	void PointLightPass(Camera* camera);

	void SwitchRenderType(RenderContentType type);

private:
	RenderContext* renderContext;
	GBuffer* mGBuffer;

	ShaderProgram* mGeometryPass;
	ShaderProgram* mDirLightingPass;
	ShaderProgram* mPointLightingPass;

	// light sphere geometry:
	GLuint mSpherePositionVbo;
	GLuint mSphereIndexVbo;
	GLuint mSphereIndexCount;
	GLuint mLightSphereVAO;

	RenderContentType mCurrentContentType;

	int mViewWidth;
	int mViewHeight;
};

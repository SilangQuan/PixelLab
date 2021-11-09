#pragma once

#include "RenderContext.h"

class Scene;
class Camera;
class Material;
class GameObject;
class Mesh;

class SceneRendererBase
{
public:
	virtual void Render(Scene* scene, Camera* camera) = 0;
	virtual void RenderGameObject(GameObject* gameObject, RenderContext* renderContext) = 0;
	virtual bool Initialize(int width, int height) = 0;
	virtual void DrawMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* mat, Camera* cam) = 0;
};

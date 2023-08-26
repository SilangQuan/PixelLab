#pragma once

#include "EngineBase.h"

class Mesh;
class Matrix4x4;
class Material;
class Camera;
class RenderContext;

class Graphics
{
public:
	static void DrawMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* mat, Camera* cam, RenderContext* context);

};
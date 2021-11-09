#pragma once

#include "Base.h"
#include "Component.h"

class Mesh;
class Material;
class Camera;
class RenderContext;
class ShaderProgram;

class MeshRenderer: public Component
{
	Mesh* mesh;
	Material* material;

public:
	MeshRenderer(Mesh* mesh, Material* material);
	virtual ~MeshRenderer();

	virtual void Render(RenderContext* renderContext, ShaderProgram* replaceShader = nullptr);

	Mesh* GetMesh();
	Material* GetMaterial();

	void SetMesh(Mesh* mesh);
	void SetMaterial(Material* material);
};
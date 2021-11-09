
#include "Components/MeshRenderer.h"
#include "Render/ShaderProgram.h"
#include "Render/RenderContext.h"
#include "Render/Material.h"
#include "Render/Mesh.h"
#include "Core/GameObject.h"
#include "Core/Camera.h"
#include "Lighting/LightManager.h"
#include "Render/Graphics.h"

MeshRenderer::MeshRenderer(Mesh* _mesh, Material* _material)
{
	mesh = _mesh;
	material = _material;
}
MeshRenderer::~MeshRenderer()
{
	
}


void MeshRenderer::Render(RenderContext* renderContext, ShaderProgram* replaceShader)
{
	//glCullFace(GL_FRONT);
	if (material->GetCullMode() == ECullMode::CM_None)
	{
		glCullFace(GL_NONE);
	}
	else
	{
		glCullFace(material->GetCullMode() == ECullMode::CM_Front ? GL_FRONT : GL_BACK);
	}

	glDepthMask(material->ZWriteMode == EZWriteMode::WM_ON ? GL_TRUE : GL_FALSE);
	glDepthFunc(Graphics::GetDepthFunc(material->ZTestMode));

	renderContext->modelMatrix = GetGameObject()->GetTransform()->GetLocalToWorldMatrix();

	ShaderProgram* shaderProgram = material->GetShaderProgram();
	if (replaceShader != nullptr)
	{
		shaderProgram = replaceShader;
		shaderProgram->Bind(renderContext);
	}
	else
	{
		material->Bind(renderContext);
	}

	//Bind the Vertex Array
	GLuint vao = mesh->GetVAO();
	glBindVertexArray(vao);

	//DrawElements
	if(mesh->GetIndices().size() > 0)
	{
		glDrawElements(
			GL_TRIANGLES,
			mesh->GetIndices().size(),
			GL_UNSIGNED_INT,
			0);
	}
	else
	{
		int size = mesh->GetPositions().size();
		glDrawArrays(GL_TRIANGLES, 0, size);
	}

	glCullFace(GL_BACK);
	//Unbind the Vertex Array
	glBindVertexArray(0);
}

Mesh*  MeshRenderer::GetMesh()
{
	return mesh;
}

Material*  MeshRenderer::GetMaterial()
{
	return material;
}

void  MeshRenderer::SetMesh(Mesh* _mesh)
{
	mesh = _mesh;
}

void  MeshRenderer::SetMaterial(Material* _material)
{
	material = _material;
}
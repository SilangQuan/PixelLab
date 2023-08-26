
#include "Components/MeshRenderer.h"
#include "Render/ShaderProgram.h"
#include "Render/RenderContext.h"
#include "Render/Material.h"
#include "Render/Mesh.h"
#include "Core/GameObject.h"
#include "Core/Camera.h"
#include "Lighting/LightManager.h"
#include "Render/Graphics.h"
#include "Render/RenderDevice.h"

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
	RenderDevice* renderDevice = GetRenderDevice();

	renderDevice->SetCullFace(material->GetCullMode());

	renderDevice->SetDepthMask(material->ZWriteMode);
	renderDevice->SetDepthFunc(material->ZTestMode);

	renderContext->modelMatrix = GetGameObject()->GetTransform()->GetLocalToWorldMatrix();

	ShaderProgram* shaderProgram = material->GetShaderProgram();
	if (replaceShader != nullptr)
	{
		shaderProgram = replaceShader;
		shaderProgram->Bind(renderDevice, renderContext);
	}
	else
	{
		material->Bind(renderDevice, renderContext);
	}

	if (renderContext->bEnableTileShading)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, renderContext->LightInfoBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, renderContext->VisibleLightsBuffer);
		glUniform1i(glGetUniformLocation(shaderProgram->GetProgramID(), "workgroup_x"), renderContext->WorkGroupsX);
		glUniform1i(glGetUniformLocation(shaderProgram->GetProgramID(), "workgroup_y"), renderContext->WorkGroupsY);
	}
	
	//Bind the Vertex Array
	renderDevice->BindVAO(mesh->GetVAO());

	if(mesh->indiceCount > 0)
	{
		renderDevice->DrawElements(mesh->indiceCount);
	}
	else
	{
		renderDevice->DrawArrays(mesh->vertexCount);
	}

	renderDevice->BindVAO(0);
}

Mesh* MeshRenderer::GetMesh()
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
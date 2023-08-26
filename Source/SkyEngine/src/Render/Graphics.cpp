#include "Render/Graphics.h"
#include "Render/Mesh.h"
#include "Render/RenderContext.h"
#include "Render/Material.h"
#include "Render/RenderDevice.h"

void Graphics::DrawMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* material, Camera* cam, RenderContext* context)
{
	RenderDevice* renderDevice = GetRenderDevice();
	context->modelMatrix = *modelMatrix;
	material->Bind(renderDevice, context);

	//Bind the Vertex Array
	GLuint vao = mesh->GetVAO();
	glBindVertexArray(vao);

	renderDevice->SetCullFace(material->GetCullMode());
	renderDevice->SetDepthMask(material->ZWriteMode);
	renderDevice->SetDepthFunc(material->ZTestMode);

	//DrawElements
	if (mesh->GetIndices().size() > 0)
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

	//Unbind the Vertex Array
	glBindVertexArray(0);

}

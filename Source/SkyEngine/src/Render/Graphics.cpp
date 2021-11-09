#include "Render/Graphics.h"
#include "Render/Mesh.h"
#include "Render/RenderContext.h"
#include "Render/Material.h"
void Graphics::DrawMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* material, Camera* cam, RenderContext* context)
{
	context->modelMatrix = *modelMatrix;
	material->Bind(context);

	//Bind the Vertex Array
	GLuint vao = mesh->GetVAO();
	glBindVertexArray(vao);

	glCullFace(material->GetCullMode() == ECullMode::CM_Front ? GL_FRONT : GL_BACK);
	glDepthMask(material->ZWriteMode == EZWriteMode::WM_ON ? GL_TRUE : GL_FALSE);
	glDepthFunc(Graphics::GetDepthFunc(material->ZTestMode));

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

GLenum Graphics::GetDepthFunc(EZTestMode ztestMode)
{
	switch (ztestMode)
	{
	case EZTestMode::TM_ALWAYS:
		return GL_ALWAYS;
	case EZTestMode::TM_NEVER:
		return GL_NEVER;
	case EZTestMode::	TM_LESS:
		return GL_LESS;
	case EZTestMode::	TM_EQUAL:
		return GL_EQUAL;
	case EZTestMode::	TM_LEQUAL:
		return GL_LEQUAL;
	case EZTestMode::	TM_GREATER:
		return GL_GREATER;
	case EZTestMode::	TM_NOTEQUAL:
		return GL_NOTEQUAL;
	case EZTestMode::	TM_GEQUAL:
		return GL_GEQUAL;
	}
	return GL_LEQUAL;
}

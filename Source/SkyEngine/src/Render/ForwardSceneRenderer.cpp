#include "Render/ForwardSceneRenderer.h"
#include "SkyEngine.h"
#include "Render/RenderContext.h"
#include "Render/RenderTexture.h"
#include "Render/Graphics.h"

ForwardSceneRenderer::ForwardSceneRenderer()
{
	engine = NULL;
	renderContext = new RenderContext();
}

ForwardSceneRenderer::~ForwardSceneRenderer()
{

}

bool ForwardSceneRenderer::Initialize(int width, int height)
{
	renderContext->windowWidth = width;
	renderContext->windowHeight = height;
	return true;
}


void ForwardSceneRenderer::ResizeGL(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	//Reset View
	glViewport(0, 0, (GLint)width, (GLint)height);
	//Choose the Matrix mode
	glMatrixMode(GL_PROJECTION);
	//reset projection
	glLoadIdentity();
	//set perspection
	gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 1000.0);
	//choose Matrix mode
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();
}

void ForwardSceneRenderer::DrawMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* mat, Camera* cam)
{
	Graphics::DrawMesh(mesh, modelMatrix, mat, cam, renderContext);
}

void ForwardSceneRenderer::SetRenderTarget(RenderTexture* RT)
{
	mRenderTarget = RT;
}

void ForwardSceneRenderer::SetTileShadingInfo(unsigned int LightInfoBuffer,unsigned int VisibleLightsBuffer,int WorkGroupsX,int WorkGroupsY)
{
	renderContext->bEnableTileShading = true;
	renderContext->LightInfoBuffer = LightInfoBuffer;
	renderContext->VisibleLightsBuffer = VisibleLightsBuffer;
	renderContext->WorkGroupsX = WorkGroupsX;
	renderContext->WorkGroupsY = WorkGroupsY;
}


void ForwardSceneRenderer::Render(Scene* scene, Camera* camera)
{
	//glFrontFace(GL_CCW);
	if (mRenderTarget != NULL)
	{
		mRenderTarget->ActivateFB();
	}

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (scene != NULL)
	{
		renderContext->projectionMatrix = camera->projectionMaxtrix;
		renderContext->viewMatrix = camera->GetViewMatrix();
		renderContext->viewProjectionMatrix = camera->GetViewMatrix() * camera->GetProjectionMatrix();
		renderContext->viewPos = camera->transform.position;
		RenderGameObject(scene->GetRoot(), renderContext);
	}
}

void ForwardSceneRenderer::RenderGameObject(GameObject* gameObject, RenderContext* renderContext)
{
	//Get a RenderingComponent from gameObject and invoke render() on it
	MeshRenderer* rc = gameObject->GetComponent<MeshRenderer>();

	if (rc != 0)
	{
		Material* mat = rc->GetMaterial();
		if (mat != NULL && mat->GetShaderProgram()->HasUniform("brdfLUT"))
		{
			mat->AddTextureVariable("irradianceMap", renderContext->DiffuseCubeMap, ETextureVariableType::TV_CUBE, 5);
			mat->AddTextureVariable("prefilterMap", renderContext->SpecCubeMap, ETextureVariableType::TV_CUBE, 6);
			mat->AddTextureVariable("brdfLUT", renderContext->BrdfLut, ETextureVariableType::TV_2D, 7);
		}

		if (mReplaceShader != nullptr)
		{
			rc->Render(renderContext, mReplaceShader);
		}
		else
		{
			rc->Render(renderContext);
		}
	}

	//For all of gameObject's children, call render() on them
	uint32 numChildren = gameObject->GetNumChildren();
	for (uint32 i = 0; i < numChildren; i++)
	{
		RenderGameObject(gameObject->GetChild(i), renderContext);
	}
}

#include "Render/ForwardSceneRenderer.h"
#include "SkyEngine.h"
#include "Render/RenderContext.h"
#include "Render/RenderTexture.h"
#include "Render/Graphics.h"

ForwardSceneRenderer::ForwardSceneRenderer()
{
	engine = nullptr;
	renderContext = new RenderContext();
	mReplaceShader = nullptr;
}

ForwardSceneRenderer::~ForwardSceneRenderer()
{

}

bool ForwardSceneRenderer::Initialize(int width, int height)
{
	renderContext->windowWidth = width;
	renderContext->windowHeight = height;

	mRenderDevice = GetRenderDevice();
	return true;
}


void ForwardSceneRenderer::ResizeGL(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	mRenderDevice->ResizeWindow(width, height);
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

void ForwardSceneRenderer::SetClusterShadingInfo(unsigned int LightInfoBuffer, unsigned int VisibleLightsBuffer, int WorkGroupsX, int WorkGroupsY, int WorkGroupsZ)
{
	renderContext->bEnableClusterShading = true;
	renderContext->LightInfoBuffer = LightInfoBuffer;
	renderContext->VisibleLightsBuffer = VisibleLightsBuffer;
	renderContext->WorkGroupsX = WorkGroupsX;
	renderContext->WorkGroupsY = WorkGroupsY;
	renderContext->WorkGroupsZ = WorkGroupsZ;
}

void ForwardSceneRenderer::Render(Scene* scene, Camera* camera)
{
	//glFrontFace(GL_CCW);
	if (mRenderTarget != NULL)
	{
		mRenderDevice->BindFrameBuffer(mRenderTarget->GetFrameBufferID());
		mRenderDevice->SetViewPort(0, 0, mRenderTarget->GetWidth(), mRenderTarget->GetHeight());
	}
	else
	{
		mRenderDevice->BindFrameBuffer(0);
		mRenderDevice->SetViewPort(0, 0, renderContext->windowWidth, renderContext->windowHeight);
	}

	mRenderDevice->SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	mRenderDevice->Clear();

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

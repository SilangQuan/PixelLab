#include "../include/Render/RenderTexture.h"

bool RenderTexture::ms_useFiltering = true;

RenderTexture::RenderTexture() :
	mFboId(0),
	mColorBufferId(0),
	mDepthBufferId(0),
	mColorFormat(ColorType::RGB16F),
	mDepthFormat(DepthType::Depth24S8)
{
}

RenderTexture::~RenderTexture()
{
	RenderDevice* renderDevice = GetRenderDevice();
	renderDevice->DeleteRenderTexture(this);
}

bool RenderTexture::InitForDepthOnly(int inWidth, int inHeight, DepthType depth)
{
	RenderDevice* renderDevice = GetRenderDevice();

	return renderDevice->InitDepthOnlyRenderTexture(this, inWidth, inHeight, depth);
}

bool RenderTexture::Init(int inWidth, int inHeight, ColorType color, DepthType depth, int msaaSamples)
{
	RenderDevice* renderDevice = GetRenderDevice();
	return renderDevice->InitRenderTexture(this, inWidth, inHeight, color, depth, msaaSamples);
}


//Need to Check this function is ok.
void RenderTexture::SetDepth(unsigned int DepthBufferId)
{
	mDepthBufferId = DepthBufferId;

	glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBufferId);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

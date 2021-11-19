#pragma once

#include "../../SkyEngine/include/SkyEngine.h"

class RenderTexture : public Texture {
public:
	RenderTexture();
	~RenderTexture();
	bool Init(int width, int height, ColorType color, DepthType depth, int msaaSamples = 0);
	bool InitForDepthOnly(int inWidth, int inHeight, DepthType depth);

	void SetDepth(unsigned int DepthBufferId);

	void ActivateFB();
	//void DeactivateFB();
	void Bind() { glBindTexture(GL_TEXTURE_2D, mColorBufferId); }
	void Release() { glBindTexture(GL_TEXTURE_2D, 0); }
	static bool ms_useFiltering;

	GLuint GetTextureID() override { return mColorBufferId; }

	GLuint GetDepthID() { return mDepthBufferId; }

	void BindForRead() { glBindFramebuffer(GL_READ_FRAMEBUFFER, mFboId); }
	
	void BindForWrite() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFboId); }

private:
	unsigned int mColorFormat;
	unsigned int mDepthFormat;
	unsigned int m_type;
	unsigned int mFboId;

	unsigned int mDepthBufferId;
	unsigned int mColorBufferId;
};

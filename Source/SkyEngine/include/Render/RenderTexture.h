#pragma once

#include "../../SkyEngine/include/SkyEngine.h"

class RenderTexture : public Texture {
public:
	RenderTexture();
	~RenderTexture();
	bool Init(int width, int height, ColorType color, DepthType depth, int msaaSamples = 0);
	bool InitForDepthOnly(int inWidth, int inHeight, DepthType depth);

	void SetDepth(unsigned int DepthBufferId);

	void Bind() { glBindTexture(GL_TEXTURE_2D, mColorBufferId); }
	void Release() { glBindTexture(GL_TEXTURE_2D, 0); }
	static bool ms_useFiltering;

	GLuint GetTextureID() override { return mColorBufferId; }

	GLuint GetDepthID() { return mDepthBufferId; }
	uint32 GetFrameBufferID() { return mFboId; }

	void BindForRead() { glBindFramebuffer(GL_READ_FRAMEBUFFER, mFboId); }
	
	void BindForWrite() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFboId); }

	ColorType GetColorFormat() { return mColorFormat; }
	DepthType GetDepthFormat() { return mDepthFormat; }

private:
	ColorType mColorFormat;
	DepthType mDepthFormat;
	unsigned int m_type;
	unsigned int mFboId;

	unsigned int mDepthBufferId;
	unsigned int mColorBufferId;

	friend class RenderDeviceGL;
};

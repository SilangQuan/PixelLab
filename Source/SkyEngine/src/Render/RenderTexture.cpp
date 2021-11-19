#include "../include/Render/RenderTexture.h"


#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))


// helper macro that checks for GL errors.
#define GL_C(stmt) do {					\
	stmt;						\
	CheckOpenGLError(#stmt, __FILE__, __LINE__);	\
    } while (0)

inline void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("OpenGL error %08x, at %s:%i - for %s.\n", err, fname, line, stmt);
		//exit(1);
	}
}

bool RenderTexture::ms_useFiltering = true;

RenderTexture::RenderTexture() :
	mFboId(0),
	mColorBufferId(0),
	mDepthBufferId(0),
	mColorFormat(0),
	mDepthFormat(0)
{
}

RenderTexture::~RenderTexture()
{
	if (mColorBufferId) {
		glDeleteTextures(1, &mColorBufferId);
		mColorBufferId = 0;
	}

	if (mDepthBufferId) {
		glDeleteRenderbuffers(1, &mDepthBufferId);
		mDepthBufferId = 0;
	}
	if (mFboId) {
		glDeleteFramebuffers(1, &mFboId);
		mFboId = 0;
	}
}

bool RenderTexture::InitForDepthOnly(int inWidth, int inHeight, DepthType depth)
{
	GLenum err;
	mWidth = inWidth;
	mHeight = inHeight;
	glGenFramebuffers(1, &mFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
	err = glGetError();

	glGenTextures(1, &mDepthBufferId);
	glBindTexture(GL_TEXTURE_2D, mDepthBufferId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, inWidth, inHeight, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		mDepthBufferId, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Could not validate framebuffer" << std::endl;
		return false;
	}
	return true;
}

bool RenderTexture::Init(int inWidth, int inHeight, ColorType color, DepthType depth, int msaaSamples)
{
	GLenum err;
	mWidth = inWidth;
	mHeight = inHeight;
	glGenFramebuffers(1, &mFboId);
	glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
	err = glGetError();

	if (color >= RGB16F) {
		// Create the HDR render target
		switch (color) {
		case RGB16F: mColorFormat = GL_RGB16F;   break;
		case RGBA16F: mColorFormat = GL_RGBA16F; break;
		case R11G11B10: mColorFormat = GL_R11F_G11F_B10F; break;

		default:  mColorFormat = GL_R11F_G11F_B10F;break;
			break;
		}
	}
	else {
		// Create the LDR render target
		switch (color) {
		case RGB565: mColorFormat = GL_RGB565; break;
		case RGBA8888: mColorFormat = GL_RGBA; break;
		default:
		case RGB888: mColorFormat = GL_RGB; break;
		}
	}

	if (depth != NoDepth) {
		switch (depth) {
		case Depth32: mDepthFormat = GL_DEPTH_COMPONENT32; break;
		case Depth24S8: mDepthFormat = GL_DEPTH24_STENCIL8; break;
		default:
		case Depth16: mDepthFormat = GL_DEPTH_COMPONENT16; break;
		}
	}

	glGenTextures(1, &mColorBufferId);
	if (msaaSamples == 0)
	{
		glBindTexture(GL_TEXTURE_2D, mColorBufferId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ms_useFiltering ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ms_useFiltering ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexStorage2D(GL_TEXTURE_2D, 1, mColorFormat, mWidth, mHeight);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorBufferId, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mColorBufferId);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, ms_useFiltering ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, ms_useFiltering ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaSamples, mColorFormat, mWidth, mHeight, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mColorBufferId, 0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}


	// Create depth buffer
	if (depth != NoDepth) {
		glGenRenderbuffers(1, &mDepthBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, mDepthBufferId);

		if (msaaSamples == 0)
		{
			glRenderbufferStorage(GL_RENDERBUFFER, mDepthFormat, mWidth, mHeight);
		}
		else
		{
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, mDepthFormat, mWidth, mHeight);
		}
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthBufferId);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return false;
	}
	return true;
}


void RenderTexture::ActivateFB()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
	glViewport(0, 0, mWidth, mHeight);
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

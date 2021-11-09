#include "../include/Render/GBuffer.h"
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


GBuffer::GBuffer()
{
	mFbo = 0;
	mColorTexture = 0;
	mDepthBuffer = 0;
	mNormalTexture = 0;
	mPositionTexture = 0;
}

GBuffer::~GBuffer()
{
	if (mFbo != 0) {
		glDeleteFramebuffers(1, &mFbo);
	}

	//if (mTextures[0] != 0) {
	//	glDeleteTextures(ARRAY_SIZE_IN_ELEMENTS(mTextures), mTextures);
	//}
	//
	//if (mDepthTexture != 0) {
	//	glDeleteTextures(1, &mDepthTexture);
	//}
	//
	//if (mFinalTexture != 0) {
	//	glDeleteTextures(1, &mFinalTexture);
	//}
}

bool GBuffer::Init(unsigned int width, unsigned int height)
{
	GLenum err = glGetError();
	// create the gbuffer. first create fbo:
	glGenFramebuffers(1, &mFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
	err = glGetError();
	// RGBA8 color texture-p
	glGenTextures(1, &mColorTexture);
	glBindTexture(GL_TEXTURE_2D, mColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, mColorTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	err = glGetError();
	//  RGBA16F normal texture.
	glGenTextures(1, &mNormalTexture);
	glBindTexture(GL_TEXTURE_2D, mNormalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D, mNormalTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	err = glGetError();
	//  RGBA16F position texture.
	glGenTextures(1, &mPositionTexture);
	glBindTexture(GL_TEXTURE_2D, mPositionTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,GL_TEXTURE_2D, mPositionTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	err = glGetError();
	// we need a z-buffer for the gbuffer. but we don't need to read from it.
	// so instead create a renderbuffer.
	glGenRenderbuffers(1, &mDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);
	err = glGetError();
	// specify that we can render to all three attachments.
	// this is very important! It won't work otherwise.
	GLenum tgts[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, tgts);
	err = glGetError();
	// make sure nothing went wrong:
	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer not complete. Status: %d", status);
		exit(1);
	}
	GL_C(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return true;
}


// configure a shader for usage in deferred rendering.
void GBuffer::SetupDeferredShader(GLuint shader, Vector3 cameraPos) 
{
	// bind gbuffer textures.
	GL_C(glUniform1i(glGetUniformLocation(shader, "uColorTex"), 0));
	GL_C(glActiveTexture(GL_TEXTURE0 + 0));
	GL_C(glBindTexture(GL_TEXTURE_2D, mColorTexture));

	GL_C(glUniform1i(glGetUniformLocation(shader, "uNormalTex"), 1));
	GL_C(glActiveTexture(GL_TEXTURE0 + 1));
	GL_C(glBindTexture(GL_TEXTURE_2D, mNormalTexture));

	GL_C(glUniform1i(glGetUniformLocation(shader, "uPositionTex"), 2));
	GL_C(glActiveTexture(GL_TEXTURE0 + 2));
	GL_C(glBindTexture(GL_TEXTURE_2D, mPositionTexture));

	GL_C(glUniform1i(glGetUniformLocation(shader, "uDepthTex"), 3));
	GL_C(glActiveTexture(GL_TEXTURE0 + 3));
	GL_C(glBindTexture(GL_TEXTURE_2D, mDepthBuffer));

	GL_C(glUniform3f(glGetUniformLocation(shader, "uCameraPos"), cameraPos.x, cameraPos.y, cameraPos.z));
}


void GBuffer::StartFrame()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFbo);
	//glDrawBuffer(GL_COLOR_ATTACHMENT4);
	//glClear(GL_COLOR_BUFFER_BIT);
}


void GBuffer::BindForGeomPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFbo);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
							 GL_COLOR_ATTACHMENT1,
							 GL_COLOR_ATTACHMENT2 };

	glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);
}


void GBuffer::BindForStencilPass()
{
	// must disable the draw buffers 
	glDrawBuffer(GL_NONE);
}



void GBuffer::BindForLightPass()
{
	//glDrawBuffer(GL_COLOR_ATTACHMENT4);
	//
	//for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(mTextures); i++) {
	//	glActiveTexture(GL_TEXTURE0 + i);
	//	glBindTexture(GL_TEXTURE_2D, mTextures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
	//}
}


void GBuffer::BindForFinalPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mFbo);
	glReadBuffer(GL_COLOR_ATTACHMENT4);
}

void GBuffer::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFbo);
}


void GBuffer::BindForReading()
{
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//
	//for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(mTextures); i++) {
	//	glActiveTexture(GL_TEXTURE0 + i);
	//	glBindTexture(GL_TEXTURE_2D, mTextures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
	//}
}

#pragma once
#include "../../SkyEngine/include/SkyEngine.h"

class GBuffer
{
public:

	enum GBUFFER_TEXTURE_TYPE {
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer();

	~GBuffer();

	bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

	void StartFrame();
	void BindForGeomPass();
	void BindForStencilPass();
	void BindForLightPass();
	void BindForFinalPass();

	void BindForWriting();

	void BindForReading();

	void SetupDeferredShader(GLuint shader, Vector3 cameraPos);
	GLuint GetFBO() { return mFbo; };
private:

	GLuint mFbo;
	//GLuint mTextures[GBUFFER_NUM_TEXTURES];
	//GLuint mDepthTexture;
	//GLuint mFinalTexture;

	// these four textures are the gbuffer.
	GLuint mColorTexture;
	GLuint mDepthBuffer;
	GLuint mNormalTexture;
	GLuint mPositionTexture;

};


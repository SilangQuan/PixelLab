#pragma once
#include "Render/RenderTexture.h"

class ShaderProgram;

enum BUFFER_PYRAMID {
    LEVEL_0 = 0,
    LEVEL_1,
    LEVEL_2,
    LEVEL_3,
    LEVEL_4,
    //LEVEL_5,
    LEVEL_TOTAL,
};


struct PostProcessingInputsForward
{
	RenderTexture* SceneColorTex;
    RenderTexture* DepthTex;
	GLuint BackBufferFBO;
    int Width;
    int Height;
    
    //BloomParams
    bool BloomActive;
    float BloomThreshold;
    float BloomIntensity;

    //UberPost
    float Exposure;

    bool EnableForwardPlusDebug;
    int WorkGroupX;
    int WorkGroupY;
};

class PostProcessor
{
public:
    PostProcessor();
    void AddPostProcessingPasses(const PostProcessingInputsForward& Inputs);
    void InitRenderData();
    void AddPass(ShaderProgram* prog, RenderTexture* target);
    void BlitToBackBuffer(RenderTexture* rt, GLuint backBufferFBO, int width, int height);
    void DrawQuard();

private:
    ShaderProgram* mToneMappingShader;

    ShaderProgram* mBloomThresholdShader;
    ShaderProgram* mGaussianBlurShader;
    ShaderProgram* mBloomMergeShader;
    ShaderProgram* mDownsampleShader;

    ShaderProgram* mSimpleBlitShader;

    ShaderProgram* mForwardPlusDebugShader;

    Texture mBlackDummy;
    Texture mWhiteDummy;
    unsigned int VAO;
};


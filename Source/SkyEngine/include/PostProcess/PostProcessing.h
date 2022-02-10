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
public:
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

    bool EnableForwardPlusDebug = false;
    int WorkGroupX;
    int WorkGroupY;
    int WorkGroupZ;

    bool bEnableClusterShadingDebug;

    GLuint SSBOVisibleLight;

    bool EnableDepthDebug = false;

    ViewInfo MainViewInfo;

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
    ShaderProgram* mDepthDebugShader;

    Texture mBlackDummy;
    Texture mWhiteDummy;
    unsigned int VAO;
};



#include "PostProcess/PostProcessing.h"
#include "Render/ShaderProgram.h"

PostProcessor::PostProcessor():
    mBlackDummy("../../BuiltinAssets/texture/BlackDummy.png"),
    mWhiteDummy("../../BuiltinAssets/texture/WhiteDummy.png")
{

}

void PostProcessor::InitRenderData()
{
	mToneMappingShader = new ShaderProgram("../../BuiltinAssets/shader/Tonemap.vert", "../../BuiltinAssets/shader/Tonemap.frag");
    mBloomThresholdShader = new ShaderProgram("../../BuiltinAssets/shader/BloomThreshold.vert", "../../BuiltinAssets/shader/BloomThreshold.frag");
    mGaussianBlurShader = new ShaderProgram("../../BuiltinAssets/shader/GaussianBlur.vert", "../../BuiltinAssets/shader/GaussianBlur.frag");
    mBloomMergeShader = new ShaderProgram("../../BuiltinAssets/shader/BloomMerge.vert", "../../BuiltinAssets/shader/BloomMerge.frag");
    mDownsampleShader = new ShaderProgram("../../BuiltinAssets/shader/Downsample.vert", "../../BuiltinAssets/shader/Downsample.frag");
    mSimpleBlitShader = new ShaderProgram("../../BuiltinAssets/shader/Tonemap.vert", "../../BuiltinAssets/shader/SimpleBlit.frag");
        
    mForwardPlusDebugShader = new ShaderProgram("../../BuiltinAssets/shader/ScreenQuad.vert", "../../BuiltinAssets/shader/ForwardPlusDebug.frag");
   
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos        // tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}


void PostProcessor::AddPass(ShaderProgram* prog, RenderTexture* target)
{
    target->ActivateFB();
    prog->Use();

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog->Bind();

    DrawQuard();
}

void PostProcessor::AddPostProcessingPasses(const PostProcessingInputsForward& Inputs)
{
    if (Inputs.EnableForwardPlusDebug)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, Inputs.BackBufferFBO);
        glViewport(0, 0, Inputs.Width, Inputs.Height);

        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_NONE);

        mForwardPlusDebugShader->Use();

        glUniform1i(glGetUniformLocation(mForwardPlusDebugShader->GetProgramID(), "workgroup_x"), Inputs.WorkGroupX);
        glUniform1i(glGetUniformLocation(mForwardPlusDebugShader->GetProgramID(), "workgroup_y"), Inputs.WorkGroupY);

        DrawQuard();
        return;
    }


    RenderTexture bloomThreshold, bloomResult;

    if (Inputs.BloomActive)
    {
        bloomResult.Init(Inputs.Width, Inputs.Height, ColorType::RGBA16F, DepthType::NoDepth);

        //Bloom Threshold pass
        bloomThreshold.Init(Inputs.Width, Inputs.Height, ColorType::RGBA16F, DepthType::NoDepth);

        TextureVariable sceneColorRT(Inputs.SceneColorTex, 0, "sceneColorRT", TV_2D);

        mBloomThresholdShader->SetUniform("sceneColorRT", sceneColorRT);
        mBloomThresholdShader->SetUniform("bloomThreshold", Inputs.BloomThreshold);
        mBloomThresholdShader->SetUniform("bloomIntensity", Inputs.BloomIntensity);

        AddPass(mBloomThresholdShader, &bloomThreshold);

        //DownSample passes
        PushGroupMarker("Bloom - DownSample");
        RenderTexture downsampledBuffer[LEVEL_TOTAL];
        RenderTexture bluredBuffer[LEVEL_TOTAL];
        RenderTexture blurTempBuffer[LEVEL_TOTAL];

        for (int i = 0; i < LEVEL_TOTAL; i++)
        {
            float ratio = 1.0f / pow(2, i + 1);
            int targetWidth = Inputs.Width * ratio;
            int targetHeight = Inputs.Height * ratio;
            downsampledBuffer[i].Init(Inputs.Width * ratio, Inputs.Height * ratio, ColorType::RGBA16F, DepthType::NoDepth);
            if (i == 0)
            {
                TextureVariable bloomThreshold(&bloomThreshold, 0, "bloomThreshold", TV_2D);
                mDownsampleShader->SetUniform("sourceRT", bloomThreshold);
            }
            else
            {
                TextureVariable downsampledBuffer(&downsampledBuffer[i - 1], 0, "downsampledBuffer", TV_2D);
                mDownsampleShader->SetUniform("sourceRT", downsampledBuffer);
            }
            mDownsampleShader->SetUniform("texelSize", Vector2(1.0f / targetWidth, 1.0f / targetHeight));
            AddPass(mDownsampleShader, downsampledBuffer + i);
        }
        PopGroupMarker();


        PushGroupMarker("Bloom - Gassian");
        //Gassian Blur
        for (int i = 0; i < LEVEL_TOTAL; i++)
        {
            float ratio = 1.0f / pow(2, i + 1);
            int targetWidth = Inputs.Width * ratio;
            int targetHeight = Inputs.Height * ratio;
            blurTempBuffer[i].Init(Inputs.Width * ratio, Inputs.Height * ratio, ColorType::RGBA16F, DepthType::NoDepth);

            TextureVariable downsampledBuffer(&downsampledBuffer[i], 0, "downsampledBuffer", TV_2D);
            mGaussianBlurShader->SetUniform("sourceRT", downsampledBuffer);
            mGaussianBlurShader->SetUniform("texelSize", Vector2(1.0f / targetWidth, 1.0f / targetHeight));
            mGaussianBlurShader->SetUniform("isVertical", 0.0f);

            AddPass(mGaussianBlurShader, blurTempBuffer + i);

            bluredBuffer[i].Init(Inputs.Width * ratio, Inputs.Height * ratio, ColorType::RGBA16F, DepthType::NoDepth);

            TextureVariable tmpBuffer(&blurTempBuffer[i], 0, "tmpBuffer", TV_2D);
            mGaussianBlurShader->SetUniform("sourceRT", tmpBuffer);
            mGaussianBlurShader->SetUniform("isVertical", 1.0f);
            AddPass(mGaussianBlurShader, bluredBuffer + i);
        }
        PopGroupMarker();

        //BloomMerge
        TextureVariable bluredBuffer0(&bluredBuffer[0], 0, "bloomBuffer0", TV_2D);
        TextureVariable bluredBuffer1(&bluredBuffer[1], 1, "bloomBuffer1", TV_2D);
        TextureVariable bluredBuffer2(&bluredBuffer[2], 2, "bloomBuffer2", TV_2D);
        TextureVariable bluredBuffer3(&bluredBuffer[3], 3, "bloomBuffer3", TV_2D);
        TextureVariable bluredBuffer4(&bluredBuffer[4], 4, "bloomBuffer4", TV_2D);
        mBloomMergeShader->SetUniform("bloomBuffer0", bluredBuffer0);
        mBloomMergeShader->SetUniform("bloomBuffer1", bluredBuffer1);
        mBloomMergeShader->SetUniform("bloomBuffer2", bluredBuffer2);
        mBloomMergeShader->SetUniform("bloomBuffer3", bluredBuffer3);
        mBloomMergeShader->SetUniform("bloomBuffer4", bluredBuffer4);

        AddPass(mBloomMergeShader, &bloomResult);
    }

    //Uber Post
    //TextureVariable sceneColorTex(Inputs.SceneColorTex, 0, "hdrBuffer", TV_2D);
    //mToneMappingShader->SetUniform("exposure", Inputs.Exposure);
    //mToneMappingShader->SetUniform("hdrBuffer", sceneColorTex);
    //AddPass(mToneMappingShader, &(Inputs.BackBufferFBO));

	glBindFramebuffer(GL_FRAMEBUFFER, Inputs.BackBufferFBO);
	glViewport(0, 0, Inputs.Width, Inputs.Height);

    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    mToneMappingShader->Use();

    glUniform1i(glGetUniformLocation(mToneMappingShader->GetProgramID(), "hdrBuffer"), 0);
    glUniform1i(glGetUniformLocation(mToneMappingShader->GetProgramID(), "bloomBuffer"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Inputs.SceneColorTex->GetTextureID());

    glActiveTexture(GL_TEXTURE1);
    if (Inputs.BloomActive)
    {
        glBindTexture(GL_TEXTURE_2D, bloomResult.GetTextureID());
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, mBlackDummy.GetTextureID());
    }

	glUniform1f(glGetUniformLocation(mToneMappingShader->GetProgramID(), "exposure"), Inputs.Exposure);

    DrawQuard();
}

void PostProcessor::BlitToBackBuffer(RenderTexture* rt, GLuint backBufferFBO, int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, backBufferFBO);
    glViewport(0, 0, width, height);

    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    mSimpleBlitShader->Use();
    glUniform1i(glGetUniformLocation(mSimpleBlitShader->GetProgramID(), "sourceRT"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rt->GetTextureID());

    DrawQuard();
}

void PostProcessor::DrawQuard()
{
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

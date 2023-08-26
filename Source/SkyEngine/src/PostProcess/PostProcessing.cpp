#include "PostProcess/PostProcessing.h"
#include "Render/ShaderProgram.h"
#include "Render/RenderDevice.h"

PostProcessor::PostProcessor()
{
    mWhiteDummy = ResourceManager::GetInstance()->FindTexture("WhiteDummy");
    mBlackDummy = ResourceManager::GetInstance()->FindTexture("BlackDummy");

    mRenderDevice = GetRenderDevice();
}

void PostProcessor::InitRenderData()
{
    mToneMappingShader = ResourceManager::GetInstance()->LoadShader("Tonemap");
    mBloomThresholdShader = ResourceManager::GetInstance()->LoadShader("BloomThreshold");
    mGaussianBlurShader = ResourceManager::GetInstance()->LoadShader("GaussianBlur");
    mBloomMergeShader = ResourceManager::GetInstance()->LoadShader("BloomMerge");
    mDownsampleShader = ResourceManager::GetInstance()->LoadShader("Downsample");
    mSimpleBlitShader = ResourceManager::GetInstance()->LoadShader("Tonemap", "SimpleBlit");

    mForwardPlusDebugShader = ResourceManager::GetInstance()->LoadShader("ScreenQuad", "ForwardPlusDebug");
    mDepthDebugShader = ResourceManager::GetInstance()->LoadShader("ScreenQuad", "DepthDebug");

    this->VAO = mRenderDevice->GenFullscreenQuadVAO();
}

void PostProcessor::AddPass(ShaderProgram* prog, uint32 fboID, int x, int y, int width, int height)
{
    mRenderDevice->BindFrameBuffer(fboID);
    mRenderDevice->SetViewPort(x, y, width, height);
    mRenderDevice->UseGPUProgram(prog);

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    mRenderDevice->SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    mRenderDevice->Clear();

    prog->Bind(mRenderDevice);

    DrawQuard();
}

void PostProcessor::AddPass(ShaderProgram* prog, RenderTexture* target)
{
    mRenderDevice->BindFrameBuffer(target->GetFrameBufferID());
    mRenderDevice->SetViewPort(0, 0, target->GetWidth(), target->GetHeight());
    mRenderDevice->UseGPUProgram(prog);

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    
    mRenderDevice->SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    mRenderDevice->Clear();

    prog->Bind(mRenderDevice);

    DrawQuard();
}


void PostProcessor::AddPostProcessingPasses(const PostProcessingInputsForward& Inputs)
{
    if (Inputs.EnableForwardPlusDebug)
    {
        mRenderDevice->BindFrameBuffer(Inputs.BackBufferFBO);
        mRenderDevice->SetViewPort(0, 0, Inputs.Width, Inputs.Height);

        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_NONE);

        mRenderDevice->UseGPUProgram(mForwardPlusDebugShader);

        glUniform1i(glGetUniformLocation(mForwardPlusDebugShader->GetProgramID(), "workgroup_x"), Inputs.WorkGroupX);
        glUniform1i(glGetUniformLocation(mForwardPlusDebugShader->GetProgramID(), "workgroup_y"), Inputs.WorkGroupY);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, Inputs.SSBOVisibleLight);

        DrawQuard();
        return;
    }
    else if (Inputs.EnableDepthDebug)
    {
        mRenderDevice->BindFrameBuffer(Inputs.BackBufferFBO);
        mRenderDevice->SetViewPort(0, 0, Inputs.Width, Inputs.Height);

        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_NONE);

        mRenderDevice->UseGPUProgram(mDepthDebugShader);


        glUniform1i(glGetUniformLocation(mDepthDebugShader->GetProgramID(), "DepthRT"), 0);

        glActiveTexture(GL_TEXTURE0);
       // glBindTexture(GL_TEXTURE_2D, Inputs.DepthTex->GetDepthID());
        glBindTexture(GL_TEXTURE_2D, Inputs.DepthTex->GetDepthID());

        glUniformMatrix4fv(glGetUniformLocation(mDepthDebugShader->GetProgramID(), "ViewInfo.Projection"), 1, GL_FALSE, Inputs.MainViewInfo.Projection.get());
        glUniformMatrix4fv(glGetUniformLocation(mDepthDebugShader->GetProgramID(), "ViewInfo.View"), 1, GL_FALSE, Inputs.MainViewInfo.View.get());
        glUniformMatrix4fv(glGetUniformLocation(mDepthDebugShader->GetProgramID(), "ViewInfo.InvProjection"), 1, GL_FALSE, Inputs.MainViewInfo.InvProject.get());
        glUniformMatrix4fv(glGetUniformLocation(mDepthDebugShader->GetProgramID(), "ViewInfo.ViewProjection"), 1, GL_FALSE, Inputs.MainViewInfo.ViewPorject.get());
        glUniform4f(glGetUniformLocation(mDepthDebugShader->GetProgramID(), "ViewInfo.ScreenSizeAndInv"), Inputs.MainViewInfo.ScreenSizeAndInv.x, Inputs.MainViewInfo.ScreenSizeAndInv.y, Inputs.MainViewInfo.ScreenSizeAndInv.z, Inputs.MainViewInfo.ScreenSizeAndInv.w);
        glUniform4f(glGetUniformLocation(mDepthDebugShader->GetProgramID(), "ViewInfo.ZParams"), Inputs.MainViewInfo.ZParams.x, Inputs.MainViewInfo.ZParams.y, Inputs.MainViewInfo.ZParams.z, Inputs.MainViewInfo.ZParams.w);

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

        //mDownsampleShader->SetUniform("sourceRT", &bloomThresholdVariable);

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
                TextureVariable bloomThresholdVariable(&bloomThreshold, 0, "bloomThreshold", TV_2D);
                mDownsampleShader->SetUniform("sourceRT", &bloomThresholdVariable);
            }
            else
            {
                TextureVariable downsampledBufferVariable(&downsampledBuffer[i - 1], 0, "downsampledBuffer", TV_2D);
                mDownsampleShader->SetUniform("sourceRT", &downsampledBufferVariable);
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
    mRenderDevice->UseGPUProgram(mToneMappingShader);
    TextureVariable sceneColor(Inputs.SceneColorTex, 0, "sceneColor", TV_2D);
    mToneMappingShader->SetUniform("hdrBuffer", sceneColor);
    TextureVariable bloomTex(Inputs.BloomActive ? &bloomResult: mBlackDummy, 1, "bloomResult", TV_2D);
    mToneMappingShader->SetUniform("bloomBuffer", bloomTex);
    mToneMappingShader->SetUniform("exposure", Inputs.Exposure);
    
    mToneMappingShader->Bind(mRenderDevice);

    AddPass(mToneMappingShader, Inputs.BackBufferFBO, 0, 0, Inputs.Width, Inputs.Height);

    //Reset render state
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
}

void PostProcessor::BlitToBackBuffer(RenderTexture* rt, GLuint backBufferFBO, int width, int height)
{
    TextureVariable source(rt, 0, "sourceRT", TV_2D);
    mSimpleBlitShader->SetUniform("sourceRT", source);
    mSimpleBlitShader->Bind(mRenderDevice);

    AddPass(mToneMappingShader, backBufferFBO, 0, 0, width, height);

    /*
    mRenderDevice->BindFrameBuffer(backBufferFBO);
    mRenderDevice->SetViewPort(0, 0, width, height);

    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_NONE);

    mRenderDevice->UseGPUProgram(mSimpleBlitShader);

    glUniform1i(glGetUniformLocation(mSimpleBlitShader->GetProgramID(), "sourceRT"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rt->GetTextureID());

    DrawQuard();
    glCullFace(GL_BACK);
    */
}

void PostProcessor::DrawQuard()
{
    mRenderDevice->BindVAO(VAO);
    mRenderDevice->DrawArrays(6);
    mRenderDevice->BindVAO(0);
}

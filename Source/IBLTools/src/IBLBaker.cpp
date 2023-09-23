
#include "Render/ShaderProgram.h"
#include "Render/RenderTexture.h"
#include "../../LibAppFramework/include/application.h"
#include "IBLBaker.h"

#pragma once


GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 



const int NumSidesInCube = 6;
const int ConvolusionMapSize = 64;

const unsigned int BoxVerticesCount = 36;
const float BoxVerticesData[108] = 
{
    -1.0,	-1.0,	1.0,
    1.0,	-1.0,	1.0,
    -1.0,	1.0,	1.0,
    1.0,	1.0,	1.0,
    -1.0,	1.0,	1.0,
    1.0,	-1.0,	1.0,
    1.0,	-1.0,	1.0,
    -1.0,	-1.0,	1.0,
    1.0,	-1.0,	-1.0,
    -1.0,	-1.0,	-1.0,
    1.0,	-1.0,	-1.0,
    -1.0,	-1.0,	1.0,
    1.0,	1.0,	1.0,
    1.0,	-1.0,	1.0,
    1.0,	1.0,	-1.0,
    1.0,	-1.0,	-1.0,
    1.0,	1.0,	-1.0,
    1.0,	-1.0,	1.0,
    -1.0,	1.0,	1.0,
    1.0,	1.0,	1.0,
    -1.0,	1.0,	-1.0,
    1.0,	1.0,	-1.0,
    -1.0,	1.0,	-1.0,
    1.0,	1.0,	1.0,
    -1.0,	-1.0,	1.0,
    -1.0,	1.0,	1.0,
    -1.0,	-1.0,	-1.0,
    -1.0,	1.0,	-1.0,
    -1.0,	-1.0,	-1.0,
    -1.0,	1.0,	1.0,
    -1.0,	-1.0,	-1.0,
    -1.0,	1.0,	-1.0,
    1.0,	-1.0,	-1.0,
    1.0,	1.0,	-1.0,
    1.0,	-1.0,	-1.0,
    -1.0,	1.0,	-1.0, 
};


typedef struct
{
    u_char head[12];
    u_short dx /* Width */, dy /* Height */, head2;
    u_char pic[768 * 1024 * 10][3];
} typetga;
typetga tga;

char captureName[256];
u_long captureNo;
u_char tgahead[12] = { 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

class ReflectionCaptureData
{
public:
    int32 CubemapSize;
   //float AverageBrightness;
    float Brightness;
    //float MaxValueRGBM;

    float* PrefilterFilterHDRData;
    float* ConvolusionHDRData;


    ReflectionCaptureData() :
        CubemapSize(0),
       // AverageBrightness(1.0f),
        Brightness(1.0f),
        PrefilterFilterHDRData(nullptr),
        ConvolusionHDRData(nullptr)
    {}

};

int GetMipNum(int size)
{
    int maxMip = 4096;
    for (int i = 0;i < maxMip; i++)
    {
        if ((1 << i) >= size)
            return i;
    }
}


int GetCubemapPixelCount(int size, bool enableMips) 
{
    int numMipmaps = 0;
    if(enableMips)
    {
        numMipmaps = 1 + floor(log2(size));
    }

    int pixelCount = 0;
    int faceSize = size;
    for (int i = 0; i <= numMipmaps; i++) {
        pixelCount += faceSize * faceSize * NumSidesInCube;
        faceSize /= 2;
    }
    return pixelCount;
}


void ExportCubeData(TextureCubemap* texCube, float** destDataPtr)
{
    int rtWidth = texCube->GetWidth();
    int rtHeight = texCube->GetHeight();

    bool bFloat = false;
    int pixelComponent = 4;
    switch (texCube->GetColorType())
    {
    case ColorType::RGB16F:
        pixelComponent = 3;
        bFloat = true;
        break;
    case ColorType::RGBA16F:
        pixelComponent = 4;
        bFloat = true;
        break;
    case ColorType::RGBA8888:
        pixelComponent = 4;
        bFloat = false;
        break;
    }

   
    
    int32 CaptureDataSize = 0;
    int NumMips = 0;

    if (texCube->HasMips())
    {
        NumMips = GetMipNum(rtWidth);
    }
    int numPixels = GetCubemapPixelCount(texCube->GetWidth(), texCube->HasMips());
    
    CaptureDataSize = pixelComponent * numPixels * (bFloat?sizeof(float):sizeof(int));

    *destDataPtr = (float*)malloc(CaptureDataSize);
    std::memset(*destDataPtr, 0, CaptureDataSize);
    int32 MipBaseIndex = 0;

    // 创建一个帧缓冲对象（FBO）
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // 创建一个渲染缓冲对象（RBO）作为深度附件
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, rtWidth, rtWidth);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete." << std::endl;
    }

    if (texCube->HasMips())
    {
        for (int32 MipIndex = 0; MipIndex <= NumMips; MipIndex++)
        {
            const int32 MipSize = 1 << (NumMips - MipIndex);
            const int32 NumFacePixels =  MipSize * MipSize;
            const int32 CubeFaceBytes = pixelComponent * NumFacePixels * sizeof(float);
            for (unsigned int CubeFace = 0; CubeFace < NumSidesInCube; ++CubeFace)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + CubeFace,
                    texCube->GetTextureID(), MipIndex);

                if (bFloat)
                {
                    float* FloatBGRAData = (float*)malloc(CubeFaceBytes);

                    glCheckError();
                    glPixelStorei(GL_PACK_ALIGNMENT, 1);
                    glReadPixels(0, 0, MipSize, MipSize, GL_RGB, GL_FLOAT, FloatBGRAData);

                    glCheckError();
                    const int32 DestIndex = MipBaseIndex + pixelComponent * CubeFace * NumFacePixels;

                    float* FaceData = &((*destDataPtr)[DestIndex]);
                    std::memcpy(FaceData, FloatBGRAData, CubeFaceBytes);
                    free(FloatBGRAData);
                }
                else
                {
                    int32 PixelComponentCount = 3 * rtWidth * rtHeight;
                    int32 BGRADataSize = sizeof(uint8) * PixelComponentCount;

                    float* OutData = (float*)malloc(BGRADataSize);
                    glPixelStorei(GL_PACK_ALIGNMENT, 1);

                    // It's a simple int format. OpenGL converts them internally to what we want.
                    glReadPixels(0, 0, rtWidth, rtHeight, GL_RGB, GL_UNSIGNED_BYTE, OutData);
                }
            }
            MipBaseIndex += pixelComponent * NumFacePixels * NumSidesInCube;
        }
    }
    else
    {
        const int32 NumFacePixels = rtWidth * rtWidth;
        const int32 CubeFaceBytes = pixelComponent * NumFacePixels * sizeof(float);

        for (unsigned int CubeFace = 0; CubeFace < NumSidesInCube; ++CubeFace)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + CubeFace,
                texCube->GetTextureID(), 0);

            if (bFloat)
            {
                float* FloatBGRAData = (float*)malloc(CubeFaceBytes);

                glCheckError();
                glPixelStorei(GL_PACK_ALIGNMENT, 1);
                glReadPixels(0, 0, rtWidth, rtWidth, GL_RGB, GL_FLOAT, FloatBGRAData);

                glCheckError();
                const int32 DestIndex =  pixelComponent * CubeFace * NumFacePixels;

                float* FaceData = &((*destDataPtr)[DestIndex]);
                std::memcpy(FaceData, FloatBGRAData, CubeFaceBytes);
                free(FloatBGRAData);
            }
            else
            {
                int32 PixelComponentCount = pixelComponent * rtWidth * rtHeight;
                int32 BGRADataSize = sizeof(uint8) * PixelComponentCount;

                float* OutData = (float*)malloc(BGRADataSize);
                glPixelStorei(GL_PACK_ALIGNMENT, 1);

                // It's a simple int format. OpenGL converts them internally to what we want.
                glReadPixels(0, 0, rtWidth, rtHeight, GL_RGBA, GL_UNSIGNED_BYTE, OutData);
            }
        }
    }
    
}
void InitGL()
{
    CreationFlags creationFlags;
    int32 width = 640;
    int32 height = 360;

    creationFlags.width = width;
    creationFlags.height = height;
    creationFlags.title = "IBL Baker";
    creationFlags.isWindowed = true;
    creationFlags.renderPath = RenderingPath::Forward;

    SdlWindow* window = new SdlWindow(creationFlags.title, creationFlags.width, creationFlags.height, creationFlags.graphicsAPI);

    CreateClientDevice(creationFlags.graphicsAPI, RenderThreadMode::ThreadModeDirect, window);
}


TextureCubemap* GenerateConvolutionMap(int size, TextureCubemap* originCube)
{
    //Here we set up the default parameters
    int samples = 1024;
    int customSize = 128;
    int bits = 16;
    std::string filepath;
  
    RenderDevice* renderDevice = GetRenderDevice();

    ShaderProgram* convolveShader = new ShaderProgram(ResourceManager::GetShaderPath() + "cubeMapShader.vert", ResourceManager::GetShaderPath() + "convolveCubemapShader.frag");

    TextureCubemap* sourceCube = ResourceManager::GetInstance()->LoadTexCube(ResourceManager::GetBuiltinAssetsPath() + "texture/skyboxes/barcelona");
    //TextureCubemap* sourceCube = ResourceManager::GetInstance()->LoadTexCube(filepath);

    TextureCubemap* texCube = new TextureCubemap();

    PushGroupMarker("GenerateConvolutionMap");
    texCube->SetColorType(ColorType::RGB16F);

    texCube->SetWidth(size);
    texCube->SetHeight(size);
    texCube->SetMipMapActive(false);

    renderDevice->SetCullFace(ECullMode::CM_None);
    renderDevice->SetFrontFace(EFrontFace::FF_CCW);


    uint32 frameBufferID = 0;
    uint32 depthBufferID = 0;

    glGenFramebuffers(1, &frameBufferID);
    glGenRenderbuffers(1, &depthBufferID);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, texCube->GetWidth(), texCube->GetHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);

    Matrix4x4 captureProjection = Transform::Perspective(90.0f, 1.0f, 0.1f, 10.0f);

    Matrix4x4 captureViews[6] = {
           Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f))
    };

    Vector3 flips[6] = { Vector3(1.0f, -1.0f, 1.0f), Vector3(1.0f, -1.0f, 1.0f), Vector3(-1.0f, 1.0f, 1.0f),
        Vector3(-1.0f, 1.0f, 1.0f), Vector3(-1.0f, 1.0f, 1.0f), Vector3(-1.0f, 1.0f, 1.0f) };

    renderDevice->GenGPUTexture(texCube);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texCube->GetTextureID());
    for (unsigned int i = 0; i < NumSidesInCube; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB16F,
            size, size, 0,
            GL_RGB, GL_FLOAT, NULL);
    }
    qDebug() << "ConvolutionMap textureID: " << "" + (int)texCube->GetTextureID();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    convolveShader->Use();

    TextureVariable tmpTextureVariable(sourceCube, 0, "environmentMap", ETextureVariableType::TV_CUBE);
    convolveShader->SetUniform("environmentMap", tmpTextureVariable);

    convolveShader->SetUniform("projection", captureProjection);

    glViewport(0, 0, size, size);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sourceCube->GetTextureID());


    unsigned int VAO, VBO;


    //Generate Buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //Bind Vertex Array Object and VBO in correct order
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //VBO initialization 
    glBufferData(GL_ARRAY_BUFFER, sizeof(BoxVerticesData), &BoxVerticesData, GL_STATIC_DRAW);

    //Vertex position pointer init
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //Unbinding VAO
    glBindVertexArray(0);


    for (unsigned int i = 0; i < NumSidesInCube; ++i)
    {
        convolveShader->SetUniform("view", captureViews[i]);
        convolveShader->SetUniform("flip", flips[i]);

        convolveShader->Bind(renderDevice);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texCube->GetTextureID(), 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, BoxVerticesCount);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    PopGroupMarker();

    uint8* OutData = nullptr;
    std::string targetPath = "";
    ReflectionCaptureData* captureData = new ReflectionCaptureData();
    //ExportCubeToTex(texCube, captureData);
    
    return texCube;
    SAFE_DELETE(sourceCube);
    SAFE_DELETE(texCube);

}

TextureCubemap* GeneratePrefilterMap(int size, TextureCubemap* originCube)
{
    ShaderProgram* prefilterShader = new ShaderProgram(ResourceManager::GetShaderPath() + "cubeMapShader.vert", ResourceManager::GetShaderPath() + "preFilteringShader.frag");

    TextureCubemap* texCube = new TextureCubemap();
    texCube->SetColorType(ColorType::RGB16F);
    RenderDevice* renderDevice = GetRenderDevice();

    PushGroupMarker("GeneratePrefilterMap");

    renderDevice->SetCullFace(ECullMode::CM_None);
    renderDevice->SetFrontFace(EFrontFace::FF_CCW);

    texCube->SetWidth(size);
    texCube->SetHeight(size);

    uint32 frameBufferID = 0;
    uint32 depthBufferID = 0;

    Matrix4x4 captureProjection = Transform::Perspective(90.0f, 1.0f, 0.1f, 10.0f);

    Matrix4x4 captureViews[6] = {
           Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f))
    };

    Vector3 flips[6] = {
    Vector3(1.0f, -1.0f, 1.0f), Vector3(1.0f, -1.0f, 1.0f), Vector3(-1.0f, 1.0f, 1.0f),
    Vector3(-1.0f, 1.0f, 1.0f), Vector3(-1.0f, 1.0f, 1.0f), Vector3(-1.0f, 1.0f, 1.0f)
    };

    int numSidesInCube = 6;
    renderDevice->GenGPUTexture(texCube);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texCube->GetTextureID());

    for (unsigned int i = 0; i < numSidesInCube; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB16F,
            size, size, 0,
            GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


    glGenFramebuffers(1, &frameBufferID);
    glGenRenderbuffers(1, &depthBufferID);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, texCube->GetWidth(), texCube->GetHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);

    prefilterShader->Use();

    TextureVariable tmpTextureVariable(originCube, 0, "environmentMap", ETextureVariableType::TV_CUBE);
    prefilterShader->SetUniform("environmentMap", tmpTextureVariable);

    prefilterShader->SetUniform("projection", captureProjection);

    glViewport(0, 0, size, size);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, originCube->GetTextureID());


    unsigned int VAO, VBO;

    //Generate Buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //Bind Vertex Array Object and VBO in correct order
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //VBO initialization 
    glBufferData(GL_ARRAY_BUFFER, sizeof(BoxVerticesData), &BoxVerticesData, GL_STATIC_DRAW);

    //Vertex position pointer init
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //Unbinding VAO
    glBindVertexArray(0);

    int maxMipLevels = 8;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        //Mip levels are decreasing powers of two of the original resolution of the cubemap
        unsigned int mipWidth = unsigned int(size * std::pow(0.5f, mip));
        unsigned int mipHeight = unsigned int(size * std::pow(0.5f, mip));

        //The depth component needs to be resized for each mip level too
       //glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
      // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        for (unsigned int i = 0; i < numSidesInCube; ++i)
        {
            prefilterShader->SetUniform("view", captureViews[i]);
            prefilterShader->SetUniform("flip", flips[i]);

            prefilterShader->Bind(renderDevice);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                texCube->GetTextureID(), mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, BoxVerticesCount);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    PopGroupMarker();
    return texCube;
}

void ExportReflectionData(const char* fileName, const ReflectionCaptureData* captureData)
{
    qDebug() << "Export Capture Data: " << fileName;

    FILE* f = fopen(fileName, "wb");
   // fwrite(captureData->name.c_str(), sizeof(char), captureData->name.length(), f);
    
    fwrite(&(captureData->CubemapSize), sizeof(int), 1, f);
    fwrite(&(captureData->Brightness), sizeof(float), 1, f);
    
    fwrite(captureData->PrefilterFilterHDRData, sizeof(float), 3 * GetCubemapPixelCount(captureData->CubemapSize, true), f);
    fwrite(captureData->ConvolusionHDRData, sizeof(float), 3 * GetCubemapPixelCount(ConvolusionMapSize, false), f);


    fclose(f);
}

bool LoadReflectionData(const char* fileName, ReflectionCaptureData& out)
{
    FILE* f = fopen(fileName, "rb");

    assert(f);

    if (!f)
    {
        qDebug() << "File not found:" << fileName;
        exit(EXIT_FAILURE);
    }

    if (fread(&(out.CubemapSize), sizeof(int), 1, f) != 1)
    {
        printf("Unable to read mesh file header\n");
        exit(EXIT_FAILURE);
    }

    qDebug() << "CubemapSize:" << out.CubemapSize;


    if (fread(&(out.Brightness), sizeof(float), 1, f) != 1)
    {
        printf("Unable to read mesh file header\n");
        exit(EXIT_FAILURE);
    }
    int PrefilterFilterHDRDataLength = 3 * GetCubemapPixelCount(out.CubemapSize, true);
    
    out.PrefilterFilterHDRData = (float*)malloc(PrefilterFilterHDRDataLength * sizeof(float));

    if (fread(out.PrefilterFilterHDRData, sizeof(float), PrefilterFilterHDRDataLength, f) != PrefilterFilterHDRDataLength)
    {
        qDebug() << "Read PrefilterFilterHDRData error.";
        exit(EXIT_FAILURE);
    }

    int ConvolusionHDRDataLength = 3 * GetCubemapPixelCount(ConvolusionMapSize, false);

    out.ConvolusionHDRData = (float*)malloc(ConvolusionHDRDataLength * sizeof(float));

    if (fread(out.ConvolusionHDRData, sizeof(float), ConvolusionHDRDataLength, f) != ConvolusionHDRDataLength)
    {
        qDebug() << "Read ConvolusionHDRData error.";
        exit(EXIT_FAILURE);
    }

    fclose(f);

    return true;
}



TextureCubemap* GenerateConvolusionCubeFromReflectionData(ReflectionCaptureData& Data)
{
    TextureCubemap* texCube = new TextureCubemap();

    texCube->SetSize(ConvolusionMapSize);

    texCube->SetColorType(ColorType::RGB16F);

    RenderDevice* renderDevice = GetRenderDevice();

    renderDevice->GenGPUTexture(texCube);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texCube->GetTextureID());

    int32 pixelComponent = 3;
    int32 NumFacePixels = ConvolusionMapSize * ConvolusionMapSize;
    for (int CubeFace = 0; CubeFace < NumSidesInCube; ++CubeFace)
    {
        int32 dataIndex = pixelComponent * CubeFace * NumFacePixels;
        if (Data.ConvolusionHDRData + dataIndex != nullptr)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + CubeFace, 0, GetGLColorType(texCube->GetColorType()), texCube->GetSize(), texCube->GetSize(), 0, GL_RGB, GL_FLOAT, Data.ConvolusionHDRData + dataIndex);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return texCube;
}

TextureCubemap* GeneratePrefilterCubeFromReflectionData(ReflectionCaptureData& Data)
{
    TextureCubemap* texCube = new TextureCubemap();
    
    texCube->SetSize(Data.CubemapSize);

    texCube->SetColorType(ColorType::RGB16F);

    RenderDevice* renderDevice = GetRenderDevice();

    renderDevice->GenGPUTexture(texCube);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texCube->GetTextureID());

    int32 pixelComponent = 3;

    int NumMips = GetMipNum(Data.CubemapSize);
    int Offset = 0;
    for (int i = 0; i < NumMips; ++i) {
        int TexSize = Data.CubemapSize >> i;
        int MipSize = TexSize * TexSize * pixelComponent * i;
        for (int j = 0; j < NumSidesInCube; ++j) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, GL_RGB16F, TexSize, TexSize, 0, GL_RGB, GL_FLOAT, Data.PrefilterFilterHDRData + Offset);
            Offset += MipSize;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return texCube;
}

void GenerateReflectionData(int prefiltersize, std::string sourceCube)
{
    InitGL();

    string path = ResourceManager::GetBuiltinAssetsPath() + "texture/skyboxes/barcelona";

    TextureCubemap* originCube = ResourceManager::GetInstance()->LoadTexCube(ResourceManager::GetBuiltinAssetsPath() + "texture/skyboxes/barcelona");

    TextureCubemap* prefilterMap = GeneratePrefilterMap(prefiltersize, originCube);

    TextureCubemap* ConvolutionMap = GenerateConvolutionMap(ConvolusionMapSize, originCube);

    ReflectionCaptureData data;
    data.CubemapSize = prefilterMap->GetWidth();

    ExportCubeData(prefilterMap, &(data.PrefilterFilterHDRData));
    ExportCubeData(ConvolutionMap, &(data.ConvolusionHDRData));

    ExportReflectionData((path +".asset").c_str(), &data);

    //Load Test
    ReflectionCaptureData loadData;
    LoadReflectionData((path + ".asset").c_str(), loadData);
    TextureCubemap* LoadConvolutionMap = GenerateConvolusionCubeFromReflectionData(loadData);
    TextureCubemap* LoadPrefilterMap = GeneratePrefilterCubeFromReflectionData(loadData);
    getchar();
}
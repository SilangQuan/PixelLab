#include "Render/TextureCubemap.h"
#include "Render/ShaderProgram.h"
#include "Render/RenderTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

TextureCubemap::TextureCubemap()
{
    isLoaded = false;
    surface = NULL;
}

TextureCubemap::~TextureCubemap()
{
}



TextureCubemap* TextureCubemap::GeneratePrefilterMap(int size, ShaderProgram* prefilterShader, TextureCubemap* originCube)
{
    TextureCubemap* texCube = new TextureCubemap();

    RenderDevice* renderDevice = GetRenderDevice();

    PushGroupMarker("GeneratePrefilterMap");

    renderDevice->SetCullFace(ECullMode::CM_None);
    renderDevice->SetFrontFace(EFrontFace::FF_CCW);

    texCube->mWidth = size;
    texCube->mHeight = size;

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
    glGenTextures(1, &texCube->textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texCube->textureID);

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

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, texCube->mWidth, texCube->mHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);

    prefilterShader->Use();

    TextureVariable tmpTextureVariable(originCube, 0, "environmentMap", ETextureVariableType::TV_CUBE);
    prefilterShader->SetUniform("environmentMap", tmpTextureVariable);

    prefilterShader->SetUniform("projection", captureProjection);

    glViewport(0, 0, size, size);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, originCube->GetTextureID());


    unsigned int VAO, VBO;
    unsigned int numVertices = 36;
    const float boxVertices[108] = {
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
-1.0,	1.0,	-1.0, };

    //Generate Buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //Bind Vertex Array Object and VBO in correct order
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //VBO initialization 
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), &boxVertices, GL_STATIC_DRAW);

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
                texCube->textureID, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, numVertices);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    PopGroupMarker();
    return texCube;
}


TextureCubemap* TextureCubemap::GenerateConvolutionMap(int size, ShaderProgram* convolveShader, TextureCubemap* originCube)
{
    TextureCubemap* texCube = new TextureCubemap();
    PushGroupMarker("GenerateConvolutionMap");
    RenderDevice* renderDevice = GetRenderDevice();

    renderDevice->SetCullFace(ECullMode::CM_None);
    renderDevice->SetFrontFace(EFrontFace::FF_CCW);
   
    uint32 frameBufferID = 0;
    uint32 depthBufferID = 0;

    texCube->mWidth = size;
    texCube->mHeight = size;


    glGenFramebuffers(1, &frameBufferID);
    glGenRenderbuffers(1, &depthBufferID);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, texCube->mWidth, texCube->mHeight);
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

    Vector3 flips[6] = {Vector3(1.0f, -1.0f, 1.0f), Vector3(1.0f, -1.0f, 1.0f), Vector3(-1.0f, 1.0f, 1.0f),
        Vector3(-1.0f, 1.0f, 1.0f), Vector3(-1.0f, 1.0f, 1.0f), Vector3(-1.0f, 1.0f, 1.0f)};

    int numSidesInCube = 6;
    glGenTextures(1, &texCube->textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texCube->textureID);
    for (unsigned int i = 0; i < numSidesInCube; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB32F,
            size, size, 0,
            GL_RGB, GL_FLOAT, NULL);
    }
    qDebug() << "ConvolutionMap textureID: " << "" + (int)texCube->textureID;
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    convolveShader->Use();
    
    TextureVariable tmpTextureVariable(originCube, 0, "environmentMap", ETextureVariableType::TV_CUBE);
    convolveShader->SetUniform("environmentMap", tmpTextureVariable);

    convolveShader->SetUniform("projection", captureProjection);

    glViewport(0, 0, size, size);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, originCube->GetTextureID());


    unsigned int VAO, VBO;
    unsigned int numVertices = 36;
    const float boxVertices[108] = {
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
-1.0,	1.0,	-1.0, };

    //Generate Buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //Bind Vertex Array Object and VBO in correct order
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //VBO initialization 
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), &boxVertices, GL_STATIC_DRAW);

    //Vertex position pointer init
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //Unbinding VAO
    glBindVertexArray(0);


    for (unsigned int i = 0; i < numSidesInCube; ++i) 
    {
        convolveShader->SetUniform("view", captureViews[i]);
        convolveShader->SetUniform("flip", flips[i]);

        convolveShader->Bind(renderDevice);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texCube->textureID, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    PopGroupMarker();
    return texCube;
}

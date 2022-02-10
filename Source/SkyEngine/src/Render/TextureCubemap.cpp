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
	glDeleteTextures(1, &textureID);
}

TextureCubemap::TextureCubemap(const std::string &file)
{
	filePath = file;
	isLoaded = false;
	surface = NULL;

	//Default value
	wrapHorizontal = GL_REPEAT;
	wrapVertical = GL_REPEAT;
	magnifiFilter = GL_LINEAR;
	minifiFilter = GL_LINEAR_MIPMAP_LINEAR;
	generateMipMap = true;
	LoadSixFaceCube();
}

bool TextureCubemap::LoadSixFaceCube()
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

   vector<std::string> faces
   {
       "/px.hdr",
       "/nx.hdr",
       "/py.hdr",
       "/ny.hdr",
       "/pz.hdr",
       "/nz.hdr"
   };
    int width, height, nrChannels;


	for (unsigned int i = 0; i < faces.size(); i++)
	{
        string fullpath = filePath+faces[i];
        //unsigned char* data = stbi_loadf(fullpath.c_str(), &width, &height, &nrChannels, 0);
        float* data = stbi_loadf(fullpath.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            //    0, GL_RGB32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            //);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);

            stbi_image_free(data);
        }
        else
        {
            qDebug() << "Cubemap tex failed to load at path: " << fullpath;
            stbi_image_free(data);
            return false;
        }
    }
		
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	isLoaded = true;
	return true;
}


bool TextureCubemap::GeneratePrefilterMap(int size, ShaderProgram* prefilterShader, TextureCubemap* originCube)
{
    glFrontFace(GL_CCW);

    unsigned int frameBufferID;
    unsigned int texColorBuffer, depthBuffer;
    int width = size;
    int height = size;
    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);


    Matrix4x4 captureProjection = Transform::Perspective(90.0f, 1.0f, 0.1f, 10.0f);
    //Matrix4x4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

    Matrix4x4 captureViews[6] = {
           Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f))
    };

    int numSidesInCube = 6;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
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

    prefilterShader->Use();
    //convolveShader->SetUniform("environmentMap", 0);

    TextureVariable* tmpTextureVariable = new TextureVariable(originCube, 0, "environmentMap", ETextureVariableType::TV_CUBE);
    prefilterShader->SetUniform("environmentMap", *tmpTextureVariable);

    prefilterShader->SetUniform("projection", captureProjection);

    glViewport(0, 0, size, size);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, originCube->GetTextureID());


    unsigned int VAO, VBO;
    unsigned int numVertices = 36;
    const float boxVertices[108] = {
       -1.0f, 1.0f, -1.0f,
       -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, 1.0f, -1.0f,
       -1.0f, 1.0f, -1.0f,

       -1.0f, -1.0f, 1.0f,
       -1.0f, -1.0f, -1.0f,
       -1.0f, 1.0f, -1.0f,
       -1.0f, 1.0f, -1.0f,
       -1.0f, 1.0f, 1.0f,
       -1.0f, -1.0f, 1.0f,

       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,

       -1.0f, -1.0f, 1.0f,
       -1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, -1.0f, 1.0f,
       -1.0f, -1.0f, 1.0f,

       -1.0f, 1.0f, -1.0f,
       1.0f, 1.0f, -1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f, -1.0f,

       -1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f, 1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f, 1.0f,
       1.0f, -1.0f, 1.0f };

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

    maxMipLevels = 8;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) 
    {
        //Mip levels are decreasing powers of two of the original resolution of the cubemap
        unsigned int mipWidth = unsigned int(size * std::pow(0.5f, mip));
        unsigned int mipHeight = unsigned int(size * std::pow(0.5f, mip));

        //The depth component needs to be resized for each mip level too
       glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
       glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);
        
            for (unsigned int i = 0; i < numSidesInCube; ++i) {
                prefilterShader->SetUniform("view", captureViews[i]);

                prefilterShader->Bind();

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    textureID, mip);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, numVertices);
            }
    }

    return true;
}


bool TextureCubemap::GenerateConvolutionMap (int size, ShaderProgram* convolveShader, TextureCubemap* originCube)
{

    unsigned int frameBufferID;
    unsigned int texColorBuffer, depthBuffer;
    int width = size;
    int height = size;
    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    Matrix4x4 captureProjection = Transform::Perspective(90.0f, 1.0f, 0.1f, 10.0f);
    //Matrix4x4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

    Matrix4x4 captureViews[6] = {
          Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
           Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
           Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)),
           Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)),
           Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f)),
           Matrix4x4::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f))
    };

    int numSidesInCube = 6;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (unsigned int i = 0; i < numSidesInCube; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB32F,
            size, size, 0,
            GL_RGB, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    convolveShader->Use();
    //convolveShader->SetUniform("environmentMap", 0);
    
    TextureVariable* tmpTextureVariable = new TextureVariable(originCube, 0, "environmentMap", ETextureVariableType::TV_CUBE);
    convolveShader->SetUniform("environmentMap", *tmpTextureVariable);

    convolveShader->SetUniform("projection", captureProjection);

    glViewport(0, 0, size, size);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, originCube->GetTextureID());


    unsigned int VAO, VBO;
    unsigned int numVertices = 36;
    const float boxVertices[108] = {
       -1.0f, 1.0f, -1.0f,
       -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, 1.0f, -1.0f,
       -1.0f, 1.0f, -1.0f,

       -1.0f, -1.0f, 1.0f,
       -1.0f, -1.0f, -1.0f,
       -1.0f, 1.0f, -1.0f,
       -1.0f, 1.0f, -1.0f,
       -1.0f, 1.0f, 1.0f,
       -1.0f, -1.0f, 1.0f,

       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,

       -1.0f, -1.0f, 1.0f,
       -1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, -1.0f, 1.0f,
       -1.0f, -1.0f, 1.0f,

       -1.0f, 1.0f, -1.0f,
       1.0f, 1.0f, -1.0f,
       1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f, 1.0f,
       -1.0f, 1.0f, -1.0f,

       -1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f, 1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f, 1.0f,
       1.0f, -1.0f, 1.0f };

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

        convolveShader->Bind();

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
    }

    return true;
}

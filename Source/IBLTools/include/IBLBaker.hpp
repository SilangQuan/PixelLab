#pragma once
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb/stb_image_write.h"
//#include "stb/stb_image.h"

#include "Render/TextureCubemap.h"
#include "Render/ShaderProgram.h"
#include "Render/RenderTexture.h"
#include "../../LibAppFramework/include/application.h"


void ExportCubeToTex(TextureCubemap* cube, std::string path)
{
    /*
    OutCaptureData->FullHDRCapturedData.Empty(CaptureDataSize);
    OutCaptureData->FullHDRCapturedData.AddZeroed(CaptureDataSize);
    int32 MipBaseIndex = 0;

    for (int32 MipIndex = 0; MipIndex < NumMips; MipIndex++)
    {
        const int32 MipSize = 1 << (NumMips - MipIndex - 1);
        const int32 CubeFaceBytes = MipSize * MipSize * sizeof(FFloat16Color);

        TArray<FFloat16Color> SurfaceData;
        // Read each mip face
        //@todo - do this without blocking the GPU so many times
        //@todo - pool the temporary textures in RHIReadSurfaceFloatData instead of always creating new ones
        RHICmdList.ReadSurfaceFloatData(EffectiveDest.ShaderResourceTexture, FIntRect(0, 0, MipSize, MipSize), SurfaceData, (ECubeFace)0, CubemapIndex, MipIndex);
        //const int32 DestIndex = MipBaseIndex + MipIndex * CubeFaceBytes;
        uint8* FaceData = &OutCaptureData->FullHDRCapturedData[MipBaseIndex];
        check(SurfaceData.Num() * SurfaceData.GetTypeSize() == CubeFaceBytes);
        FMemory::Memcpy(FaceData, SurfaceData.GetData(), CubeFaceBytes);
        MipBaseIndex += CubeFaceBytes;

    }
    */
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


void GenerateConvolutionMap(int argc, char** argv)
{
    InitGL();

	//Here we set up the default parameters
	int samples = 1024;
	const int size = 256;
	int customSize = 128;
	int bits = 16;
	std::string filepath;

	//Must have at least 3 arguments to account for filename
	if (argc < 4)
	{
		// Inform the user of how to use the program
		std::cout << "Usage: " << argv[0] << " -f filename <Options>\n"
			<< "Options:\n"
			<< "\t-s SIZE \tThe size of the cube in pixels [size x size]. Default: 256\n"
			<< std::endl;

		exit(0);
	}
	else
	{
		//variables for error checking
		errno = 0;
		char* p;

		/* We will iterate over argv[] to get the parameters stored inside.
		 * Note that we're starting on 1 because we don't need to know the
		 * path of the program, which is stored in argv[0] */
		for (int i = 1; i < argc; i++)
		{
			if (i + 1 != argc)
			{
				if (!strcmp(argv[i], "-f")) {
					filepath = argv[i + 1];
				}
				else if (!strcmp(argv[i], "-s")) {
					customSize = strtol(argv[i + 1], &p, 10);
					if (errno != 0 || *p != '\0' || size > INT_MAX || size < 0) {
						std::cout << "Invalid size input, should be an integer value greater than 0.\n";
						exit(0);
					}
				}
			}
		}

		if (filepath.empty())
		{
			std::cout << "Must provide filename, please try again.\n";
			exit(0);
		}
	}
    
    ShaderProgram* prefilterShader = new ShaderProgram(ResourceManager::GetShaderPath() + "cubeMapShader.vert", ResourceManager::GetShaderPath() + "preFilteringShader.frag");
    
    TextureCubemap* sourceCube = ResourceManager::GetInstance()->LoadTexCube(filepath);

    TextureCubemap* texCube = new TextureCubemap();

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
    uint32 texid = 0;
    glGenTextures(1, &texid);
    texCube->SetTextureID(texid);
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

    TextureVariable tmpTextureVariable(sourceCube, 0, "environmentMap", ETextureVariableType::TV_CUBE);
    prefilterShader->SetUniform("environmentMap", tmpTextureVariable);

    prefilterShader->SetUniform("projection", captureProjection);

    glViewport(0, 0, size, size);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sourceCube->GetTextureID());


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
                texCube->GetTextureID(), mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, numVertices);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    PopGroupMarker();

    std::string targetPath = "";
    ExportCubeToTex(texCube, targetPath);
    SAFE_DELETE(sourceCube);
    SAFE_DELETE(texCube);

}

void GeneratePrefilterMap(int argc, char** argv)
{

}


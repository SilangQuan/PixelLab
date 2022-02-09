#pragma once
#include "../../LibAppFramework/include/application.h"

struct VolumeTileAABB {
	Vector4 minPoint;
	Vector4 maxPoint;
};

struct ScreenToView {
	Matrix4x4 inverseProjectionMat;
	unsigned int tileSizes[4];
	unsigned int screenWidth;
	unsigned int screenHeight;
	float sliceScalingFactor;
	float sliceBiasFactor;
};

struct GPULight {
	Vector4 position;
	Vector4 color;
	unsigned int enabled;
	float intensity;
	float range;
	float padding;
};


class App : public Application
{
public:
	//bool Initialize();
	bool CreateWorld();
	void DestroyWorld();

	void FrameMove();
	void RenderWorld();

private:
	GLuint boxVBO, boxVAO;

	GLuint lightVAO;

	Texture* diffuseMap;
	Texture* specularMap;

	Shader* lampShader;
	Shader* lightingShader;

	Vector3* cubePositions;
	Vector3* pointlightPositions;
	Vector3* pointlightDiffuses;
	Camera* camera;

	Vector3 lightPos;
	float mixValue;

	void UpdateCamera();
	void InitLights();
	void InitSSBOs();
	void ClusterSetUp();

	ShaderProgram* lampProgram;
	ShaderProgram* lightingProgram;

	ShaderProgram* clusterSetupShader;
	ShaderProgram* clusterCullLightShader;

	Scene* scene;
	GameObject* box;
	GameObject* lightObjs;
	GameObject tmpBox;

	Material* boxMat;
	Material* lampMat;

	CubeMesh* cubeMesh;
	SphereMesh* sphereMesh;

	Light* light;


	//The variables that determine the size of the cluster grid. They're hand picked for now, but
	 //there is some space for optimization and tinkering as seen on the Olsson paper and the ID tech6
	 //presentation.
	const unsigned int gridSizeX = 16;
	const unsigned int gridSizeY = 9;
	const unsigned int gridSizeZ = 24;
	const unsigned int numClusters = gridSizeX * gridSizeY * gridSizeZ;
	unsigned int sizeX, sizeY;

	const unsigned int maxLights = 1000; // pretty overkill for sponza, but ok for testing
	const unsigned int maxLightsPerTile = 10;

	unsigned int AABBvolumeGridSSBO, screenToViewSSBO;
	unsigned int lightSSBO, lightIndexListSSBO, lightGridSSBO, lightIndexGlobalCountSSBO;



	PostProcessor* mPostProcessor;

	//Diffuse IBL
	TextureCubemap* mSpecCubeMap;
	//Specular IBL
	TextureCubemap* mDiffuseCubeMap;

	TextureCubemap* skyCubeBarcelona;
	TextureCubemap* skyCubeTokyo;
	TextureCubemap* skyCubeCatwalk;
	RenderTexture* mHdrRT;
	RenderTexture* mHdrRTMSAA;
	bool mEnableMsaa = false;
	float exposure = 1;
	float bloomThreshold = 1;
	float bloomIntensity = 1;
};
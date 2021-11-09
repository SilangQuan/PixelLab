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
	Vector3 position = {};
	float radius = 1.0f;
	Vector3 color = {};
	float intensity = 1.0f;
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
	ShaderProgram* mDepthPreShader;

	ShaderProgram* clusterSetupShader;
	ShaderProgram* clusterCullLightShader;
	ShaderProgram* mCullLightShader;

	Scene* scene;
	GameObject* box;
	GameObject* boxes;
	GameObject* lightObjs;
	GameObject tmpBox;

	Material* boxMat;
	Material* lampMat;
	Material* mDepthPreMat;
	
	CubeMesh* cubeMesh;
	SphereMesh* sphereMesh;

	Light* light;


	GLuint ssbo_lights = 0;
	GLuint ssbo_visible_lights = 0;

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

	RenderTexture* mDepthPrePassTarget;
	PostProcessor* mPostProcessor;
	ForwardSceneRenderer* mForwardRenderer;

	unsigned int mWorkGroupsX;
	unsigned int mWorkGroupsY;
};
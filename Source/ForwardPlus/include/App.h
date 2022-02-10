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

	void RenderUI();
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

	ShaderProgram* lampProgram;
	ShaderProgram* mShadingProgram;
	ShaderProgram* mDepthPreShader;

	ShaderProgram* mCullLightShader;
	ShaderProgram* mTileDebugShader;

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
	unsigned int sizeX, sizeY;

	const unsigned int maxLights = 1000; // pretty overkill for sponza, but ok for testing
	const unsigned int maxLightsPerTile = 10;

	unsigned int AABBvolumeGridSSBO, screenToViewSSBO;
	unsigned int lightSSBO, lightIndexListSSBO, lightGridSSBO, lightIndexGlobalCountSSBO;

	RenderTexture* mDepthPrePassTarget;
	RenderTexture* mHdrRT;
	PostProcessor* mPostProcessor;
	ForwardSceneRenderer* mForwardRenderer;

	unsigned int mWorkGroupsX;
	unsigned int mWorkGroupsY;

	ViewInfo mMainViewInfo;

	bool mEnableTileDebug = false;
};
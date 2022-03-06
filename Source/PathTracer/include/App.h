#pragma once
#include "../../LibAppFramework/include/application.h"

class PostProcessor;

struct SphereData
{
	Vector4 MaterialData;
	Vector3 MaterialAlbedo;
	int MaterialType;
	Vector3 Center;
	float Radius;
};

enum MaterialType
{
	MAT_LAMBERTIAN = 0,
	MAT_METAL = 1,
	MAT_DIELECTRIC = 2,
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

	void RunComputeFilter(Texture* srcTex, Texture* outputTex, ShaderProgram* filterProgram);

	void TraceScene(Texture* outputTex, ShaderProgram* tracerProgram);
	
	void InitGPUScene();


private:

	Scene* scene;
	GameObject* quad;
	Material* mat;
	Mesh* mesh;
	ShaderProgram* unlitShader;

	ShaderProgram* computePathTracing;

	Camera* camera;
	Texture* texture1;

	Texture* targetTex;
	int filterIndex;

	unsigned int mGPUSceneSSBO;

	int mNumFrames;

	vector<SphereData> mGPUscene;
};

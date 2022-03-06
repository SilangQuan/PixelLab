#pragma once
#include "../../LibAppFramework/include/application.h"

class PostProcessor;

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

private:

	Scene* scene;
	GameObject* quad;
	Material* mat;
	Mesh* mesh;
	ShaderProgram* unlitShader;
	ShaderProgram* computeBlurShader;
	ShaderProgram* computeInvertShader;
	Camera* camera;
	Texture* texture1;

	Texture* targetTex;
	int filterIndex;
};

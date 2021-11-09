#pragma once
#include "../../LibAppFramework/include/application.h"

class GLShader;

class App : public Application
{
public:
	//bool Initialize();
	bool CreateWorld();
	void DestroyWorld();

	void FrameMove();
	void RenderWorld();

private:
	Texture* texture1;

	Scene* scene;
	GameObject* quad;
	GameObject* light;
	Material* mat;
	Mesh* mesh;
	ShaderProgram* refractionShader;
	RenderTexture* mRT;
	PostProcessor* mPostProcessor;


	Camera* camera;
};


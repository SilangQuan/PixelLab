#pragma once
#include "../../LibAppFramework/include/application.h"

class GBuffer;

class App : public Application
{
public:
	//bool Initialize();
	bool CreateWorld();
	void DestroyWorld();

	void FrameMove();
	void RenderWorld();

	void CreatePointLights();

	void UpdateLights();

	~App();
private:
	GameObject* sponza;

	Scene* scene;
	Material* mat;
	Material* mat2;
	Mesh* mesh;
	Model* bunny;
	Camera* camera;
	Light* light;
	void UpdateCamera();
	
	GBuffer* mGBuffer;
	float totalTime = 0.0f; // global time.

	GLuint outputGeoShader;
	GLuint directionalLightShader;
	GLuint pointLightShader;

	Texture* diffuseMap2;
	ShaderProgram*  mGeometryPass;

	std::vector<GameObject*> mLightObjs;

};
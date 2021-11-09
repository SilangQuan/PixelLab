#pragma once
#include "Base.h"

class GameObject;
class Camera;

class Scene
{
public:

	Scene();
	virtual ~Scene();

	void Init(const std::string& file);

	void Shutdown();

	virtual void Update();	//TODO make this non-virtual. Have everything done in custom GameComponents

	void Start();

	void AddGameObject(GameObject* gameObject);

	virtual GameObject* GetRoot();

	Camera* GetActiveCamera();
	void SetActiveCamera(Camera* camera);

	GameObject* FindByName(const string& name, GameObject* root = NULL);


private:
	GameObject* rootObject;
	Camera* activeCamera;
	string mSceneId;

	void LoadCamera(const json& configJson);
	void LoadGameObjs(const json& configJson);

};
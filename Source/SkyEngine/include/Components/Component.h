#pragma once
#include "EngineBase.h"

class GameObject;

class Component
{
public:
	Component();
	virtual ~Component();

	void AttachToGameObject(GameObject* gameObject);
	GameObject* GetGameObject() const;

	virtual void Start();
	virtual void Update();
	GameObject* gameObject;

protected:
private:
};
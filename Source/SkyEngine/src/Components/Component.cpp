#include "Components/Component.h"

Component::Component() : gameObject(0) {}

void Component::AttachToGameObject(GameObject* _gameObject)
{
	gameObject = _gameObject;
}

GameObject* Component::GetGameObject() const
{
	return gameObject;
}

void Component::Start() {}
void Component::Update() {}

Component::~Component() {}
#pragma once

#include "Base.h"

class Camera;
class Input;

class OrbitCameraController
{
private:
	Camera* mCamera;
	float mSensitive;
	Input* mInput;
	int mWindowHeight;
	int mWindowWidth;

	float mRotX;
	float mRotY;
	float mDistanceToPivot;

public:
	OrbitCameraController(Camera* _camera, Input* _input, float _scens, int _windowHeight, int _windowWidth);
	~OrbitCameraController();

	void Update();
};
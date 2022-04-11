#pragma once

#include "Base.h"

class Camera;
class Input;

class FPSCameraController 
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

	float mMoveSpeed;

public:
	FPSCameraController(Camera* _camera, Input* _input, float _scens, float _moveSpeed, int _windowHeight, int _windowWidth);
	~FPSCameraController();

	void Update();
};
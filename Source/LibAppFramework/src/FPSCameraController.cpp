
#include "../include/FPSCameraController.h"
#include "../include/Input.h"

FPSCameraController::FPSCameraController(Camera* _camera, Input* _input, float _scens, float _moveSpeed, int _windowHeight, int _windowWidth)
{
	mCamera = _camera;
	mSensitive = _scens;
	mInput = _input;

	mWindowHeight = _windowHeight;
	mWindowWidth = _windowWidth;

	Vector3 distanceToPivot = mCamera->transform.position - mCamera->target;
	mDistanceToPivot = distanceToPivot.magnitude();

	mRotX = Vector3::Angle(distanceToPivot, Vector3(0, 0, -1));
	mRotY = Vector3::Angle(distanceToPivot, Vector3(distanceToPivot.x, 0, distanceToPivot.z));
	mCamera->transform.rotation = Quaternion::Euler(mRotY, mRotX, 0);
	mMoveSpeed = _moveSpeed;
}

FPSCameraController::~FPSCameraController()
{
	
}

void FPSCameraController::Update()
{
	if (mInput->GetMouseButton(MOUSE_LEFT))
	{
		qDebug() << "DeltaX: " << mInput->GetMouseDeltaX();
		qDebug() << "DeltaY: " << mInput->GetMouseDeltaY();

		// step 1 : Calculate the amount of rotation given the mouse movement.
		float deltaAngleX = (2 * M_PI / mWindowHeight); // a movement from left to right = 2*PI = 360 deg
		float deltaAngleY = (M_PI / mWindowHeight);  // a movement from top to bottom = PI = 180 deg
		float xAngle = mInput->GetMouseDeltaX() * deltaAngleX * mSensitive;
		float yAngle = mInput->GetMouseDeltaY() * deltaAngleY * mSensitive;

		mRotX = Mathf::Mod(mRotX + xAngle, 360.0f);
		mRotY += yAngle;
		//Limiting the range of the pitch to avoid flips
		if (mRotY > 89.0f)
		{
			mRotY = 89.0f;
		}
		else if (mRotY < -89.0f)
		{
			mRotY = -89.0f;
		}

		// Extra step to handle the problem when the camera direction is the same as the up vector
		//float cosAngle = dot(app->m_camera.GetViewDir(), app->m_upVector);
		//if (cosAngle * sgn(yAngle) > 0.99f)
		//	yAngle = 0;
		qDebug() << "xAngle: " << xAngle;
		qDebug() << "mCamera rotation: " << mCamera->transform.rotation.EulerAngle();
		//mCamera->transform.rotation = mCamera->transform.rotation * Quaternion::Euler(yAngle, xAngle, 0);
		mCamera->transform.rotation = Quaternion::Euler(mRotY, mRotX, 0);
	}

	if (mInput->GetKey(KeyCode::KEY_W))
	{
		mCamera->transform.position += mCamera->transform.GetForward() * Time::deltaTime * mMoveSpeed;
	}

	if (mInput->GetKey(KeyCode::KEY_S))
	{
		mCamera->transform.position -= mCamera->transform.GetForward() * Time::deltaTime * mMoveSpeed;
	}

	if (mInput->GetKey(KeyCode::KEY_A))
	{
		mCamera->transform.position -= mCamera->transform.GetRight() * Time::deltaTime * mMoveSpeed;
	}

	if (mInput->GetKey(KeyCode::KEY_D))
	{
		mCamera->transform.position += mCamera->transform.GetRight() * Time::deltaTime * mMoveSpeed;
	}

	mCamera->transform.SetDirty(true);

	Matrix4x4 rotMatrix = Matrix4x4::LookAt(mCamera->transform.position, mCamera->transform.position + mCamera->transform.rotation * Vector3::forward, Vector3::up);
	mCamera->viewMatrix = rotMatrix;
}

#pragma once
#include "Math/Matrix.h"
#include "Math/Vector3.h"

class TextureCubemap;
class Texture;

class RenderContext
{

public:
	RenderContext() {};
	int windowWidth;
	int windowHeight;
	Vector3 viewPos;
	Matrix4x4 modelMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 invProjectionMatrix;
	Matrix4x4 viewProjectionMatrix;

	TextureCubemap* SpecCubeMap;
	TextureCubemap* DiffuseCubeMap;
	Texture* BrdfLut;
};
#pragma once

#include "Math/Matrix.h"

class Matrix4x4;

struct ViewInfo
{
public:
	Matrix4x4 View;
	Matrix4x4 Projection;
	Matrix4x4 InvProject;
	Matrix4x4 ViewPorject;
	Vector4 ScreenSizeAndInv;
	Vector4 ZParams;
};

struct BoundingBox
{
	Vector3 min_;
	Vector3 max_;
	BoundingBox() = default;
	BoundingBox(const Vector3& min, const Vector3& max) : min_(Vector3::Min(min, max)), max_(Vector3::Max(min, max)) {}
	BoundingBox(const Vector3* points, size_t numPoints)
	{
		
		Vector3 vmin(Mathf::Infinity);
		Vector3 vmax(std::numeric_limits<float>::lowest());

		for (size_t i = 0; i != numPoints; i++)
		{
			vmin = Vector3::Min(vmin, points[i]);
			vmax = Vector3::Max(vmax, points[i]);
		}
		min_ = vmin;
		max_ = vmax;
	}
	Vector3 getSize() const { return Vector3(max_.x - min_.x, max_.y - min_.y, max_.z - min_.z); }
	Vector3 getCenter() const { return 0.5f * Vector3(max_.x + min_.x, max_.y + min_.y, max_.z + min_.z); }
	void transform(const Matrix4x4& t)
	{
		Vector3 corners[] = {
			Vector3(min_.x, min_.y, min_.z),
			Vector3(min_.x, max_.y, min_.z),
			Vector3(min_.x, min_.y, max_.z),
			Vector3(min_.x, max_.y, max_.z),
			Vector3(max_.x, min_.y, min_.z),
			Vector3(max_.x, max_.y, min_.z),
			Vector3(max_.x, min_.y, max_.z),
			Vector3(max_.x, max_.y, max_.z),
		};

		for (auto& v : corners)
		{
			Vector4 transformed = t * Vector4(v.x, v.y, v.z, 1.0f);
			v = Vector3(transformed.x, transformed.y, transformed.z);
		}

		*this = BoundingBox(corners, 8);
	}
	BoundingBox getTransformed(const Matrix4x4& t) const
	{
		BoundingBox b = *this;
		b.transform(t);
		return b;
	}
	void combinePoint(const Vector3& p)
	{
		min_ = Vector3::Min(min_, p);
		max_ = Vector3::Max(max_, p);
	}
};

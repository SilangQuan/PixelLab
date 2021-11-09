#pragma once
#include "Mathf.h"
#include "Vector3.h"
#include "Matrix.h"

class Quaternion
{
public: 
	Quaternion();
	Quaternion(float x, float y, float z, float w);
	~Quaternion();
	static Quaternion identity;
	static float Dot(const Quaternion &lhs, const Quaternion &rhs);
	static Quaternion Lerp(const Quaternion &a, const Quaternion &b, float t);
	static Quaternion Slerp(const Quaternion &a, const Quaternion &b, float t);
	static Quaternion Euler(float x, float y, float z);
	static float Angle(const Quaternion &lhs, const Quaternion &rhs);
	//void SetEulerAngle(float yaw, float pitch, float roll);
	void Set(float _x, float _y, float _z, float _w);
	Matrix4x4 GetRotMatrix();

	Quaternion Conjugate() const;
	Quaternion Inverse() const;
	Vector3 EulerAngle() const;

	void operator+(const Quaternion &q);
	void operator*(float s);
	void operator-(const Quaternion& q);
	void operator /(float s);

	//inline static float Dot(const Quaternion& q1, const Quaternion& q2)
	//{
	//	return (q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w);
	//}
	static float Magnitude(const Quaternion& q);


	inline static float SqrMagnitude(const Quaternion& q)
	{
		return Dot(q, q);
	}


	static Quaternion Normalize(Quaternion& q) 
	{
		float mag = Magnitude(q);
		return Quaternion(q.x / mag, q.y / mag, q.z / mag, q.w / mag);
	}


	friend Quaternion operator * (const Quaternion& lhs, const Quaternion& rhs);
	friend Vector3 operator *(const Quaternion& rotation, const Vector3& point);

	static inline Quaternion AxisAngleToQuaternionSafe(const Vector3& axis, float angle)
	{
		Quaternion q;
		float mag = axis.magnitude();
		if (mag > 0.000001F)
		{
			float halfAngle = angle * 0.5F;

			q.w = cosf(halfAngle);

			float s = sinf(halfAngle) / mag;
			q.x = s * axis.x;
			q.y = s * axis.y;
			q.z = s * axis.z;
			return q;
		}
		else
		{
			return Quaternion::identity;
		}
	}
	float x;
	float y;
	float z;
	float w;

private: 
	
	Vector3 eulerAngles;
};
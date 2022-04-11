#pragma once
#include <cmath>
#include <cfloat>
#include <math.h>


class Mathf
{
public:
	Mathf() {}
	~Mathf() {}

	static float Pi;
	static float Rad2Deg;
	static float Deg2Rad;
	static float Infinity;
	static float NegativeInfinity;
	static float EPSILON; 
	static float Clamp(float x, float min, float max)
	{
		return fmin(max, fmax(x, min));
	}

	static float Max(float a, float b)
	{
		return fmax(a, b);
	}

	static float Min(float a, float b)
	{
		return fmin(a, b);
	}


	static inline float Lerp(float a, float b, float t)
	{
		return a + (b - a) * t;
	}

	static float Sin(float angle)
	{
		return sin(angle);
	}

	static float Cos(float angle)
	{
		return cos(angle);
	}

	template<typename genType>
	static genType Radians(genType degrees)
	{
		return degrees * static_cast<genType>(0.01745329251994329576923690768489);
	}

	inline static float Abs(float v)
	{
		return v < 0.0F ? -v : v;
	}

	// Returns true if the distance between f0 and f1 is smaller than epsilon
	inline static bool CompareApproximately(float f0, float f1, float epsilon = 0.000001F)
	{
		float dist = (f0 - f1);
		dist = Abs(dist);
		return dist <= epsilon;
	}

	inline static float Sqrt(float f)
	{
		return sqrt(f);
	}

	inline static float Mod(float f, float denom )
	{
		return fmod(f, denom);
	}

	template <typename T>  static int Sign(T val) {
		return (T(0) < val) - (val < T(0));
	}
};




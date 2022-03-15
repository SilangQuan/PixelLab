#include "Math/Vector4.h"
#include "Core/Color.h"
#include <sstream>


Vector4 Vector4::one(1, 1, 1, 1);


void Vector4::set(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _x;
	z = _x;
	w = _x;
}

Vector4 Vector4::operator+(const Vector4 &v) const
{
	return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}

Vector4 Vector4::operator-(const Vector4 &v) const
{
	return Vector4(x - v.x, y - v.y, z - v.z, w- v.w);
}

Vector4 Vector4::MakeVec4(Color* c)
{
	return Vector4(c->r, c->g, c->b, c->a);
}

std::string Vector4::ToString() const {
	stringstream ss;
	ss << std::to_string(x);
	ss << ",";
	ss << std::to_string(y);
	ss << ",";
	ss << std::to_string(z);
	return ss.str();
};
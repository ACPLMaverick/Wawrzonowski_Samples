#include "Float2.h"


namespace math
{
	Float2::Float2() :
		u(0.0f),
		v(0.0f)
	{
	}

	Float2::Float2(float nx, float ny) :
		u(nx),
		v(ny)
	{
	}


	Float2::~Float2()
	{
	}


	Float2 Float2::operator-() const
	{
		return Float2(-u, -v);
	}

	Float2 Float2::operator*(const float right) const
	{
		return Float2(x * right, y * right);
	}
	Float2 Float2::operator/(const float right) const
	{
		return Float2(x / right, y / right);
	}
	Float2 Float2::operator+(const Float2 & right) const
	{
		return Float2(x + right.x, y + right.y);
	}
	Float2 Float2::operator-(const Float2 & right) const
	{
		return Float2(x - right.x, y - right.y);
	}
	Float2 Float2::operator*(const Float2 & right) const
	{
		return Float2(x * right.x, y * right.y);
	}
	Float2 Float2::operator/(const Float2 & right) const
	{
		return Float2(x / right.x, y / right.y);
	}
	Float2 & Float2::operator=(const Float2 & right)
	{
		u = right.u;
		v = right.v;
		return *this;
	}
	Float2 & Float2::operator+=(const Float2 & right)
	{
		*this = *this + right;
		return *this;
	}
	Float2 & Float2::operator-=(const Float2 & right)
	{
		*this = *this - right;
		return *this;
	}
	Float2 & Float2::operator*=(const Float2 & right)
	{
		*this = *this * right;
		return *this;
	}
	Float2 & Float2::operator/=(const Float2 & right)
	{
		*this = *this / right;
		return *this;
	}
	Float2 & Float2::operator*=(const float right)
	{
		*this = *this * right;
		return *this;
	}
	Float2 & Float2::operator/=(const float right)
	{
		*this = *this / right;
		return *this;
	}
}
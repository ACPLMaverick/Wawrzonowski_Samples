#pragma once

#include "Optimizations.h"
#include <Windows.h>
#include <iostream>

namespace math
{
	struct Float2
	{
		union
		{
			float tab[2];
			struct { float u; float v; };
			struct { float x; float y; };
#ifdef RENDERER_FGK_SIMD
			__m128 vector;
#endif // RENDERER_FGK_SIMD
		};
		Float2();
		Float2(float nx, float ny);
		~Float2();

		Float2 operator-() const;
		Float2 operator*(const float right) const;
		Float2 operator/(const float right) const;
		Float2 operator+(const Float2& right) const;
		Float2 operator-(const Float2& right) const;
		Float2 operator*(const Float2& right) const;
		Float2 operator/(const Float2& right) const;
		Float2& operator=(const Float2& right);
		Float2& operator+=(const Float2& right);
		Float2& operator-=(const Float2& right);
		Float2& operator*=(const Float2& right);
		Float2& operator/=(const Float2& right);
		Float2& operator*=(const float right);
		Float2& operator/=(const float right);
	};
}
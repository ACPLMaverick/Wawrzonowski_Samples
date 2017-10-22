#pragma once

#include "Optimizations.h"

#include <Windows.h>
#include <iostream>
#include "Color32.h"
#include "Float4.h"
#include "Float2.h"

typedef __declspec(align(16)) __m128 __am128;

namespace math
{
	struct __declspec(align(16)) Float3
	{
		union
		{
#ifdef RENDERER_FGK_SIMD
			__am128 vector;
#endif // RENDERER_FGK_SIMD

			float tab[3];
			struct { float x, y, z; };
		};

		Float3()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}

		Float3(const Float3& c)
		{
#ifdef RENDERER_FGK_SIMD
			this->vector = c.vector;
#else
			this->x = c.x;
			this->y = c.y;
			this->z = c.z;
#endif // RENDERER_FGK_SIMD
		}

		Float3(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		Float3(const Float4& g)
		{
			this->x = g.x;
			this->y = g.y;
			this->z = g.z;
		}

		Float3(const Float2& g)
		{
			this->x = g.x;
			this->y = g.y;
			this->z = 0.0f;
		}

#ifdef RENDERER_FGK_SIMD

		Float3(const __am128& vec) :
			vector(vec)
		{
		}

#endif // RENDERER_FGK_SIMD

		Float3& operator=(const Float3& copy)
		{
#ifdef RENDERER_FGK_SIMD
			this->vector = copy.vector;
#else
			this->x = copy.x;
			this->y = copy.y;
			this->z = copy.z;
#endif
			return *this;
		}

		Float3 operator-() const
		{
			return Float3(-x, -y, -z);
		}

		Float3 operator*(const Float3& right) const
		{
#ifdef RENDERER_FGK_SIMD

			return Float3(_mm_mul_ps(this->vector, right.vector));

#else

			return Float3(this->x * right.x, this->y * right.y, this->z * right.z);

#endif // RENDERER_FGK_SIMD
		}

		Float3 operator/(const Float3& right) const
		{
#ifdef RENDERER_FGK_SIMD

			return Float3(_mm_div_ps(this->vector, right.vector));

#else

			return Float3(this->x / right.x, this->y / right.y, this->z / right.z);

#endif // RENDERER_FGK_SIMD
		}

		Float3& operator/=(const Float3& right)
		{
			*this = *this / right;
			return *this;
		}

		Float3 operator*(float scalar) const
		{
#ifdef RENDERER_FGK_SIMD

			return Float3(_mm_mul_ps(this->vector, _mm_set1_ps(scalar)));

#else

			return Float3(this->x * scalar, this->y * scalar, this->z * scalar);

#endif // RENDERER_FGK_SIMD
		}

		Float3& operator*=(float scalar)
		{
			*this = *this * scalar;
			return *this;
		}

		Float3 operator/(float scalar) const
		{

			float inverse = 1.0f / scalar;

#ifdef RENDERER_FGK_SIMD

			return Float3(_mm_mul_ps(this->vector, _mm_set1_ps(inverse)));

#else

			return Float3(this->x * inverse, this->y * inverse, this->z * inverse);

#endif // RENDERER_FGK_SIMD
		}

		Float3& operator/=(float scalar)
		{
			*this = *this / scalar;
			return *this;
		}

		friend Float3 operator/(Float3& left, Float3& right)
		{
#ifdef RENDERER_FGK_SIMD

			return Float3(_mm_div_ps(left.vector, right.vector));

#else

			return Float3(left.x / right.x, left.y / right.y, left.z / right.z);

#endif // RENDERER_FGK_SIMD
		}

		friend Float3 operator/(float scalar, Float3& right)
		{
#ifdef RENDERER_FGK_SIMD

			return Float3(_mm_div_ps(_mm_set1_ps(scalar), right.vector));

#else

			return Float3(scalar / right.x, scalar / right.y, scalar / right.z);

#endif // RENDERER_FGK_SIMD
		}

		Float3 operator+(const Float3& right) const
		{
#ifdef RENDERER_FGK_SIMD

			return _mm_add_ps(this->vector, right.vector);

#else

			return Float3(this->x + right.x, this->y + right.y, this->z + right.z);

#endif // RENDERER_FGK_SIMD
			
		}

		Float3 operator+(const float right) const
		{
#ifdef RENDERER_FGK_SIMD

			return Float3(_mm_add_ps(this->vector, _mm_set1_ps(right)));

#else

			return Float3(this->x + right, this->y + right, this->z + right);

#endif // RENDERER_FGK_SIMD
		}

		Float3 operator-(const float right) const
		{
#ifdef RENDERER_FGK_SIMD

			return Float3(_mm_sub_ps(this->vector, _mm_set1_ps(right)));

#else

			return Float3(this->x - right, this->y - right, this->z - right);

#endif // RENDERER_FGK_SIMD
		}

		Float3 operator-(const Float3& right) const
		{
#ifdef RENDERER_FGK_SIMD

			return Float3(_mm_sub_ps(this->vector, right.vector));

#else

			return Float3(this->x - right.x, this->y - right.y, this->z - right.z);

#endif // RENDERER_FGK_SIMD
		}

		Float3 operator+(const Float2& right) const
		{
#ifdef RENDERER_FGK_SIMD

			return Float3(_mm_add_ps(this->vector, right.vector));

#else

			return Float3(this->x + right.x, this->y + right.y, this->z);

#endif // RENDERER_FGK_SIMD
		}

		Float3 operator-(const Float2& right) const
		{
#ifdef RENDERER_FGK_SIMD

			return Float3(_mm_sub_ps(this->vector, right.vector));

#else

			return Float3(this->x - right.x, this->y - right.y, this->z);

#endif // RENDERER_FGK_SIMD
		}

		Float3 operator-()
		{
			return Float3(-this->x, -this->y, -this->z);
		}

		bool operator==(const Float3& right)
		{
//#ifdef RENDERER_FGK_SIMD
//
//			__m128 cmp = _mm_cmpeq_ps(this->vector, right.vector);
//			return (cmp.m128_u32[0] != 0 && cmp.m128_u32[1] != 0 && cmp.m128_u32[2] != 0 && cmp.m128_u32[3] != 0);
//
//#else

			return (this->x == right.x && this->y == right.y && this->z == right.z);

//#endif // RENDERER_FGK_SIMD
		}

		bool operator>(const Float3& right)
		{
//#ifdef RENDERER_FGK_SIMD
//
//			__m128 cmp = _mm_cmpgt_ps(this->vector, right.vector);
//			return (cmp.m128_u32[0] != 0 && cmp.m128_u32[1] != 0 && cmp.m128_u32[2] != 0 && cmp.m128_u32[3] != 0);
//
//#else

			return (this->x > right.x && this->y > right.y && this->z > right.z);

//#endif // RENDERER_FGK_SIMD
		}

		bool operator<(const Float3& right)
		{
//#ifdef RENDERER_FGK_SIMD
//
//			__m128 cmp = _mm_cmplt_ps(this->vector, right.vector);
//			return (cmp.m128_u32[0] != 0 && cmp.m128_u32[1] != 0 && cmp.m128_u32[2] != 0 && cmp.m128_u32[3] != 0);
//
//#else

			return (this->x < right.x && this->y < right.y && this->z < right.z);

//#endif // RENDERER_FGK_SIMD
		}

		bool operator>=(const Float3& right)
		{
//#ifdef RENDERER_FGK_SIMD
//
//			__m128 cmp = _mm_cmpge_ps(this->vector, right.vector);
//			return (cmp.m128_u32[0] != 0 && cmp.m128_u32[1] != 0 && cmp.m128_u32[2] != 0 && cmp.m128_u32[3] != 0);
//
//#else
//
			return (this->x >= right.x && this->y >= right.y && this->z >= right.z);

//#endif // RENDERER_FGK_SIMD
		}

		bool operator<=(const Float3& right)
		{
//#ifdef RENDERER_FGK_SIMD
//
//			__m128 cmp = _mm_cmple_ps(this->vector, right.vector);
//			return (cmp.m128_u32[0] != 0 && cmp.m128_u32[1] != 0 && cmp.m128_u32[2] != 0 && cmp.m128_u32[3] != 0);
//
//#else

			return (this->x <= right.x && this->y <= right.y && this->z <= right.z);

//#endif // RENDERER_FGK_SIMD
		}

		bool operator!=(const Float3& right)
		{
//#ifdef RENDERER_FGK_SIMD
//
//			__m128 cmp = _mm_cmpneq_ps(this->vector, right.vector);
//			return (cmp.m128_u32[0] != 0 && cmp.m128_u32[1] != 0 && cmp.m128_u32[2] != 0 && cmp.m128_u32[3] != 0);
//
//#else

			return (this->x != right.x || this->y != right.y || this->z != right.z);

//#endif // RENDERER_FGK_SIMD
		}

		float operator[](const size_t ind)
		{
			return tab[ind];
		}

		friend std::ostream& operator<<(std::ostream& ost, const Float3 flt)
		{
			ost << "(" << flt.x << ", " << flt.y << ", " << flt.z << ")" << std::endl;
		}

		operator Color32() const
		{
			Color32 data;

			data.a = 0xFF;
			float xc = Clamp(x, 0.0f, 1.0f);
			float yc = Clamp(y, 0.0f, 1.0f);
			float zc = Clamp(z, 0.0f, 1.0f);
			xc *= 255.0f;
			yc *= 255.0f;
			zc *= 255.0f;
			data.r = (uint8_t)(xc);
			data.g = (uint8_t)(yc);
			data.b = (uint8_t)(zc);

			return data;
		}

		bool EqualsEpsilon(const Float3& right, const float epsilon);
		bool GreaterEpsilon(const Float3& right, const float epsilon);
		bool SmallerEpsilon(const Float3& right, const float epsilon);
		bool GreaterEqualsEpsilon(const Float3& right, const float epsilon);
		bool SmallerEqualsEpsilon(const Float3& right, const float epsilon);
		
		static void Normalize(Float3& f);
		static float Length(const Float3& f);
		static float LengthSquared(const Float3& f);
		static float Dot(const Float3& f1, const Float3& f2);
		static Float3 SqrtComponentWise(const Float3& f);
		static Float3 Cross(const Float3& f1, const Float3& f2);
		static Float3 Reflect(const Float3& left, const Float3& normal);
		static Float3 Refract(const Float3& dir, const Float3& normal, const float coeff);
		static Float3 Lerp(const Float3& a, const Float3 & b, float f);
	};
}

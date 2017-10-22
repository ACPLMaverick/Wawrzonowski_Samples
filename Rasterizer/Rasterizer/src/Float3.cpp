#include "Float3.h"

namespace math
{
	bool Float3::EqualsEpsilon(const Float3 & right, const float epsilon)
	{
		return *this >= (right - epsilon) && *this <= (right + epsilon);
	}

	bool Float3::GreaterEpsilon(const Float3 & right, const float epsilon)
	{
		return *this > (right - epsilon);
	}

	bool Float3::SmallerEpsilon(const Float3 & right, const float epsilon)
	{
		return *this < (right + epsilon);
	}

	bool Float3::GreaterEqualsEpsilon(const Float3 & right, const float epsilon)
	{
		return *this >= (right - epsilon);
	}

	bool Float3::SmallerEqualsEpsilon(const Float3 & right, const float epsilon)
	{
		return *this <= (right + epsilon);
	}

	void Float3::Normalize(Float3& f)
	{
		float n = Length(f);

		if (n >= 0.000001f)
		{
			f = f / n;
		}
		else
		{
			f.x = f.y = f.z = 0.0f;
		}
	}

	float Float3::Length(const Float3 & f)
	{
		return sqrt(LengthSquared(f));
	}

	float Float3::LengthSquared(const Float3 & f)
	{
#ifdef RENDERER_FGK_SIMD

		return _mm_dp_ps(f.vector, f.vector, 0x71).m128_f32[0];

#else

		return (f.x * f.x) + (f.y * f.y) + (f.z * f.z);

#endif // RENDERER_FGK_SIMD
	}

	float Float3::Dot(const Float3 & f1, const Float3 & f2)
	{
#ifdef RENDERER_FGK_SIMD

		return _mm_dp_ps(f1.vector, f2.vector, 0x71).m128_f32[0];

#else

		return (f1.x * f2.x + f1.y * f2.y + f1.z * f2.z);

#endif // RENDERER_FGK_SIMD
	}

	Float3 Float3::SqrtComponentWise(const Float3 & f)
	{
		return Float3(sqrt(f.x), sqrt(f.y), sqrt(f.z));
	}

	Float3 Float3::Cross(const Float3 & f1, const Float3 & f2)
	{
#ifdef RENDERER_FGK_SIMD

		__m128 a = f1.vector;
		__m128 b = f2.vector;

		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2))),
			_mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1)))
		);

#else

		return Float3(f1.y * f2.z - f1.z * f2.y, f1.z * f2.x - f1.x * f2.z, f1.x * f2.y - f1.y * f2.x);

#endif // RENDERER_FGK_SIMD
	}

	Float3 Float3::Reflect(const Float3 & left, const Float3 & normal)
	{
		return left - (normal * 2.0f * Dot(left, normal));
	}

	Float3 Float3::Refract(const Float3 & dir, const Float3 & normal, const float coeff)
	{
		float coeffR = 1.0f / coeff;
		float dot = Dot(dir, normal);
		Float3 refracted( 
			(dir - normal * dot * coeffR) - 
			( normal * sqrt( 1.0f - ( (1.0f - dot * dot) * coeffR * coeffR) ) )
			);
		Normalize(refracted);
		return refracted;
	}

	Float3 Float3::Lerp(const Float3& a, const Float3 & b, float f)
	{
		return Float3(
			FloatLerp(a.x, b.x, f),
			FloatLerp(a.y, b.y, f),
			FloatLerp(a.z, b.z, f)
		);
	}
}
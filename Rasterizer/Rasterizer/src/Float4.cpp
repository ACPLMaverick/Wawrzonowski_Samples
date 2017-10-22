#include "Float4.h"
#include "Float3.h"
#include "Matrix4x4.h"

namespace math
{
	Float4::Float4()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 0.0f;
	}

	Float4::Float4(float nx, float ny, float nz, float nw)
	{
		x = nx;
		y = ny;
		z = nz;
		w = nw;
	}

	Float4::Float4(const Float4 & copy)
	{
#ifdef RENDERER_FGK_SIMD
		this->vector = copy.vector;
#else
		this->x = copy.x;
		this->y = copy.y;
		this->z = copy.z;
		this->w = copy.w;
#endif // RENDERER_FGK_SIMD
	}

	Float4::Float4(const Float3 & g)
	{
		x = g.x;
		y = g.y;
		z = g.z;
		w = 1.0f;
	}

	Float4::Float4(const Float3 & g, float w)
	{
		x = g.x;
		y = g.y;
		z = g.z;
		this->w = w;				// AAAAAAAAAAAAAAAA KRUWAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA AMæ
	}

	Float4::~Float4()
	{

	}

	Float4 & Float4::operator=(const Float4 & copy)
	{
#ifdef RENDERER_FGK_SIMD
		this->vector = copy.vector;
#else
		this->x = copy.x;
		this->y = copy.y;
		this->z = copy.z;
		this->w = copy.w;
#endif
		return *this;
	}

	Float4 Float4::operator*(const Float4& right) const
	{
#ifdef RENDERER_FGK_SIMD

		return Float4(_mm_mul_ps(this->vector, right.vector));

#else

		return Float4(this->x * right.x, this->y * right.y, this->z * right.z, this->w * right.w);

#endif // RENDERER_FGK_SIMD	
	}

	Float4 Float4::operator*(float scalar) const
	{
#ifdef RENDERER_FGK_SIMD

		return Float4(_mm_mul_ps(this->vector, _mm_set1_ps(scalar)));

#else

		return Float4(this->x * scalar, this->y * scalar, this->z * scalar, this->w * scalar);

#endif // RENDERER_FGK_SIMD		
	}

	Float4& Float4::operator*=(float scalar)
	{
		*this = *this * scalar;
		return *this;
	}

	Float4 Float4::operator/(float scalar) const
	{
		float inverse = 1.0f / scalar;

		return *this * inverse;
	}

	Float4 Float4::operator+(const Float4& right) const
	{
#ifdef RENDERER_FGK_SIMD

		return Float4(_mm_add_ps(this->vector, right.vector));

#else

		return Float4(this->x + right.x, this->y + right.y, this->z + right.z, this->w + right.w);

#endif // RENDERER_FGK_SIMD		

	}

	Float4 Float4::operator-(const Float4& right) const
	{
#ifdef RENDERER_FGK_SIMD

		return Float4(_mm_sub_ps(this->vector, right.vector));

#else

		return Float4(this->x - right.x, this->y - right.y, this->z - right.z, this->w - right.w);

#endif // RENDERER_FGK_SIMD		

	}

	Float4 Float4::operator-() const
	{
		return Float4(-this->x, -this->y, -this->z, -this->w);
	}

	bool Float4::operator==(const Float4& right) const
	{
		return (this->x == right.x && this->y == right.y && this->z == right.z && this->w == right.w);
	}

	bool Float4::operator!=(const Float4& right) const
	{
		return (this->x != right.x || this->y != right.y || this->z != right.z || this->w != right.w);
	}

	float Float4::operator[](size_t ind)
	{
		return tab[ind];
	}

	std::ostream & operator<<(std::ostream & ost, const Float4 & flt)
	{
		ost << "(" << flt.x << ", " << flt.y << ", " << flt.z << ", " << flt.w << ")" << std::endl;
		return ost;
	}

	void Float4::Normalize(Float4 & f)
	{
		float n = Length(f);
		if (n >= 0.000001f)
		{
			f = f / n;
		}
		else
		{
			f.x = f.y = f.z = f.w = 0.0f;
		}
	}

	float Float4::Length(const Float4 & f)
	{
		return sqrt(LengthSquared(f));
	}

	float Float4::LengthSquared(const Float4 & f)
	{
#ifdef RENDERER_FGK_SIMD

		return _mm_dp_ps(f.vector, f.vector, 0xF1).m128_f32[0];

#else

		return (f.x * f.x) + (f.y * f.y) + (f.z * f.z) + (f.w * f.w);

#endif // RENDERER_FGK_SIMD

	}

	float Float4::Dot(const Float4 & f1, const Float4 & f2)
	{
#ifdef RENDERER_FGK_SIMD

		return _mm_dp_ps(f1.vector, f2.vector, 0xF1).m128_f32[0];

#else

		return (f1.x * f2.x + f1.y * f2.y + f1.z * f2.z + f1.w * f2.w);

#endif // RENDERER_FGK_SIMD

	}

	Float4 Float4::Cross(const Float4 & f1, const Float4 & f2)
	{
#ifdef RENDERER_FGK_SIMD

		__m128 a = f1.vector;
		__m128 b = f2.vector;

		return _mm_sub_ps(
			_mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2))),
			_mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1)))
		);

#else

		Float3 f1c = Float3(f1.x, f1.y, f1.z);
		Float3 f2c = Float3(f2.x, f2.y, f2.z);
		Float3 c = Float3::Cross(f1c, f2c);
		return Float4(c.x, c.y, c.z, f1.w);

#endif // RENDERER_FGK_SIMD
	}

	Float4 Float4::Reflect(const Float4 & left, const Float4 & normal)
	{
		return left - (normal * 2.0f * Dot(left, normal));
	}

	Float4 Float4::Lerp(const Float4 & a, const Float4 & b, float f)
	{
		return Float4(
			FloatLerp(a.x, b.x, f),
			FloatLerp(a.y, b.y, f),
			FloatLerp(a.z, b.z, f),
			FloatLerp(a.w, b.w, f)
		);
	}

}
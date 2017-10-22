#pragma once

#include "Optimizations.h"

#include <Windows.h>
#include <iostream>
#include "Math.h"
#include "Float4.h"
#include "Float3.h"

namespace math
{
	struct Matrix4x4
	{
		union
		{
			float tab[16];
			Float4 tabf[4];
			struct
			{
				Float4 row1;
				Float4 row2;
				Float4 row3;
				Float4 row4;
			};
			struct
			{
				float a00;
				float a01;
				float a02;
				float a03;
				float a10;
				float a11;
				float a12;
				float a13;
				float a20;
				float a21;
				float a22;
				float a23;
				float a30;
				float a31;
				float a32;
				float a33;
			};

#ifdef RENDERER_FGK_SIMD
			__m128 vector[4];
#endif // RENDERER_FGK_SIMD
		};

#pragma region Ctors

		Matrix4x4();
		Matrix4x4(const Matrix4x4& c);
		Matrix4x4(
			const Float4& r1,
			const Float4& r2,
			const Float4& r3,
			const Float4& r4
		);
		~Matrix4x4();

#pragma endregion


#pragma region Operators

		Matrix4x4& operator=(const Matrix4x4& right);
		Matrix4x4 operator*(const Matrix4x4& right) const;
		Matrix4x4& operator*(const float right);
		Matrix4x4& operator*=(const Matrix4x4& right);
		Matrix4x4& operator*=(const float right);
		Matrix4x4& operator+(const Matrix4x4& right);
		Matrix4x4& operator+=(const Matrix4x4& right);
		Float4& operator[](const size_t ind);
		friend std::ostream& operator<<(std::ostream& ost, const Matrix4x4& m);
		friend Float4 operator*(Float4& left, const Matrix4x4 right);
		friend Float4& operator*(const Matrix4x4& left, Float4& right);

#pragma endregion

#pragma region Static Function Declarations

		static void Identity(Matrix4x4* out);
		static void Translation(const Float3* trans, Matrix4x4* out);
		static void Scale(const Float3* scale, Matrix4x4* out);
		static void Rotation(const Float3* rotationXYZdeg, Matrix4x4* out);
		static void Transpose(const Matrix4x4* in, Matrix4x4* out);
		static void Inverse(const Matrix4x4* in, Matrix4x4* out);
		static void LookAt(const Float3* cameraPos, const Float3* cameraTarget, const Float3* cameraUp, Matrix4x4* out);
		static void Perspective(const float fovAngle, const float aspectRatio, const float nearPlane, const float farPlane, Matrix4x4* out);

#pragma endregion
	};
}
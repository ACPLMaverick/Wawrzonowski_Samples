#include "MVector.h"

namespace morphEngine
{
	namespace utility
	{
#pragma region MVector2

		MVector2 MVector2::Zero(0, 0);
		MVector2 MVector2::Up(0, 1);
		MVector2 MVector2::Down(0, -1);
		MVector2 MVector2::Left(-1, 0);
		MVector2 MVector2::Right(1, 0);
		MVector2 MVector2::One(1, 1);

		MFloat32 MVector2::Cross(const MVector2& v1, const MVector2& v2)
		{
			return v1.X * v2.Y - v1.Y * v2.X;
		}

		MVector2 MVector2::Lerp(const MVector2& from, const MVector2& to, const MFloat32 t)
		{
			MFloat32 tClamped = MMath::Clamp(t);
			return from * (1 - tClamped) + to * tClamped;
		}

		MVector2 MVector2::Slerp(const MVector2& from, const MVector2& to, const MFloat32 t)
		{
			MFloat32 dot = Dot(from.Normalized(), to.Normalized());
			MFloat32 omega = MMath::Acos(dot);
			MFloat32 sinOmega = MMath::Sin(omega);
			MFloat32 tClamped = MMath::Clamp(t);
			return from * MMath::Sin((1.0f - tClamped) * omega) / sinOmega + to * MMath::Sin(tClamped * omega) / sinOmega;
		}

		MVector2 MVector2::Clamp(const MVector2& v, const MVector2& min, const MVector2& max)
		{
			return MVector2(MMath::Clamp(v.X, min.X, max.X), MMath::Clamp(v.Y, min.Y, max.Y));
		}

		MFloat32 MVector2::Dot(const MVector2& v1, const MVector2& v2)
		{
			return v1.X * v2.X + v1.Y * v2.Y;
		}

		MFloat32 MVector2::Angle(const MVector2& from, const MVector2& to)
		{
			return MMath::Acos(Dot(from.Normalized(), to.Normalized()));
		}

		MFloat32 MVector2::AngleD(const MVector2& from, const MVector2& to)
		{
			return MMath::AcosD(Dot(from.Normalized(), to.Normalized()));
		}

		MVector2 MVector2::Min(const MVector2 & v1, const MVector2 & v2)
		{
			return MVector2(MMath::Min(v1.X, v2.X), MMath::Min(v1.Y, v2.Y));
		}

		MVector2 MVector2::Max(const MVector2 & v1, const MVector2 & v2)
		{
			return MVector2(MMath::Max(v1.X, v2.X), MMath::Max(v1.Y, v2.Y));
		}

#pragma endregion

#pragma region MVector3

		MVector3 MVector3::Zero(0, 0, 0);
		MVector3 MVector3::Left(-1, 0, 0);
		MVector3 MVector3::Right(1, 0, 0);
		MVector3 MVector3::Down(0, -1, 0);
		MVector3 MVector3::Up(0, 1, 0);
		MVector3 MVector3::Back(0, 0, -1);
		MVector3 MVector3::Forward(0, 0, 1);
		MVector3 MVector3::One(1, 1, 1);

#pragma endregion

#pragma region MVector4

		MVector4 MVector4::Zero(0, 0, 0, 0);
		MVector4 MVector4::One(1, 1, 1, 1);

#pragma endregion

#if USE_SIMD

#pragma region MVector3

		MVector3 MVector3::Cross(const MVector3& v1, const MVector3& v2)
		{
			return MVector3(_mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(v1.SIMDValue, v1.SIMDValue, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(v2.SIMDValue, v2.SIMDValue, _MM_SHUFFLE(3, 1, 0, 2))),
				_mm_mul_ps(_mm_shuffle_ps(v1.SIMDValue, v1.SIMDValue, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(v2.SIMDValue, v2.SIMDValue, _MM_SHUFFLE(3, 0, 2, 1)))));
		}

		MVector3 MVector3::Lerp(const MVector3& from, const MVector3& to, const MFloat32 t)
		{
			MFloat32 tClamped = MMath::Clamp(t);
			return from * (1.0f - tClamped) + to * tClamped;
		}

		MVector3 MVector3::Slerp(const MVector3& from, const MVector3& to, const MFloat32 t)
		{
			MFloat32 dot = Dot(from.Normalized(), to.Normalized());
			MFloat32 omega = MMath::Acos(dot);
			MFloat32 sinOmega = MMath::Sin(omega);
			MFloat32 tClamped = MMath::Clamp(t);
			return from * MMath::Sin((1.0f - tClamped) * omega) / sinOmega + to * MMath::Sin(tClamped * omega) / sinOmega;
		}

		MVector3 MVector3::Clamp(const MVector3& v, const MVector3& min, const MVector3& max)
		{
			return MVector3(MMath::Clamp(v.X, min.X, max.X), MMath::Clamp(v.Y, min.Y, max.Y), MMath::Clamp(v.Z, min.Z, max.Z));
		}

		MFloat32 MVector3::Dot(const MVector3& v1, const MVector3& v2)
		{
			return _mm_cvtss_f32(_mm_dp_ps(v1.SIMDValue, v2.SIMDValue, 0x71));
		}

		MVector3 MVector3::Abs(const MVector3 & v)
		{
			return MVector3(MMath::Abs(v.X), MMath::Abs(v.Y), MMath::Abs(v.Z));
		}

		MFloat32 MVector3::Angle(const MVector3& from, const MVector3& to)
		{
			return MMath::Acos(Dot(from.Normalized(), to.Normalized()));
		}

		MFloat32 MVector3::AngleD(const MVector3& from, const MVector3& to)
		{
			return MMath::AcosD(Dot(from.Normalized(), to.Normalized()));
		}

		MVector3 MVector3::Min(const MVector3 & v1, const MVector3 & v2)
		{
			return MVector3(MMath::Min(v1.X, v2.X), MMath::Min(v1.Y, v2.Y), MMath::Min(v1.Z, v2.Z));
		}

		MVector3 MVector3::Max(const MVector3 & v1, const MVector3 & v2)
		{
			return MVector3(MMath::Max(v1.X, v2.X), MMath::Max(v1.Y, v2.Y), MMath::Max(v1.Z, v2.Z));
		}

#pragma endregion

#pragma region MVector4

		MVector4 MVector4::Lerp(const MVector4& from, const MVector4& to, const MFloat32 t)
		{
			MFloat32 tClamped = MMath::Clamp(t);
			return from * (1.0f - tClamped) + to * tClamped;
		}

		MVector4 MVector4::Slerp(const MVector4& from, const MVector4& to, const MFloat32 t)
		{
			MFloat32 dot = Dot(from.Normalized(), to.Normalized());
			MFloat32 omega = MMath::Acos(dot);
			MFloat32 sinOmega = MMath::Sin(omega);
			MFloat32 tClamped = MMath::Clamp(t);
			return from * MMath::Sin((1.0f - tClamped) * omega) / sinOmega + to * MMath::Sin(tClamped * omega) / sinOmega;
		}

		MVector4 MVector4::Clamp(const MVector4& v, const MVector4& min, const MVector4 & max)
		{
			return MVector4(MMath::Clamp(v.X, min.X, max.X), MMath::Clamp(v.Y, min.Y, max.Y), MMath::Clamp(v.Z, min.Z, max.Z), MMath::Clamp(v.W, min.W, max.W));
		}

		MFloat32 MVector4::Dot(const MVector4& v1, const MVector4& v2)
		{
			return _mm_cvtss_f32(_mm_dp_ps(v1.SIMDValue, v2.SIMDValue, 0xF1));
		}

		MFloat32 MVector4::Angle(const MVector4& from, const MVector4& to)
		{
			return MMath::Acos(Dot(from.Normalized(), to.Normalized()));
		}

		MFloat32 MVector4::AngleD(const MVector4& from, const MVector4& to)
		{
			return MMath::AcosD(Dot(from.Normalized(), to.Normalized()));
		}

		MVector4 MVector4::Min(const MVector4 & v1, const MVector4 & v2)
		{
			return MVector4();
		}

		MVector4 MVector4::Max(const MVector4 & v1, const MVector4 & v2)
		{
			return MVector4();
		}

#pragma endregion

#else //USE_SIMD == 0

#pragma region MVector3

		MVector3 MVector3::Cross(const MVector3& from, const MVector3& to)
		{
			return MVector3(from.Y * to.Z - from.Z * to.Y,
							from.Z * to.X - from.X * to.Z,
							from.X * to.Y - from.Y * to.X);
		}

		MVector3 MVector3::Lerp(const MVector3& from, const MVector3& to, const MFloat32 t)
		{
			MFloat32 tClamped = MMath::Clamp(t);
			return from * (1.0f - tClamped) + to * tClamped;
		}

		MVector3 MVector3::Slerp(const MVector3& from, const MVector3& to, const MFloat32 t)
		{
			MFloat32 dot = Dot(from.Normalized(), to.Normalized());
			MFloat32 omega = MMath::Acos(dot);
			MFloat32 sinOmega = MMath::Sin(omega);
			MFloat32 tClamped = MMath::Clamp(t);
			return from * MMath::Sin((1.0f - tClamped) * omega) / sinOmega + to * MMath::Sin(tClamped * omega) / sinOmega;
		}

		MVector3 MVector3::Clamp(const MVector3& v, const MVector3& min, const MVector3& max)
		{
			return MVector3(MMath::Clamp(v.X, min.X, max.X), MMath::Clamp(v.Y, min.Y, max.Y), MMath::Clamp(v.Z, min.Z, max.Z));
		}

		MFloat32 MVector3::Dot(const MVector3& v1, const MVector3& v2)
		{
			return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
		}

		MFloat32 MVector3::Angle(const MVector3& from, const MVector3& to)
		{
			return MMath::Acos(Dot(from.Normalized(), to.Normalized()));
		}

		MFloat32 MVector3::AngleD(const MVector3& from, const MVector3& to)
		{
			return MMath::AcosD(Dot(from.Normalized(), to.Normalized()));
		}

		MVector3 MVector3::Abs(const MVector3 & v)
		{
			return MVector3(MMath::Abs(v.X), MMath::Abs(v.Y), MMath::Abs(v.Z));
		}

		MVector3 MVector3::Min(const MVector3 & v1, const MVector3 & v2)
		{
			return MVector3(MMath::Min(v1.X, v2.X), MMath::Min(v1.Y, v2.Y), MMath::Min(v1.Z, v2.Z));
		}

		MVector3 MVector3::Max(const MVector3 & v1, const MVector3 & v2)
		{
			return MVector3(MMath::Max(v1.X, v2.X), MMath::Max(v1.Y, v2.Y), MMath::Max(v1.Z, v2.Z));
		}

#pragma endregion

#pragma region MVector4

		MVector4 MVector4::Lerp(const MVector4& from, const MVector4& to, const MFloat32 t)
		{
			MFloat32 tClamped = MMath::Clamp(t);
			return from * (1.0f - tClamped) + to * tClamped;
		}

		MVector4 MVector4::Slerp(const MVector4& from, const MVector4& to, const MFloat32 t)
		{
			MFloat32 dot = Dot(from.Normalized(), to.Normalized());
			MFloat32 omega = MMath::Acos(dot);
			MFloat32 sinOmega = MMath::Sin(omega);
			MFloat32 tClamped = MMath::Clamp(t);
			return from * MMath::Sin((1.0f - tClamped) * omega) / sinOmega + to * MMath::Sin(tClamped * omega) / sinOmega;
		}

		MVector4 MVector4::Clamp(const MVector4& v, const MVector4& min, const MVector4 & max)
		{
			return MVector4(MMath::Clamp(v.X, min.X, max.X), MMath::Clamp(v.Y, min.Y, max.Y), MMath::Clamp(v.Z, min.Z, max.Z), MMath::Clamp(v.W, min.W, max.W));
		}

		MFloat32 MVector4::Dot(const MVector4& v1, const MVector4& v2)
		{
			return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z + v1.W * v2.W;
		}

		MFloat32 MVector4::Angle(const MVector4& from, const MVector4& to)
		{
			return MMath::Acos(Dot(from.Normalized(), to.Normalized()));
		}

		MFloat32 MVector4::AngleD(const MVector4& from, const MVector4& to)
		{
			return MMath::AcosD(Dot(from.Normalized(), to.Normalized()));
		}

		MVector4 MVector4::Min(const MVector4 & v1, const MVector4 & v2)
		{
			return MVector4(MMath::Min(v1.X, v2.X), MMath::Min(v1.Y, v2.Y), MMath::Min(v1.Z, v2.Z), MMath::Min(v1.W, v2.W));
		}

		MVector4 MVector4::Max(const MVector4 & v1, const MVector4 & v2)
		{
			return MVector4(MMath::Max(v1.X, v2.X), MMath::Max(v1.Y, v2.Y), MMath::Max(v1.Z, v2.Z), MMath::Max(v1.W, v2.W));
		}

#pragma endregion

#endif
	}
}
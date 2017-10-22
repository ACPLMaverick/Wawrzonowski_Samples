#pragma once
#include "MMath.h"
#include "MString.h"

#include "PhysX\PxPhysicsAPI.h"

namespace morphEngine
{
	namespace utility
	{
		class MVector2
		{
		public:
#pragma region Statics
			static MVector2 Zero;
			static MVector2 Up;
			static MVector2 Down;
			static MVector2 Left;
			static MVector2 Right;
			static MVector2 One;
#pragma endregion

#pragma region Variables
			union
			{
				struct
				{
					MFloat32 X;
					MFloat32 Y;
				};
				physx::PxVec2 PhysXVector;
			};
#pragma endregion

		public:
#pragma region Constructors
			inline MVector2(MFloat32 x = 0.0f, MFloat32 y = 0.0f) : X(x), Y(y) {}
			inline MVector2(const physx::PxVec2& physxOther) : PhysXVector(physxOther) { }
			inline MVector2(const MVector2& other) : X(other.X), Y(other.Y) {}
#pragma endregion

#pragma region Vector operations (Normalize, Normalized, Length etc.)
			//Normalizes vector
			inline void Normalize()
			{
				MFloat32 length = Length();
				if(MMath::Approximately(length, 0) || MMath::Approximately(length, 1))
				{
					return;
				}
				*this /= length;
			}

			//Returns normalized vector
			inline MVector2 Normalized() const
			{
				MVector2 v = (*this);
				v.Normalize();
				return v;
			}

			//Returns magnitude of vector
			inline MFloat32 Length() const
			{
				return MMath::Sqrt(X*X + Y*Y);
			}

			//Returns squared magnitude of vector
			inline MFloat32 LengthSquared() const
			{
				return X*X + Y*Y;
			}

			inline MString ToString() const
			{
				return "[" + MString::FromFloat(X) + ", " + MString::FromFloat(Y) + "]";
			}
#pragma endregion

#pragma region Operators
			inline void operator=(const MVector2& other)
			{
				X = other.X;
				Y = other.Y;
			}

			inline MVector2 operator-() const
			{
				return MVector2(-X, -Y);
			}

			inline MVector2 operator+() const
			{
				return MVector2(X, Y);
			}

			inline MVector2 operator-(const MVector2& other) const
			{
				return MVector2(X - other.X, Y - other.Y);

			}

			inline MVector2 operator+(const MVector2& other) const
			{
				return MVector2(X + other.X, Y + other.Y);
			}

			inline MVector2 operator*(const MVector2& other) const
			{
				return MVector2(X * other.X, Y * other.Y);
			}

			inline MVector2 operator/(const MVector2& other) const
			{
				return MVector2(X / other.X, Y / other.Y);
			}

			inline MVector2 operator*(float s) const
			{
				return MVector2(X * s, Y * s);
			}

			inline MVector2 operator/(float s) const
			{
				return (*this) * (1.0f / s);
			}

			inline void operator+=(const MVector2& other)
			{
				X += other.X;
				Y += other.Y;
			}

			inline void operator-=(const MVector2& other)
			{
				X -= other.X;
				Y -= other.Y;
			}

			inline void operator*=(const MVector2& other)
			{
				X *= other.X;
				Y *= other.Y;
			}

			inline void operator/=(const MVector2& other)
			{
				X /= other.X;
				Y /= other.Y;
			}

			inline void operator*=(float s)
			{
				X *= s;
				Y *= s;
			}

			inline void operator/=(float s)
			{
				(*this) *= (1.0f / s);
			}

			inline bool operator==(const MVector2& other) const
			{
				return MMath::Approximately(X, other.X) && MMath::Approximately(Y, other.Y);
			}

			inline bool operator!=(const MVector2& other) const
			{
				return !(MMath::Approximately(X, other.X) || !(MMath::Approximately(Y, other.Y)));
			}

			inline operator physx::PxVec2() const
			{
				return PhysXVector;
			}

#pragma endregion

#pragma region Static operations (Cross, Dot etc)
			//Returns v1.x*v2.y - v1.y * v2.x
			static MFloat32 Cross(const MVector2& v1, const MVector2& v2 = One);
			//Returns vector between from and to vectors depending on t value (linear interpolation)
			static MVector2 Lerp(const MVector2& from, const MVector2& to, const MFloat32 t);
			//Returns vector between from and to vectors depending on t value (spherical interpolation)
			static MVector2 Slerp(const MVector2& from, const MVector2& to, const MFloat32 t);
			//Clamps all components of vector using min and max components as constraints
			static MVector2 Clamp(const MVector2& v, const MVector2& min, const MVector2& max);
			//Returns dot product of two vectors
			static MFloat32 Dot(const MVector2& v1, const MVector2& v2);
			//Returns angle between two vectors (in radians)
			static MFloat32 Angle(const MVector2& from, const MVector2& to);
			//Returns angle between two vectors (in degrees)
			static MFloat32 AngleD(const MVector2& from, const MVector2& to);
			//Returns a vector which constists of each lesser component from both input vectors.
			static MVector2 Min(const MVector2& v1, const MVector2& v2);
			//Returns a vector which constists of each bigger component from both input vectors.
			static MVector2 Max(const MVector2& v1, const MVector2& v2);

#pragma endregion
		};

#if USE_SIMD

		class MVector3
		{
		public:
#pragma region Statics
			static MVector3 Zero;
			static MVector3 Left;
			static MVector3 Right;
			static MVector3 Down;
			static MVector3 Up;
			static MVector3 Back;
			static MVector3 Forward;
			static MVector3 One;
#pragma endregion

#pragma region Variables
			union
			{
				__m128 SIMDValue;
				struct
				{
					MFloat32 X;
					MFloat32 Y;
					MFloat32 Z;
				};
				physx::PxVec3 PhysXVector;
				MFloat32 Components[3];
			};
#pragma endregion

		public:
#pragma region Constructors
			inline MVector3(MFloat32 x = 0.0f, MFloat32 y = 0.0f, MFloat32 z = 0.0f) : SIMDValue(_mm_set_ps(0, z, y, x))
			{}
			inline MVector3(__m128 simdValue) : SIMDValue(simdValue) {}
			inline MVector3(const MVector2& v, MFloat32 z = 0.0f) : SIMDValue(_mm_set_ps(0, z, v.Y, v.X)) {}
			inline MVector3(const physx::PxVec3& physxOther) : PhysXVector(physxOther) {}
			inline MVector3(const MVector3& v) : SIMDValue(v.SIMDValue) {}
#pragma endregion

#pragma region Vector operations (Normalize, Normalized, Length etc.)
			//Normalizes vector
			inline void Normalize()
			{
				MFloat32 length = Length();
				if(MMath::Approximately(length, 0) || MMath::Approximately(length, 1))
				{
					return;
				}
				SIMDValue = _mm_mul_ps(SIMDValue, _mm_rsqrt_ps(_mm_dp_ps(SIMDValue, SIMDValue, 0x7F)));
			}

			//Returns normalized vector
			inline MVector3 Normalized() const
			{
				MVector3 v = (*this);
				v.Normalize();
				return v;
			}

			//Returns normalized vector
			inline MFloat32 Length() const
			{
				return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(SIMDValue, SIMDValue, 0x71)));
			}

			//Returns squared magnitude of vector
			inline MFloat32 LengthSquared() const
			{
				return _mm_cvtss_f32(_mm_dp_ps(SIMDValue, SIMDValue, 0x71));
			}

			inline MString ToString() const
			{
				return "[" + MString::FromFloat(X) + ", " + MString::FromFloat(Y) + ", " + MString::FromFloat(Z) + "]";
			}
#pragma endregion

#pragma region Operators
			inline void operator=(const MVector2& other)
			{
				SIMDValue = _mm_set_ps(0, 0, other.Y, other.X);
			}

			inline void operator=(const MVector3& other)
			{
				SIMDValue = _mm_set_ps(0, other.Z, other.Y, other.X);
			}

			inline MVector3 operator-() const
			{
				return MVector3(-X, -Y, -Z);
			}

			inline MVector3 operator+() const
			{
				return MVector3(X, Y, Z);
			}

			inline MVector3 operator-(const MVector3& other) const
			{
				return MVector3(X - other.X, Y - other.Y, Z - other.Z);

			}

			inline MVector3 operator+(const MVector3& other) const
			{
				return MVector3(X + other.X, Y + other.Y, Z + other.Z);
			}

			inline MVector3 operator*(const MVector3& other) const
			{
				return MVector3(X * other.X, Y * other.Y, Z * other.Z);
			}

			inline MVector3 operator/(const MVector3& other) const
			{
				return MVector3(X / other.X, Y / other.Y, Z / other.Z);
			}

			inline MVector3 operator*(float s) const
			{
				return MVector3(X * s, Y * s, Z * s);
			}

			inline MVector3 operator/(float s) const
			{
				return (*this) * (1.0f / s);
			}

			inline void operator+=(const MVector3& other)
			{
				SIMDValue = _mm_add_ps(SIMDValue, other.SIMDValue);
			}

			inline void operator-=(const MVector3& other)
			{
				SIMDValue = _mm_sub_ps(SIMDValue, other.SIMDValue);
			}

			inline void operator*=(const MVector3& other)
			{
				SIMDValue = _mm_mul_ps(SIMDValue, other.SIMDValue);
			}

			inline void operator/=(const MVector3& other)
			{
				SIMDValue = _mm_div_ps(SIMDValue, other.SIMDValue);
			}

			inline void operator*=(float s)
			{
				SIMDValue = _mm_mul_ps(SIMDValue, _mm_set_ps1(s));
			}

			inline void operator/=(float s)
			{
				SIMDValue = _mm_div_ps(SIMDValue, _mm_set_ps1(s));
			}

			inline bool operator==(const MVector3& other) const
			{
				return MMath::Approximately(X, other.X) && MMath::Approximately(Y, other.Y) && MMath::Approximately(Z, other.Z);
			}

			inline bool operator!=(const MVector3& other) const
			{
				return !((*this) == other);
			}

			inline MFloat32& operator[](MInt32 index)
			{
				return Components[MMath::Clamp(index, 0, 2)];
			}

			inline MFloat32 operator[](MInt32 index) const
			{
				return Components[MMath::Clamp(index, 0, 2)];
			}

			inline operator physx::PxVec3() const
			{
				return PhysXVector;
			}

#pragma endregion

#pragma region Static operations (Cross, Dot etc)
			//Returns cross product of two vectors
			static MVector3 Cross(const MVector3& v1, const MVector3& v2 = One);
			//Returns vector between from and to vectors depending on t value (linear interpolation)
			static MVector3 Lerp(const MVector3& from, const MVector3& to, const MFloat32 t);
			//Returns vector between from and to vectors depending on t value (spherical interpolation)
			static MVector3 Slerp(const MVector3& from, const MVector3& to, const MFloat32 t);
			//Clamps all components of vector using min and max components as constraints
			static MVector3 Clamp(const MVector3& v, const MVector3& min, const MVector3& max);
			//Returns vector consisting of abs of each input component
			static MVector3 Abs(const MVector3& v);
			//Returns dot product of two vectors
			static MFloat32 Dot(const MVector3& v1, const MVector3& v2);
			//Returns angle between two vectors (in radians)
			static MFloat32 Angle(const MVector3& from, const MVector3& to);
			//Returns angle between two vectors (in degrees)
			static MFloat32 AngleD(const MVector3& from, const MVector3& to);
#pragma endregion
		};

		class MVector4
		{
		public:
#pragma region Statics
			static MVector4 Zero;
			static MVector4 One;
#pragma endregion

#pragma region Variables
			union
			{
				__m128 SIMDValue;
				struct
				{
					MFloat32 X;
					MFloat32 Y;
					MFloat32 Z;
					MFloat32 W;
				};
				physx::PxVec4 PhysXVector;
				MFloat32 Components[4];
			};
#pragma endregion

		public:
#pragma region Constructors
			inline MVector4(MFloat32 x = 0.0f, MFloat32 y = 0.0f, MFloat32 z = 0.0f, MFloat32 w = 0.0f) : SIMDValue(_mm_set_ps(w, z, y, x)) {}
			inline MVector4(__m128 simdValue) : SIMDValue(simdValue) {}
			inline MVector4(const MVector2& v, MFloat32 z = 0.0f, MFloat32 w = 0.0f) : SIMDValue(_mm_set_ps(w, z, v.Y, v.X)) {}
			inline MVector4(const MVector3& v, MFloat32 w = 0.0f) : SIMDValue(_mm_set_ps(w, v.Z, v.Y, v.X)) {}
			inline MVector4(const physx::PxVec4& physxOther) : PhysXVector(physxOther) {}
			inline MVector4(const MVector4& v) : SIMDValue(v.SIMDValue) {}
#pragma endregion

#pragma region Vector operations (Normalize, Normalized, Length etc.)
			//Normalizes vector
			inline void Normalize()
			{
				MFloat32 length = Length();
				if(MMath::Approximately(length, 0) || MMath::Approximately(length, 1))
				{
					return;
				}
				SIMDValue = _mm_mul_ps(SIMDValue, _mm_rsqrt_ps(_mm_dp_ps(SIMDValue, SIMDValue, 0xFF)));
			}

			//Returns normalized vector
			inline MVector4 Normalized() const
			{
				MVector4 v = (*this);
				v.Normalize();
				return v;
			}

			//Returns magnitude of vector
			inline MFloat32 Length() const
			{
				return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(SIMDValue, SIMDValue, 0xF1)));
			}

			//Returns squared magnitude of vector
			inline MFloat32 LengthSquared() const
			{
				return _mm_cvtss_f32(_mm_dp_ps(SIMDValue, SIMDValue, 0xF1));
			}

			inline MString ToString() const
			{
				return "[" + MString::FromFloat(X) + ", " + MString::FromFloat(Y) + ", " + MString::FromFloat(Z) + ", " + MString::FromFloat(W) + "]";
			}
#pragma endregion

#pragma region Operators
			inline void operator=(const MVector2& other)
			{
				SIMDValue = _mm_set_ps(0, 0, other.Y, other.X);
			}

			inline void operator=(const MVector3& other)
			{
				SIMDValue = _mm_set_ps(0, other.Z, other.Y, other.X);
			}

			inline void operator=(const MVector4& other)
			{
				SIMDValue = _mm_set_ps(other.W, other.Z, other.Y, other.X);
			}

			inline MVector4 operator-() const
			{
				return MVector4(-X, -Y, -Z, -W);
			}

			inline MVector4 operator+() const
			{
				return MVector4(X, Y, Z, W);
			}

			inline MVector4 operator-(const MVector4& other) const
			{
				return MVector4(X - other.X, Y - other.Y, Z - other.Z, W - other.W);
			}

			inline MVector4 operator+(const MVector4& other) const
			{
				return MVector4(X + other.X, Y + other.Y, Z + other.Z, W + other.W);
			}

			inline MVector4 operator*(const MVector4& other) const
			{
				return MVector4(X * other.X, Y * other.Y, Z * other.Z, W * other.W);
			}

			inline MVector4 operator/(const MVector4& other) const
			{
				return MVector4(X / other.X, Y / other.Y, Z / other.Z, W / other.W);
			}

			inline MVector4 operator*(float s) const
			{
				return MVector4(X * s, Y * s, Z * s, W * s);
			}

			inline MVector4 operator/(float s) const
			{
				return (*this) * (1.0f / s);
			}

			inline void operator+=(const MVector4& other)
			{
				SIMDValue = _mm_add_ps(SIMDValue, other.SIMDValue);
			}

			inline void operator-=(const MVector4& other)
			{
				SIMDValue = _mm_sub_ps(SIMDValue, other.SIMDValue);
			}

			inline void operator*=(const MVector3& other)
			{
				SIMDValue = _mm_mul_ps(SIMDValue, other.SIMDValue);
			}

			inline void operator/=(const MVector4& other)
			{
				SIMDValue = _mm_div_ps(SIMDValue, other.SIMDValue);
			}

			inline void operator*=(float s)
			{
				SIMDValue = _mm_mul_ps(SIMDValue, _mm_set_ps1(s));
			}

			inline void operator/=(float s)
			{
				(*this) *= (1.0f / s);
			}

			inline bool operator==(const MVector4& other) const
			{
				return MMath::Approximately(X, other.X) && MMath::Approximately(Y, other.Y) && MMath::Approximately(Z, other.Z) && MMath::Approximately(W, other.W);
			}

			inline bool operator!=(const MVector4& other) const
			{
				return !((*this) == other);
			}

			inline operator MVector3() const
			{
				return MVector3(X, Y, Z);
			}

			inline MFloat32& operator[](MInt32 index)
			{
				return Components[MMath::Clamp(index, 0, 3)];
			}

			inline MFloat32 operator[](MInt32 index) const
			{
				return Components[MMath::Clamp(index, 0, 3)];
			}

			inline operator physx::PxVec4() const
			{
				return PhysXVector;
			}

#pragma endregion

#pragma region Static operations (Cross, Dot etc)
			//Returns vector between from and to vectors depending on t value (linear interpolation)
			static MVector4 Lerp(const MVector4& from, const MVector4& to, const MFloat32 t);
			//Returns vector between from and to vectors depending on t value (spherical interpolation)
			static MVector4 Slerp(const MVector4& from, const MVector4& to, const MFloat32 t);
			//Clamps all components of vector using min and max components as constraints
			static MVector4 Clamp(const MVector4& v, const MVector4& min, const MVector4& max);
			//Returns dot product of two vectors
			static MFloat32 Dot(const MVector4& v1, const MVector4& v2);
			//Returns angle between two vectors (in radians)
			static MFloat32 Angle(const MVector4& from, const MVector4& to);
			//Returns angle between two vectors (in degrees)
			static MFloat32 AngleD(const MVector4& from, const MVector4& to);
#pragma endregion
		};

#else //USE_SIMD == 0

		class MVector3
		{
		public:
#pragma region Statics
			static MVector3 Zero;
			static MVector3 Up;
			static MVector3 Down;
			static MVector3 Left;
			static MVector3 Right;
			static MVector3 Back;
			static MVector3 Forward;
			static MVector3 One;
#pragma endregion

#pragma region Variables
			union
			{
				struct
				{
					MFloat32 X;
					MFloat32 Y;
					MFloat32 Z;
				};
				physx::PxVec3 PhysXVector;
				MFloat32 Components[3];
			};
#pragma endregion

		public:
#pragma region Constructors
			inline MVector3(MFloat32 x = 0.0f, MFloat32 y = 0.0f, MFloat32 z = 0.0f) : X(x), Y(y), Z(z) {}
			inline MVector3(const MVector2& v, MFloat32 z = 0.0f) : X(v.X), Y(v.Y), Z(z) {}
			inline MVector3(const physx::PxVec3& physxOther) : PhysXVector(physxOther) {}
			inline MVector3(const MVector3& v) : X(v.X), Y(v.Y), Z(v.Z) {}
#pragma endregion

#pragma region Vector operations (Normalize, Normalized, Length etc.)
			//Normalizes vector
			inline void Normalize()
			{
				MFloat32 length = Length();
				if(MMath::Approximately(length, 0) || MMath::Approximately(length, 1))
				{
					return;
				}
				*this /= length;
			}

			//Returns normalized vector
			inline MVector3 Normalized() const
			{
				MVector3 v = (*this);
				v.Normalize();
				return v;
			}

			//Returns magnitude of vector
			inline MFloat32 Length() const
			{
				return MMath::Sqrt(X*X + Y*Y + Z*Z);
			}

			//Returns squared magnitude of vector
			inline MFloat32 LengthSquared() const
			{
				return X*X + Y*Y;
			}

			inline MString ToString() const
			{
				return "[" + MString::FromFloat(X) + ", " + MString::FromFloat(Y) + ", " + MString::FromFloat(Z) + "]";
			}
#pragma endregion

#pragma region Operators
			inline void operator=(const MVector2& other)
			{
				X = other.X;
				Y = other.Y;
				Z = 0.0f;
			}

			inline void operator=(const MVector3& other)
			{
				X = other.X;
				Y = other.Y;
				Z = other.Z;
			}

			inline MVector3 operator-() const
			{
				return MVector3(-X, -Y, -Z);
			}

			inline MVector3 operator+() const
			{
				return MVector3(X, Y, Z);
			}

			inline MVector3 operator-(const MVector3& other) const
			{
				return MVector3(X - other.X, Y - other.Y, Z - other.Z);

			}

			inline MVector3 operator+(const MVector3& other) const
			{
				return MVector3(X + other.X, Y + other.Y, Z + other.Z);
			}

			inline MVector3 operator*(const MVector3& other) const
			{
				return MVector3(X * other.X, Y * other.Y, Z * other.Z);
			}

			inline MVector3 operator/(const MVector3& other) const
			{
				return MVector3(X / other.X, Y / other.Y, Z / other.Z);
			}

			inline MVector3 operator*(float s) const
			{
				return MVector3(X * s, Y * s, Z * s);
			}

			inline MVector3 operator/(float s) const
			{
				return (*this) * (1.0f / s);
			}

			inline void operator+=(const MVector3& other)
			{
				X += other.X;
				Y += other.Y;
				Z += other.Z;
			}

			inline void operator-=(const MVector3& other)
			{
				X -= other.X;
				Y -= other.Y;
				Z -= other.Z;
			}

			inline void operator*=(const MVector3& other)
			{
				X *= other.X;
				Y *= other.Y;
				Z *= other.Z;
			}

			inline void operator/=(const MVector3& other)
			{
				X /= other.X;
				Y /= other.Y;
				Z /= other.Z;
			}

			inline void operator*=(float s)
			{
				X *= s;
				Y *= s;
				Z *= s;
			}

			inline void operator/=(float s)
			{
				(*this) *= (1.0f / s);
			}

			inline bool operator==(const MVector3& other) const
			{
				return MMath::Approximately(X, other.X) && MMath::Approximately(Y, other.Y) && MMath::Approximately(Z, other.Z);
			}

			inline bool operator!=(const MVector3& other) const
			{
				return !((*this) == other);
			}

			inline MFloat32& operator[](MInt32 index)
			{
				return Components[MMath::Clamp(index, 0, 2)];
			}

			inline MFloat32 operator[](MInt32 index) const
			{
				return Components[MMath::Clamp(index, 0, 2)];
			}

			inline operator physx::PxVec3() const
			{
				return PhysXVector;
			}

#pragma endregion

#pragma region Static operations (Cross, Dot etc)
			//Returns cross product of two vectors
			static MVector3 Cross(const MVector3& from, const MVector3& to = One);
			//Returns vector between from and to vectors depending on t value (linear interpolation)
			static MVector3 Lerp(const MVector3& from, const MVector3& to, const MFloat32 t);
			//Returns vector between from and to vectors depending on t value (spherical interpolation)
			static MVector3 Slerp(const MVector3& from, const MVector3& to, const MFloat32 t);
			//Clamps all components of vector using min and max components as constraints
			static MVector3 Clamp(const MVector3& v, const MVector3& min, const MVector3& max);
			//Returns vector consisting of abs of each input component
			static MVector3 Abs(const MVector3& v);
			//Returns dot product of two vectors
			static MFloat32 Dot(const MVector3& v1, const MVector3& v2);
			//Returns angle between two vectors (in radians)
			static MFloat32 Angle(const MVector3& from, const MVector3& to);
			//Returns angle between two vectors (in degrees)
			static MFloat32 AngleD(const MVector3& from, const MVector3& to);
			//Returns a vector which constists of each lesser component from both input vectors.
			static MVector3 Min(const MVector3& v1, const MVector3& v2);
			//Returns a vector which constists of each bigger component from both input vectors.
			static MVector3 Max(const MVector3& v1, const MVector3& v2);

#pragma endregion
		};

		class MVector4
		{
		public:
#pragma region Statics
			static MVector4 Zero;
			static MVector4 One;
#pragma endregion

#pragma region Variables
			union
			{
				struct
				{
					MFloat32 X;
					MFloat32 Y;
					MFloat32 Z;
					MFloat32 W;
				};
				physx::PxVec4 PhysXVector;
				MFloat32 Components[4];
			};
#pragma endregion

		public:
#pragma region Constructors
			inline MVector4(MFloat32 x = 0.0f, MFloat32 y = 0.0f, MFloat32 z = 0.0f, MFloat32 w = 0.0f) : X(x), Y(y), Z(z), W(w) {}
			inline MVector4(const MVector2& v, MFloat32 z = 0.0f, MFloat32 w = 0.0f) : X(v.X), Y(v.Y), Z(z), W(w) {}
			inline MVector4(const MVector3& v, MFloat32 w = 0.0f) : X(v.X), Y(v.Y), Z(v.Z), W(w) {}
			inline MVector4(const physx::PxVec4& physxOther) : PhysXVector(physxOther) {}
			inline MVector4(const MVector4& v) : X(v.X), Y(v.Y), Z(v.Z), W(v.W) {}
#pragma endregion

#pragma region Vector operations (Normalize, Normalized, Length etc.)
			//Normalizes vector
			inline void Normalize()
			{
				MFloat32 length = Length();
				if(MMath::Approximately(length, 0) || MMath::Approximately(length, 1))
				{
					return;
				}
				*this /= length;
			}

			//Returns normalized vector
			inline MVector4 Normalized() const
			{
				MVector4 v = (*this);
				v.Normalize();
				return v;
			}

			//Returns magnitude of vector
			inline MFloat32 Length() const
			{
				return MMath::Sqrt(X*X + Y*Y + Z*Z);
			}

			//Returns squared magnitude of vector
			inline MFloat32 LengthSquared() const
			{
				return X*X + Y*Y;
			}

			inline MString ToString() const
			{
				return "[" + MString::FromFloat(X) + ", " + MString::FromFloat(Y) + ", " + MString::FromFloat(Z) + ", " + MString::FromFloat(W) + "]";
			}
#pragma endregion

#pragma region Operators
			inline void operator=(const MVector2& other)
			{
				X = other.X;
				Y = other.Y;
				Z = 0.0f;
				W = 0.0f;
			}

			inline void operator=(const MVector3& other)
			{
				X = other.X;
				Y = other.Y;
				Z = other.Z;
				W = 0.0f;
			}

			inline void operator=(const MVector4& other)
			{
				X = other.X;
				Y = other.Y;
				Z = other.Z;
				W = other.W;
			}

			inline MVector4 operator-() const
			{
				return MVector4(-X, -Y, -Z, -W);
			}

			inline MVector4 operator+() const
			{
				return MVector4(X, Y, Z, W);
			}

			inline MVector4 operator-(const MVector4& other) const
			{
				return MVector4(X - other.X, Y - other.Y, Z - other.Z, W - other.W);

			}

			inline MVector4 operator+(const MVector4& other) const
			{
				return MVector4(X + other.X, Y + other.Y, Z + other.Z, W + other.W);
			}

			inline MVector4 operator*(const MVector4& other) const
			{
				return MVector4(X * other.X, Y * other.Y, Z * other.Z, W * other.W);
			}

			inline MVector4 operator/(const MVector4& other) const
			{
				return MVector4(X / other.X, Y / other.Y, Z / other.Z, W / other.W);
			}

			inline MVector4 operator*(float s) const
			{
				return MVector4(X * s, Y * s, Z * s, W * s);
			}

			inline MVector4 operator/(float s) const
			{
				return (*this) * (1.0f / s);
			}

			inline void operator+=(const MVector4& other)
			{
				X += other.X;
				Y += other.Y;
				Z += other.Z;
				W += other.W;
			}

			inline void operator-=(const MVector4& other)
			{
				X -= other.X;
				Y -= other.Y;
				Z -= other.Z;
				W -= other.W;
			}

			inline void operator*=(const MVector3& other)
			{
				X *= other.X;
				Y *= other.Y;
				Z *= other.Z;
			}

			inline void operator/=(const MVector4& other)
			{
				X /= other.X;
				Y /= other.Y;
				Z /= other.Z;
				W /= other.W;
			}

			inline void operator*=(float s)
			{
				X *= s;
				Y *= s;
				Z *= s;
				W *= s;
			}

			inline void operator/=(float s)
			{
				(*this) *= (1.0f / s);
			}

			inline bool operator==(const MVector4& other) const
			{
				return MMath::Approximately(X, other.X) && MMath::Approximately(Y, other.Y) && MMath::Approximately(Z, other.Z) && MMath::Approximately(W, other.W);
			}

			inline bool operator!=(const MVector4& other) const
			{
				return !((*this) == other);
			}

			inline operator MVector3() const
			{
				return MVector3(X, Y, Z);
			}

			inline MFloat32& operator[](MInt32 index)
			{
				return Components[MMath::Clamp(index, 0, 3)];
			}

			inline MFloat32 operator[](MInt32 index) const
			{
				return Components[MMath::Clamp(index, 0, 3)];
			}

			inline operator physx::PxVec4() const
			{
				return PhysXVector;
			}

#pragma endregion

#pragma region Static operations (Cross, Dot etc)
			//Returns vector between from and to vectors depending on t value (linear interpolation)
			static MVector4 Lerp(const MVector4& from, const MVector4& to, const MFloat32 t);
			//Returns vector between from and to vectors depending on t value (spherical interpolation)
			static MVector4 Slerp(const MVector4& from, const MVector4& to, const MFloat32 t);
			//Clamps all components of vector using min and max components as constraints
			static MVector4 Clamp(const MVector4& v, const MVector4& min, const MVector4& max);
			//Returns dot product of two vectors
			static MFloat32 Dot(const MVector4& v1, const MVector4& v2);
			//Returns angle between two vectors (in radians)
			static MFloat32 Angle(const MVector4& from, const MVector4& to);
			//Returns angle between two vectors (in degrees)
			static MFloat32 AngleD(const MVector4& from, const MVector4& to);
			//Returns a vector which constists of each lesser component from both input vectors.
			static MVector4 Min(const MVector4& v1, const MVector4& v2);
			//Returns a vector which constists of each bigger component from both input vectors.
			static MVector4 Max(const MVector4& v1, const MVector4& v2);

#pragma endregion
		};

#endif
	}
}
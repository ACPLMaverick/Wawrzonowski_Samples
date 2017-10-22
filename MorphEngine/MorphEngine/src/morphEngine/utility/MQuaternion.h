#pragma once

#include "MMath.h"
#include "MVector.h"

namespace morphEngine
{
	namespace utility
	{
		class MMatrix;

		//Quaternion class
		class MQuaternion
		{
		public:
			union
			{
				struct
				{
					union
					{
						struct
						{
							MFloat32 X;
							MFloat32 Y;
							MFloat32 Z;
						};
						MVector3 V;
					};
					MFloat32 W;
				};
				physx::PxQuat PhysxQuaternion;
			};

		public:
			inline MQuaternion() : X(0.0f), Y(0.0f), Z(0.0f), W(1.0f) {}
			inline MQuaternion(MFloat32 x, MFloat32 y, MFloat32 z, MFloat32 w) : X(x), Y(y), Z(z), W(w) {}
			inline MQuaternion(MFloat32 w, const MVector3& v) : V(v), W(w) {}
			inline MQuaternion(const MVector4& v) : V(v), W(v.W) {}
			inline MQuaternion(const MQuaternion& other) : X(other.X), Y(other.Y), Z(other.Z), W(other.W) {}
			inline MQuaternion(const physx::PxQuat& physxQuaternion) : PhysxQuaternion(physxQuaternion) {}
			inline ~MQuaternion() {}

			//Can;t inline due to incomplete MMatrix class (#include "MMatrix.h" will cause circular includes)
			MMatrix ToMatrix() const;

			//Returns magnitude of quaterion
			inline MFloat32 Length() const
			{
				return MMath::Sqrt(X * X + Y * Y + Z * Z + W * W);
			}

			//Returns squared magnitude of quaternion
			inline MFloat32 LengthSquared() const
			{
				return X * X + Y * Y + Z * Z + W * W;
			}

			//Normalizes quaternion
			inline void Normalize()
			{
				MFloat32 length = Length();
				if(MMath::Approximately(length, 0) || MMath::Approximately(length, 1))
				{
					return;
				}

				(*this) *= (1.0f / length);
			}

			//Returns normalized quaternion
			inline MQuaternion Normalized() const
			{
				MFloat32 length = Length();
				if(MMath::Approximately(length, 0) || MMath::Approximately(length, 1))
				{
					return *this;
				}

				return *this * (1.0f / length);
			}

			inline MString ToString() const
			{
				return "[" + MString::FromFloat(X) + ", " + MString::FromFloat(Y) + ", " + MString::FromFloat(Z) + ", " + MString::FromFloat(W) + "]";
			}

			//Returns conjugate of a quaternion
			inline MQuaternion Conjugate() const
			{
				return MQuaternion(-X, -Y, -Z, W);
			}

			inline MQuaternion Inversed() const
			{
				MFloat32 length = Length();
				if(MMath::Approximately(length, 0))
				{
					length = 1;
				}
				return Conjugate() / length;
			}

			inline MVector3 ToEuler() const
			{
				MFloat32 x, y, z;
				MFloat32 ysqr = Y * Y;

				// y (y-axis rotation)
				MFloat32 thres = 0.9999f;
				MFloat32 t2 = 2.0f * (X * Y + Z * W);

				if (t2 > thres)
				{
					x = MMath::PIOver2;
					y = 2.0f * MMath::Atan2(Y, W);
					z = 0.0f;
				}
				else if (t2 < -thres)
				{
					x = -MMath::PIOver2;
					y = -2.0f * MMath::Atan2(Y, W);
					z = 0.0f;
				}
				else
				{
					x = MMath::Asin(t2);

					y = MMath::Atan2(2.0f * Y * W - 2.0f * X * Z,
						1.0f - 2.0f * Y * Y - 2.0f * Z * Z);

					z = MMath::Atan2(2.0f * X * W - 2.0f * Y * Z,
						1.0f - 2.0f * X * X - 2.0f * Z * Z);

				}

				return MVector3(x, y, z) * MMath::Rad2Deg;
			}

			//Returns quaternion constructed from given pitch, yaw and roll angles
			//All angles have to be given in degrees
			// Commented out until further notice.
			static inline MQuaternion FromPitchYawRoll(MFloat32 pitch, MFloat32 yaw, MFloat32 roll)
			{
				const MFloat32 pitchSin = MMath::SinD(pitch * 0.5f);
				const MFloat32 pitchCos = MMath::CosD(pitch * 0.5f);
				const MFloat32 yawSin = MMath::SinD(yaw * 0.5f);
				const MFloat32 yawCos = MMath::CosD(yaw * 0.5f);
				const MFloat32 rollSin = MMath::SinD(roll * 0.5f);
				const MFloat32 rollCos = MMath::CosD(roll * 0.5f);

				return MQuaternion(
					pitchCos * yawSin * rollSin + pitchSin * yawCos * rollCos,
					-pitchSin * yawCos * rollSin + pitchCos * yawSin * rollCos,
					-pitchSin * yawSin * rollCos + pitchCos * yawCos * rollSin,
					pitchSin * yawSin * rollSin + pitchCos * yawCos * rollCos
				);
			}

			//Returns quaternion constructed from given axis and angle
			//Angle has to be given in degrees
			//Commented out until further notice.
			static inline MQuaternion FromAxisAngle(const MVector3& axis, MFloat32 angle)
			{
				MVector3 axisNormalized = axis.Normalized();
				MFloat32 sinA = MMath::SinD(angle * 0.5f);
				MFloat32 cosA = MMath::CosD(angle * 0.5f);

				return MQuaternion(axisNormalized.X * sinA, axisNormalized.Y * sinA, axisNormalized.Z * sinA, cosA);
			}

			//Returns quaternion constructed from given Euler angles
			//All angles have to be given in degrees
			static MQuaternion FromEuler(MFloat32 x, MFloat32 y, MFloat32 z)
			{
				MQuaternion q;

				MFloat32 c1 = MMath::CosD(y * 0.5f);
				MFloat32 c2 = MMath::CosD(x * 0.5f);
				MFloat32 c3 = MMath::CosD(z * 0.5f);
				MFloat32 s1 = MMath::SinD(y * 0.5f);
				MFloat32 s2 = MMath::SinD(x * 0.5f);
				MFloat32 s3 = MMath::SinD(z * 0.5f);

				q.W = c1 * c2 * c3 - s1 * s2 * s3;
				q.X = s1 * s2 * c3 + c1 * c2 * s3;
				q.Y = s1 * c2 * c3 + c1 * s2 * s3;
				q.Z = c1 * s2 * c3 - s1 * c2 * s3;

				return q;
			}

			//Returns quaternion constructed from given Euler angles
			//All angles have to be given in degrees
			static MQuaternion FromEuler(MVector3 eulerAngles)
			{
				return FromEuler(eulerAngles.X, eulerAngles.Y, eulerAngles.Z);
			}

			//Returns quaternion in between of from and to quaternions based on t parameter (linear interpolation)
			static inline MQuaternion Lerp(const MQuaternion& from, const MQuaternion& to, MFloat32 t)
			{
				const MFloat32 tClamped = MMath::Clamp(t);
				const MFloat32 oneMinusTClamped = 1.0f - tClamped;
				return (from * (1.0f - tClamped) + to * tClamped).Normalized();
			}

			//Returns quaternion in between of from and to quaternions based on t parameter (spherical interpolation)
			static inline MQuaternion Slerp(const MQuaternion& from, const MQuaternion& to, MFloat32 t)
			{
				const MFloat32 tClamped = MMath::Clamp(t);
				const MFloat32 theta = MMath::Acos(from.X * to.X + from.Y * to.Y + from.Z * to.Z + from.W * to.W);
				const MFloat32 sin = MMath::Sin(theta);
				const MFloat32 wa = MMath::Sin((1.0f - tClamped) * theta) / sin;
				const MFloat32 wb = MMath::Sin(t * theta) / sin;
				return (from * wa + to * wb).Normalized();
			}

			static MFloat32 ClampAxis(MFloat32 angle)
			{
				// returns Angle in the range (-360,360)
				angle = MMath::Fmod(angle, 360.0f);

				if (angle < 0.0f)
				{
					// shift to [0,360) range
					angle += 360.f;
				}

				return angle;
			}

			static inline MFloat32 NormalizeAxis(MFloat32 angle)
			{
				// returns Angle in the range [0,360)
				angle = ClampAxis(angle);

				if (angle > 180.0f)
				{
					// shift to (-180,180]
					angle -= 360.0f;
				}
				return angle;
			}

			inline bool operator==(const MQuaternion& other) const
			{
				return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
			}

			inline bool operator!=(const MQuaternion& other) const
			{
				return X != other.X || Y != other.Y || Z != other.Z || W != other.W;
			}

			inline void operator=(const MQuaternion& other)
			{
				X = other.X;
				Y = other.Y;
				Z = other.Z;
				W = other.W;
			}

			inline MQuaternion operator*(MFloat32 s) const
			{
				return MQuaternion(X * s, Y * s, Z * s, W * s);
			}

			inline void operator*=(MFloat32 s)
			{
				X *= s;
				Y *= s;
				Z *= s;
				W *= s;
			}

			inline MQuaternion operator/(MFloat32 s) const
			{
				return (*this) * (1.0f / s);
			}

			inline void operator/=(MFloat32 s)
			{
				(*this) *= (1.0f / s);
			}

			MQuaternion operator*(const MQuaternion& other) const
			{
				return MQuaternion
				(
					X * other.W + Y * other.Z - Z * other.Y + W * other.X,
					-X * other.Z + Y * other.W + Z * other.X + W * other.Y,
					X * other.Y - Y * other.X + Z * other.W + W * other.Z,
					-X * other.X - Y * other.Y - Z * other.Z + W * other.W
				);
			}

			inline void operator*=(const MQuaternion& other)
			{
				MQuaternion multiplied = *this * other;
				*this = multiplied;
			}

			//Rotates given vector by quaternion
			MVector3 operator*(const MVector3& v) const
			{
				MVector3 g = -MVector3(Z, Y, X);
				return g * 2.0f * MVector3::Dot(g, v) + v * (W * W - MVector3::Dot(g, g)) + MVector3::Cross(g, v) * W * 2.0f;
			}

			//Rotates given vector by quaternion
			inline MVector4 operator*(const MVector4& v) const
			{
				return (*this) * MVector3(v);
			}

		protected:
			inline MQuaternion operator+(const MQuaternion& other) const
			{
				return MQuaternion(X + other.X, Y + other.Y, Z + other.Z, W + other.W);
			}
		};
	}
}
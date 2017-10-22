#pragma once

#include "MVector.h"
#include "MQuaternion.h"

namespace morphEngine
{
	namespace utility
	{
		//4x4 Matrix class
		class MMatrix
		{
		protected:
			MVector4 _rows[4];

		public:
			static MMatrix Identity;

			MMatrix(MFloat32 value = 0);
			MMatrix(const MMatrix& other);
			MMatrix(MVector4 rows[4]);
			MMatrix(MFloat32 elements[16]);
			MMatrix(MFloat32 m11, MFloat32 m12, MFloat32 m13, MFloat32 m14,
					MFloat32 m21, MFloat32 m22, MFloat32 m23, MFloat32 m24,
					MFloat32 m31, MFloat32 m32, MFloat32 m33, MFloat32 m34,
					MFloat32 m41, MFloat32 m42, MFloat32 m43, MFloat32 m44);

			//Returns inverted matrix
			MMatrix Inverted() const;
			//Inverts matrix (consider using Inverted() version since it should be faster)
			void Inverse();
			//Returns determinant of matrix
			MFloat32 Determinant() const;
			//Returns determinant of matrix replicated to each component of MVector4
			MVector4 DeterminantVector() const;
			//Converts matrix to quaternion
			MQuaternion ToQuaternion() const;

			//Returns transposed matrix
			inline MMatrix Transposed() const
			{
				MMatrix transposed;
				for(MInt32 i = 0; i < 4; ++i)
				{
					transposed._rows[i][0] = _rows[0][i];
					transposed._rows[i][1] = _rows[1][i];
					transposed._rows[i][2] = _rows[2][i];
					transposed._rows[i][3] = _rows[3][i];
				}
				return transposed;
			}

			//Transposes matrix
			inline void Transpose()
			{
				for(MInt32 i = 0; i < 4; ++i)
				{
					for(MInt32 j = 0; j <= i; ++j)
					{
						MFloat32 tmp = _rows[i][j];
						_rows[i][j] = _rows[j][i];
						_rows[j][i] = tmp;
					}
				}
			}

			inline MString ToString()
			{
				MString s = "";
				for(MInt32 i = 0; i < 4; ++i)
				{
					s += MString::FromFloat(_rows[i][0]) + " | " + MString::FromFloat(_rows[i][1]) + " | " + MString::FromFloat(_rows[i][2]) + " | " + MString::FromFloat(_rows[i][3]);
					if(i < 3)
					{
						s += "\n";
					}
				}
				return s;
			}

			//Returns view matrix
			static inline MMatrix LookAt(const MVector3& eyePosition, const MVector3& targetPosition, const MVector3& upDirection)
			{
				MVector3 forward = (targetPosition - eyePosition).Normalized();
				MVector3 right = MVector3::Cross(upDirection, forward).Normalized();
				MVector3 up = MVector3::Cross(forward, right);

				MMatrix lookAt(0.0f);
				lookAt[0] = MVector4(right.X, up.X, forward.X, 0.0f);
				lookAt[1] = MVector4(right.Y, up.Y, forward.Y, 0.0f);
				lookAt[2] = MVector4(right.Z, up.Z, forward.Z, 0.0f);
				lookAt[3] = MVector4(-MVector3::Dot(right, eyePosition), -MVector3::Dot(up, eyePosition), -MVector3::Dot(forward, eyePosition), 1.0f);
				return lookAt;
			}

			//Returns orthogonal matrix
			static inline MMatrix OrthoLH(MFloat32 width, MFloat32 height, MFloat32 nearZ, MFloat32 farZ)
			{
				MFloat32 zRange = 1.0f / (farZ - nearZ);

				MMatrix ortho(0.0f);
				ortho[0][0] = 2.0f / width;
				ortho[1][1] = 2.0f / height;
				ortho[2][2] = zRange;
				ortho[3][2] = -zRange * nearZ;
				ortho[3][3] = 1.0f;
				return ortho;
			}

			//Returns perspective matrix
			//fovAngleY has to be given in degrees
			static inline MMatrix PerspectiveFovLH(MFloat32 fovAngleY, MFloat32 aspectRatio, MFloat32 nearZ, MFloat32 farZ)
			{
				MFloat32 fovSin = MMath::SinD(fovAngleY * 0.5f);
				MFloat32 fovCos = MMath::CosD(fovAngleY * 0.5f);
				MFloat32 height = fovCos / fovSin;
				MFloat32 width = height / (aspectRatio);
				MFloat32 zRange = farZ / (farZ - nearZ);

				MMatrix perspective(0.0f);
				perspective[0][0] = width;
				perspective[1][1] = height;
				perspective[2][2] = zRange;
				perspective[2][3] = 1.0f;
				perspective[3][2] = -zRange * nearZ;
				return perspective;
			}

			//Returns perspective matrix
			static inline MMatrix PerspectiveLH(MFloat32 width, MFloat32 height, MFloat32 nearZ, MFloat32 farZ)
			{
				MFloat32 twoNearZ = nearZ + nearZ;
				MFloat32 zRange = farZ / (farZ - nearZ);
				MMatrix perspective(0.0f);
				perspective[0][0] = twoNearZ / width;
				perspective[1][1] = twoNearZ / height;
				perspective[2][2] = zRange;
				perspective[2][3] = 1.0f;
				perspective[3][2] = -zRange * nearZ;
				return perspective;
			}

			//Returns matrix constructed from translation
			static inline MMatrix Translation(MFloat32 x, MFloat32 y, MFloat32 z)
			{
				MMatrix translation = MMatrix::Identity;
				translation[0][3] = x;
				translation[1][3] = y;
				translation[2][3] = z;
				return translation;
			}

			//Returns matrix constructed from translation
			static inline MMatrix Translation(const MVector3& translation)
			{
				MMatrix translationMatrix = MMatrix::Identity;
				translationMatrix[0][3] = translation.X;
				translationMatrix[1][3] = translation.Y;
				translationMatrix[2][3] = translation.Z;
				return translationMatrix;
			}

			//Returns matrix scaled by s (uniform scaling)
			static inline MMatrix Scale(MFloat32 s)
			{
				MMatrix scale(MMatrix::Identity);
				scale[0][0] = s;
				scale[1][1] = s;
				scale[2][2] = s;
				return scale;
				//return MMatrix::Identity * s; // Ÿle kurwa
			}

			//Returns matrix scaled by parameters (non-uniform)
			static inline MMatrix Scale(MFloat32 sX, MFloat32 sY, MFloat32 sZ)
			{
				MMatrix scale = MMatrix::Identity;
				scale[0][0] *= sX;
				scale[1][1] *= sY;
				scale[2][2] *= sZ;
				return scale;
			}

			//Returns matrix scaled by components of scale vector (non-uniform)
			static inline MMatrix Scale(const MVector3& scale)
			{
				MMatrix scaleMatrix = MMatrix::Identity;
				scaleMatrix[0][0] *= scale.X;
				scaleMatrix[1][1] *= scale.Y;
				scaleMatrix[2][2] *= scale.Z;
				return scaleMatrix;
			}

			//Rotation around x axis
			//angle has to be given in degrees
			static inline MMatrix RotationX(MFloat32 angle)
			{
				MFloat32 angleSin = MMath::Sin(angle * MMath::Deg2Rad);
				MFloat32 angleCos = MMath::Cos(angle * MMath::Deg2Rad);

				MMatrix rotationMatrix = MMatrix::Identity;

				rotationMatrix[1][1] = angleCos;
				rotationMatrix[1][2] = angleSin;
				rotationMatrix[2][1] = -angleSin;
				rotationMatrix[2][2] = angleCos;

				return rotationMatrix;
			}

			//Rotation around y axis
			//angle has to be given in degrees
			static inline MMatrix RotationY(MFloat32 angle)
			{
				MFloat32 angleSin = MMath::Sin(angle * MMath::Deg2Rad);
				MFloat32 angleCos = MMath::Cos(angle * MMath::Deg2Rad);

				MMatrix rotationMatrix = MMatrix::Identity;

				rotationMatrix[0][0] = angleCos;
				rotationMatrix[0][2] = -angleSin;
				rotationMatrix[2][0] = angleSin;
				rotationMatrix[2][2] = angleCos;

				return rotationMatrix;
			}

			//Rotation around z axis
			//angle has to be given in degrees
			static inline MMatrix RotationZ(MFloat32 angle)
			{
				MFloat32 angleSin = MMath::Sin(angle * MMath::Deg2Rad);
				MFloat32 angleCos = MMath::Cos(angle * MMath::Deg2Rad);

				MMatrix rotationMatrix = MMatrix::Identity;

				rotationMatrix[0][0] = angleCos;
				rotationMatrix[0][1] = angleSin;
				rotationMatrix[1][0] = -angleSin;
				rotationMatrix[1][1] = angleCos;

				return rotationMatrix;
			}

			//Rotation around given axis
			//angle has to be given in degrees
			static inline MMatrix RotationAxis(const MVector3& axis, MFloat32 angle)
			{
				return MQuaternion::FromAxisAngle(axis, angle).ToMatrix();
			}

			//Pitch, yaw, roll rotation
			//All angles have to be given in degrees
			static inline MMatrix RotationPitchYawRoll(MFloat32 pitch, MFloat32 yaw, MFloat32 roll)
			{
				return MQuaternion::FromPitchYawRoll(pitch, yaw, roll).ToMatrix();
			}

			//Checks if given matrix is identity
			static inline bool IsIdentity(const MMatrix& matrix)
			{
				static const MVector4 targetRow0(1, 0, 0, 0);
				static const MVector4 targetRow1(0, 1, 0, 0);
				static const MVector4 targetRow2(0, 0, 1, 0);
				static const MVector4 targetRow3(0, 0, 0, 1);
				return matrix[0] == targetRow0 && matrix[1] == targetRow1 && matrix[2] == targetRow2 && matrix[3] == targetRow3;
			}

			// Interp is clamped to <0, 1>
			static inline MMatrix Lerp(const MMatrix& a, const MMatrix& b, MFloat32 interp)
			{
				MMatrix out;
				interp = utility::MMath::Clamp(interp, 0.0f, 1.0f);
				MFloat32 iInterp = 1.0f - interp;
				out = (a * iInterp) + (b * interp);

				return out;
			}

			// Assuming weights are normalized.
			static inline MMatrix LerpMultiple(const MMatrix* matArray, const MFloat32* weightArray, MSize matCount)
			{
				MMatrix out(0.0f);
				
				for (MSize i = 0; i < matCount; ++i)
				{
					out = out + matArray[i] * weightArray[i];
				}

				return out;
			}

			//Checks if any element of given matrix is not a number
			static bool IsNaN(const MMatrix& matrix);
			//Checks if any element of given matrix is infinity
			static bool IsInf(const MMatrix& matrix);

			inline void operator=(const MMatrix& other)
			{
				_rows[0] = other[0];
				_rows[1] = other[1];
				_rows[2] = other[2];
				_rows[3] = other[3];
			}

			inline MMatrix operator*(const MMatrix& other) const
			{
				MMatrix toRet;
				MMatrix otherT = other.Transposed();
				for(MInt32 i = 0; i < 4; ++i)
				{
					for(MInt32 j = 0; j < 4; ++j)
					{
						toRet[i][j] = MVector4::Dot(_rows[i], otherT[j]);
					}
				}
				return toRet;
			}

			inline void operator*=(const MMatrix& other)
			{
				MVector4 rows[4];
				rows[0] = _rows[0];
				rows[1] = _rows[1];
				rows[2] = _rows[2];
				rows[3] = _rows[3];
				MMatrix otherT = other.Transposed();
				for(MInt32 i = 0; i < 4; ++i)
				{
					for(MInt32 j = 0; j < 4; ++j)
					{
						_rows[i][j] = MVector4::Dot(rows[j], otherT[i]);
					}
				}
			}

			inline MVector4 operator*(const MVector4& vector) const
			{
				return MVector4(MVector4::Dot(_rows[0], vector), MVector4::Dot(_rows[1], vector), MVector4::Dot(_rows[2], vector), MVector4::Dot(_rows[3], vector));
			}

			inline MVector3 operator*(const MVector3& vector) const
			{
				return operator*(MVector4(vector, 1));
			}

			inline MMatrix operator+(const MMatrix& other) const
			{
				MMatrix toRet;
				toRet[0] = _rows[0] + other[0];
				toRet[1] = _rows[1] + other[1];
				toRet[2] = _rows[2] + other[2];
				toRet[3] = _rows[3] + other[3];
				return toRet;
			}

			inline void operator+=(const MMatrix& other)
			{
				_rows[0] = _rows[0] + other[0];
				_rows[1] = _rows[1] + other[1];
				_rows[2] = _rows[2] + other[2];
				_rows[3] = _rows[3] + other[3];
			}

			inline MMatrix operator-(const MMatrix& other) const
			{
				MMatrix toRet;
				toRet[0] = _rows[0] - other[0];
				toRet[1] = _rows[1] - other[1];
				toRet[2] = _rows[2] - other[2];
				toRet[3] = _rows[3] - other[3];
				return toRet;
			}

			inline void operator-=(const MMatrix& other)
			{
				_rows[0] = _rows[0] - other[0];
				_rows[1] = _rows[1] - other[1];
				_rows[2] = _rows[2] - other[2];
				_rows[3] = _rows[3] - other[3];
			}

			inline MMatrix operator*(MFloat32 s) const
			{
				MMatrix m = *this;
				m *= s;
				return m;
			}

			inline void operator*=(MFloat32 s)
			{
				for(MInt32 i = 0; i < 4; ++i)
				{
					_rows[i][0] *= s;
					_rows[i][1] *= s;
					_rows[i][2] *= s;
					_rows[i][3] *= s;
				}
			}

			inline MMatrix operator/(MFloat32 s) const
			{
				MMatrix m = *this;
				m *= (1.0f / s);
				return m;
			}

			inline void operator/=(MFloat32 s)
			{
				operator*=(1.0f / s);
			}

			inline MVector4& operator[](MInt32 index)
			{
				return _rows[MMath::Clamp(index, 0, 3)];
			}

			inline const MVector4& operator[](MInt32 index) const
			{
				return _rows[MMath::Clamp(index, 0, 3)];
			}

			inline bool operator==(const MMatrix& other) const
			{
				return _rows[0] == other[0] &&
					_rows[1] == other[1] &&
					_rows[2] == other[2] &&
					_rows[3] == other[3];
			}

			inline bool operator!=(const MMatrix& other) const
			{
				return _rows[0] != other[0] ||
					_rows[1] != other[1] ||
					_rows[2] != other[2] ||
					_rows[3] != other[3];
			}
		};
	}
}
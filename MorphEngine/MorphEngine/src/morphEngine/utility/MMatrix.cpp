#include "MMatrix.h"
#include "MQuaternion.h"

namespace morphEngine
{
	namespace utility
	{
		MMatrix MMatrix::Identity(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

		MMatrix::MMatrix(MFloat32 value)
		{
			for(int i = 0; i < 4; ++i)
			{
				_rows[i][0] = value;
				_rows[i][1] = value;
				_rows[i][2] = value;
				_rows[i][3] = value;
			}
		}

		MMatrix::MMatrix(const MMatrix& other)
		{
			_rows[0] = other._rows[0];
			_rows[1] = other._rows[1];
			_rows[2] = other._rows[2];
			_rows[3] = other._rows[3];
		}

		MMatrix::MMatrix(MVector4 rows[4])
		{
			_rows[0] = rows[0];
			_rows[1] = rows[1];
			_rows[2] = rows[2];
			_rows[3] = rows[3];
		}

		MMatrix::MMatrix(MFloat32 elements[16])
		{
			for(int i = 0; i < 4; ++i)
			{
				_rows[i][0] = elements[i];
				_rows[i][1] = elements[4 * i + 1];
				_rows[i][2] = elements[4 * i + 2];
				_rows[i][3] = elements[4 * i + 3];
			}
		}

		MMatrix::MMatrix(MFloat32 m11, MFloat32 m12, MFloat32 m13, MFloat32 m14, MFloat32 m21, MFloat32 m22, MFloat32 m23, MFloat32 m24, MFloat32 m31, MFloat32 m32, MFloat32 m33, MFloat32 m34, MFloat32 m41, MFloat32 m42, MFloat32 m43, MFloat32 m44)
		{
			_rows[0][0] = m11; _rows[0][1] = m12; _rows[0][2] = m13; _rows[0][3] = m14;
			_rows[1][0] = m21; _rows[1][1] = m22; _rows[1][2] = m23; _rows[1][3] = m24;
			_rows[2][0] = m31; _rows[2][1] = m32; _rows[2][2] = m33; _rows[2][3] = m34;
			_rows[3][0] = m41; _rows[3][1] = m42; _rows[3][2] = m43; _rows[3][3] = m44;
		}

		MMatrix MMatrix::Inverted() const
		{
			MFloat32 det = Determinant();
			if(MMath::Approximately(det, 0))
			{
				return *this;
			}

			MMatrix inv;

			inv[0][0] = _rows[1][1] * _rows[2][2] * _rows[3][3] + _rows[1][2] * _rows[2][3] * _rows[3][1] + _rows[1][3] * _rows[2][1] * _rows[3][2] - _rows[1][1] * _rows[2][3] * _rows[3][2] - _rows[1][2] * _rows[2][1] * _rows[3][3] - _rows[1][3] * _rows[2][2] * _rows[3][1];
			inv[0][1] = _rows[0][1] * _rows[2][3] * _rows[3][2] + _rows[0][2] * _rows[2][1] * _rows[3][3] + _rows[0][3] * _rows[2][2] * _rows[3][1] - _rows[0][1] * _rows[2][2] * _rows[3][3] - _rows[0][2] * _rows[2][3] * _rows[3][1] - _rows[0][3] * _rows[2][1] * _rows[3][2];
			inv[0][2] = _rows[0][1] * _rows[1][2] * _rows[3][3] + _rows[0][2] * _rows[1][3] * _rows[3][1] + _rows[0][3] * _rows[1][1] * _rows[3][2] - _rows[0][1] * _rows[1][3] * _rows[3][2] - _rows[0][2] * _rows[1][1] * _rows[3][3] - _rows[0][3] * _rows[1][2] * _rows[3][1];
			inv[0][3] = _rows[0][1] * _rows[1][3] * _rows[2][2] + _rows[0][2] * _rows[1][1] * _rows[2][3] + _rows[0][3] * _rows[1][2] * _rows[2][1] - _rows[0][1] * _rows[1][2] * _rows[2][3] - _rows[0][2] * _rows[1][3] * _rows[2][1] - _rows[0][3] * _rows[1][1] * _rows[2][2];
			inv[1][0] = _rows[1][0] * _rows[2][3] * _rows[3][2] + _rows[1][2] * _rows[2][0] * _rows[3][3] + _rows[1][3] * _rows[2][2] * _rows[3][0] - _rows[1][0] * _rows[2][2] * _rows[3][3] - _rows[1][2] * _rows[2][3] * _rows[3][0] - _rows[1][3] * _rows[2][0] * _rows[3][2];
			inv[1][1] = _rows[0][0] * _rows[2][2] * _rows[3][3] + _rows[0][2] * _rows[2][3] * _rows[3][0] + _rows[0][3] * _rows[2][0] * _rows[3][2] - _rows[0][0] * _rows[2][3] * _rows[3][2] - _rows[0][2] * _rows[2][0] * _rows[3][3] - _rows[0][3] * _rows[2][2] * _rows[3][0];
			inv[1][2] = _rows[0][0] * _rows[1][3] * _rows[3][2] + _rows[0][2] * _rows[1][0] * _rows[3][3] + _rows[0][3] * _rows[1][2] * _rows[3][0] - _rows[0][0] * _rows[1][2] * _rows[3][3] - _rows[0][2] * _rows[1][3] * _rows[3][0] - _rows[0][3] * _rows[1][0] * _rows[3][2];
			inv[1][3] = _rows[0][0] * _rows[1][2] * _rows[2][3] + _rows[0][2] * _rows[1][3] * _rows[2][0] + _rows[0][3] * _rows[1][0] * _rows[2][2] - _rows[0][0] * _rows[1][3] * _rows[2][2] - _rows[0][2] * _rows[1][0] * _rows[2][3] - _rows[0][3] * _rows[1][2] * _rows[2][0];
			inv[2][0] = _rows[1][0] * _rows[2][1] * _rows[3][3] + _rows[1][1] * _rows[2][3] * _rows[3][0] + _rows[1][3] * _rows[2][0] * _rows[3][1] - _rows[1][0] * _rows[2][3] * _rows[3][1] - _rows[1][1] * _rows[2][0] * _rows[3][3] - _rows[1][3] * _rows[2][1] * _rows[3][0];
			inv[2][1] = _rows[0][0] * _rows[2][3] * _rows[3][1] + _rows[0][1] * _rows[2][0] * _rows[3][3] + _rows[0][3] * _rows[2][1] * _rows[3][0] - _rows[0][0] * _rows[2][1] * _rows[3][3] - _rows[0][1] * _rows[2][3] * _rows[3][0] - _rows[0][3] * _rows[2][0] * _rows[3][1];
			inv[2][2] = _rows[0][0] * _rows[1][1] * _rows[3][3] + _rows[0][1] * _rows[1][3] * _rows[3][0] + _rows[0][3] * _rows[1][0] * _rows[3][1] - _rows[0][0] * _rows[1][3] * _rows[3][1] - _rows[0][1] * _rows[1][0] * _rows[3][3] - _rows[0][3] * _rows[1][1] * _rows[3][0];
			inv[2][3] = _rows[0][0] * _rows[1][3] * _rows[2][1] + _rows[0][1] * _rows[1][0] * _rows[2][3] + _rows[0][3] * _rows[1][1] * _rows[2][0] - _rows[0][0] * _rows[1][1] * _rows[2][3] - _rows[0][1] * _rows[1][3] * _rows[2][0] - _rows[0][3] * _rows[1][0] * _rows[2][1];
			inv[3][0] = _rows[1][0] * _rows[2][2] * _rows[3][1] + _rows[1][1] * _rows[2][0] * _rows[3][2] + _rows[1][2] * _rows[2][1] * _rows[3][0] - _rows[1][0] * _rows[2][1] * _rows[3][2] - _rows[1][1] * _rows[2][2] * _rows[3][0] - _rows[1][2] * _rows[2][0] * _rows[3][1];
			inv[3][1] = _rows[0][0] * _rows[2][1] * _rows[3][2] + _rows[0][1] * _rows[2][2] * _rows[3][0] + _rows[0][2] * _rows[2][0] * _rows[3][1] - _rows[0][0] * _rows[2][2] * _rows[3][1] - _rows[0][1] * _rows[2][0] * _rows[3][2] - _rows[0][2] * _rows[2][1] * _rows[3][0];
			inv[3][2] = _rows[0][0] * _rows[1][2] * _rows[3][1] + _rows[0][1] * _rows[1][0] * _rows[3][2] + _rows[0][2] * _rows[1][1] * _rows[3][0] - _rows[0][0] * _rows[1][1] * _rows[3][2] - _rows[0][1] * _rows[1][2] * _rows[3][0] - _rows[0][2] * _rows[1][0] * _rows[3][1];
			inv[3][3] = _rows[0][0] * _rows[1][1] * _rows[2][2] + _rows[0][1] * _rows[1][2] * _rows[2][0] + _rows[0][2] * _rows[1][0] * _rows[2][1] - _rows[0][0] * _rows[1][2] * _rows[2][1] - _rows[0][1] * _rows[1][0] * _rows[2][2] - _rows[0][2] * _rows[1][1] * _rows[2][0];

			return inv / det;
		}

		void MMatrix::Inverse()
		{
			*this = Inverted();
		}

		MFloat32 MMatrix::Determinant() const
		{
			return
				_rows[0][3] * _rows[1][2] * _rows[2][1] * _rows[3][0] - _rows[0][2] * _rows[1][3] * _rows[2][1] * _rows[3][0] -
				_rows[0][3] * _rows[1][1] * _rows[2][2] * _rows[3][0] + _rows[0][1] * _rows[1][3] * _rows[2][2] * _rows[3][0] +
				_rows[0][2] * _rows[1][1] * _rows[2][3] * _rows[3][0] - _rows[0][1] * _rows[1][2] * _rows[2][3] * _rows[3][0] -
				_rows[0][3] * _rows[1][2] * _rows[2][0] * _rows[3][1] + _rows[0][2] * _rows[1][3] * _rows[2][0] * _rows[3][1] +
				_rows[0][3] * _rows[1][0] * _rows[2][2] * _rows[3][1] - _rows[0][0] * _rows[1][3] * _rows[2][2] * _rows[3][1] -
				_rows[0][2] * _rows[1][0] * _rows[2][3] * _rows[3][1] + _rows[0][0] * _rows[1][2] * _rows[2][3] * _rows[3][1] +
				_rows[0][3] * _rows[1][1] * _rows[2][0] * _rows[3][2] - _rows[0][1] * _rows[1][3] * _rows[2][0] * _rows[3][2] -
				_rows[0][3] * _rows[1][0] * _rows[2][1] * _rows[3][2] + _rows[0][0] * _rows[1][3] * _rows[2][1] * _rows[3][2] +
				_rows[0][1] * _rows[1][0] * _rows[2][3] * _rows[3][2] - _rows[0][0] * _rows[1][1] * _rows[2][3] * _rows[3][2] -
				_rows[0][2] * _rows[1][1] * _rows[2][0] * _rows[3][3] + _rows[0][1] * _rows[1][2] * _rows[2][0] * _rows[3][3] +
				_rows[0][2] * _rows[1][0] * _rows[2][1] * _rows[3][3] - _rows[0][0] * _rows[1][2] * _rows[2][1] * _rows[3][3] -
				_rows[0][1] * _rows[1][0] * _rows[2][2] * _rows[3][3] + _rows[0][0] * _rows[1][1] * _rows[2][2] * _rows[3][3];
		}

		MVector4 MMatrix::DeterminantVector() const
		{
			MFloat32 det = Determinant();
			return MVector4(det, det, det, det);
		}

		MQuaternion MMatrix::ToQuaternion() const
		{
			//Tricky version, gathered from OpenGL documentation (https://www.j3d.org/matrix_faq/matrfaq_latest.html#Q55)
			MFloat32 trace = 1 + _rows[0][0] + _rows[1][1] + _rows[2][2];

			MFloat32 s;
			MFloat32 x;
			MFloat32 y;
			MFloat32 z;
			MFloat32 w;

			//To avoid large distortions
			if(MMath::Approximately(trace, 0))
			{
				//Find which major diagonal has the greatest value
				if(_rows[0][0] > _rows[1][1] && _rows[0][0] > _rows[2][2])
				{
					s = MMath::Sqrt(1.0f + _rows[0][0] - _rows[1][1] - _rows[2][2]) * 2.0f;
					x = 0.25f * s;
					y = (_rows[1][0] + _rows[0][1]) / s;
					z = (_rows[0][2] + _rows[2][0]) / s;
					w = (_rows[2][1] - _rows[1][2]) / s;
				}
				else if(_rows[1][1] > _rows[2][2])
				{
					s = MMath::Sqrt(1.0f + _rows[1][1] - _rows[0][0] - _rows[2][2]) * 2.0f;
					x = (_rows[1][0] + _rows[0][1]) / s;
					y = 0.25f * s;
					z = (_rows[2][1] + _rows[1][2]) / s;
					w = (_rows[0][2] - _rows[2][0]) / s;
				}
				else
				{
					s = MMath::Sqrt(1.0f + _rows[2][2] - _rows[1][1] - _rows[0][0]) * 2.0f;
					x = (_rows[0][2] + _rows[2][0]) / s;
					y = (_rows[2][1] + _rows[1][2]) / s;
					z = 0.25f * s;
					w = (_rows[1][0] - _rows[0][1]) / s;
				}
			}
			else
			{
				s = sqrt(trace) * 2.0f;
				x = (_rows[2][1] - _rows[1][2]) / s;
				y = (_rows[0][2] - _rows[2][0]) / s;
				z = (_rows[1][0] - _rows[0][1]) / s;
				w = 0.25f * s;
			}

			return MQuaternion(x, y, z, w);
		}

		bool MMatrix::IsNaN(const MMatrix& matrix)
		{
			for(MInt32 i = 0; i < 4; ++i)
			{
				if(MMath::IsNan(matrix[i][0]))
				{
					return true;
				}
				if(MMath::IsNan(matrix[i][1]))
				{
					return true;
				}
				if(MMath::IsNan(matrix[i][2]))
				{
					return true;
				}
				if(MMath::IsNan(matrix[i][3]))
				{
					return true;
				}
			}
			return false;
		}

		bool MMatrix::IsInf(const MMatrix& matrix)
		{
			for(MInt32 i = 0; i < 4; ++i)
			{
				if(MMath::IsInf(matrix[i][0]))
				{
					return true;
				}
				if(MMath::IsInf(matrix[i][1]))
				{
					return true;
				}
				if(MMath::IsInf(matrix[i][2]))
				{
					return true;
				}
				if(MMath::IsInf(matrix[i][3]))
				{
					return true;
				}
			}
			return false;
		}
	}
}
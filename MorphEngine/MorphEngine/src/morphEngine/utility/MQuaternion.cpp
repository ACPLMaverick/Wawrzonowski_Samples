#include "MQuaternion.h"
#include "MMatrix.h"

namespace morphEngine
{
	namespace utility
	{
		MMatrix MQuaternion::ToMatrix() const
		{
			MMatrix matrix;
			matrix[0] = MVector4(1.0f - 2 * Y * Y - 2.0f * X * X, 2.0f * (Z * Y + X * W), 2.0f*(X * Z - Y * W), 0);
			matrix[1] = MVector4(2.0f * (Z * Y - X * W), 1 - 2 * Z * Z - 2.0f * X * X, 2.0f * (Y * X + Z * W), 0);
			matrix[2] = MVector4(2.0f * (X * Z + Y * W), 2.0f * (Y * X - Z * W), 1.0f - 2 * Z * Z - 2.0f * Y * Y, 0);
			matrix[3] = MVector4(0, 0, 0, 1);
			return matrix;
		}
	}
}
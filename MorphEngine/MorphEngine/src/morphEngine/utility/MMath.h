#pragma once

#include "../core/GlobalDefines.h"

/// <summary>
/// Contains every function needed to calculate something
/// Trigonometric functions operates on radias as default (they have degrees version by adding D to their name)
/// </summary>
namespace morphEngine
{
	namespace utility
	{
		class MMath
		{
		protected:
			const static MFloat32 _tolerance;

		public:
			const static MFloat32 E;
			const static MFloat32 PI;
			const static MFloat32 PIOver2;
			const static MFloat32 PIOver4;
			const static MFloat32 TwoPI;
			const static MFloat32 Rad2Deg;
			const static MFloat32 Deg2Rad;

		public:
			static inline MFloat32 Abs(const MFloat32 x)
			{
				return x >= 0 ? x : -x;
			}

			static inline MInt32 Abs(const MInt32 x)
			{
				return x >= 0 ? x : -x;
			}

			static inline MInt32 Sign(const MFloat32 x)
			{
				return x > 0 ? 1 : (x < 0 ? -1 : 0);
			}

			static inline MInt32 Sign(const MInt32 x)
			{
				return x > 0 ? 1 : (x < 0 ? -1 : 0);
			}

			static inline MUint32 Sign(const MUint32 x)
			{
				return 1;
			}

			static inline bool Approximately(const MFloat32 x, const MFloat32 target, const MFloat32 tolerance = _tolerance)
			{
				//It's faster than "return Abs(x-target) <= tolerance";
				MFloat32 a = x - target;
				a = a >= 0 ? a : -a;
				return a <= tolerance;
			}

			static inline MInt32 Ceil(const MFloat32 x)
			{
				return (MInt32)x + 1;
			}

			static inline MInt64 Ceil(const MDouble64 x)
			{
				return (MInt64)x + 1;
			}

			static inline MInt64 Ceil(const MLongDouble64 x)
			{
				return (MInt64)x + 1;
			}

			static inline MInt32 Floor(const MFloat32 x)
			{
				return (MInt32)x;
			}

			static inline MInt64 Floor(const MDouble64 x)
			{
				return (MInt64)x;
			}

			static inline MInt64 Floor(const MLongDouble64 x)
			{
				return (MInt64)x;
			}

			static inline MFloat32 Min(const MFloat32 a, const MFloat32 b)
			{
				return (a < b ? a : b);
			}

			static inline MFloat32 Max(const MFloat32 a, const MFloat32 b)
			{
				return (a > b ? a : b);
			}

			static inline MDouble64 Min(const MDouble64 a, const MDouble64 b)
			{
				return (a < b ? a : b);
			}

			static inline MDouble64 Max(const MDouble64 a, const MDouble64 b)
			{
				return (a > b ? a : b);
			}

			static inline MInt32 Min(const MInt32 a, const MInt32 b)
			{
				return (a < b ? a : b);
			}

			static inline MInt32 Max(const MInt32 a, const MInt32 b)
			{
				return (a > b ? a : b);
			}

			static inline MUint32 Min(const MUint32 a, const MUint32 b)
			{
				return (a < b ? a : b);
			}

			static inline MUint32 Max(const MUint32 a, const MUint32 b)
			{
				return (a > b ? a : b);
			}

			static inline MInt32 Min(const MInt32 a, const MUint32 b)
			{
				if(a < 0)
				{
					return a;
				}
				MUint32 aU = (MUint32)a;
				return (aU < b ? aU : b);
			}

			static inline MInt32 Max(const MInt32 a, const MUint32 b)
			{
				if(a < 0)
				{
					return (MInt32)b;
				}
				MUint32 aU = (MUint32)a;
				return (aU > b ? aU : b);
			}

			static inline MInt32 Min(const MUint32 a, const MInt32 b)
			{
				if(b < 0)
				{
					return b;
				}
				MUint32 bU = (MUint32)b;
				return (a < bU ? a : bU);
			}

			static inline MInt32 Max(const MUint32 a, const MInt32 b)
			{
				if(b < 0)
				{
					return (MInt32)a;
				}
				MUint32 bU = (MUint32)b;
				return (a > bU ? a : bU);
			}

			static inline MFloat32 Min(const MInt32 a, const MFloat32 b)
			{
				return (a < b ? a : b);
			}

			static inline MFloat32 Max(const MInt32 a, const MFloat32 b)
			{
				return (a > b ? a : b);
			}

			static inline MFloat32 Min(const MUint32 a, const MFloat32 b)
			{
				return (a < b ? a : b);
			}

			static inline MFloat32 Max(const MUint32 a, const MFloat32 b)
			{
				return (a > b ? a : b);
			}

			static inline MFloat32 Min(const MFloat32 a, const MInt32 b)
			{
				return (a < b ? a : b);
			}

			static inline MFloat32 Max(const MFloat32 a, const MInt32 b)
			{
				return (a > b ? a : b);
			}

			static inline MFloat32 Min(const MFloat32 a, const MUint32 b)
			{
				return (a < b ? a : b);
			}

			static inline MFloat32 Max(const MFloat32 a, const MUint32 b)
			{
				return (a > b ? a : b);
			}

			static inline MSize Min(const MSize a, const MSize b)
			{
				return (a < b ? a : b);
			}

			static inline MSize Max(const MSize a, const MSize b)
			{
				return (a > b ? a : b);
			}

			static inline MSize Min(const MSize a, const MInt32 b)
			{
				return (a < b ? a : b);
			}

			static inline MSize Max(const MSize a, const MInt32 b)
			{
				return (a > b ? a : b);
			}

			static inline MSize Min(const MInt32 a, const MSize b)
			{
				return (a < b ? a : b);
			}

			static inline MSize Max(const MInt32 a, const MSize b)
			{
				return (a > b ? a : b);
			}

			static inline MFloat32 Cos(const MFloat32 x)
			{
				return (MFloat32)cos(x);
			}

			static inline MFloat32 Cos(const MInt32 x)
			{
				return (MFloat32)cos(x);
			}

			static inline MFloat32 Cos(const MUint32 x)
			{
				return (MFloat32)cos(x);
			}

			static inline MFloat32 Cos(const MDouble64 x)
			{
				return (MFloat32)cos(x);
			}

			static inline MFloat32 CosD(const MFloat32 x)
			{
				return (MFloat32)cos(x * Deg2Rad);
			}

			static inline MFloat32 CosD(const MInt32 x)
			{
				return (MFloat32)cos(x * Deg2Rad);
			}

			static inline MFloat32 CosD(const MUint32 x)
			{
				return (MFloat32)cos(x * Deg2Rad);
			}

			static inline MFloat32 CosD(const MDouble64 x)
			{
				return (MFloat32)cos(x * Deg2Rad);
			}

			static inline MFloat32 Ctan(const MFloat32 x)
			{
				return (MFloat32)(cos(x) / sin(x));
			}

			static inline MFloat32 Ctan(const MInt32 x)
			{
				return (MFloat32)(cos(x) / sin(x));
			}

			static inline MFloat32 Ctan(const MUint32 x)
			{
				return (MFloat32)(cos(x) / sin(x));
			}

			static inline MFloat32 Ctan(const MDouble64 x)
			{
				return (MFloat32)(cos(x) / sin(x));
			}

			static inline MFloat32 CtanD(const MFloat32 x)
			{
				return Ctan(x * Deg2Rad);
			}

			static inline MFloat32 CtanD(const MInt32 x)
			{
				return Ctan(x * Deg2Rad);
			}

			static inline MFloat32 CtanD(const MUint32 x)
			{
				return Ctan(x * Deg2Rad);
			}

			static inline MFloat32 CtanD(const MDouble64 x)
			{
				return Ctan(x * Deg2Rad);
			}

			static inline MFloat32 Sin(const MFloat32 x)
			{
				return (MFloat32)sin(x);
			}

			static inline MFloat32 Sin(const MInt32 x)
			{
				return (MFloat32)sin(x);
			}

			static inline MFloat32 Sin(const MUint32 x)
			{
				return (MFloat32)sin(x);
			}

			static inline MFloat32 Sin(const MDouble64 x)
			{
				return (MFloat32)sin(x);
			}

			static inline MFloat32 SinD(const MFloat32 x)
			{
				return (MFloat32)sin(x * Deg2Rad);
			}

			static inline MFloat32 SinD(const MInt32 x)
			{
				return (MFloat32)sin(x * Deg2Rad);
			}

			static inline MFloat32 SinD(const MUint32 x)
			{
				return (MFloat32)sin(x * Deg2Rad);
			}

			static inline MFloat32 SinD(const MDouble64 x)
			{
				return (MFloat32)sin(x * Deg2Rad);
			}

			static inline MFloat32 Tan(const MFloat32 x)
			{
				return (MFloat32)tan(x);
			}

			static inline MFloat32 Tan(const MInt32 x)
			{
				return (MFloat32)tan(x);
			}

			static inline MFloat32 Tan(const MUint32 x)
			{
				return (MFloat32)tan(x);
			}

			static inline MFloat32 Tan(const MDouble64 x)
			{
				return (MFloat32)tan(x);
			}

			static inline MFloat32 TanD(const MFloat32 x)
			{
				return (MFloat32)tan(x * Deg2Rad);
			}

			static inline MFloat32 TanD(const MInt32 x)
			{
				return (MFloat32)tan(x * Deg2Rad);
			}

			static inline MFloat32 TanD(const MUint32 x)
			{
				return (MFloat32)tan(x * Deg2Rad);
			}

			static inline MFloat32 TanD(const MDouble64 x)
			{
				return (MFloat32)tan(x * Deg2Rad);
			}

			static inline MFloat32 Acos(const MFloat32 x)
			{
				return (MFloat32)acos(x);
			}

			static inline MFloat32 Acos(const MInt32 x)
			{
				return (MFloat32)acos(x);
			}

			static inline MFloat32 Acos(const MUint32 x)
			{
				return (MFloat32)acos(x);
			}

			static inline MFloat32 Acos(const MDouble64 x)
			{
				return (MFloat32)acos(x);
			}

			static inline MFloat32 AcosD(const MFloat32 x)
			{
				return (MFloat32)acos(x) * Rad2Deg;
			}

			static inline MFloat32 AcosD(const MInt32 x)
			{
				return (MFloat32)acos(x) * Rad2Deg;
			}

			static inline MFloat32 AcosD(const MUint32 x)
			{
				return (MFloat32)acos(x) * Rad2Deg;
			}

			static inline MFloat32 AcosD(const MDouble64 x)
			{
				return (MFloat32)acos(x) * Rad2Deg;
			}

			static inline MFloat32 Asin(const MFloat32 x)
			{
				return (MFloat32)asin(x);
			}

			static inline MFloat32 Asin(const MInt32 x)
			{
				return (MFloat32)asin(x);
			}

			static inline MFloat32 Asin(const MUint32 x)
			{
				return (MFloat32)asin(x);
			}

			static inline MFloat32 Asin(const MDouble64 x)
			{
				return (MFloat32)asin(x);
			}

			static inline MFloat32 AsinD(const MFloat32 x)
			{
				return (MFloat32)asin(x) * Rad2Deg;
			}

			static inline MFloat32 AsinD(const MInt32 x)
			{
				return (MFloat32)asin(x) * Rad2Deg;
			}

			static inline MFloat32 AsinD(const MUint32 x)
			{
				return (MFloat32)asin(x) * Rad2Deg;
			}

			static inline MFloat32 AsinD(const MDouble64 x)
			{
				return (MFloat32)asin(x) * Rad2Deg;
			}

			static inline void SinCos(MFloat32* s, MFloat32* c, MFloat32 val)
			{
				// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
				MFloat32 quotient = ((1.0f / PI)*0.5f)*val;
				if (val >= 0.0f)
				{
					quotient = (MFloat32)((int)(quotient + 0.5f));
				}
				else
				{
					quotient = (MFloat32)((int)(quotient - 0.5f));
				}
				MFloat32 y = val - (2.0f*PI)*quotient;

				// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
				MFloat32 sign;
				if (y > PIOver2)
				{
					y = PI - y;
					sign = -1.0f;
				}
				else if (y < -PIOver2)
				{
					y = -PI - y;
					sign = -1.0f;
				}
				else
				{
					sign = +1.0f;
				}

				MFloat32 y2 = y * y;

				// 11-degree minimax approximation
				*s = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

				// 10-degree minimax approximation
				MFloat32 p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
				*c = sign*p;
			}

			static inline void SinCosD(MFloat32* s, MFloat32* c, MFloat32 val)
			{
				SinCos(s, c, val * Rad2Deg);
			}

			static inline MFloat32 Atan(const MFloat32 x)
			{
				return (MFloat32)atan(x);
			}

			static inline MFloat32 Atan(const MInt32 x)
			{
				return (MFloat32)atan(x);
			}

			static inline MFloat32 Atan(const MUint32 x)
			{
				return (MFloat32)atan(x);
			}

			static inline MFloat32 Atan(const MDouble64 x)
			{
				return (MFloat32)atan(x);
			}

			static inline MFloat32 AtanD(const MFloat32 x)
			{
				return (MFloat32)atan(x) * Rad2Deg;
			}

			static inline MFloat32 AtanD(const MInt32 x)
			{
				return (MFloat32)atan(x) * Rad2Deg;
			}

			static inline MFloat32 AtanD(const MUint32 x)
			{
				return (MFloat32)atan(x) * Rad2Deg;
			}

			static inline MFloat32 AtanD(const MDouble64 x)
			{
				return (MFloat32)atan(x) * Rad2Deg;
			}

			static inline MFloat32 Atan2(const MFloat32 y, const MFloat32 x)
			{
				return atan2(y, x);
			}

			static inline MFloat32 Atan2(const MDouble64 y, const MDouble64 x)
			{
				return (MFloat32)atan2(y, x);
			}

			static inline MFloat32 Atan2(const MInt32 y, const MInt32 x)
			{
				return (MFloat32)atan2(y, x);
			}

			static inline MFloat32 Atan2D(const MFloat32 y, const MFloat32 x)
			{
				return atan2(y, x) * Rad2Deg;
			}

			static inline MFloat32 Atan2D(const MDouble64 y, const MDouble64 x)
			{
				return (MFloat32)atan2(y, x) * Rad2Deg;
			}

			static inline MFloat32 Atan2D(const MInt32 y, const MInt32 x)
			{
				return (MFloat32)atan2(y, x) * Rad2Deg;
			}

			static inline MFloat32 Exp(const MFloat32 x)
			{
				return (MFloat32)exp(x);
			}

			static inline MFloat32 Exp(const MInt32 x)
			{
				return (MFloat32)exp(x);
			}

			static inline MFloat32 Exp(const MUint32 x)
			{
				return (MFloat32)exp(x);
			}

			static inline MFloat32 Exp(const MDouble64 x)
			{
				return (MFloat32)exp(x);
			}

			static inline MFloat32 Ln(const MFloat32 x)
			{
				return (MFloat32)log(x);
			}

			static inline MFloat32 Ln(const MInt32 x)
			{
				return (MFloat32)log(x);
			}

			static inline MFloat32 Ln(const MUint32 x)
			{
				return (MFloat32)log(x);
			}

			static inline MFloat32 Ln(const MDouble64 x)
			{
				return (MFloat32)log(x);
			}

			static inline MFloat32 Pow(const MFloat32 x, const MFloat32 power)
			{
				return (MFloat32)pow(x, power);
			}

			static inline MFloat32 Pow(const MInt32 x, const MInt32 power)
			{
				return (MFloat32)pow(x, power);
			}

			static inline MFloat32 Pow(const MFloat32 x, const MInt32 power)
			{
				return (MFloat32)pow(x, power);
			}

			static inline MFloat32 Pow(const MInt32 x, const MFloat32 power)
			{
				return (MFloat32)pow(x, power);
			}

			static inline MFloat32 Pow(const MUint32 x, const MUint32 power)
			{
				return (MFloat32)pow(x, power);
			}

			static inline MFloat32 Pow(const MUint32 x, const MFloat32 power)
			{
				return (MFloat32)pow(x, power);
			}

			static inline MFloat32 Pow(const MFloat32 x, const MUint32 power)
			{
				return (MFloat32)pow(x, power);
			}

			static inline MFloat32 Pow(const MDouble64 x, const MDouble64 power)
			{
				return (MFloat32)pow(x, power);
			}

			static inline MFloat32 Sqrt(const MFloat32 x)
			{
				return (MFloat32)sqrt(x);
			}

			static inline MFloat32 Sqrt(const MInt32 x)
			{
				return (MFloat32)sqrt(x);
			}

			static inline MFloat32 Sqrt(const MUint32 x)
			{
				return (MFloat32)sqrt(x);
			}

			static inline MFloat32 Sqrt(const MDouble64 x)
			{
				return (MFloat32)sqrt(x);
			}

			static inline MFloat32 Lerp(const MFloat32 a, const MFloat32 b, const MFloat32 t)
			{
				MFloat32 tClamped = Clamp(t);
				return a * (1.0f - tClamped) + b * tClamped;
			}

			static inline MInt32 Lerp(const MInt32 a, const MInt32 b, const MFloat32 t)
			{
				MFloat32 tClamped = Clamp(t);
				return (MInt32)(a * (1.0f - tClamped) + b * tClamped);
			}

			static inline MUint32 Lerp(const MUint32 a, const MUint32 b, const MFloat32 t)
			{
				MFloat32 tClamped = Clamp(t);
				return (MInt32)(a * (1.0f - tClamped) + b * tClamped);
			}

			static inline MDouble64 Lerp(const MDouble64 a, const MDouble64 b, const MFloat32 t)
			{
				MFloat32 tClamped = Clamp(t);
				return (MInt32)(a * (1.0f - tClamped) + b * tClamped);
			}

			static inline MFloat32 Clamp(const MFloat32 value, const MFloat32 min = 0.0f, const MFloat32 max = 1.0f)
			{
				if(min > max)
				{
					return Clamp(value, max, min);
				}
				return Max(Min(value, max), min);
			}

			static inline MInt32 Clamp(const MInt32 value, const MInt32 min = 0, const MInt32 max = 1)
			{
				if(min > max)
				{
					return Clamp(value, max, min);
				}
				return Max(Min(value, max), min);
			}

			static inline MUint32 Clamp(const MUint32 value, const MUint32 min = 0, const MUint32 max = 1)
			{
				if(min > max)
				{
					return Clamp(value, max, min);
				}
				return Max(Min(value, max), min);
			}

			static inline MDouble64 Clamp(const MDouble64 value, const MDouble64 min = 0, const MDouble64 max = 1)
			{
				if(min > max)
				{
					return Clamp(value, max, min);
				}
				return Max(Min(value, max), min);
			}

			static inline MFloat32 Square(MFloat32 arg)
			{
				return arg * arg;
			}

			static inline MFloat32 Fmod(MFloat32 x, MFloat32 y)
			{
				if (fabsf(y) <= 1.e-8f)
				{
					return 0.0f;
				}
				const MFloat32 quotient = (MFloat32)((MInt32)(x / y));
				MFloat32 intPortion = y * quotient;

				// Rounding and imprecision could cause IntPortion to exceed X and cause the result to be outside the expected range.
				// For example Fmod(55.8, 9.3) would result in a very small negative value!
				if (fabsf(intPortion) > fabsf(x))
				{
					intPortion = x;
				}

				const MFloat32 result = x - intPortion;
				return result;
			}

			static inline bool IsInf(const MFloat32 x)
			{
				return isinf(x);
			}

			static inline bool IsInf(const MDouble64 x)
			{
				return isinf(x);
			}

			static inline bool IsNan(const MFloat32 x)
			{
				return isnan(x);
			}

			static inline bool IsNan(const MDouble64 x)
			{
				return isnan(x);
			}
		};
	}
}
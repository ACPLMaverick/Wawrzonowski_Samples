#include "MColor.h"
#include "MMath.h"
#include "MVector.h"

namespace morphEngine
{
	namespace utility
	{
		MColor MColor::White(1.0f, 1.0f, 1.0f, 1.0f);
		MColor MColor::Black(0.0f, 0.0f, 0.0f, 1.0f);
		MColor MColor::Transparent(0.0f, 0.0f, 0.0f, 0.0f);
		MColor MColor::Red(1.0f, 0.0f, 0.0f, 1.0f);
		MColor MColor::Green(0.0f, 1.0f, 0.0f, 1.0f);
		MColor MColor::Blue(0.0f, 0.0f, 1.0f, 1.0f);
		MColor MColor::Cyan(0.0f, 1.0f, 1.0f, 1.0f);
		MColor MColor::Magenta(1.0f, 0.0f, 1.0f, 1.0f);
		MColor MColor::Yellow(1.0f, 1.0f, 0.0f, 1.0f);

		static const MFloat32 Rec255 = 1.0f / 255.0f;

		MColor::MColor(MFloat32 r, MFloat32 g, MFloat32 b, MFloat32 a) :
			_r(r),
			_g(g),
			_b(b),
			_a(a)
		{
		}

		MColor::MColor(MUint32 color)
		{
			IntToFloat(color, _data);
		}

		MColor::MColor(MFloat32 * color)
		{
			memcpy(_data, color, 4 * sizeof(MFloat32));
		}

		MColor::MColor(const MVector2 & vec)
		{
		}

		MColor::MColor(const MVector3 & vec)
		{
		}

		MColor::MColor(const MVector4 & vec)
		{
		}

		MColor::MColor() : 
			MColor(0.0f, 0.0f, 0.0f, 1.0f)
		{
		}

		MColor::MColor(const MColor & c)
		{
			memcpy(_data, c._data, 4 * sizeof(MFloat32));
		}

		MColor::~MColor()
		{
		}

		MColor MColor::operator+(const MColor & rhs)
		{
			return MColor(_r + rhs._r, _g + rhs._g, _b + rhs._b, _a + rhs._a);
		}

		MColor MColor::operator+(const MFloat32 rhs)
		{
			return MColor(_r + rhs, _g + rhs, _b + rhs, _a + rhs);
		}

		MColor MColor::operator-(const MColor & rhs)
		{
			return MColor(_r - rhs._r, _g - rhs._g, _b - rhs._b, _a - rhs._a);
		}

		MColor MColor::operator-(const MFloat32 rhs)
		{
			return MColor(_r - rhs, _g - rhs, _b - rhs, _a - rhs);
		}

		MColor MColor::operator*(const MColor & rhs)
		{
			return MColor(_r * rhs._r, _g * rhs._g, _b * rhs._b, _a * rhs._a);
		}

		MColor MColor::operator*(const MFloat32 rhs)
		{
			return MColor(_r * rhs, _g * rhs, _b * rhs, _a * rhs);
		}

		MColor MColor::operator/(const MColor & rhs)
		{
			return MColor(_r / rhs._r, _g / rhs._g, _b / rhs._b, _a / rhs._a);
		}

		MColor MColor::operator/(const MFloat32 rhs)
		{
			return MColor(_r / rhs, _g / rhs, _b / rhs, _a / rhs);
		}

		MColor & MColor::operator+=(const MColor & rhs)
		{
			_r += rhs._r;
			_g += rhs._g;
			_b += rhs._b;
			_a += rhs._a;
			return *this;
		}

		MColor & MColor::operator+=(const MFloat32 rhs)
		{
			_r += rhs;
			_g += rhs;
			_b += rhs;
			_a += rhs;
			return *this;
		}

		MColor & MColor::operator-=(const MColor & rhs)
		{
			_r -= rhs._r;
			_g -= rhs._g;
			_b -= rhs._b;
			_a -= rhs._a;
			return *this;
		}

		MColor & MColor::operator-=(const MFloat32 rhs)
		{
			_r -= rhs;
			_g -= rhs;
			_b -= rhs;
			_a -= rhs;
			return *this;
		}

		MColor & MColor::operator*=(const MColor & rhs)
		{
			_r *= rhs._r;
			_g *= rhs._g;
			_b *= rhs._b;
			_a *= rhs._a;
			return *this;
		}

		MColor & MColor::operator*=(const MFloat32 rhs)
		{
			_r *= rhs;
			_g *= rhs;
			_b *= rhs;
			_a *= rhs;
			return *this;
		}

		MColor & MColor::operator/=(const MColor & rhs)
		{
			_r /= rhs._r;
			_g /= rhs._g;
			_b /= rhs._b;
			_a /= rhs._a;
			return *this;
		}

		MColor & MColor::operator/=(const MFloat32 rhs)
		{
			_r /= rhs;
			_g /= rhs;
			_b /= rhs;
			_a /= rhs;
			return *this;
		}

		MColor & MColor::operator=(const MColor & c)
		{
			memcpy(_data, c._data, sizeof(MFloat32) * 4);
			return *this;
		}

		MColor & MColor::operator=(const MFloat32 * ptr)
		{
			memcpy(_data, ptr, sizeof(MFloat32) * 4);
			return *this;
		}

		MColor & MColor::operator=(MUint32 integerValue)
		{
			IntToFloat(integerValue, _data);
			return *this;
		}

		MFloat32& MColor::operator[](MSize index)
		{
			ME_ASSERT(index >= 0 && index < 4, "MColor: Index out of range.");
			return _data[index];
		}

		MFloat32 MColor::operator[](MSize index) const
		{
			ME_ASSERT(index >= 0 && index < 4, "MColor: Index out of range.");
			return _data[index];
		}

		bool MColor::operator==(const MColor & rhs) const
		{
			return _r == rhs._r && _g == rhs._g && _b == rhs._b && _a == rhs._a;
		}

		bool MColor::operator!=(const MColor & rhs) const
		{
			return !operator==(rhs);
		}

		MColor::operator MFloat32*()
		{
			return _data;
		}

		MColor::operator MUint32()
		{
			return FloatToInt(_data);
		}

		MColor::operator MVector2()
		{
			return MVector2(_r, _a);
		}

		MColor::operator MVector3()
		{
			return MVector3(_r, _g, _b);
		}

		MColor::operator MVector4()
		{
			return MVector4(_r, _g, _b, _a);
		}

		inline MUint32 MColor::FloatToInt(MFloat32 * col)
		{
			MUint8 iR = static_cast<MUint8>(MMath::Clamp(col[0], 0.0f, 1.0f) * 255.0f);
			MUint8 iG = static_cast<MUint8>(MMath::Clamp(col[1], 0.0f, 1.0f) * 255.0f);
			MUint8 iB = static_cast<MUint8>(MMath::Clamp(col[2], 0.0f, 1.0f) * 255.0f);
			MUint8 iA = static_cast<MUint8>(MMath::Clamp(col[3], 0.0f, 1.0f) * 255.0f);
			MInt32 ret = 0;
			ret |= ((iR << 24) | (iG << 16) | (iB << 8) | iA);
			return ret;
		}

		inline void MColor::IntToFloat(MUint32 col, MFloat32 * outData)
		{
			outData[0] = static_cast<MFloat32>(((col & 0xFF000000) >> 24)) * Rec255;
			outData[1] = static_cast<MFloat32>(((col & 0x00FF0000) >> 16)) * Rec255;
			outData[2] = static_cast<MFloat32>(((col & 0x0000FF00) >> 8)) * Rec255;
			outData[3] = static_cast<MFloat32>(((col & 0x000000FF))) * Rec255;
		}
	}
}
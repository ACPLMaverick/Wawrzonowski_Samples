#pragma once

#include "core/GlobalDefines.h"

namespace morphEngine
{
	namespace utility
	{
		class MVector2;
		class MVector3;
		class MVector4;

		/// <summary>
		/// Class for color representation.
		/// Color is stored in memory as 4-component 32-bit float array and can be converted to integer (4-component 8-bit each) value.
		/// Beware as this conversion can be costly, so it's best to use float representation whenever possible. 
		/// </summary>
		class MColor
		{
		public:

#pragma region Statics

			static MColor Black;
			static MColor White;
			static MColor Transparent;
			static MColor Red;
			static MColor Green;
			static MColor Blue;
			static MColor Cyan;
			static MColor Magenta;
			static MColor Yellow;

#pragma endregion

		protected:

#pragma region Protected

#pragma endregion

#pragma region Functions Protected

			inline MUint32 FloatToInt(MFloat32* col);
			inline void IntToFloat(MUint32 col, MFloat32* outData);

#pragma endregion

		public:

#pragma region Public

			union
			{
				struct
				{
					MFloat32 _r;
					MFloat32 _g;
					MFloat32 _b;
					MFloat32 _a;
				};
				MFloat32 _data[4];
			};

#pragma endregion

#pragma region Functions Public

			MColor(MFloat32 r, MFloat32 g, MFloat32 b, MFloat32 a);
			MColor(MUint32 color);
			MColor(MFloat32* color);
			MColor(const MVector2& vec);
			MColor(const MVector3& vec);
			MColor(const MVector4& vec);
			MColor();
			MColor(const MColor& c);
			~MColor();

			MColor operator+(const MColor& rhs);
			MColor operator+(const MFloat32 rhs);
			MColor operator-(const MColor& rhs);
			MColor operator-(const MFloat32 rhs);
			MColor operator*(const MColor& rhs);
			MColor operator*(const MFloat32 rhs);
			MColor operator/(const MColor& rhs);
			MColor operator/(const MFloat32 rhs);

			MColor& operator+=(const MColor& rhs);
			MColor& operator+=(const MFloat32 rhs);
			MColor& operator-=(const MColor& rhs);
			MColor& operator-=(const MFloat32 rhs);
			MColor& operator*=(const MColor& rhs);
			MColor& operator*=(const MFloat32 rhs);
			MColor& operator/=(const MColor& rhs);
			MColor& operator/=(const MFloat32 rhs);

			MColor& operator=(const MColor& c);
			MColor& operator=(const MFloat32* ptr);
			MColor& operator=(MUint32 integerValue);
			MFloat32& operator[](MSize index);
			MFloat32 operator[](MSize index) const;
			bool operator==(const MColor& rhs) const;
			bool operator!=(const MColor& rhs) const;
			operator MFloat32*();
			operator MUint32();
			// Conversion is performed by taking R and A channels (black-white with alpha)
			operator MVector2();
			// Conversion is performed by taking RGB channels (opaque color)
			operator MVector3();
			operator MVector4();

#pragma endregion
		};

	}
}
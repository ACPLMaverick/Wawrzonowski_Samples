#pragma once

#include <stdint.h>

namespace math
{
	/// <summary>
	/// It is for indexing faces in obj-like manner
	/// Order is: Position, UV, Normal
	/// </summary>
	struct UShort3
	{
		union
		{
			uint16_t tab[3];
			struct { uint16_t p; uint16_t t; uint16_t n; };
		};

		UShort3();
		UShort3(uint16_t p, uint16_t t, uint16_t n);
		~UShort3();

		uint16_t operator[](size_t index) const { return tab[index]; }
	};
}
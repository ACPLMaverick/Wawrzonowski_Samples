#pragma once

#include <stdint.h>
#include "Math.h"

namespace math
{
	struct Int2
	{
		union
		{
			int32_t tab[2];
			struct { int32_t x; int32_t y; };
		};

		Int2()
		{
			this->x = 0;
			this->y = 0;
		}

		Int2(int32_t x, int32_t y)
		{
			this->x = x;
			this->y = y;
		}

		~Int2()
		{

		}

	};


}
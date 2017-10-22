#pragma once

#include "GlobalDefines.h"

namespace Postprocesses
{
	namespace Utility
	{
		class Helper
		{
		public:

			static inline uint32_t GetPowerOfTwoHigherThan(uint32_t val)
			{
				--val;
				val |= val >> 1;
				val |= val >> 2;
				val |= val >> 4;
				val |= val >> 8;
				val |= val >> 16;
				return val + 1;
			}

			static inline uint32_t GetTextureLevel(uint32_t inputWidth, uint32_t outputWidth)
			{
				uint32_t level = 0;
				while (inputWidth > outputWidth)
				{
					inputWidth >>= 1;
					++level;
				}
				return level;
			}
		};
	}
}
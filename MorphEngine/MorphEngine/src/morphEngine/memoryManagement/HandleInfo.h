#pragma once
#include "core/Singleton.h"

namespace morphEngine
{
	namespace memoryManagement
	{
		class HandleInfo : public core::Singleton<HandleInfo>
		{
		public:
			bool IsShutdown = false;
		};
	}
}
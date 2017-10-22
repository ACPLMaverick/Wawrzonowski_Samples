#pragma once

#include "../core/GlobalDefines.h"

#ifdef PLATFORM_WINDOWS
#include "ITime.h"

namespace morphEngine
{
	namespace core
	{
		class WindowsTime : public ITime
		{
			friend class ITime;

		protected:
			LARGE_INTEGER _frequency;
			LARGE_INTEGER _lastCounter;
			LARGE_INTEGER _currentCounter;
			LARGE_INTEGER _initCounter;

			WindowsTime() { }

		public:
			virtual void Init() override;
			virtual MFloat32 Tick() override;
			virtual MFloat32 GetRealtime() override;
			virtual MFloat32 GetLifetime() override;
		};
	}
}
#endif
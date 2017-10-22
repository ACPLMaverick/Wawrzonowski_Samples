#include "ITime.h"

#include "WindowsTime.h"

namespace morphEngine
{
	namespace core
	{
		void ITime::SetTimeScale(MFloat32 timeScale)
		{
			_timeScale = timeScale; 
		}

		MFloat32 ITime::GetTimeScale()
		{
			return _timeScale; 
		}

		MFloat32 ITime::GetUnscaledDeltaTime() 
		{
			return _unscaledDeltaTime; 
		}

		ITime* ITime::GetITimeInstance()
		{
#ifdef PLATFORM_WINDOWS
			WindowsTime* time = new WindowsTime();
#endif
			ME_ASSERT(time, "Time instance is null");
			time->Init();
			return time;
		}
	}
}
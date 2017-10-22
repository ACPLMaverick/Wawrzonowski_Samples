#include "WindowsTime.h"

#ifdef PLATFORM_WINDOWS

namespace morphEngine
{
	namespace core
	{
		void WindowsTime::Init()
		{
			_unscaledDeltaTime = 1.0f / 60.0f;
			_timeScale = 1.0f;

			QueryPerformanceFrequency(&_frequency);
			QueryPerformanceCounter(&_initCounter);
			_lastCounter = _initCounter;
			_currentCounter = _initCounter;
		}

		MFloat32 WindowsTime::Tick()
		{
			QueryPerformanceCounter(&_currentCounter);
			LARGE_INTEGER elapsed;
			elapsed.QuadPart = _currentCounter.QuadPart - _lastCounter.QuadPart;
			_unscaledDeltaTime = (MFloat32)elapsed.QuadPart / (MFloat32)_frequency.QuadPart;
			_lastCounter = _currentCounter;
			_deltaTime = _unscaledDeltaTime * _timeScale;
			_fps = 1.0f / _deltaTime;
			return _deltaTime;
		}

		MFloat32 WindowsTime::GetRealtime()
		{
			QueryPerformanceCounter(&_currentCounter);
			return (MFloat32)_currentCounter.QuadPart / (MFloat32)_frequency.QuadPart;
		}

		MFloat32 WindowsTime::GetLifetime()
		{
			LARGE_INTEGER current;
			LARGE_INTEGER elapsed;
			QueryPerformanceCounter(&current);
			elapsed.QuadPart = current.QuadPart - _initCounter.QuadPart;
			return (MFloat32)elapsed.QuadPart / (MFloat32)_frequency.QuadPart;
		}
	}
}

#endif
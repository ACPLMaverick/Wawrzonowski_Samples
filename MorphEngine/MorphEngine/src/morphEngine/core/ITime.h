#pragma once

#include "../core/GlobalDefines.h"

namespace morphEngine
{
	namespace core
	{
		class ITime
		{
		protected:
			MFloat32 _timeScale;
			MFloat32 _unscaledDeltaTime;
			MFloat32 _deltaTime;
			MFloat32 _fps;

			ITime() { }

		public:
			//Initializes timer
			virtual void Init() = 0;
			//Returns delta time (in seconds) from last Tick call (or Init) multiplied by time scale
			virtual MFloat32 Tick() = 0;
			//Returns realtime in seconds
			virtual MFloat32 GetRealtime() = 0;
			//Returns timer lifetime (whole time passed from calling Init function)
			virtual MFloat32 GetLifetime() = 0;
			//Sets timescale
			virtual void SetTimeScale(MFloat32 timeScale);
			//Returns timescale
			virtual MFloat32 GetTimeScale();
			//Returns unscaled delta time
			virtual MFloat32 GetUnscaledDeltaTime();
			//Returns scaled delta time. You should probably use this.
			inline virtual MFloat32 GetDeltaTime() { return _deltaTime; }
			
			// Calculated using unscaled delta time
			inline virtual MFloat32 GetFPS() { return _fps; }

			static ITime* GetITimeInstance();
		};
	}
}
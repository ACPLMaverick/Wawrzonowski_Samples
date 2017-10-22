#pragma once

#include "core/GlobalDefines.h"
#include "utility/Collections.h"
#include "core/Delegate.h"
#include "utility/MMath.h"

namespace morphEngine
{
	namespace animation
	{
		class AnimationFrame;

		class Timeline
		{
		protected:

			MFloat32 _length = FLT_MAX;
			MFloat32 _currentTime = 0.0f;
			MFloat32 _timeScale = 1.0f;
			bool _bIsLooped = true;
			MSize _lastIndex = 0;

		public:

			core::Event<void, Timeline&> EvtRewind;

			Timeline();
			Timeline(const Timeline& c);
			~Timeline();

			Timeline& operator=(const Timeline& c);
			inline bool operator==(const Timeline& other) const { return _length == other._length && _currentTime == other._currentTime && 
				_timeScale == other._timeScale && _bIsLooped == other._bIsLooped; }
			inline bool operator!=(const Timeline& other) const { return !operator==(other); }

			void Initialize(MFloat32 length, MFloat32 timeScale = 1.0f, bool bIsLooped = true);
			void Update();

			void Evaluate(const utility::MArray<AnimationFrame>& frames, AnimationFrame& outA, AnimationFrame& outB, MFloat32& outLerp) const;

			inline void Reset() { _currentTime = 0.0f; _lastIndex = 0; }
			inline void SetLength(MFloat32 length) { _length = length; }
			inline void SetTime(MFloat32 currentTime) { _currentTime = currentTime; }
			inline void SetTimeScale(MFloat32 timeScale) { _timeScale = timeScale; }
			inline void SetIsLooped(bool looped) { _bIsLooped = looped; }

			inline MFloat32 GetLength() const { return _length; }
			inline MSize GetFrame() const { return _lastIndex; }
			inline MFloat32 GetTime() const { return _currentTime; }
			inline MFloat32 GetTimeScale() const { return _timeScale; }
			inline bool GetIsForward() const { return _timeScale > 0.0f; }
			inline bool GetIsLooped() const { return _bIsLooped; }
			// Calling this function will not take into account animation's custom start and end offsets.
			inline bool GetIsFinished() const { return GetIsLooped() ? false : utility::MMath::Abs(_currentTime) == _length; }
		};

	}
}
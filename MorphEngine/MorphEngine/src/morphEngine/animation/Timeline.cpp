#include "Timeline.h"
#include "core/ITime.h"
#include "core/Engine.h"
#include "utility/MMath.h"
#include "animation/AnimationFrame.h"
#include "debugging/Debug.h"

namespace morphEngine
{
	namespace animation
	{

		Timeline::Timeline()
		{
		}

		Timeline::Timeline(const Timeline & c) :
			_length(c._length),
			_currentTime(c._currentTime),
			_timeScale(c._timeScale),
			_bIsLooped(c._bIsLooped),
			_lastIndex(c._lastIndex)
		{
		}

		Timeline::~Timeline()
		{
		}

		Timeline & Timeline::operator=(const Timeline & c)
		{
			_length = c._length;
			_currentTime = c._currentTime;
			_timeScale = c._timeScale;
			_bIsLooped = c._bIsLooped;
			_lastIndex = c._lastIndex;
			return *this;
		}

		void Timeline::Initialize(MFloat32 length, MFloat32 timeScale, bool bIsLooped)
		{
			_length = length;
			_timeScale = timeScale;
			_bIsLooped = bIsLooped;
			_currentTime = 0.0f;
		}

		void Timeline::Update()
		{
			_currentTime += core::Engine::GetInstance()->GetGlobalTime().GetDeltaTime() * _timeScale;

			if (GetIsForward())
			{
				if (_bIsLooped)
				{
					if (_currentTime > _length)
					{
						while (_currentTime > _length)
							_currentTime = (_currentTime - _length);
						_lastIndex = 0;
						EvtRewind(*this);
					}
					else if (_currentTime < 0.0f)
					{
						while (_currentTime < 0.0f)
							_currentTime = (_length + _currentTime);
						_lastIndex = 0;
						EvtRewind(*this);
					}
				}
				else
				{
					_currentTime = utility::MMath::Clamp(_currentTime, 0.0f, _length);
				}
			}
			else
			{
				if (_bIsLooped)
				{
					if (_currentTime < _length)
					{
						while (_currentTime < _length)
							_currentTime = (_currentTime + _length);
						_lastIndex = 0;
						EvtRewind(*this);
					}
					else if (_currentTime > 0.0f)
					{
						while (_currentTime > 0.0f)
							_currentTime = (_length - _currentTime);
						_lastIndex = 0;
						EvtRewind(*this);
					}
				}
				else
				{
					_currentTime = utility::MMath::Clamp(_currentTime, -_length, 0.0f);
				}
			}
		}

		void Timeline::Evaluate(const utility::MArray<AnimationFrame>& frames, AnimationFrame & outA, AnimationFrame & outB, MFloat32 & outLerp) const
		{
			MSize frameSize = frames.GetSize() - 1;

			if (frameSize < 1)
			{
				outA = frames[0];
				outB = frames[1];
				outLerp = 0.0f;
			}

			for (MSize i = _lastIndex; i < frameSize; ++i)
			{
				MFloat32 timeA = frames[i].GetTime();
				MFloat32 timeB = frames[i + 1].GetTime();
				if (_currentTime > timeA && _currentTime <= timeB)
				{
					const_cast<Timeline*>(this)->_lastIndex = i;
					outLerp = (_currentTime - timeA) / (timeB - timeA);
					outA = frames[i];
					outB = frames[i + 1];
					return;
				}
				else if (_currentTime > timeB)
				{
					outA = frames[frameSize - 1];
					outB = frames[frameSize];
					outLerp = 1.0f;
				}
				else
				{
					outA = frames[0];
					outB = frames[1];
					outLerp = 0.0f;
				}
			}
		}

	}
}
#include "Animation.h"
#include "assetLibrary/MAnimationClip.h"

namespace morphEngine
{
	namespace animation
	{
		Animation::Animation(const gom::ObjectInitializer& initializer) :
			BaseObject(initializer)
		{
			RegisterProperties();
		}

		void Animation::RegisterProperties()
		{
			RegisterProperty("Clip", &_clip);
			RegisterProperty("Timeline", &_timeline);
			RegisterProperty("Name", &_name);
			RegisterProperty("EvtsPerFrame", &_evtsPerFrame);
			RegisterProperty("EvtStartOffset", &_evtStartOffset);
			RegisterProperty("EvtEndOffset", &_evtEndOffset);
			RegisterProperty("EvtStart", &EvtStart);
			RegisterProperty("EvtEnd", &EvtEnd);
		}

		Animation::Animation(const Animation & c, bool bDeepCopy) :
			BaseObject(c, bDeepCopy),
			_clip(c._clip),
			_evaluatedFrame(c._evaluatedFrame),
			_timeline(c._timeline),
			_name(c._name),
			_evtsPerFrame(c._evtsPerFrame),
			_evtStartOffset(c._evtStartOffset),
			_evtEndOffset(c._evtEndOffset),
			_bInvokedStartEvent(c._bInvokedStartEvent),
			_bInvokedEndEvent(c._bInvokedEndEvent),
			EvtStart(c.EvtStart),
			EvtEnd(c.EvtEnd)
		{
			RegisterProperties();
		}


		Animation::~Animation()
		{
		}

		Animation & Animation::operator=(const Animation & c)
		{
			_clip = c._clip;
			_evaluatedFrame = c._evaluatedFrame;
			_timeline = c._timeline;
			_name = c._name;
			_evtsPerFrame = c._evtsPerFrame;
			_evtStartOffset = c._evtStartOffset;
			_evtEndOffset = c._evtEndOffset;
			_bInvokedStartEvent = c._bInvokedStartEvent;
			_bInvokedEndEvent = c._bInvokedEndEvent;
			EvtStart = c.EvtStart;
			EvtEnd = c.EvtEnd;
			return *this;
		}

		inline void Animation::Update()
		{
			if (_bEnabled)
			{
				MFloat32 time = _timeline.GetTime();
				MFloat32 timeAbs = MMath::Abs(time);

				if (timeAbs >= _evtStartOffset && !_bInvokedStartEvent)
				{
					EvtStart(*this, time);
					_bInvokedStartEvent = true;
				}

				if (timeAbs >= (_timeline.GetLength() - _evtEndOffset) && !_bInvokedEndEvent)
				{
					EvtEnd(*this, time);
					_bInvokedEndEvent = true;
				}


				MSize frame = _timeline.GetFrame();
				ME_ASSERT_S(frame < _evtsPerFrame.GetSize());
				_evtsPerFrame[frame].operator()(*this, time, frame);

				_timeline.Update();
			}
		}

		const AnimationFrame & Animation::EvaluateAnimation() const
		{
#ifdef _DEBUG
			if (_clip != nullptr)
			{
#endif // _DEBUG

			AnimationFrame& a = const_cast<AnimationFrame&>(_evaluatedFrame), b = const_cast<AnimationFrame&>(_evaluatedFrame);
			MFloat32 lerp;
			_timeline.Evaluate(_clip->GetFrames(), a, b, lerp);
			AnimationFrame::Interpolate(a, b, lerp, const_cast<AnimationFrame&>(_evaluatedFrame));

#ifdef _DEBUG
			}
#endif // _DEBUG

			return _evaluatedFrame;
		}

		const AnimationFrame & Animation::Lerp(Animation& a, const Animation& b, MFloat32 lerp)
		{
			const AnimationFrame& frameA = a.EvaluateAnimation(), 
				frameB = const_cast<AnimationFrame&>(b.EvaluateAnimation());
			AnimationFrame::Interpolate(frameA, frameB, lerp, const_cast<AnimationFrame&>(a._evaluatedFrame));

			return a._evaluatedFrame;
		}

		void Animation::OnTimelineRewind(Timeline & tm)
		{
			if (!_bInvokedEndEvent)
			{
				EvtEnd(*this, tm.GetLength() * MMath::Sign(tm.GetTimeScale()));
			}
			_bInvokedEndEvent = false;
			_bInvokedStartEvent = false;
		}
	}
}
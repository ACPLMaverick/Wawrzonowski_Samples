#pragma once

#include "gom/BaseObject.h"
#include "animation/Timeline.h"
#include "animation/AnimationFrame.h"
#include "assetLibrary/MAnimationClip.h"
#include "utility/Collections.h"
#include "utility/MMatrix.h"
#include "core/Delegate.h"

namespace morphEngine
{
	namespace animation
	{
		class Animation : public gom::BaseObject
		{
			ME_TYPE
		protected:

			AnimationFrame _evaluatedFrame;
			Timeline _timeline;
			utility::MString _name;
			assetLibrary::MAnimationClip* _clip = nullptr;

			// Each Event in this MArray corresponds to a single frame. 
			// Animation, Current Animation Time (seconds)
			utility::MArray<core::Event<void, Animation&, MFloat32, MSize>> _evtsPerFrame;

			MFloat32 _evtStartOffset = 0.0f;
			MFloat32 _evtEndOffset = 0.0f;
			bool _bInvokedStartEvent = false;
			bool _bInvokedEndEvent = false;

			void OnTimelineRewind(Timeline& tm);

			Animation(const gom::ObjectInitializer& initializer);

			virtual void RegisterProperties() override;

		public:

			// Animation, Current Animation Time (seconds)
			core::Event<void, Animation&, MFloat32> EvtStart;

			// Animation, Current Animation Time (seconds)
			core::Event<void, Animation&, MFloat32> EvtEnd;

			Animation(const Animation& c, bool bDeepCopy = true);
			virtual ~Animation();

			virtual Animation& operator=(const Animation& c);

			inline virtual void Initialize(assetLibrary::MAnimationClip* clip, const utility::MString& name = "", bool bIsLooped = true,
				MFloat32 eventStartTimeOffset = 0.0f, MFloat32 eventEndTimeOffset = 0.0f, MFloat32 speed = 1.0f) 
			{ 
				_evtStartOffset = eventStartTimeOffset;
				_evtEndOffset = eventEndTimeOffset;

				ME_ASSERT_S(clip != nullptr); 

				if (name == "")
				{
					_name = clip->GetName();
				}
				else
				{
					_name = name;
				}

				_clip = clip; 
				_evaluatedFrame.InitializeEmpty(_clip->GetFrames()[0].GetDataSize());
				_timeline.Initialize(_clip->GetLength(), speed, bIsLooped);
				_timeline.EvtRewind += new core::Event<void, Timeline&>::ClassRawDelegate<Animation>(this, &Animation::OnTimelineRewind);

				core::Event<void, Animation&, MFloat32, MSize> ev;
				_evtsPerFrame.Resize(_clip->GetFrames().GetSize());
				_evtsPerFrame.Fill(ev);
			}
			inline virtual void Update();

			virtual inline assetLibrary::MAnimationClip* GetClip() const { return _clip; }
			virtual inline Timeline& GetTimeline() { return _timeline; }
			inline const utility::MString& GetName() const { return _name; }

			virtual const AnimationFrame& EvaluateAnimation() const;
			static const AnimationFrame& Lerp(Animation& a, const Animation& b, MFloat32 lerp);

			inline bool operator==(const Animation& other) const { return _clip == other._clip && _evaluatedFrame == other._evaluatedFrame && _timeline == other._timeline; }
			inline bool operator!=(const Animation& other) const { return !operator==(other); }

			// Calling this function will take into account animation's custom start and end offsets.
			virtual inline bool GetIsFinished() const
			{
				if (_timeline.GetIsLooped())
					return false;
				else
					return (_timeline.GetTime() >= (_timeline.GetLength() - _evtEndOffset));
			}
		};
	}
}
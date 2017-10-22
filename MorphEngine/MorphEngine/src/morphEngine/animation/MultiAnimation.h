#pragma once

#include "animation/Animation.h"

namespace morphEngine
{
	namespace animation
	{
		typedef core::Event<
			void,
			utility::MArray<memoryManagement::Handle<Animation>>&,
			utility::MArray<MFloat32>&
		> MultiAnimationEvaluationFuncSet;
		typedef typename MultiAnimationEvaluationFuncSet::BaseDelegate* MultiAnimationEvaluationFunc;

		class MultiAnimation :
			public Animation
		{
			ME_TYPE
		protected:

#pragma region Protected

			utility::MArray<memoryManagement::Handle<Animation>> _animations;
			utility::MArray<MFloat32> _weights;
			utility::MArray<const AnimationFrame*> _tempFrames;

			MultiAnimationEvaluationFunc _evalFunc = nullptr;

#pragma endregion

#pragma region Functions Protected

			MultiAnimation(const gom::ObjectInitializer& initializer);

			virtual void RegisterProperties() override;

			inline void NormalizeWeights();

#pragma endregion

		public:

#pragma region Functions Public

			MultiAnimation(const MultiAnimation& other, bool bDeepCopy = true);
			virtual ~MultiAnimation() { if (_evalFunc != nullptr) delete _evalFunc; }

			virtual MultiAnimation& operator=(const MultiAnimation& c);

			// Calling this function is not supported.
			inline virtual void Initialize(assetLibrary::MAnimationClip* clip = nullptr, const utility::MString& name = "", bool bIsLooped = true,
				MFloat32 eventStartTimeOffset = 0.0f, MFloat32 eventEndTimeOffset = 0.0f, MFloat32 speed = 1.0f)
			{
				ME_WARNING(false, "MultiAnimation: Calling this function is not supported.");
			}

			inline void Initialize(MultiAnimationEvaluationFunc evalFunc, const utility::MString& name)
			{
				_evalFunc = evalFunc;
				_name = name;
			}

			virtual void Update() override;

			inline void SetEvaluationFunc(MultiAnimationEvaluationFunc evalFunc, bool bDeletePrevious = true)
			{
				if (_evalFunc != nullptr && bDeletePrevious)
					delete _evalFunc;
				_evalFunc = evalFunc;
			}

			inline MultiAnimationEvaluationFunc GetEvaluationFunc() { return _evalFunc; }

			inline void AddAnimation(memoryManagement::Handle<Animation> anim)
			{
				if (_animations.IsEmpty())
				{
					_evaluatedFrame.InitializeEmpty(anim->GetClip()->GetFrames()[0].GetDataSize());
				}
				_animations.Add(anim);
				_weights.Add(1.0f);
			}

			inline void RemoveAnimation(memoryManagement::Handle<Animation> anim)
			{
				MSize ind = _animations.IndexOf(anim);
				if (ind != UINT64_MAX)
				{
					_animations.RemoveAt(ind);
					_weights.RemoveAt(ind);
				}
			}

			inline void RemoveAnimation(MSize index)
			{
				_animations.RemoveAt(index);
				_weights.RemoveAt(index);
			}

			inline memoryManagement::Handle<Animation> GetAnimation(MSize index) const { return _animations[index]; }
			virtual inline assetLibrary::MAnimationClip* GetClip() const { return _animations[0]->GetClip(); }
			virtual inline Timeline& GetTimeline() { return _animations[0]->GetTimeline(); }

			virtual const AnimationFrame& EvaluateAnimation() const;

			virtual inline bool GetIsFinished() const
			{
				for (auto it = const_cast<MultiAnimation*>(this)->_animations.GetBegin(); it.IsValid(); ++it)
				{
					if (!(*it)->GetIsFinished())
						return false;
				}
				return true;
			}

#pragma endregion

		};
	}
}
#include "MultiAnimation.h"
#include "debugging\Debug.h"

namespace morphEngine
{
	using namespace debugging;
	namespace animation
	{
		MultiAnimation::MultiAnimation(const gom::ObjectInitializer& initializer) :
			Animation(initializer)
		{
			RegisterProperties();
		}

		void MultiAnimation::RegisterProperties()
		{
			RegisterProperty("Animations", &_animations);
			RegisterProperty("Weights", &_weights);
		}

		MultiAnimation::MultiAnimation(const MultiAnimation & c, bool bDeepCopy) :
			Animation(c, bDeepCopy),
			_animations(c._animations),
			_weights(c._weights),
			_evalFunc(c._evalFunc)
		{
			RegisterProperties();
		}

		MultiAnimation & MultiAnimation::operator=(const MultiAnimation & c)
		{
			Animation::operator=(c);

			_animations = c._animations;
			_weights = c._weights;
			_evalFunc = c._evalFunc;

			return *this;
		}

		void MultiAnimation::Update()
		{
			// evaluate weights
#if _DEBUG
			if (_evalFunc != nullptr)
			{
#endif

				(*_evalFunc)(_animations, _weights);

#if _DEBUG
			}
#endif

			// normalize weights
			NormalizeWeights();

			// update animations
			for (auto it = _animations.GetBegin(); it.IsValid(); ++it)
			{
				(*it)->Update();
			}
		}

		const AnimationFrame & MultiAnimation::EvaluateAnimation() const
		{
			const_cast<MultiAnimation*>(this)->_tempFrames.Clear();

			for (MSize i = 0; i < _animations.GetSize(); ++i)
			{
				const_cast<MultiAnimation*>(this)->_tempFrames.Add(&_animations[i]->EvaluateAnimation());
			}

			AnimationFrame::InterpolateMultiple(const_cast<MultiAnimation*>(this)->_tempFrames,
				const_cast<MultiAnimation*>(this)->_weights, 
				const_cast<MultiAnimation*>(this)->_evaluatedFrame);

			return _evaluatedFrame;
		}

		inline void MultiAnimation::NormalizeWeights()
		{
			MFloat32 sum = 0.0f;
			for (auto it = _weights.GetBegin(); it.IsValid(); ++it)
			{
				sum += (*it);
			}

			for (auto it = _weights.GetBegin(); it.IsValid(); ++it)
			{
				(*it) /= sum;
			}
		}
	}
}
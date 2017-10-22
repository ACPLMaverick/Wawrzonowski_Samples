#pragma once

#include "gom/GameObjectComponent.h"
#include "animation/Animation.h"
#include "core/StateMachine.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MAnimationClip;
	}

	namespace animation
	{
		typedef core::StateMachine<Handle<animation::Animation>, animation::AnimationFrame&> AnimStateMachine;

		class Animator : public gom::GameObjectComponent
		{
			ME_TYPE

		protected:


#pragma region Protected

			AnimStateMachine _sm;
			animation::AnimationFrame _dummyFrame;

#pragma endregion

#pragma region Functions Protected

			void AnimUpdate(Handle<animation::Animation>& anim) { anim->Update(); }
			void OnTransitionBeginResetAnimTo(AnimStateMachine::Transition* transition)
			{
				_sm.GetState(transition->StateTo)->Obj->GetTimeline().Reset();
			}
			bool IsNonLoopingAnimationFinished(AnimStateMachine::Transition* transition)
			{
				Handle<Animation> anim = _sm.GetState(transition->StateFrom)->Obj;
				if (anim->GetIsFinished())
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			inline Animator(const gom::ObjectInitializer& initializer) : GameObjectComponent(initializer), 
				_sm(static_cast<memoryManagement::Handle<gom::GameObjectComponent>>(_this),
					new AnimStateMachine::StateObjectUpdateFunctionSet::ClassDelegate<Animator>(_this, &Animator::AnimUpdate))
			{
				RegisterProperties();
			}

			virtual void RegisterProperties() override;

			virtual void RegisterComponent() { }
			virtual void DeregisterComponent() { }

#pragma endregion

		public:

#pragma region Functions Public

			inline Animator(const Animator& c, bool bDeepCopy = true) : GameObjectComponent(c, bDeepCopy), _sm(c._sm) { RegisterProperties(); _dummyFrame.InitializeEmpty(96); }
			inline virtual ~Animator() { }

			virtual void Initialize() override;

			inline virtual void Update() override
			{
				GameObjectComponent::Update();

				_sm.Update();

				if (_sm.GetIsTransition())
				{
					auto transition = _sm.GetCurrentTransition();
					_dummyFrame = Animation::Lerp(*_sm.GetState(transition->StateFrom)->Obj, *_sm.GetState(transition->StateTo)->Obj, transition->Progress);
				}
				else
				{
					_dummyFrame = GetCurrentAnimation()->EvaluateAnimation();
				}
			}

			inline virtual Animator& operator=(const Animator& c) { return reinterpret_cast<Animator&>(GameObjectComponent::operator=(c)); _sm = c._sm; }

			// Returns index of the state created by this animation.
			inline MSize AddAnimation(Handle<animation::Animation> animation)
			{
				auto state = _sm.AddState(animation, animation->GetName());
				return state->FlatIndex;
			}

			inline void RemoveAnimation(const utility::MString& name)
			{
				_sm.RemoveState(_sm.GetState(name));
			}

			// Note that for transitions to NON-looping animations, a return transition condition will automatically be added, turning true when such animation ends.
			// Recommended TransitionConditionMode for this kind of situation is OR.
			inline AnimStateMachine::Transition*
				AddTransition(const utility::MString& nameFrom, const utility::MString& nameTo,
					AnimStateMachine::TransitionConditionalFunction conditionalFunction,
					AnimStateMachine::TransitionType transitionType = AnimStateMachine::TransitionType::LINEAR,
					AnimStateMachine::TransitionConditionMode transitionMode = AnimStateMachine::TransitionConditionMode::OR,
					MFloat32 speed = 1.0f)
			{
				AnimStateMachine::State* stateFrom = _sm.GetState(nameFrom);
				AnimStateMachine::State* stateTo = _sm.GetState(nameTo);
				AnimStateMachine::Transition* tr = 
					_sm.AddTransition(stateFrom, stateTo, conditionalFunction, nullptr, transitionType, transitionMode, speed);
				tr->EvtBegin += new AnimStateMachine::TransitionBeginEvent::ClassDelegate<Animator>(_this, &Animator::OnTransitionBeginResetAnimTo);

				//  // if state to is non-looping animation, automatically add return transition after animation ends
				if (!stateTo->Obj->GetTimeline().GetIsLooped())
				{
					AnimStateMachine::Transition* tr2 =
						_sm.AddTransition(stateTo, stateFrom, 
							new AnimStateMachine::TransitionConditionalFunctionSet::ClassDelegate<Animator>(_this, &Animator::IsNonLoopingAnimationFinished),
							nullptr, transitionType, transitionMode, speed);
					tr2->EvtBegin += new AnimStateMachine::TransitionBeginEvent::ClassDelegate<Animator>(_this, &Animator::OnTransitionBeginResetAnimTo);
				}

				return tr;
			}

			inline void RemoveTransition(MSize transitionIndex)
			{
				_sm.RemoveTransition(transitionIndex);
			}

			inline void SetAnimation(const utility::MString& name) { _sm.SetCurrentState(_sm.GetState(name)); }

			inline AnimStateMachine::Transition*
				GetTransition(MSize index)
			{
				return _sm.GetTransition(index);
			}

			inline AnimStateMachine::State*
				GetState(MSize index)
			{
				return _sm.GetState(index);
			}

			inline AnimStateMachine::State*
				GetState(const utility::MString& name)
			{
				return _sm.GetState(name);
			}

			inline Handle<animation::Animation> GetAnimation(MSize index)
			{
				return _sm.GetState(index)->Obj;
			}

			inline Handle<animation::Animation> GetAnimation(const utility::MString& name)
			{
				return _sm.GetState(name)->Obj;
			}

			// Returns nullptr when no animation is set
			inline AnimStateMachine::State*
				GetCurrentState()
			{
				return _sm.GetCurrentState();
			}

			// Returns nullptr when no transition occurs
			inline AnimStateMachine::Transition*
				GetCurrentTransition()
			{
				return _sm.GetCurrentTransition();
			}

			// Always returns "FROM" animation if an animation state transition occurs.
			inline Handle<animation::Animation> GetCurrentAnimation() { return _sm.GetCurrentState()->Obj; }

			// This may include one or two animations (if state transition actually occurs).
			// If transition does not occur at the moment of call, nullptr is returned
			inline void GetCurrentAnimations(Handle<animation::Animation>* outAnimFrom, Handle<animation::Animation>* outAnimTo)
			{
				*outAnimFrom = (GetCurrentAnimation());

				if (_sm.GetIsTransition())
				{
					*outAnimTo = _sm.GetState(_sm.GetCurrentTransition()->StateTo)->Obj;
				}
				else
				{
					*outAnimTo = nullptr;
				}
			}

			inline const animation::AnimationFrame& GetCurrentFrame() 
			{ 
				return _dummyFrame;
			}

#pragma endregion

		};

	}
}
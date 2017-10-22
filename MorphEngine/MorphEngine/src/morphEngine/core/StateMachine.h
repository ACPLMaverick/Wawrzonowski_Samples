#pragma once
#include "GlobalDefines.h"
#include "utility/Collections.h"
#include "core/Engine.h"
#include "core/ITime.h"
#include "utility/MMath.h"
#include "memoryManagement/Handle.h"
#include "gom/GameObjectComponent.h"
#include "core/Delegate.h"

namespace morphEngine
{
	namespace core
	{
		/*
			T defines object that is stored in each state. T is required to have Update function defined. 
			O is used for Evaluation function, this is the type that is returned when blending two states.
			For example, when blending two animation it is more convenient to have blending done on AnimationFrame and result returned as AnimationFrame.
			Nevertheless, it may perfectly be the same type as T for simpler applications.

			ConditionalFuncSet is per-frame called static update function which evaluates some predicate and specifies whether transition can occur.
			BlendFuncSet is a function called to evaluate a value between origin and destination states when blending.

			Usage: Call GetIsTransition to check if transition is in progress. If false, you can safely call GetCurrentState and extract your object from
			the state variable. If true, call GetTransition to get current transition and call BlendFuncSet to get blended object of this transition. Transitions
			and states are updated automatically. Only currently active state[s] are updated. Only transitions originating from currently active state are
			updated.
		*/
		template <class T, class O>
		class StateMachine
		{
		public:

#pragma region StaticConst

			static const MSize INVALID_INDEX = UINT64_MAX;

#pragma endregion


#pragma region Classes Enums Public

			class Transition;

			typedef MSize StateIndex;
			typedef MSize TransitionIndex;
			//typedef bool(*TransitionConditionalFunction)(memoryManagement::Handle<gom::GameObjectComponent> ownerInstance);
			//typedef void(*TransitionBlendFunction)(T& one, T& two, MFloat32 lerp, O& outResult);
			typedef Event<bool, Transition*> TransitionConditionalFunctionSet;
			typedef Event<void> TransitionBlendFunctionSet;
			typedef Event<void, Transition*> TransitionBeginEvent;
			typedef TransitionBeginEvent TransitionEndEvent;
			typedef typename TransitionConditionalFunctionSet::BaseDelegate* TransitionConditionalFunction;
			typedef TransitionBlendFunctionSet::BaseDelegate* TransitionBlendFunction;
			typedef Event<void, T&> StateObjectUpdateFunctionSet;
			typedef typename StateObjectUpdateFunctionSet::BaseDelegate* StateObjectUpdateDelegate;


			enum class TransitionType
			{
				INSTANT,
				LINEAR
			};

			enum class TransitionConditionMode
			{
				OR,
				AND
			};

			class State
			{
				friend class StateMachine<T, O>;
			protected:
				inline State(T& obj, const utility::MString& name, MSize flatIndex) : Obj(obj), Name(name), FlatIndex(flatIndex),
					TransitionsIn(4), TransitionsOut(4)
				{

				}
			public:
				T Obj;
				utility::MString Name;
				MSize FlatIndex;
				utility::MArray<TransitionIndex> TransitionsIn;
				utility::MArray<TransitionIndex> TransitionsOut;

				inline State& operator=(const State& c) { Obj = c.Obj; FlatIndex = c.FlatIndex; return *this; }

				inline bool operator==(const State& other) const { return Obj == other.Obj && FlatIndex == other.FlatIndex; }
				inline bool operator!=(const State& other) const { return !operator==(other); }
			};

			class Transition
			{
				friend class StateMachine<T, O>;
			protected:
				inline Transition() { }
				inline Transition(StateIndex from, StateIndex to, TransitionConditionalFunction func,
					TransitionBlendFunction blendFunc, TransitionIndex index, memoryManagement::Handle<gom::GameObjectComponent> ownerInstance,
					TransitionType type = TransitionType::LINEAR, TransitionConditionMode cMode = TransitionConditionMode::OR, MFloat32 speed = 1.0f) :
					StateFrom(from),
					StateTo(to),
					Type(type),
					ConditionMode(cMode),
					Speed(speed),
					FlatIndex(index),
					OwnerInstance(ownerInstance)
				{
					ME_ASSERT(func != nullptr, "ConditionalFunction for Transition cannot be null!");
					ConditionalFuncSet += func;
					if (blendFunc != nullptr)
					{
						BlendFuncSet += blendFunc;
					}
				}

				// Returns true when needed to call in next frame, returns false when finished
				inline bool EvaluateConditional();
				inline bool Update();
				inline void Update_Internal();

			public:
				StateIndex StateFrom = INVALID_INDEX;
				StateIndex StateTo = INVALID_INDEX;

				TransitionBeginEvent EvtBegin;
				TransitionEndEvent EvtEnd;

				TransitionConditionalFunctionSet ConditionalFuncSet;
				TransitionBlendFunctionSet BlendFuncSet;

				TransitionType Type = TransitionType::LINEAR;
				TransitionConditionMode ConditionMode = TransitionConditionMode::OR;
				MFloat32 Speed = 1.0f;
				MFloat32 Progress = 0.0f;
				TransitionIndex FlatIndex = INVALID_INDEX;
				memoryManagement::Handle<gom::GameObjectComponent> OwnerInstance;

				inline Transition(const Transition& c) :
					StateFrom(c.StateFrom),
					StateTo(c.StateTo),
					EvtBegin(c.EvtBegin),
					EvtEnd(c.EvtEnd),
					ConditionalFuncSet(c.ConditionalFuncSet),
					BlendFuncSet(c.BlendFuncSet),
					Type(c.Type),
					ConditionMode(c.ConditionMode),
					Speed(c.Speed),
					Progress(c.Progress),
					FlatIndex(c.FlatIndex),
					OwnerInstance(c.OwnerInstance)
				{

				}

				Transition& operator=(const Transition& c)
				{
					StateFrom = c.StateFrom;
					StateTo = c.StateTo;
					EvtBegin = c.EvtBegin;
					EvtEnd = c.EvtEnd;
					ConditionalFuncSet = c.ConditionalFuncSet;
					BlendFuncSet = c.BlendFuncSet;
					Type = c.Type;
					ConditionMode = c.ConditionMode;
					Speed = c.Speed;
					Progress = c.Progress;
					FlatIndex = c.FlatIndex;
					OwnerInstance = c.OwnerInstance;
				}

				inline bool operator==(const Transition& other) const { return StateFrom == other.StateFrom && StateTo == other.StateTo && 
					ConditionalFuncSet == other.ConditionalFuncSet && BlendFuncSet == other.BlendFuncSet && Type == other.Type; }
				inline bool operator!=(const Transition& other) const { return !operator==(other); }
			};

#pragma endregion

		protected:

#pragma region Protected:

			utility::MArray<State> _states;
			utility::MArray<Transition> _transitions;

			utility::MDictionary<utility::MString, StateIndex> _statesByName;

			memoryManagement::Handle<gom::GameObjectComponent> _owner;

			StateObjectUpdateFunctionSet _stateUpdateFunc;

			StateIndex _currentState = INVALID_INDEX;
			TransitionIndex _currentTransition = INVALID_INDEX;

#pragma endregion

#pragma region Functions Protected

			inline void RemoveNullPointingTransitions(StateIndex deletedState);
			inline void FixStateIndicesOnDelete(StateIndex startIndex, MInt64 howMany);
			inline void FixTransitionIndicesOnDelete(TransitionIndex startIndex, MInt64 howMany);

#pragma endregion

		public:

#pragma region Functions Public

			inline StateMachine(memoryManagement::Handle<gom::GameObjectComponent> owner, StateObjectUpdateDelegate updateFunction = nullptr);
			inline StateMachine(const StateMachine& copy);
			inline ~StateMachine();

			inline StateMachine& operator=(const StateMachine& copy);

			inline void Update();

			inline State* AddState(T& object, const utility::MString& name);
			inline void RemoveState(State* state);
			inline void RemoveState(MSize index);
			inline Transition* AddTransition(State* stateFrom, State* stateTo, TransitionConditionalFunction condFunc, 
				TransitionBlendFunction blendFunc, TransitionType type = TransitionType::LINEAR, 
				TransitionConditionMode cMode = TransitionConditionMode::OR, MFloat32 speed = 1.0f);
			inline void RemoveTransition(Transition* transition);
			inline void RemoveTransition(MSize index);

			inline void SetCurrentState(State* state);

			inline State* GetCurrentState();
			inline bool GetIsTransition();
			inline bool GetIsInitialized();
			inline Transition* GetCurrentTransition();
			inline State* GetState(MSize index);
			inline State* GetState(utility::MString name);
			inline Transition* GetTransition(MSize index);
			inline MSize GetStateCount();
			inline MSize GetTransitionCount();

#pragma endregion

		};

#pragma region Definitions

		template<class T, class O>
		inline StateMachine<T, O>::StateMachine(memoryManagement::Handle<gom::GameObjectComponent> owner, StateObjectUpdateDelegate updateFunction) :
			_owner(owner)
		{
			if (updateFunction != nullptr)
			{
				_stateUpdateFunc += updateFunction;
			}
		}

		template<class T, class O>
		inline StateMachine<T, O>::StateMachine(const StateMachine & copy) : 
			_states(copy._states),
			_transitions(copy._transitions),
			_statesByName(copy._statesByName)
		{
		}

		template<class T, class O>
		inline StateMachine<T, O>::~StateMachine()
		{
		}

		template<class T, class O>
		inline StateMachine<T, O>& StateMachine<T, O>::operator=(const StateMachine & copy)
		{
			_states = copy._states;
			_transitions = copy._transitions;
			_statesByName = copy._statesByName;
			return *this;
		}

		template<class T, class O>
		inline void StateMachine<T, O>::Update()
		{
			if (GetIsInitialized())
			{
				if (_currentTransition == INVALID_INDEX)
				{
					State* state = GetCurrentState();
					_stateUpdateFunc(state->Obj);

					for (auto it = state->TransitionsOut.GetBegin(); it.IsValid(); ++it)
					{
						Transition* trans = GetTransition(*it);
						if (trans->Update() && _currentTransition == INVALID_INDEX)
						{
							// first step of transition interpolation is done, so update TO state as well
							_stateUpdateFunc(GetState(trans->StateTo)->Obj);
							if (trans->Type == TransitionType::LINEAR)
								_currentTransition = *it;
							else
								_currentState = trans->StateTo;
						}
					}
				}
				else
				{
					Transition* trans = GetTransition(_currentTransition);
					State* from = GetState(trans->StateFrom);
					State* to = GetState(trans->StateTo);

					_stateUpdateFunc(from->Obj);
					_stateUpdateFunc(to->Obj);

					if (!trans->Update())
					{
						_currentState = to->FlatIndex;
						_currentTransition = INVALID_INDEX;
					}
				}
			}
		}

		template<class T, class O>
		inline typename StateMachine<T, O>::State* StateMachine<T, O>::AddState(T & object, const utility::MString& name)
		{
			StateIndex sIndex = _states.GetSize();
			State newState(object, name, sIndex);
			_states.Add(newState);
			State* newStatePtr = &_states[sIndex];

			_statesByName.Add(name, sIndex);

			if (_currentState == INVALID_INDEX)
			{
				_currentState = sIndex;
			}

			return newStatePtr;
		}

		template<class T, class O>
		inline void StateMachine<T, O>::RemoveState(State * state)
		{
			RemoveState(state->FlatIndex);
		}

		template<class T, class O>
		inline void StateMachine<T, O>::RemoveState(MSize index)
		{
			if (index == _currentState)
			{
				if (index == 0)
					_currentState = INVALID_INDEX;
				else
					--_currentState;	// we always go "down" in states. Though it is generally not recommended to remove current state.
			}

			_statesByName.Remove(GetState(index)->Name);
			_states.RemoveAt(index);

			RemoveNullPointingTransitions(index);
			FixStateIndicesOnDelete(index, -1);
		}

		template<class T, class O>
		inline typename StateMachine<T, O>::Transition* StateMachine<T, O>::AddTransition(State * stateFrom, State * stateTo, TransitionConditionalFunction condFunc,
			TransitionBlendFunction blendFunc, TransitionType type, TransitionConditionMode cMode, MFloat32 speed)
		{
			TransitionIndex ind = _transitions.GetSize();
			Transition tr(stateFrom->FlatIndex, stateTo->FlatIndex, condFunc, blendFunc, ind, _owner, type, cMode, speed);
			_transitions.Add(tr);
			Transition* trPtr = &_transitions[ind];

			stateFrom->TransitionsOut.Add(ind);
			stateTo->TransitionsIn.Add(ind);

			return trPtr;
		}

		template<class T, class O>
		inline void StateMachine<T, O>::RemoveTransition(Transition * transition)
		{
			RemoveTransition(transition->FlatIndex);
		}

		template<class T, class O>
		inline void StateMachine<T, O>::RemoveTransition(MSize index)
		{
			if (_currentTransition == index)
			{
				// when deleting current transition, destination state is set
				_currentState = GetTransition(index)->StateTo;
				_currentTransition = INVALID_INDEX;
			}

			_transitions.RemoveAt(index);

			FixTransitionIndicesOnDelete(index, -1);
		}

		template<class T, class O>
		inline void StateMachine<T, O>::SetCurrentState(State * state)
		{
			_currentState = state->FlatIndex;
		}

		template<class T, class O>
		inline typename StateMachine<T, O>::State* StateMachine<T, O>::GetCurrentState()
		{
			return _currentState != INVALID_INDEX ? GetState(_currentState) : nullptr;
		}

		template<class T, class O>
		inline bool StateMachine<T, O>::GetIsTransition()
		{
			return _currentTransition != INVALID_INDEX;
		}

		template<class T, class O>
		inline bool StateMachine<T, O>::GetIsInitialized()
		{
			return _states.GetSize() > 0;
		}

		template<class T, class O>
		inline typename StateMachine<T, O>::Transition * StateMachine<T, O>::GetCurrentTransition()
		{
			return _currentTransition != INVALID_INDEX ? GetTransition(_currentTransition) : nullptr;
		}

		template<class T, class O>
		inline typename StateMachine<T, O>::State * StateMachine<T, O>::GetState(MSize index)
		{
			return index < GetStateCount() ? &_states[index] : nullptr;
		}

		template<class T, class O>
		inline typename StateMachine<T, O>::State * StateMachine<T, O>::GetState(utility::MString name)
		{
			return GetState(_statesByName[name]);
		}

		template<class T, class O>
		inline typename StateMachine<T, O>::Transition * StateMachine<T, O>::GetTransition(MSize index)
		{
			return index < GetTransitionCount() ? &_transitions[index] : nullptr;
		}

		template<class T, class O>
		inline MSize StateMachine<T, O>::GetStateCount()
		{
			return _states.GetSize();
		}

		template<class T, class O>
		inline MSize StateMachine<T, O>::GetTransitionCount()
		{
			return _transitions.GetSize();;
		}

		template<class T, class O>
		inline void StateMachine<T, O>::RemoveNullPointingTransitions(StateIndex deletedState)
		{
			for (auto it = _transitions.GetBegin(); it.IsValid(); ++it)
			{
				if ((*it).StateFrom == deletedState || (*it).StateTo == deletedState)
				{
					RemoveTransition((*it).FlatIndex);
					--it;	// to avoid iterator fuckup when modifying collection
				}
			}
		}

		template<class T, class O>
		inline void StateMachine<T, O>::FixStateIndicesOnDelete(StateIndex startIndex, MInt64 howMany)
		{
			for (auto it = _transitions.GetBegin(); it.IsValid(); ++it)
			{
				if ((*it).StateFrom >= startIndex)
					(*it).StateFrom += howMany;
				if ((*it).StateTo >= startIndex)
					(*it).StateTo += howMany;
			}

			for (auto it = _states.GetBegin(); it.IsValid(); ++it)
			{
				if ((*it).FlatIndex >= startIndex)
					(*it).FlatIndex += howMany;
			}
		}

		template<class T, class O>
		inline void StateMachine<T, O>::FixTransitionIndicesOnDelete(TransitionIndex startIndex, MInt64 howMany)
		{
			for (auto it = _transitions.GetBegin(); it.IsValid(); ++it)
			{
				if ((*it).FlatIndex >= startIndex)
					(*it).FlatIndex += howMany;
			}

			for (auto it = _states.GetBegin(); it.IsValid(); ++it)
			{
				for (auto jt = (*it).TransitionsIn.GetBegin(); it.IsValid(); ++it)
				{
					if ((*jt) >= startIndex)
						(*jt) += howMany;
				}

				for (auto jt = (*it).TransitionsOut.GetBegin(); it.IsValid(); ++it)
				{
					if ((*jt) >= startIndex)
						(*jt) += howMany;
				}
			}
		}

		// //////////

		template<class T, class O>
		inline bool StateMachine<T, O>::Transition::EvaluateConditional()
		{
			const utility::MArray<const morphEngine::core::Event<bool, Transition*>::BaseDelegate *>& fc = ConditionalFuncSet.GetDelegates();
			
			if (ConditionMode == TransitionConditionMode::OR)
			{
				for (MSize i = 0; i < fc.GetSize(); ++i)
				{
					const morphEngine::core::Event<bool, Transition*>::BaseDelegate * del = fc[i];
					if (del->operator()(this))
						return true;
				}
				return false;
			}
			else /* AND */
			{
				for (MSize i = 0; i < fc.GetSize(); ++i)
				{
					const morphEngine::core::Event<bool, Transition*>::BaseDelegate * del = fc[i];
					if (!del->operator()(this))
						return false;
				}
				return true;
			}
		}

		template<class T, class O>
		inline bool StateMachine<T, O>::Transition::Update()
		{
			if (Progress >= 1.0f)
				Progress = 0.0f;

			if (Progress == 0.0f)
			{
				if (EvaluateConditional())
				{
					EvtBegin(this);
					Update_Internal();
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				Update_Internal();
				if (Progress < 1.0f)
				{
					return true;
				}
				else
				{
					EvtEnd(this);
					return false;
				}
			}
		}

		template<class T, class O>
		inline void StateMachine<T, O>::Transition::Update_Internal()
		{
			if (Type == TransitionType::LINEAR)
			{
				Progress = MMath::Clamp(Progress + core::Engine::GetInstance()->GetGlobalTime().GetDeltaTime() * Speed);
			}
			else
			{
				Progress = 0.0f;
			}
		}

#pragma endregion
	}
}



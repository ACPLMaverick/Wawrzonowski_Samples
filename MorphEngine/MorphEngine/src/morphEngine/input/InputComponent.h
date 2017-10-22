#pragma once
#include "EGamepadAxis.h"
#include "EKeyCode.h"
#include "gom/GameObjectComponent.h"
#include "utility/MDictionary.h"
#include "core/MessageSystem.h"
#include "utility/MMath.h"
#include "core/Config.h"

namespace morphEngine
{
	namespace input
	{
		//Component for use in game objects
		//Invokes event based on EPlayerIndex enum (i.e. only for PlayerOne)
		//Axis and actions callbacks can be bound to this component easily
		class InputComponent : public gom::GameObjectComponent
		{
			ME_TYPE;
		public:
			//Gamepad axis binding
			struct InputBindingGamepadAxis
			{
			public:
				EGamepadAxis Axis;
				MFloat32 Scale;

				inline InputBindingGamepadAxis(EGamepadAxis axis, MFloat32 scale) : Axis(axis), Scale(scale) { }
				inline InputBindingGamepadAxis(const InputBindingGamepadAxis& other) : Axis(other.Axis), Scale(other.Scale) { }

				inline bool operator==(const InputBindingGamepadAxis& other) const
				{
					return Axis == other.Axis && Scale == other.Scale;
				}

				inline bool operator!=(const InputBindingGamepadAxis& other) const
				{
					return Axis != other.Axis || Scale != other.Scale;
				}
			};

			//Binding axis-like behavior for gamepad buttons (generating axis events for pressing left dpad for example)
			struct InputBindingGamepadButtonAxis
			{
			public:
				EGamepadButton Button;
				MFloat32 Scale;

				inline InputBindingGamepadButtonAxis(EGamepadButton button, MFloat32 scale) : Button(button), Scale(scale) { }
				inline InputBindingGamepadButtonAxis(const InputBindingGamepadButtonAxis& other) : Button(other.Button), Scale(other.Scale) {}

				inline bool operator==(const InputBindingGamepadButtonAxis& other) const
				{
					return Button == other.Button && Scale == other.Scale;
				}

				inline bool operator!=(const InputBindingGamepadButtonAxis& other) const
				{
					return Button != other.Button || Scale != other.Scale;
				}
			};

			//Binding axis-like behavior for keys (generating axis events for pressing W key for example)
			struct InputBindingKeyAxis
			{
			public:
				EKeyCode Key;
				MFloat32 Scale;

				inline InputBindingKeyAxis(EKeyCode key, MFloat32 scale) : Key(key), Scale(scale) { }
				inline InputBindingKeyAxis(const InputBindingKeyAxis& other) : Key(other.Key), Scale(other.Scale) {}

				inline bool operator==(const InputBindingKeyAxis& other) const
				{
					return Key == other.Key && Scale == other.Scale;
				}

				inline bool operator!=(const InputBindingKeyAxis& other) const
				{
					return Key != other.Key || Scale != other.Scale;
				}
			};

			//Mouse axis binding
			struct InputBindingMouseAxis
			{
			public:
				EMouseAxis Axis;
				MFloat32 Scale;

				inline InputBindingMouseAxis(EMouseAxis axis, MFloat32 scale) : Axis(axis), Scale(scale) {}
				inline InputBindingMouseAxis(const InputBindingMouseAxis& other) : Axis(other.Axis), Scale(other.Scale) {}

				inline bool operator==(const InputBindingMouseAxis& other) const
				{
					return Axis == other.Axis && Scale == other.Scale;
				}

				inline bool operator!=(const InputBindingMouseAxis& other) const
				{
					return Axis != other.Axis || Scale != other.Scale;
				}
			};
			
			//Base structure for action bindings
			struct InputBindingAction
			{
			public:
				bool OnPressed;
				bool RequiresAlt;
				bool RequiresControl;
				bool RequiresShift;

				inline InputBindingAction(bool onPressed, bool requiresAlt = false, bool requiresControl = false, bool requiresShift = false) : OnPressed(onPressed), RequiresControl(requiresControl), RequiresAlt(requiresAlt), RequiresShift(requiresShift) { }
				inline InputBindingAction(const InputBindingAction& other) : OnPressed(other.OnPressed), RequiresControl(other.RequiresControl), RequiresAlt(other.RequiresAlt), RequiresShift(other.RequiresShift) { }

				inline virtual bool operator==(const InputBindingAction& other) const
				{
					return OnPressed == other.OnPressed &&
						RequiresControl == other.RequiresControl &&
						RequiresAlt == other.RequiresAlt &&
						RequiresShift == other.RequiresShift;
				}

				inline virtual bool operator!=(const InputBindingAction& other) const
				{
					return OnPressed != other.OnPressed ||
						RequiresControl != other.RequiresControl ||
						RequiresAlt != other.RequiresAlt ||
						RequiresShift != other.RequiresShift;
				}
			};

			//Key action binding (pressing S or releasing H for example)
			struct InputBindingKeyAction : public InputBindingAction
			{
			public:
				EKeyCode Key;

				inline InputBindingKeyAction(EKeyCode key, bool onPressed, bool requiresAlt = false, bool requiresControl = false, bool requiresShift = false) : Key(key), InputBindingAction(onPressed, requiresAlt, requiresControl, requiresShift) { }
				inline InputBindingKeyAction(const InputBindingKeyAction& other) : Key(other.Key), InputBindingAction(other) { }

				inline virtual bool operator==(const InputBindingKeyAction& other) const
				{
					return InputBindingAction::operator==(other) && Key == other.Key;
				}

				inline virtual bool operator!=(const InputBindingKeyAction& other) const
				{
					return InputBindingAction::operator!=(other) || Key != other.Key;
				}
			};

			//Action binding for gamepad buttons
			struct InputBindingGamepadAction : public InputBindingAction
			{
			public:
				EGamepadButton Button;

				inline InputBindingGamepadAction(EGamepadButton button, bool onPressed) : Button(button), InputBindingAction(onPressed, false, false, false) { }
				inline InputBindingGamepadAction(const InputBindingGamepadAction& other) : Button(other.Button), InputBindingAction(other) { }

				inline virtual bool operator==(const InputBindingGamepadAction& other) const
				{
					return InputBindingAction::operator==(other) && Button == other.Button;
				}

				inline virtual bool operator!=(const InputBindingGamepadAction& other) const
				{
					return InputBindingAction::operator!=(other) || Button != other.Button;
				}
			};

			//Action binding for mouse buttons
			struct InputBindingMouseAction : public InputBindingAction
			{
			public:
				EMouseButton Button;

				inline InputBindingMouseAction(EMouseButton button, bool onPressed) : Button(button), InputBindingAction(onPressed, false, false, false) {}
				inline InputBindingMouseAction(const InputBindingMouseAction& other) : Button(other.Button), InputBindingAction(other) {}

				inline virtual bool operator==(const InputBindingMouseAction& other) const
				{
					return InputBindingAction::operator==(other) && Button == other.Button;
				}

				inline virtual bool operator!=(const InputBindingMouseAction& other) const
				{
					return InputBindingAction::operator!=(other) || Button != other.Button;
				}

			};

			//Action structure
			//Used for managing ActionBindgins
			struct InputAction
			{
				friend class InputComponent;

			protected:
				utility::MArray<InputBindingGamepadAction> _gamepadActions;
				utility::MArray<InputBindingKeyAction> _keyActions;
				utility::MArray<InputBindingMouseAction> _mouseActions;
				core::Event<void> _callback;

			protected:
				//Checks if given keyCode with given alt/control/shift state should invoke this action
				inline void TryExecute(EKeyCode keyCode, bool pressed, bool isAlt, bool isControl, bool isShift)
				{
					const MSize size = _keyActions.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_keyActions[i].Key == keyCode &&
						   _keyActions[i].OnPressed == pressed &&
						   (!_keyActions[i].RequiresAlt || isAlt) &&
						   (!_keyActions[i].RequiresControl || isControl) &&
						   (!_keyActions[i].RequiresShift || isShift)
						   )
						{
							_callback();
							break;
						}
					}
				}

				//Checks if given keyCode with given alt/control/shift state should invoke this action
				inline void TryExecute(EKeyCode::Value keyCode, bool pressed, bool isAlt, bool isControl, bool isShift)
				{
					const MSize size = _keyActions.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_keyActions[i].Key == keyCode &&
						   _keyActions[i].OnPressed == pressed &&
						   (!_keyActions[i].RequiresAlt || isAlt) &&
						   (!_keyActions[i].RequiresControl || isControl) &&
						   (!_keyActions[i].RequiresShift || isShift)
						   )
						{
							_callback();
							break;
						}
					}
				}

				//Checks if given button with given state should invoke this action
				inline void TryExecute(EGamepadButton button, bool pressed)
				{
					const MSize size = _gamepadActions.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_gamepadActions[i].Button == button &&
						   _gamepadActions[i].OnPressed == pressed
						   )
						{
							_callback();
							break;
						}
					}
				}

				//Checks if given button with given state should invoke this action
				inline void TryExecute(EGamepadButton::Value button, bool pressed)
				{
					const MSize size = _gamepadActions.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_gamepadActions[i].Button == button &&
						   _gamepadActions[i].OnPressed == pressed
						   )
						{
							_callback();
							break;
						}
					}
				}

				//Check if given button with given state should invoke this action
				inline void TryExecute(EMouseButton button, bool pressed)
				{
					const MSize size = _mouseActions.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_mouseActions[i].Button == button &&
						   _mouseActions[i].OnPressed == pressed
						   )
						{
							_callback();
							break;
						}
					}
				}

				//Check if given button with given state should invoke this action
				inline void TryExecute(EMouseButton::Value button, bool pressed)
				{
					const MSize size = _mouseActions.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_mouseActions[i].Button == button &&
						   _mouseActions[i].OnPressed == pressed
						   )
						{
							_callback();
							break;
						}
					}
				}

				//Adds function to call when invoking action
				inline void AddCallback(const core::Event<void>::BaseDelegate* function)
				{
					_callback += function;
				}

			public:
				inline InputAction() { }
				inline InputAction(const InputAction& other) : _gamepadActions(other._gamepadActions), _keyActions(other._keyActions), _mouseActions(other._mouseActions), _callback(other._callback) { }

				//Binds key event to this action (if the key was pressed/released then this action will be invoked)
				inline void Bind(const InputBindingKeyAction& binding)
				{
					_keyActions.Add(binding);
				}

				//Binds gamepad button event to this action (if the button was pressed/released then this action will be invoked)
				inline void Bind(const InputBindingGamepadAction& binding)
				{
					_gamepadActions.Add(binding);
				}

				//Binds mouse button event to this action (it the button was pressed/released then this action will be invoked)
				inline void Bind(const InputBindingMouseAction& binding)
				{
					_mouseActions.Add(binding);
				}

				inline bool operator==(const InputAction& other) const
				{
					return _gamepadActions == other._gamepadActions && _keyActions == other._keyActions && _mouseActions == other._mouseActions;
				}

				inline bool operator!=(const InputAction& other) const
				{
					return _gamepadActions != other._gamepadActions || _keyActions != other._keyActions || _mouseActions != other._mouseActions;
				}
			};

			//Axis structure
			//Used for managing AxisBindings
			struct InputAxis
			{
				friend class InputComponent;

			protected:
				utility::MArray<InputBindingGamepadAxis> _gamepadAxes;
				utility::MArray<InputBindingGamepadButtonAxis> _gamepadButtonsAxes;
				utility::MArray<InputBindingKeyAxis> _keyAxes;
				utility::MArray<InputBindingMouseAxis> _mouseAxes;
				core::Event<void, MFloat32> _callback;
				MFloat32 _deadZone;

			protected:
				//Checks if given axis should invoke axis event with given value
				inline void TryExecute(EGamepadAxis axis, MFloat32 value)
				{
					MFloat32 val = value;
					if(utility::MMath::Abs(value) <= _deadZone)
					{
						val = 0.0f;
					}
					const MSize size = _gamepadAxes.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_gamepadAxes[i].Axis == axis)
						{
							_callback(_gamepadAxes[i].Scale * val);
						}
					}
				}

				//Checks if given axis should invoke axis event with given value
				inline void TryExecute(EGamepadAxis::Value axis, MFloat32 value)
				{
					MFloat32 val = value;
					if(utility::MMath::Abs(value) <= _deadZone)
					{
						val = 0.0f;
					}
					const MSize size = _gamepadAxes.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_gamepadAxes[i].Axis == axis)
						{
							_callback(_gamepadAxes[i].Scale * val);
						}
					}
				}

				//Checks if given button should invoke axis event
				inline void TryExecute(EGamepadButton button)
				{
					const MSize size = _gamepadButtonsAxes.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_gamepadButtonsAxes[i].Button == button)
						{
							_callback(_gamepadButtonsAxes[i].Scale);
						}
					}
				}

				//Checks if given button should invoke axis event
				inline void TryExecute(EGamepadButton::Value button)
				{
					const MSize size = _gamepadButtonsAxes.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_gamepadButtonsAxes[i].Button == button)
						{
							_callback(_gamepadButtonsAxes[i].Scale);
						}
					}
				}

				//Checks if given key should invoke axis event
				inline void TryExecute(EKeyCode key)
				{
					const MSize size = _keyAxes.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_keyAxes[i].Key == key)
						{
							_callback(_keyAxes[i].Scale);
						}
					}
				}

				//Checks if given key should invoke axis event
				inline void TryExecute(EKeyCode::Value key)
				{
					const MSize size = _keyAxes.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_keyAxes[i].Key == key)
						{
							_callback(_keyAxes[i].Scale);
						}
					}
				}

				//Checks if given mouse axis should invoke axis event with given value
				inline void TryExecute(EMouseAxis axis, MFloat32 value)
				{
					const MSize size = _mouseAxes.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_mouseAxes[i].Axis == axis)
						{
							_callback(_mouseAxes[i].Scale * value);
						}
					}
				}

				//Checks if given mouse axis should invoke axis event with given value
				inline void TryExecute(EMouseAxis::Value axis, MFloat32 value)
				{
					const MSize size = _mouseAxes.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(_mouseAxes[i].Axis == axis)
						{
							_callback(_mouseAxes[i].Scale * value);
						}
					}
				}

				//Adds function to call when invoking axis event
				inline void AddCallback(const core::Event<void, MFloat32>::BaseDelegate* function)
				{
					_callback += function;
				}

			public:
				inline InputAxis(MFloat32 deadZone = 0.05f) : _deadZone(deadZone) { }
				inline InputAxis(const InputAxis& other) : _deadZone(other._deadZone), _gamepadAxes(other._gamepadAxes), _gamepadButtonsAxes(other._gamepadButtonsAxes), _keyAxes(other._keyAxes), _mouseAxes(other._mouseAxes), _callback(other._callback) { }

				//Binds given gamepad axis
				inline void Bind(const InputBindingGamepadAxis& binding)
				{
					_gamepadAxes.Add(binding);
				}

				//Binds given gamepad button
				inline void Bind(const InputBindingGamepadButtonAxis& binding)
				{
					_gamepadButtonsAxes.Add(binding);
				}

				//Binds given key
				inline void Bind(const InputBindingKeyAxis& binding)
				{
					_keyAxes.Add(binding);
				}

				//Binds given mouse axis
				inline void Bind(const InputBindingMouseAxis& binding)
				{
					_mouseAxes.Add(binding);
				}


				inline bool operator==(const InputAxis& other) const
				{
					return _gamepadAxes == other._gamepadAxes && _keyAxes == other._keyAxes && _gamepadButtonsAxes == other._gamepadButtonsAxes && _mouseAxes == other._mouseAxes;
				}

				inline bool operator!=(const InputAxis& other) const
				{
					return _gamepadAxes != other._gamepadAxes || _keyAxes != other._keyAxes || _gamepadButtonsAxes != other._gamepadButtonsAxes || _mouseAxes != other._mouseAxes;
				}
			};
			
		protected:
			utility::MDictionary<EKeyCode, core::Event<void>> _keysDownEvents;
			utility::MDictionary<EKeyCode, core::Event<void>> _keysUpEvents;
			utility::MDictionary<EKeyCode, bool> _keysStates;
			utility::MDictionary<EGamepadButton, bool> _gamepadButtonsStates;
			utility::MDictionary<EMouseButton, bool> _mouseButtonsStates;
			utility::MDictionary<MString, InputAction> _actions;
			utility::MDictionary<MString, InputAxis> _axes;
			EPlayerInputIndex _gamepadIndex;
			core::Config _inputConfig;

			const MString _actionKeyName = "Action";
			const MString _axisKeyName = "Axis";
			const MString _actionBindingKeyName = "ActionBinding";
			const MString _axisBindingKeyName = "AxisBinding";

		protected:
			//Fills initial keys state (all as "released")
			inline void FillKeysStates()
			{
				MInt32 keysCount = (MInt32)EKeyCode::GetValuesStatic().size();
				for(MInt32 i = 0; i < keysCount; ++i)
				{
					_keysStates.Add(EKeyCode(i), false);
				}
			}

			//Fills initial buttons state (all as "released")
			inline void FillGamepadButtonsStates()
			{
				MInt32 gamepadButtonsCount = (MInt32)EGamepadButton::GetValuesStatic().size();
				for(MInt32 i = 0; i < gamepadButtonsCount; ++i)
				{
					_gamepadButtonsStates.Add((EGamepadButton)i, false);
				}
			}

			//Fills initial mouse buttons states (all as "released")
			inline void FillMouseButtonsStates()
			{
				MInt32 mouseButtonsCount = (MInt32)EMouseButton::GetValuesStatic().size();
				for(MInt32 i = 0; i < mouseButtonsCount; ++i)
				{
					_mouseButtonsStates.Add(EMouseButton(i), false);
				}
			}

			//Called when some key is down
			//keyCode - code of the key that is detected to be in "down" state
			inline void OnKeyDown(EKeyCode keyCode)
			{
				if(!_keysStates[keyCode])
				{
					_keysStates[keyCode] = true;

					if(_keysDownEvents.Contains(keyCode))
					{
						_keysDownEvents[keyCode]();
					}

					MDictionary<MString, InputAction>::MIteratorDictionary& actionsIt = _actions.GetBegin();
					for(; actionsIt.IsValid(); ++actionsIt)
					{
						(*actionsIt).GetValue().TryExecute(keyCode, true, IsAlt(), IsControl(), IsShift());
					}
				}

				MDictionary<MString, InputAxis>::MIteratorDictionary& axesIt = _axes.GetBegin();
				for(; axesIt.IsValid(); ++axesIt)
				{
					(*axesIt).GetValue().TryExecute(keyCode);
				}
			}

			//Called when some key is up
			//keyCode - code of the key that is detected to be in "up" state
			inline void OnKeyUp(EKeyCode keyCode)
			{
				if(_keysStates[keyCode])
				{
					_keysStates[keyCode] = false;

					if(_keysUpEvents.Contains(keyCode))
					{
						_keysUpEvents[keyCode]();
					}

					MDictionary<MString, InputAction>::MIteratorDictionary& iterator = _actions.GetBegin();
					for(iterator; iterator.IsValid(); ++iterator)
					{
						(*iterator).GetValue().TryExecute(keyCode, false, IsAlt(), IsControl(), IsShift());
					}
				}
			}

			//Called when some button is down
			//button - code of the button that is detected to be in "down" state
			inline void OnGamepadButtonDown(EPlayerInputIndex player, EGamepadButton button)
			{
				if(_gamepadIndex != EPlayerInputIndex::PLAYER_ANY && player != _gamepadIndex)
				{
					return;
				}

				if(!_gamepadButtonsStates[button])
				{
					_gamepadButtonsStates[button] = true;

					MDictionary<MString, InputAction>::MIteratorDictionary& iterator = _actions.GetBegin();
					for(iterator; iterator.IsValid(); ++iterator)
					{
						(*iterator).GetValue().TryExecute(button, true);
					}
				}

				MDictionary<MString, InputAxis>::MIteratorDictionary& axesIt = _axes.GetBegin();
				for(; axesIt.IsValid(); ++axesIt)
				{
					(*axesIt).GetValue().TryExecute(button);
				}
			}

			//Called when some button is up
			//button - code of the button that is detected to be in "up" state
			inline void OnGamepadButtonUp(EPlayerInputIndex player, EGamepadButton button)
			{
				if(_gamepadIndex != EPlayerInputIndex::PLAYER_ANY && player != _gamepadIndex)
				{
					return;
				}

				if(_gamepadButtonsStates[button])
				{
					_gamepadButtonsStates[button] = false;

					MDictionary<MString, InputAction>::MIteratorDictionary& iterator = _actions.GetBegin();
					for(iterator; iterator.IsValid(); ++iterator)
					{
						(*iterator).GetValue().TryExecute(button, false);
					}
				}
			}

			//Called when some button is down
			//button - code of the button that is detected to be in "down" state
			inline void OnMouseButtonDown(EMouseButton button)
			{
				if(!_mouseButtonsStates[button])
				{
					_mouseButtonsStates[button] = true;
					MDictionary<MString, InputAction>::MIteratorDictionary& iterator = _actions.GetBegin();
					for(iterator; iterator.IsValid(); ++iterator)
					{
						(*iterator).GetValue().TryExecute(button, true);
					}
				}
			}

			//Called when some button is up
			//button - code of the button that is detected to be in "up" state
			inline void OnMouseButtonUp(EMouseButton button)
			{
				if(_mouseButtonsStates[button])
				{
					_mouseButtonsStates[button] = false;
					MDictionary<MString, InputAction>::MIteratorDictionary& iterator = _actions.GetBegin();
					for(iterator; iterator.IsValid(); ++iterator)
					{
						(*iterator).GetValue().TryExecute(button, false);
					}
				}
			}

			//Called almost every frame for given player and axis
			//NOTE: this will generate axis event even if the value is 0 (it IS GOING to be useful, trust me)
			inline void OnGamepadAxis(EPlayerInputIndex player, EGamepadAxis axis, MFloat32 value)
			{
				if(_gamepadIndex != EPlayerInputIndex::PLAYER_ANY && player != _gamepadIndex)
				{
					return;
				}

				MDictionary<MString, InputAxis>::MIteratorDictionary& axesIt = _axes.GetBegin();
				for(; axesIt.IsValid(); ++axesIt)
				{
					(*axesIt).GetValue().TryExecute(axis, value);
				}
			}

			//Called on mouse move
			//Tries to invoke axis events for horizontal and vertical mouse axis
			inline void OnMouseMove(MInt32 horizontal, MInt32 vertical)
			{
				MDictionary<MString, InputAxis>::MIteratorDictionary& axesIt = _axes.GetBegin();
				for(; axesIt.IsValid(); ++axesIt)
				{
					(*axesIt).GetValue().TryExecute(EMouseAxis::HORIZONTAL, (MFloat32)horizontal);
					(*axesIt).GetValue().TryExecute(EMouseAxis::VERTICAL, (MFloat32)vertical);
				}
			}

			//Indicates if alt key (either left or right) is pressed
			inline bool IsAlt() const
			{
				return IsKeyPressed(EKeyCode::LEFT_ALT) || IsKeyPressed(EKeyCode::RIGHT_ALT);
			}

			//Indicates if control key (either left or right) is pressed
			inline bool IsControl() const
			{
				return IsKeyPressed(EKeyCode::LEFT_CONTROL) || IsKeyPressed(EKeyCode::RIGHT_CONTROL);
			}

			//Indicates if shift key (either left or right) is pressed
			inline bool IsShift() const
			{
				return IsKeyPressed(EKeyCode::LEFT_SHIFT) || IsKeyPressed(EKeyCode::RIGHT_SHIFT);
			}

			void ParseActionBinding(const MArray<MString>& values);
			void ParseAxisBinding(const MArray<MString>& values);
			void ParseAction(const MArray<MString>& values);
			void ParseAxis(const MArray<MString>& values);
			void FillFromConfig();
			void ConstructInputComponent(bool bIsCopy = false);
			void DestructInputComponent();
			
		public:
			//Copy contructor
			inline InputComponent(const InputComponent& other, bool bDeepCopy = true) : gom::GameObjectComponent(other, bDeepCopy), _gamepadIndex(other._gamepadIndex)
			{
				ConstructInputComponent(true);
			}
			//Constructs InputComponent and binds its events with MessageSystem events
			inline InputComponent(const gom::ObjectInitializer& initializer) : gom::GameObjectComponent(initializer)
			{
				ConstructInputComponent(false);
			}
			
			inline virtual ~InputComponent()
			{
				DestructInputComponent();
			}
			
			//Binds given function to key down for given keyCode event
			inline void BindKeyDown(EKeyCode keyCode, const core::Event<void>::BaseDelegate* function)
			{
				if(!_keysDownEvents.Contains(keyCode))
				{
					_keysDownEvents.Add(keyCode, core::Event<void>());
				}
				_keysDownEvents[keyCode] += function;
			}
			
			//Binds given function to key up for given keyCode event
			inline void BindKeyUp(EKeyCode keyCode, const core::Event<void>::BaseDelegate* function)
			{
				if(!_keysUpEvents.Contains(keyCode))
				{
					_keysUpEvents.Add(keyCode, core::Event<void>());
				}
				_keysUpEvents[keyCode] += function;
			}

			//Binds given function to action with given name
			inline void BindAction(const MString& name, const core::Event<void>::BaseDelegate* function)
			{
				if(!_actions.Contains(name))
				{
					delete function;
					ME_WARNING_RETURN_STATEMENT(false, "Trying to add callback to non-existing action! Aborting!");
				}
				_actions[name].AddCallback(function);
			}

			//Binds given function to axis with given name
			inline void BindAxis(const MString& name, const core::Event<void, MFloat32>::BaseDelegate* function)
			{
				if(!_axes.Contains(name))
				{
					delete function;
					ME_WARNING_RETURN_STATEMENT(false, "Trying to add callback to non-existing axis! Aborting!");
				}
				_axes[name].AddCallback(function);
			}

			//Registers new action
			inline void RegisterAction(const MString& name, const InputAction& action)
			{
				if(_actions.Contains(name))
				{
					ME_WARNING_RETURN_STATEMENT(false, "Trying to register action that name was already registered");
				}
				_actions.Add(name, action);
			}

			//Register axis
			inline void RegisterAxis(const MString& name, const InputAxis& axis)
			{
				if(_axes.Contains(name))
				{
					ME_WARNING_RETURN_STATEMENT(false, "Trying to register axis that name was already registered");
				}
				_axes.Add(name, axis);
			}

			//Indicates if key with given keyCode is pressed
			inline bool IsKeyPressed(EKeyCode keyCode) const
			{
				return _keysStates[keyCode];
			}

			//Indicates if given button is pressed (NOTE: this function does not care about which player pressed the button)
			inline bool IsButtonPressed(EGamepadButton button) const
			{
				return _gamepadButtonsStates[button];
			}

			inline bool IsMouseButtonPressed(EMouseButton button) const
			{
				return _mouseButtonsStates[button];
			}

			inline EPlayerInputIndex GetPlayerIndex() const
			{
				return _gamepadIndex;
			}

			inline void SetPlayerIndex(EPlayerInputIndex playerIndex)
			{
				_gamepadIndex = playerIndex;
			}

			void SaveConfig();
		};

	}
}
#include "InputComponent.h"

#define INPUT_CONFIG_PATH "Configs/input.conf"

using namespace morphEngine::core;

namespace morphEngine
{
	namespace input
	{

		void InputComponent::ParseActionBinding(const MArray<MString>& values)
		{
			//values in [] are optional
			//actionBindingKeyName = actionName code onPressed [altRequired controlRequired shiftRequired]
			//code can be either EKeyCode, EMouseButton or EGamepadButton
			if(values.GetSize() < 2)
			{
				return;
			}
			MString& actionName = values[0];
			if(!_actions.Contains(actionName))
			{
				return;
			}
			InputAction& action = _actions[actionName];
			MString& code = values[1];
			MString& pressed = values[2];
			bool onPressed = MString::ToBoolean(pressed);
			EKeyCode keyCode;
			if(keyCode.FromString(*code))
			{
				if(values.GetSize() < 6)
				{
					return;
				}
				action.Bind(InputBindingKeyAction(keyCode, onPressed, MString::ToBoolean(values[3]), MString::ToBoolean(values[4]), MString::ToBoolean(values[5])));
				return;
			}
			EGamepadButton gamepadButton;
			if(gamepadButton.FromString(*code))
			{
				action.Bind(InputBindingGamepadAction(gamepadButton, onPressed));
				return;
			}
			EMouseButton mouseButton(*code);
			action.Bind(InputBindingMouseAction(mouseButton, onPressed));
		}

		void InputComponent::ParseAxisBinding(const MArray<MString>& values)
		{
			//axisBindingKeyName = axisName code scale
			//code can be either EKeyCode, EMouseAxis, EGamepadAxis or EGamepadButton
			if(values.GetSize() < 3)
			{
				return;
			}

			MString& axisName = values[0];
			const char* a = axisName;
			if(!_axes.Contains(axisName))
			{
				return;
			}
			InputAxis& axis = _axes[axisName];
			MString& code = values[1];
			const char* c = code;
			const char* v = values[2];
			MFloat32 scale = MString::ToFloat32(values[2]);
			EKeyCode keyCode;
			const std::vector<std::string> names = keyCode.GetNames();
			if(keyCode.FromString(*code))
			{
				axis.Bind(InputBindingKeyAxis(keyCode, scale));
				return;
			}
			EGamepadAxis gamepadAxis;
			if(gamepadAxis.FromString(*code))
			{
				axis.Bind(InputBindingGamepadAxis(gamepadAxis, scale));
				return;
			}
			EMouseAxis mouseAxis;
			if(mouseAxis.FromString(*code))
			{
				axis.Bind(InputBindingMouseAxis(mouseAxis, scale));
				return;
			}
			EGamepadButton button;
			if(button.FromString(*code))
			{
				axis.Bind(InputBindingGamepadButtonAxis(button, scale));
			}
		}

		void InputComponent::ParseAction(const MArray<MString>& values)
		{
			//actionKeyName = actionName
			if(values.GetSize() < 1)
			{
				return;
			}
			//The first value = Action name
			//Ignore the rest of values
			RegisterAction(values[0], InputAction());
		}

		void InputComponent::ParseAxis(const MArray<MString>& values)
		{
			//axisKeyName = axisName
			if(values.GetSize() < 1)
			{
				return;
			}
			//The first value = Axis name
			//Ignore the rest of values
			RegisterAxis(values[0], InputAxis());
		}

		void InputComponent::FillFromConfig()
		{
			_inputConfig.Read(INPUT_CONFIG_PATH);
			Config::ConfigType configs = _inputConfig.GetAll();

			Config::ConfigType::MIteratorDictionary it = configs.GetBegin();
			for(; it.IsValid(); ++it)
			{
				MString& keyName = (*it).GetKey();
				Config::ValueType& values = (*it).GetValue();

				Config::ValueType::MIteratorArray arrayIt = values.GetBegin();

				for(; arrayIt.IsValid(); ++arrayIt)
				{
					MArray<char> chars;
					chars.Add(' ');
					MArray<MString> splitted;
					MString::SplitNCS(*arrayIt, chars, splitted);
					if(keyName == _actionKeyName)
					{
						ParseAction(splitted);
					}
					else if(keyName == _axisKeyName)
					{
						ParseAxis(splitted);
					}
					else if(keyName == _actionBindingKeyName)
					{
						ParseActionBinding(splitted);
					}
					else if(keyName == _axisBindingKeyName)
					{
						ParseAxisBinding(splitted);
					}
				}
			}
		}

		void InputComponent::ConstructInputComponent(bool bIsCopy)
		{
			if(bIsCopy)
			{
				_keysStates.Shutdown();
				_gamepadButtonsStates.Shutdown();
				_mouseButtonsStates.Shutdown();
				_keysDownEvents.Shutdown();
				_keysUpEvents.Shutdown();
				_actions.Shutdown();
				_axes.Shutdown();
			}
			
			FillFromConfig();

			core::MessageSystem::OnKeyDown += new core::Event<void, EKeyCode>::ClassDelegate<InputComponent>(_this, &InputComponent::OnKeyDown);
			core::MessageSystem::OnKeyUp += new core::Event<void, EKeyCode>::ClassDelegate<InputComponent>(_this, &InputComponent::OnKeyUp);
			core::MessageSystem::OnGamepadAxis += new core::Event<void, EPlayerInputIndex, EGamepadAxis, MFloat32>::ClassDelegate<InputComponent>(_this, &InputComponent::OnGamepadAxis);
			core::MessageSystem::OnGamepadButtonDown += new core::Event<void, EPlayerInputIndex, EGamepadButton>::ClassDelegate<InputComponent>(_this, &InputComponent::OnGamepadButtonDown);
			core::MessageSystem::OnGamepadButtonUp += new core::Event<void, EPlayerInputIndex, EGamepadButton>::ClassDelegate<InputComponent>(_this, &InputComponent::OnGamepadButtonUp);
			core::MessageSystem::OnMouseButtonDown += new core::Event<void, EMouseButton>::ClassDelegate<InputComponent>(_this, &InputComponent::OnMouseButtonDown);
			core::MessageSystem::OnMouseButtonUp += new core::Event<void, EMouseButton>::ClassDelegate<InputComponent>(_this, &InputComponent::OnMouseButtonUp);
			core::MessageSystem::OnMouseMove += new core::Event<void, MInt32, MInt32>::ClassDelegate<InputComponent>(_this, &InputComponent::OnMouseMove);

			FillKeysStates();
			FillGamepadButtonsStates();
			FillMouseButtonsStates();

			if(!bIsCopy)
			{
				_gamepadIndex = EPlayerInputIndex::PLAYER_ANY;
			}

			RegisterProperty("Player index", &_gamepadIndex);

			FillKeysStates();
			FillGamepadButtonsStates();
			FillMouseButtonsStates();
		}

		void InputComponent::DestructInputComponent()
		{
			core::MessageSystem::OnKeyDown -= new core::Event<void, EKeyCode>::ClassDelegate<InputComponent>(_this, &InputComponent::OnKeyDown);
			core::MessageSystem::OnKeyUp -= new core::Event<void, EKeyCode>::ClassDelegate<InputComponent>(_this, &InputComponent::OnKeyUp);
			core::MessageSystem::OnGamepadAxis -= new core::Event<void, EPlayerInputIndex, EGamepadAxis, MFloat32>::ClassDelegate<InputComponent>(_this, &InputComponent::OnGamepadAxis);
			core::MessageSystem::OnGamepadButtonDown -= new core::Event<void, EPlayerInputIndex, EGamepadButton>::ClassDelegate<InputComponent>(_this, &InputComponent::OnGamepadButtonDown);
			core::MessageSystem::OnGamepadButtonUp -= new core::Event<void, EPlayerInputIndex, EGamepadButton>::ClassDelegate<InputComponent>(_this, &InputComponent::OnGamepadButtonUp);
			core::MessageSystem::OnMouseButtonDown -= new core::Event<void, EMouseButton>::ClassDelegate<InputComponent>(_this, &InputComponent::OnMouseButtonDown);
			core::MessageSystem::OnMouseButtonUp -= new core::Event<void, EMouseButton>::ClassDelegate<InputComponent>(_this, &InputComponent::OnMouseButtonUp);
			core::MessageSystem::OnMouseMove -= new core::Event<void, MInt32, MInt32>::ClassDelegate<InputComponent>(_this, &InputComponent::OnMouseMove);
		}

		void InputComponent::SaveConfig()
		{
			_inputConfig.Clear();

			MDictionary<MString, InputAction>::MIteratorDictionary actionsIt = _actions.GetBegin();
			for(; actionsIt.IsValid(); ++actionsIt)
			{
				_inputConfig.SetValue(_actionKeyName, (*actionsIt).GetKey());
			}
			MDictionary<MString, InputAxis>::MIteratorDictionary axesIt = _axes.GetBegin();
			for(; axesIt.IsValid(); ++axesIt)
			{
				_inputConfig.SetValue(_axisKeyName, (*axesIt).GetKey());
			}
			MDictionary<MString, InputAction>::MIteratorDictionary actionBindingsIt = _actions.GetBegin();
			for(; actionBindingsIt.IsValid(); ++actionBindingsIt)
			{
				MString& key = (*actionBindingsIt).GetKey();
				InputAction& action = (*actionBindingsIt).GetValue();
				MArray<InputBindingKeyAction>::MIteratorArray keyActionsIt = action._keyActions.GetBegin();
				for(; keyActionsIt.IsValid(); ++keyActionsIt)
				{
					MString value = key + " " + (*keyActionsIt).Key.ToString().c_str() + " " + MString::FromBoolean((*keyActionsIt).OnPressed) + " " + MString::FromBoolean((*keyActionsIt).RequiresAlt) + " " + MString::FromBoolean((*keyActionsIt).RequiresControl) + " " + MString::FromBoolean((*keyActionsIt).RequiresShift);
					_inputConfig.SetValue(_actionBindingKeyName, value);
				}
				MArray<InputBindingGamepadAction>::MIteratorArray gamepadActionsIt = action._gamepadActions.GetBegin();
				for(; gamepadActionsIt.IsValid(); ++gamepadActionsIt)
				{
					MString value = key + " " + (*gamepadActionsIt).Button.ToString().c_str() + " " + MString::FromBoolean((*gamepadActionsIt).OnPressed);
					_inputConfig.SetValue(_actionBindingKeyName, value);
				}
				MArray<InputBindingMouseAction>::MIteratorArray mouseActionsIt = action._mouseActions.GetBegin();
				for(; mouseActionsIt.IsValid(); ++mouseActionsIt)
				{
					MString value = key + " " + ((*mouseActionsIt).Button).ToString().c_str() + " " + MString::FromBoolean((*mouseActionsIt).OnPressed);
					_inputConfig.SetValue(_actionBindingKeyName, value);
				}
			}
			MDictionary<MString, InputAxis>::MIteratorDictionary axisBindingsIt = _axes.GetBegin();
			for(; axisBindingsIt.IsValid(); ++axisBindingsIt)
			{
				MString& key = (*axisBindingsIt).GetKey();
				InputAxis& axis = (*axisBindingsIt).GetValue();
				MArray<InputBindingGamepadAxis>::MIteratorArray gamepadAxisIt = axis._gamepadAxes.GetBegin();
				for(; gamepadAxisIt.IsValid(); ++gamepadAxisIt)
				{
					MString value = key + " " +  (*gamepadAxisIt).Axis.ToString().c_str() + " " + MString::FromFloat((*gamepadAxisIt).Scale);
					_inputConfig.SetValue(_axisBindingKeyName, value);
				}
				MArray<InputBindingGamepadButtonAxis>::MIteratorArray gamepadButtonAxisIt = axis._gamepadButtonsAxes.GetBegin();
				for(; gamepadButtonAxisIt.IsValid(); ++gamepadButtonAxisIt)
				{
					MString value = key + " " + (*gamepadButtonAxisIt).Button.ToString().c_str() + " " + MString::FromFloat((*gamepadButtonAxisIt).Scale);
					_inputConfig.SetValue(_axisBindingKeyName, value);
				}
				MArray<InputBindingKeyAxis>::MIteratorArray keyAxesIt = axis._keyAxes.GetBegin();
				for(; keyAxesIt.IsValid(); ++keyAxesIt)
				{
					MString value = key + " " + (*keyAxesIt).Key.ToString().c_str() + " " + MString::FromFloat((*keyAxesIt).Scale);
					_inputConfig.SetValue(_axisBindingKeyName, value);
				}
				MArray<InputBindingMouseAxis>::MIteratorArray mouseAxesIt = axis._mouseAxes.GetBegin();
				for(; mouseAxesIt.IsValid(); ++mouseAxesIt)
				{
					MString value = key + " " + ((*mouseAxesIt).Axis).ToString().c_str() + " " + MString::FromFloat((*mouseAxesIt).Scale);
					_inputConfig.SetValue(_axisBindingKeyName, value);
				}
			}

			_inputConfig.Write();
		}
	}
}
#include "WindowsInputManager.h"

namespace morphEngine
{
	namespace input
	{
		void WindowsInputManager::UpdatePlayer(input::EPlayerInputIndex player, XINPUT_STATE currentState, bool connected, bool prevConnected)
		{
			static const MFloat32 Inv255 = 1.0f / 255.0f;
			static const MFloat32 Inv32767 = 1.0f / 32767.0f;

			if(connected)
			{
				if(!prevConnected)
				{
					core::MessageSystem::OnPlayerConnected(player, true);
				}

				core::MessageSystem::OnGamepadAxis(player, EGamepadAxis::LEFT_TRIGGER, (MFloat32)currentState.Gamepad.bLeftTrigger * Inv255);
				core::MessageSystem::OnGamepadAxis(player, EGamepadAxis::RIGHT_TRIGGER, (MFloat32)currentState.Gamepad.bRightTrigger * Inv255);
				core::MessageSystem::OnGamepadAxis(player, EGamepadAxis::LEFT_THUMBSTICK_X, utility::MMath::Max(-1.0f, (MFloat32)currentState.Gamepad.sThumbLX * Inv32767));
				core::MessageSystem::OnGamepadAxis(player, EGamepadAxis::LEFT_THUMBSTICK_Y, utility::MMath::Max(-1.0f, (MFloat32)currentState.Gamepad.sThumbLY * Inv32767));
				core::MessageSystem::OnGamepadAxis(player, EGamepadAxis::RIGHT_THUMBSTICK_X, utility::MMath::Max(-1.0f, (MFloat32)currentState.Gamepad.sThumbRX * Inv32767));
				core::MessageSystem::OnGamepadAxis(player, EGamepadAxis::RIGHT_THUMBSTICK_Y, utility::MMath::Max(-1.0f, (MFloat32)currentState.Gamepad.sThumbRY * Inv32767));

				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_A, EGamepadButton::FACE_BUTTON_DOWN, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_B, EGamepadButton::FACE_BUTTON_RIGHT, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_X, EGamepadButton::FACE_BUTTON_LEFT, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_Y, EGamepadButton::FACE_BUTTON_UP, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, EGamepadButton::DPAD_DOWN, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP, EGamepadButton::DPAD_UP, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, EGamepadButton::DPAD_LEFT, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, EGamepadButton::DPAD_RIGHT, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, EGamepadButton::LEFT_SHOULDER, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, EGamepadButton::RIGHT_SHOULDER, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB, EGamepadButton::LEFT_THUMB, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB, EGamepadButton::RIGHT_THUMB, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_BACK, EGamepadButton::BACK, player);
				CheckButton(currentState.Gamepad.wButtons, XINPUT_GAMEPAD_START, EGamepadButton::START, player);


			}
			else
			{
				if(prevConnected)
				{
					core::MessageSystem::OnPlayerConnected(player, false);
				}
			}
		}
	}
}

#pragma once

#include "core/GlobalDefines.h"

#ifdef PLATFORM_WINDOWS
#include "core/Singleton.h"
#include "utility/MMath.h"
#include "core/MessageSystem.h"

#include <Xinput.h>

typedef DWORD(WINAPI *XInputGetStateProcedure)(int, XINPUT_STATE*);

namespace morphEngine
{
	namespace input
	{
		//Class for managing XInput and other Windows dependent input things
		class WindowsInputManager : public core::Singleton<WindowsInputManager>
		{
			friend class core::Singleton<WindowsInputManager>;

		protected:
			XInputGetStateProcedure _xinputGetState;
			XINPUT_STATE _states[4];
			bool _prevConnected[4];
			bool _connected[4];

		protected:
			inline WindowsInputManager()
			{}
			inline virtual ~WindowsInputManager()
			{}

			//Checks if button went down or up and invokes proper event
			inline void CheckButton(MUint16 buttonsState, MUint16 xinputButton, EGamepadButton button, EPlayerInputIndex player)
			{
				if((buttonsState & xinputButton) != 0)
				{
					core::MessageSystem::OnGamepadButtonDown(player, button);
				}
				else
				{
					core::MessageSystem::OnGamepadButtonUp(player, button);
				}
			}

			//Invokes proper events (if needed) for buttons and axis of given player and XINPUT_STATE structure
			void UpdatePlayer(input::EPlayerInputIndex player, XINPUT_STATE currentState, bool connected, bool prevConnected);

		public:
			//Initializes WindowsInputManager (loading xinput library etc.)
			inline void Initialize()
			{
				HMODULE xinputModule = LoadLibraryA("xinput1_3.dll");
				ME_ASSERT(xinputModule != NULL, "xinput1_3.dll is missing on your computer!");
				_xinputGetState = (XInputGetStateProcedure)GetProcAddress(xinputModule, "XInputGetState");
				ME_ASSERT(_xinputGetState != NULL, "Can't locate XInputGetState procedure");

				ZeroMemory(&_states[0], sizeof(XINPUT_STATE));
				ZeroMemory(&_states[1], sizeof(XINPUT_STATE));
				ZeroMemory(&_states[2], sizeof(XINPUT_STATE));
				ZeroMemory(&_states[3], sizeof(XINPUT_STATE));

				_prevConnected[0] = _connected[0] = _xinputGetState(0, &_states[0]) == ERROR_SUCCESS;
				_prevConnected[1] = _connected[1] = _xinputGetState(1, &_states[1]) == ERROR_SUCCESS;
				_prevConnected[2] = _connected[2] = _xinputGetState(2, &_states[2]) == ERROR_SUCCESS;
				_prevConnected[3] = _connected[3] = _xinputGetState(3, &_states[3]) == ERROR_SUCCESS;
			}

			//Cleans WindowsInputManager
			inline void Shutdown()
			{

			}

			//Updates input from XInput controllers
			inline void Update()
			{
				ZeroMemory(&_states[0], sizeof(XINPUT_STATE));
				ZeroMemory(&_states[1], sizeof(XINPUT_STATE));
				ZeroMemory(&_states[2], sizeof(XINPUT_STATE));
				ZeroMemory(&_states[3], sizeof(XINPUT_STATE));

				_connected[0] = _xinputGetState(0, &_states[0]) == ERROR_SUCCESS;
				_connected[1] = _xinputGetState(1, &_states[1]) == ERROR_SUCCESS;
				_connected[2] = _xinputGetState(2, &_states[2]) == ERROR_SUCCESS;
				_connected[3] = _xinputGetState(3, &_states[3]) == ERROR_SUCCESS;

				UpdatePlayer(input::EPlayerInputIndex::PLAYER_1, _states[0], _connected[0], _prevConnected[0]);
				UpdatePlayer(input::EPlayerInputIndex::PLAYER_2, _states[1], _connected[1], _prevConnected[1]);
				UpdatePlayer(input::EPlayerInputIndex::PLAYER_3, _states[2], _connected[2], _prevConnected[2]);
				UpdatePlayer(input::EPlayerInputIndex::PLAYER_4, _states[3], _connected[3], _prevConnected[3]);

				_prevConnected[0] = _connected[0];
				_prevConnected[1] = _connected[1];
				_prevConnected[2] = _connected[2];
				_prevConnected[3] = _connected[3];
			}
		};
	}
}

#endif
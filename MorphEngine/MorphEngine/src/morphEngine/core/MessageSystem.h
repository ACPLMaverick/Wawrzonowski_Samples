#pragma once

#include "Delegate.h"
#include "input/EKeyCode.h"
#include "input/EGamepadAxis.h"
#include "input/EPlayerInputIndex.h"
#include "input/EMouseButton.h"

namespace morphEngine
{
	namespace core
	{
		class MessageSystem
		{
		public:
			static Event<void, input::EKeyCode> OnKeyUp;
			static Event<void, input::EKeyCode> OnKeyDown;
			static Event<void, input::EMouseButton> OnMouseButtonUp;
			static Event<void, input::EMouseButton> OnMouseButtonDown;
			static Event<void, input::EPlayerInputIndex, input::EGamepadAxis, MFloat32> OnGamepadAxis;
			static Event<void, input::EPlayerInputIndex, input::EGamepadButton> OnGamepadButtonUp;
			static Event<void, input::EPlayerInputIndex, input::EGamepadButton> OnGamepadButtonDown;
			//Event for player (dis)connected, bool indicates if player has connected or disconnected
			static Event<void, input::EPlayerInputIndex, bool> OnPlayerConnected;
			static Event<void, MInt32, MInt32> OnMouseMove;
			static Event<void, MInt32, MInt32> OnWindowMove;
			static Event<void, MInt32, MInt32> OnWindowResize;
			static Event<void> OnWindowCreate;
			static Event<void> OnWindowDestroy;
			static Event<void> OnWindowQuit;
			static Event<void> OnGatherMessages;

			inline static void GatherMessages()
			{
				OnGatherMessages();
			}

			inline static void Shutdown()
			{
				OnKeyUp.Shutdown();
				OnKeyDown.Shutdown();
				OnMouseButtonUp.Shutdown();
				OnMouseButtonDown.Shutdown();
				OnGamepadAxis.Shutdown();
				OnGamepadButtonUp.Shutdown();
				OnGamepadButtonDown.Shutdown();
				OnPlayerConnected.Shutdown();
				OnMouseMove.Shutdown();
				OnWindowMove.Shutdown();
				OnWindowResize.Shutdown();
				OnWindowCreate.Shutdown();
				OnWindowDestroy.Shutdown();
				OnWindowQuit.Shutdown();
				OnGatherMessages.Shutdown();
			}
		};
	}
}
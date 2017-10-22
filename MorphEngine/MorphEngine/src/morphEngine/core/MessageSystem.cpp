#include "MessageSystem.h"

namespace morphEngine
{
	namespace core
	{
		Event<void, input::EKeyCode> MessageSystem::OnKeyUp;
		Event<void, input::EKeyCode> MessageSystem::OnKeyDown;
		Event<void, input::EMouseButton> MessageSystem::OnMouseButtonUp;
		Event<void, input::EMouseButton> MessageSystem::OnMouseButtonDown;
		Event<void, input::EPlayerInputIndex, input::EGamepadAxis, MFloat32> MessageSystem::OnGamepadAxis;
		Event<void, input::EPlayerInputIndex, input::EGamepadButton> MessageSystem::OnGamepadButtonUp;
		Event<void, input::EPlayerInputIndex, input::EGamepadButton> MessageSystem::OnGamepadButtonDown;
		Event<void, input::EPlayerInputIndex, bool> MessageSystem::OnPlayerConnected;
		Event<void, MInt32, MInt32> MessageSystem::OnMouseMove;
		Event<void, MInt32, MInt32> MessageSystem::OnWindowMove;
		Event<void, MInt32, MInt32> MessageSystem::OnWindowResize;
		Event<void> MessageSystem::OnWindowCreate;
		Event<void> MessageSystem::OnWindowDestroy;
		Event<void> MessageSystem::OnWindowQuit;
		Event<void> MessageSystem::OnGatherMessages;
	}
}
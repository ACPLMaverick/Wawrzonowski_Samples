#pragma once

#include "utility/MString.h"
#include "utility\MEnum.h"

namespace morphEngine
{
	namespace input
	{
		DECLARE_MENUM(EGamepadAxis, LEFT_THUMBSTICK_X,
									LEFT_THUMBSTICK_Y,
									RIGHT_THUMBSTICK_X,
									RIGHT_THUMBSTICK_Y,
									LEFT_TRIGGER,
									RIGHT_TRIGGER,
									);

		DECLARE_MENUM(EGamepadButton,	FACE_BUTTON_DOWN,
										FACE_BUTTON_UP,
										FACE_BUTTON_LEFT,
										FACE_BUTTON_RIGHT,
										DPAD_DOWN,
										DPAD_UP,
										DPAD_LEFT,
										DPAD_RIGHT,
										LEFT_SHOULDER,
										RIGHT_SHOULDER,
										LEFT_THUMB,
										RIGHT_THUMB,
										BACK,
										START,
										);
	}
}
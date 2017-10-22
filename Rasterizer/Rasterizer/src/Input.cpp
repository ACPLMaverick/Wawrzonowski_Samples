#include "Input.h"
#include "System.h"


Input::Input()
{
}


Input::~Input()
{
}

void Input::Initialize()
{
	// mouse
	_deviceID[0].usUsagePage = 0x01;
	_deviceID[0].usUsage = 0x02;
	_deviceID[0].dwFlags = RIDEV_NOLEGACY;
	_deviceID[0].hwndTarget = System::GetInstance()->GetSystemSettings()->GetWindowPtr();

	// keyboard
	_deviceID[1].usUsagePage = 0x01;
	_deviceID[1].usUsage = 0x06;
	_deviceID[1].dwFlags = RIDEV_NOLEGACY;
	_deviceID[1].hwndTarget = System::GetInstance()->GetSystemSettings()->GetWindowPtr();

	bool b = RegisterRawInputDevices(_deviceID, 2, sizeof(_deviceID[0]));
}

void Input::Shutdown()
{
}

void Input::Update(HRAWINPUT hRawInput)
{
	UINT dwSize;
	GetRawInputData(hRawInput, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	uint8_t* lpb = new uint8_t[dwSize];
	GetRawInputData(hRawInput, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

	RAWINPUT* raw = (RAWINPUT*)lpb;

	if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		if (raw->data.mouse.usButtonFlags == RI_MOUSE_LEFT_BUTTON_DOWN)
		{
			_keysDown.emplace(VK_LBUTTON, VK_LBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_RIGHT_BUTTON_DOWN)
		{
			_keysDown.emplace(VK_RBUTTON, VK_RBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_MIDDLE_BUTTON_DOWN)
		{
			_keysDown.emplace(VK_MBUTTON, VK_MBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_LEFT_BUTTON_UP)
		{
			_keysDown.erase(VK_LBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_RIGHT_BUTTON_UP)
		{
			_keysDown.erase(VK_RBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_MIDDLE_BUTTON_UP)
		{
			_keysDown.erase(VK_MBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_WHEEL)
		{
			_mouseWheelAbsoluteLast = _mouseWheelAbsolute;
			_mouseWheelAbsolute += raw->data.mouse.usButtonData;
		}

		if (raw->data.mouse.usFlags == MOUSE_MOVE_ABSOLUTE)
		{
			_mousePosAbsoluteLast = _mousePosAbsolute;
			_mousePosAbsolute = math::Float2((float)raw->data.mouse.lLastX, (float)raw->data.mouse.lLastY);
		}
		else if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
		{
			_mousePosAbsoluteLast = _mousePosAbsolute;
			_mousePosAbsolute += math::Float2((float)raw->data.mouse.lLastX, (float)raw->data.mouse.lLastY);
		}
	}
	else if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		if (raw->data.keyboard.Flags == RI_KEY_MAKE)
		{
			_keysDown.emplace(raw->data.keyboard.VKey, raw->data.keyboard.VKey);
		}
		else if (raw->data.keyboard.Flags == RI_KEY_BREAK)
		{
			_keysDown.erase(raw->data.keyboard.VKey);
		}
	}

	delete lpb;
}

bool Input::GetKeyDown(int keyCode)
{
	return _keysDown.find(keyCode) != _keysDown.end();
}

int64_t Input::GetMouseWheelRelativePosition()
{
	return _mouseWheelAbsolute - _mouseWheelAbsoluteLast;
}

int64_t Input::GetMouseWheelPosition()
{
	return _mouseWheelAbsolute;
}

math::Float2 Input::GetMouseRelativePosition()
{
	return _mousePosAbsolute - _mousePosAbsoluteLast;
}

math::Float2 Input::GetMousePosition()
{
	return _mousePosAbsolute;
}

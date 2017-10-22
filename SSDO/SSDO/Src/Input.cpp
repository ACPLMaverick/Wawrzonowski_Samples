#include "stdafx.h"
#include "Input.h"
#include "System.h"


Input::Input() : 
	_mousePosAbsolute(0.0f, 0.0f),
	_mousePosAbsoluteLast(0.0f, 0.0f)
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
	_deviceID[0].dwFlags = NULL;
	_deviceID[0].hwndTarget = System::GetInstance()->GetHWND();

	// keyboard
	_deviceID[1].usUsagePage = 0x01;
	_deviceID[1].usUsage = 0x06;
	_deviceID[1].dwFlags = NULL;
	_deviceID[1].hwndTarget = System::GetInstance()->GetHWND();

	bool b = RegisterRawInputDevices(_deviceID, 2, sizeof(_deviceID[0]));
}

void Input::Shutdown()
{
}

void Input::Update()
{
	if (_bEverySecondFlag)
	{
		_mousePosAbsoluteLast = _mousePosAbsolute;
		_mouseWheelAbsoluteLast = _mouseWheelAbsolute;

		_bEverySecondFlag = false;
	}
	else
	{
		_bEverySecondFlag = true;
	}

	_keysDown.clear();
	_keysUp.clear();
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
			_keys.emplace(VK_LBUTTON, VK_LBUTTON);
			_keysDown.emplace(VK_LBUTTON, VK_LBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_RIGHT_BUTTON_DOWN)
		{
			_keys.emplace(VK_RBUTTON, VK_RBUTTON);
			_keysDown.emplace(VK_RBUTTON, VK_RBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_MIDDLE_BUTTON_DOWN)
		{
			_keys.emplace(VK_MBUTTON, VK_MBUTTON);
			_keysDown.emplace(VK_MBUTTON, VK_MBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_LEFT_BUTTON_UP)
		{
			_keys.erase(VK_LBUTTON);
			_keysUp.erase(VK_LBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_RIGHT_BUTTON_UP)
		{
			_keys.erase(VK_RBUTTON);
			_keysUp.erase(VK_RBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_MIDDLE_BUTTON_UP)
		{
			_keys.erase(VK_MBUTTON);
			_keysUp.erase(VK_MBUTTON);
		}
		else if (raw->data.mouse.usButtonFlags == RI_MOUSE_WHEEL)
		{
			_mouseWheelAbsoluteLast = _mouseWheelAbsolute;
			_mouseWheelAbsolute += raw->data.mouse.usButtonData;
		}

		if (raw->data.mouse.usFlags == MOUSE_MOVE_ABSOLUTE)
		{
			_mousePosAbsoluteLast = _mousePosAbsolute;
			_mousePosAbsolute = XMFLOAT2A((float)raw->data.mouse.lLastX, (float)raw->data.mouse.lLastY);
		}
		else if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
		{
			_mousePosAbsoluteLast = _mousePosAbsolute;
			_mousePosAbsolute.x += (float)raw->data.mouse.lLastX;
			_mousePosAbsolute.y += (float)raw->data.mouse.lLastY;
		}
	}
	else if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		if (raw->data.keyboard.Flags == RI_KEY_MAKE)
		{
			if (!GetKey(raw->data.keyboard.VKey))
			{
				_keys.emplace(raw->data.keyboard.VKey, raw->data.keyboard.VKey);
				_keysDown.emplace(raw->data.keyboard.VKey, raw->data.keyboard.VKey);
			}
		}
		else if (raw->data.keyboard.Flags == RI_KEY_BREAK)
		{
			if (GetKey(raw->data.keyboard.VKey))
			{
				_keys.erase(raw->data.keyboard.VKey);
				_keysUp.emplace(raw->data.keyboard.VKey, raw->data.keyboard.VKey);
			}
		}
	}

	delete lpb;
}

bool Input::GetKey(int keyCode)
{
	return _keys.find(keyCode) != _keys.end();
}

bool Input::GetKeyDown(int keyCode)
{
	return _keysDown.find(keyCode) != _keysDown.end();
}

bool Input::GetKeyUp(int keyCode)
{
	return _keysUp.find(keyCode) != _keysUp.end();
}

int64_t Input::GetMouseWheelRelativePosition()
{
	return _mouseWheelAbsolute - _mouseWheelAbsoluteLast;
}

int64_t Input::GetMouseWheelPosition()
{
	return _mouseWheelAbsolute;
}

XMFLOAT2A Input::GetMouseRelativePosition()
{
	return XMFLOAT2A(_mousePosAbsolute.x - _mousePosAbsoluteLast.x, _mousePosAbsolute.y - _mousePosAbsoluteLast.y);
}

XMFLOAT2A Input::GetMousePosition()
{
	return _mousePosAbsolute;
}

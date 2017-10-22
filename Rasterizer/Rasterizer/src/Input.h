#pragma once

#include "stdafx.h"
#include "Singleton.h"
#include "Float2.h"

#include <map>

class Input :
	public Singleton<Input>
{
	friend class Singleton<Input>;
protected:

#pragma region Protected

	RAWINPUTDEVICE _deviceID[2];

	std::multimap<uint16_t, uint16_t> _keysDown;
	int64_t _mouseWheelAbsoluteLast = 0;
	int64_t _mouseWheelAbsolute = 0;
	math::Float2 _mousePosAbsoluteLast;
	math::Float2 _mousePosAbsolute;

#pragma endregion

#pragma region Functions Protected

	Input();

#pragma endregion

public:
	
#pragma region Functions Public

	virtual ~Input();

	void Initialize();
	void Shutdown();
	void Update(HRAWINPUT hRawInput);

	bool GetKeyDown(int keyCode);
	int64_t GetMouseWheelRelativePosition();
	int64_t GetMouseWheelPosition();
	math::Float2 GetMouseRelativePosition();
	math::Float2 GetMousePosition();

#pragma endregion

};


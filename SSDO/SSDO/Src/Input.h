#pragma once

#include "GlobalDefines.h"
#include "Singleton.h"

#include <map>

class Input :
	public Singleton<Input>
{
	friend class Singleton<Input>;
protected:

#pragma region Protected

	RAWINPUTDEVICE _deviceID[2];

	std::multimap<uint16_t, uint16_t> _keys;
	std::multimap<uint16_t, uint16_t> _keysDown;
	std::multimap<uint16_t, uint16_t> _keysUp;
	int64_t _mouseWheelAbsoluteLast = 0;
	int64_t _mouseWheelAbsolute = 0;
	XMFLOAT2A _mousePosAbsoluteLast;
	XMFLOAT2A _mousePosAbsolute;
	bool _bEverySecondFlag = false;

#pragma endregion

#pragma region Functions Protected

	Input();

#pragma endregion

public:
	
#pragma region Functions Public

	virtual ~Input();

	void Initialize();
	void Shutdown();
	void Update();
	void Update(HRAWINPUT hRawInput);

	bool GetKey(int keyCode);
	bool GetKeyDown(int keyCode);
	bool GetKeyUp(int keyCode);
	int64_t GetMouseWheelRelativePosition();
	int64_t GetMouseWheelPosition();
	XMFLOAT2A GetMouseRelativePosition();
	XMFLOAT2A GetMousePosition();

#pragma endregion

};


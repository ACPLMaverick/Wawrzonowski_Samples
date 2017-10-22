#pragma once

#include "stdafx.h"
#include "Singleton.h"
#include "Buffer.h"
#include "IRenderer.h"
#include "Sphere.h"
#include "Plane.h"

#include <vector>
#include <functional>

class Scene;

class __declspec(align(16)) System : public Singleton<System>
{
	friend class Singleton<System>;
protected:

#pragma region SettingsSystem

	SystemSettings _settings;

#pragma endregion

#pragma region Draw Related

	IRenderer* _renderer;

	HBITMAP _bitmapScreenBuffer;
	BITMAPINFO _bitmapScreenBufferInfo;
	void* _bitmapScreenBufferDataPtr;

#pragma endregion

#pragma region Variables

	bool _active = true;
	bool _running = true;

#pragma endregion

#pragma region Collections

	std::vector<std::function<void(UINT, WPARAM, LPARAM)>*> _eventsMessage;
	std::vector<Scene*> _scenes;
	uint32_t _currentScene = 0;

#pragma endregion

#pragma region Timers



#pragma endregion

	System();

	inline void InitWindow(
		_In_ HINSTANCE hInstance,
		_In_ LPWSTR    lpCmdLine,
		_In_ int       nCmdShow);
	inline void RunMessages();
	inline void ResizeWindowBitmap();
	inline void DrawColorBuffer();
	inline void DrawFPS();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:

	~System();

	void Initialize(
		_In_ HINSTANCE hInstance,
		_In_ LPWSTR    lpCmdLine,
		_In_ int       nCmdShow);
	void Shutdown();
	void Run();
	void Pause();
	void Stop();

#pragma region Accessors

	SystemSettings* GetSystemSettings() { return &_settings; }
	Scene* GetCurrentScene() { return _scenes[_currentScene]; }
	std::vector<Scene*>* const GetSceneCollection() { return &_scenes; }
	IRenderer* GetRenderer() { return _renderer; }

	void AddEventHandlerMessage(std::function<void(UINT, WPARAM, LPARAM)>* func);
	bool RemoveEventHandlerMessage(std::function<void(UINT, WPARAM, LPARAM)>* func);

#pragma endregion
};


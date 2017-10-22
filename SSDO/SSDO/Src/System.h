#pragma once

#include "stdafx.h"
#include "Singleton.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Scenes
{
	class Scene;
}

class Random;

class System : public Singleton<System>
{
public:

	struct Options
	{
		const int32_t _windowWidth;
		const int32_t _windowHeight;
		const int32_t _refreshRate;
		const float _aspectRatio;
		const bool _bFullscreen;

		Options(int32_t ww, int32_t wh, int32_t rr, bool bw) :
			_windowWidth(ww),
			_windowHeight(wh),
			_refreshRate(rr),
			_bFullscreen(bw),
			_aspectRatio(static_cast<float>(ww) / static_cast<float>(wh))
		{

		}
	};

protected:

#pragma region Protected

	static const int32_t MAX_LOADSTRING = 100;

	Options _options;

	HINSTANCE _hInst;                                // current instance
	HWND _hwnd;
	WCHAR _szTitle[MAX_LOADSTRING];                  // The title bar text
	WCHAR _szWindowClass[MAX_LOADSTRING];            // the main window class name

	Scenes::Scene* _scene = nullptr;

	Random* _random = nullptr;

	FT_Library _ftLibrary;

	bool _bNeedToQuit = false;

#pragma endregion

#pragma region Functions Protected

	ATOM MyRegisterClass(HINSTANCE hInstance);
	void InitInstance(HINSTANCE hInstance, int nCmdShow);
	void RunMessageLoop();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#pragma endregion

public:

#pragma region Functions Public

	System();
	~System();

	void Initialize(_In_ HINSTANCE hInstance,
		_In_opt_ HINSTANCE hPrevInstance,
		_In_ LPWSTR    lpCmdLine,
		_In_ int       nCmdShow);
	void Run();
	void Shutdown();
	void Exit();

	inline HINSTANCE GetHInst() const { return _hInst; }
	inline HWND GetHWND() const { return _hwnd; }
	inline const Options& GetOptions() const { return _options; }
	inline Scenes::Scene* GetScene() { return _scene; }
	inline Random* GetRandom() const { return _random; }
	inline FT_Library GetFreeTypeLibrary() { return _ftLibrary; }

#pragma endregion
};


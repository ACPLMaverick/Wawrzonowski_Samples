#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "System.h"
#include "Scene.h"
#include "SceneTriangle.h"
#include "SceneMeshes.h"
#include "SceneCornel.h"
#include "SceneSphere.h"
#include "SpecificObjectFactory.h"
#include "Timer.h"
#include "Input.h"

#include "rendererFGK\RendererFGK.h"

// testing
//#ifdef _DEBUG
#define _CRT_SECURE_NO_DEPRECATE
#include "Float3.h"
#include "Matrix4x4.h"
#include "Float4.h"
//#endif // _DEBUG

#include <Commctrl.h>

#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <string>
#include <iomanip>

System::System()
{
}


System::~System()
{
}

void System::Initialize(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	_settings._hInstance = hInstance;
	_settings._lpCmdLine = lpCmdLine;
	_settings._nCmdShow = nCmdShow;

#ifdef _DEBUG

	std::cout.clear();

	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	std::cout.clear();
	std::ios::sync_with_stdio();
	std::cout << "Hi." << std::endl;
	
#endif // _DEBUG

	// initialize window in current OS
	InitWindow(hInstance, lpCmdLine, nCmdShow);

	// initialize console - debug only
	// initialize DIB
	RECT r;
	GetClientRect(_settings._hwnd, &r);
	
	ZeroMemory(&_bitmapScreenBufferInfo, sizeof(BITMAPINFO));
	_bitmapScreenBufferInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	_bitmapScreenBufferInfo.bmiHeader.biWidth = r.right - r.left;
	_bitmapScreenBufferInfo.bmiHeader.biHeight = r.bottom - r.top;
	_bitmapScreenBufferInfo.bmiHeader.biPlanes = 1;
	_bitmapScreenBufferInfo.bmiHeader.biBitCount = 32;
	_bitmapScreenBufferInfo.bmiHeader.biSizeImage = _settings.GetWindowWidth() * _settings.GetWindowHeight();
	_bitmapScreenBufferInfo.bmiHeader.biCompression = BI_RGB;
	_bitmapScreenBufferInfo.bmiHeader.biSizeImage = 0;

	HDC dc = GetWindowDC(_settings._hwnd);
	_bitmapScreenBuffer = CreateDIBSection(dc, &_bitmapScreenBufferInfo, DIB_RGB_COLORS, &_bitmapScreenBufferDataPtr, NULL, NULL);

	_settings._displayWidth = _bitmapScreenBufferInfo.bmiHeader.biWidth;
	_settings._displayHeight = _bitmapScreenBufferInfo.bmiHeader.biHeight;

	// initialize managers
	_renderer = SpecificObjectFactory::GetRenderer(&_settings);

	Timer::GetInstance()->Initialize();
	Input::GetInstance()->Initialize();

	// tu bedom testy a co
	/*

	const int num = 10000;

	math::Float3 aTab[num];
	math::Float3 bTab[num];
	math::Float3 resTab[num];

	Timer::GetInstance()->Update();
	double time = Timer::GetInstance()->GetActualTime();

	for (int i = 0; i < num; ++i)
	{
		resTab[i] = aTab[i] + bTab[i];
	}

	Timer::GetInstance()->Update();
	time = Timer::GetInstance()->GetActualTime() - time;
	std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(10) << time;

	// tu sie koñczom testy 
	*/

	// initialize scenes
	//_scenes.push_back(new SceneTriangle());
	//std::string sName = "SceneTriangle";
	//_scenes.push_back(new SceneMeshes());
	//std::string sName = "SceneMeshes";
	_scenes.push_back(new SceneCornel());
	std::string sName = "SceneCornel";
	//_scenes.push_back(new SceneSphere());
	//std::string sName = "SceneSpheres";
	_scenes[0]->Initialize(0, &sName);

#ifdef RENDERER_FGK
#ifdef RENDERER_FGK_MULTITHREAD

	((rendererFGK::RendererFGK*)_renderer)->InitThreads();

#endif // RENDERER_FGK_MULTITHREAD
#endif
}

void System::Shutdown()
{
#ifdef _DEBUG

	FreeConsole();

#endif // _DEBUG

	// shutdown managers
	delete _renderer;
	Input::GetInstance()->Shutdown();
	Input::GetInstance()->DestroyInstance();
	Timer::GetInstance()->Shutdown();
	Timer::GetInstance()->DestroyInstance();

	UnregisterClass((_settings.s_windowTitle.c_str()), _settings._hInstance);
	DestroyWindow(_settings._hwnd);
	PostQuitMessage(0);
}

void System::Run()
{
	while (_running)
	{
		RunMessages();

		if (Input::GetInstance()->GetKeyDown(VK_ESCAPE))
		{
			Stop();
		}

		// update timer
		Timer::GetInstance()->Update();

		// update scene instances
		_scenes[_currentScene]->Update();

		// draw scene to buffer
		_renderer->Draw(_scenes[_currentScene]);

		// fill bitmap with color buffer data
		size_t tj = _bitmapScreenBufferInfo.bmiHeader.biWidth;
		size_t ti = _bitmapScreenBufferInfo.bmiHeader.biHeight;
		for (size_t i = 0; i < ti; ++i)
		{
			for (size_t j = 0; j < tj; ++j)
			{
				((uint32_t*)_bitmapScreenBufferDataPtr)[i * tj + j] = (uint32_t)(_renderer->GetColorBuffer()->GetPixelScaled((uint16_t)j, (uint16_t)i, (uint16_t)tj, (uint16_t)ti).color);
			}
		}

		// draw bitmap in window
		RECT r;
		GetClientRect(_settings._hwnd, &r);
		InvalidateRect(_settings._hwnd, &r, false);
	}
}

void System::Pause()
{
}

void System::Stop()
{
	_running = false;
}

void System::AddEventHandlerMessage(std::function<void(UINT, WPARAM, LPARAM)>* func)
{
	_eventsMessage.push_back(func);
}

bool System::RemoveEventHandlerMessage(std::function<void(UINT, WPARAM, LPARAM)>* func)
{
	for (std::vector<std::function<void(UINT, WPARAM, LPARAM)>*>::iterator it = _eventsMessage.begin(); it != _eventsMessage.end(); ++it)
	{
		if (*it == func)
		{
			_eventsMessage.erase(it);
			return true;
		}
	}

	return false;
}

void System::InitWindow(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hCursor = NULL;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = (_settings.s_windowTitle.c_str());
	wc.lpszMenuName = 0;
	wc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);

	_settings._hwnd = CreateWindow(
		(_settings.s_windowTitle.c_str()),
		(_settings.s_windowTitle.c_str()),
		(WS_OVERLAPPED |
			WS_CAPTION |
			WS_SYSMENU),
		10, 10,
		_settings.s_windowWidth, _settings.s_windowHeight,
		NULL, NULL,
		hInstance, NULL
		);

	// bottom bar for fps
	InitCommonControls();
	_settings._hwndStatus = CreateWindowEx(
		0,
		STATUSCLASSNAME,
		(PCTSTR)NULL,
		WS_CHILD | WS_VISIBLE,
		0, 0, 0, 0,
		_settings._hwnd,
		(HMENU)1,
		_settings._hInstance,
		NULL
	);

	ShowWindow(_settings._hwnd, nCmdShow);
	UpdateWindow(_settings._hwnd);
}

inline void System::RunMessages()
{
	MSG msg;

	while (PeekMessageW(&msg, _settings._hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

inline void System::ResizeWindowBitmap()
{
	if (_bitmapScreenBuffer == nullptr || _bitmapScreenBufferDataPtr == nullptr)
		return;

}

inline void System::DrawColorBuffer()
{
	if (_bitmapScreenBuffer == nullptr || _bitmapScreenBufferDataPtr == nullptr)
		return;

	PAINTSTRUCT ps;
	BITMAP nbm;
	HDC hdc = BeginPaint(_settings._hwnd, &ps);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, (HBITMAP)_bitmapScreenBuffer);
	GetObject((HBITMAP)_bitmapScreenBuffer, sizeof(BITMAP), &nbm);
	BitBlt(hdc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top,
		hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);

	EndPaint(_settings._hwnd, &ps);
}

inline void System::DrawFPS()
{
	std::string fps = std::to_string(Timer::GetInstance()->GetFPS());
	std::string fpsAvg = std::to_string(Timer::GetInstance()->GetFPSAverage());
	std::string time = std::to_string(Timer::GetInstance()->GetActualTime());
	std::string deltaTime = std::to_string(Timer::GetInstance()->GetDeltaTime());
	fps = fps.substr(0, 6);
	fpsAvg = fpsAvg.substr(0, 6);
	time = time.substr(0, 6);
	deltaTime = deltaTime.substr(0, 6);

	std::string fpsFormatted = "FPS: " + fps + " | FPSAvg: " + fpsAvg + " | Time: " + time + " | Delta: " + deltaTime ;

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(_settings._hwndStatus, &ps);

	int result = DrawText(
		hdc,
		fpsFormatted.c_str(),
		-1,
		&ps.rcPaint,
		DT_LEFT | DT_BOTTOM
	);

	EndPaint(_settings._hwndStatus, &ps);
}

LRESULT System::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// pass message to event handlers
	for (std::vector<std::function<void(UINT, WPARAM, LPARAM)>*>::iterator it = instance->_eventsMessage.begin(); it != instance->_eventsMessage.end(); ++it)
	{
		(*(*it))(message, wParam, lParam);
	}

	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_SIZE:
		{
			System::GetInstance()->ResizeWindowBitmap();
		}

		break;
	case WM_PAINT:
		{
			System::GetInstance()->DrawColorBuffer();
			System::GetInstance()->DrawFPS();
		}
		
		break;
	case WM_DESTROY:
		System::GetInstance()->Stop();
		break;
	case WM_SETCURSOR:
		SetCursor(NULL);
		break;
	case WM_INPUT:

		Input::GetInstance()->Update((HRAWINPUT)lParam);

		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
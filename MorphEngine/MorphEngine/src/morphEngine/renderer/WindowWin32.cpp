#include "WindowWin32.h"

#ifdef PLATFORM_WINDOWS
#include "core/MessageSystem.h"
#include "debugging/Debug.h"
#include "utility/MMath.h"
#include <map>

namespace morphEngine
{
	namespace renderer
	{
		LRESULT CALLBACK Win32Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		void ProcessMouseInput(const RAWINPUT* rawInput);
		void ProcessKeyboardInput(const RAWINPUT* rawInput);

		bool WindowWin32::OpenWindow(const utility::MString& title, MInt32 windowWidth, MInt32 windowHeight, bool fullScreen)
		{
			_hInstance = GetModuleHandle(NULL);
			if(_hInstance == INVALID_HANDLE_VALUE)
			{
				return false;
			}

			static const char* _className = "MorphEngineWin32WindowClass";

			WNDCLASSEX wndClassEx;
			wndClassEx.cbSize = sizeof(WNDCLASSEX);
			wndClassEx.hInstance = _hInstance;
			wndClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
			wndClassEx.cbClsExtra = 0;
			wndClassEx.cbWndExtra = 0;
			wndClassEx.hIcon = LoadIconA(_hInstance, IDI_APPLICATION);
			wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
			wndClassEx.lpszMenuName = NULL;
			wndClassEx.lpszClassName = _className;
			wndClassEx.hIconSm = wndClassEx.hIcon;
			wndClassEx.lpfnWndProc = Win32Proc;

			if(!RegisterClassEx(&wndClassEx))
			{
				debugging::Debug::Log(debugging::ELogType::ERR, "Can't register Win32 class");
				return false;
			}

			_fullScreen = false;
			_windowWidth = windowWidth;
			_windowHeight = windowHeight;

			RECT targetRect = {0, 0, _windowWidth, _windowHeight};
			AdjustWindowRect(&targetRect, WS_OVERLAPPEDWINDOW, FALSE);
			_hWnd = CreateWindow(_className, title, WS_OVERLAPPEDWINDOW, 0, 0, utility::MMath::Abs(targetRect.right - targetRect.left), utility::MMath::Abs(targetRect.top - targetRect.bottom), 0, 0, _hInstance, 0);
			if(!_hWnd)
			{
				debugging::Debug::Log(debugging::ELogType::ERR, "Can't create Win32 class");
				return false;
			}

			static const MUint8 DEVICES_COUNT = 2;
			RAWINPUTDEVICE deviceID[DEVICES_COUNT];
			//mouse
			deviceID[0].usUsagePage = 0x01;
			deviceID[0].usUsage = 0x02;
			deviceID[0].dwFlags = 0;
			deviceID[0].hwndTarget = _hWnd;

			// keyboard
			deviceID[1].usUsagePage = 0x01;
			deviceID[1].usUsage = 0x06;
			deviceID[1].dwFlags = RIDEV_NOHOTKEYS;
			deviceID[1].hwndTarget = _hWnd;

			if(!RegisterRawInputDevices(deviceID, DEVICES_COUNT, sizeof(deviceID[0])))
			{
				debugging::Debug::Log(debugging::ELogType::ERR, "Can't register raw input devices");
				return false;
			}

			SetIsFullscreen(fullScreen);

			core::MessageSystem::OnGatherMessages += new core::Event<void>::ClassRawDelegate<WindowWin32>(this, &WindowWin32::GatherMessages);

			return true;
		}

		void WindowWin32::ShutdownWindow()
		{
			if(IsOpened())
			{
				CloseWindow(_hWnd);
				DestroyWindow(_hWnd);
				_hWnd = 0;
			}
		}

		void WindowWin32::Resize(MInt32 newWidth, MInt32 newHeight)
		{
			if(!_fullScreen)
			{
				_windowWidth = newWidth;
				_windowHeight = newHeight;
				RECT targetRect = {0, 0, _windowWidth, _windowHeight};
				AdjustWindowRect(&targetRect, WS_OVERLAPPEDWINDOW, FALSE);
				SetWindowPos(_hWnd, HWND_TOP, 0, 0, utility::MMath::Abs(targetRect.right - targetRect.left), utility::MMath::Abs(targetRect.top - targetRect.bottom), SWP_SHOWWINDOW);
			}
		}

		LRESULT CALLBACK Win32Proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			switch(msg)
			{
			case WM_DESTROY:
				core::MessageSystem::OnWindowDestroy();
				return 0;
			case WM_QUIT:
				core::MessageSystem::OnWindowQuit();
				return 0;
			case WM_CREATE:
				core::MessageSystem::OnWindowCreate();
				return 0;
			case WM_SIZE:
				//According to MSDN 32-bit (or 64-bit) lParam consists of two 16-bit (or 32-bit) values
				//Low order word is width and high order word is height
				core::MessageSystem::OnWindowResize(LOWORD(lParam), HIWORD(lParam));
				return 0;
			case WM_MOVE:
				//According to MSDN 32-bit (or 64-bit) lParam consists of two 16-bit (or 32-bit) values
				//Low order word is x and high order word is y
				core::MessageSystem::OnWindowMove(LOWORD(lParam), HIWORD(lParam));
				return 0;
			case WM_INPUT:
				HRAWINPUT hRawInput = (HRAWINPUT)lParam;
				MUint32 dataSize;
				GetRawInputData(hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
				MUint8* lbp = new MUint8[dataSize];
				GetRawInputData(hRawInput, RID_INPUT, lbp, &dataSize, sizeof(RAWINPUTHEADER));
				RAWINPUT* raw = (RAWINPUT*)lbp;

				if(raw->header.dwType == RIM_TYPEMOUSE)
				{
					ProcessMouseInput(raw);
				}
				else if(raw->header.dwType == RIM_TYPEKEYBOARD)
				{
					ProcessKeyboardInput(raw);
				}

				delete[] lbp;
				return 0;
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}

		void ProcessMouseInput(const RAWINPUT* rawInput)
		{
			ME_ASSERT(rawInput, "RAWINPUT structure is nullptr");
			MUint16 buttonFlag = rawInput->data.mouse.usButtonFlags;
			if(buttonFlag == RI_MOUSE_LEFT_BUTTON_DOWN)
			{
				core::MessageSystem::OnMouseButtonDown(input::EMouseButton::LEFT);
			}
			else if(buttonFlag == RI_MOUSE_LEFT_BUTTON_UP)
			{
				core::MessageSystem::OnMouseButtonUp(input::EMouseButton::LEFT);
			}
			else if(buttonFlag == RI_MOUSE_RIGHT_BUTTON_DOWN)
			{
				core::MessageSystem::OnMouseButtonDown(input::EMouseButton::RIGHT);
			}
			else if(buttonFlag == RI_MOUSE_RIGHT_BUTTON_UP)
			{
				core::MessageSystem::OnMouseButtonUp(input::EMouseButton::RIGHT);
			}
			else if(buttonFlag == RI_MOUSE_MIDDLE_BUTTON_DOWN)
			{
				core::MessageSystem::OnMouseButtonDown(input::EMouseButton::MIDDLE);
			}
			else if(buttonFlag == RI_MOUSE_MIDDLE_BUTTON_UP)
			{
				core::MessageSystem::OnMouseButtonUp(input::EMouseButton::MIDDLE);
			}
			
			if(rawInput->data.mouse.usFlags == MOUSE_MOVE_ABSOLUTE || rawInput->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
			{
				core::MessageSystem::OnMouseMove(rawInput->data.mouse.lLastX, rawInput->data.mouse.lLastY);
			}
		}

		void ProcessKeyboardInput(const RAWINPUT* rawInput)
		{
			ME_ASSERT(rawInput, "RAWINPUT structure is nullptr");
			input::EKeyCode keyCode = VKtoKeyCode(rawInput->data.keyboard.VKey, rawInput->data.keyboard.MakeCode, rawInput->data.keyboard.Flags);
			if(rawInput->data.keyboard.Flags & 0b1)
			{
				core::MessageSystem::OnKeyUp(keyCode);
			}
			else
			{
				core::MessageSystem::OnKeyDown(keyCode);
			}
		}
	}
}

#endif
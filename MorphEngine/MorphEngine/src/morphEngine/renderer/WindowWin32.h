#pragma once

#include "renderer/Window.h"

#ifdef PLATFORM_WINDOWS
#include "input/EKeyCode.h"

namespace morphEngine
{
	namespace renderer
	{
		class WindowWin32 : public Window
		{
		protected:
			HINSTANCE _hInstance;
			HWND _hWnd;
			MInt32 _windowWidth;
			MInt32 _windowHeight;
			bool _fullScreen;

		protected:
			inline void GatherMessages()
			{
				if(!IsOpened())
				{
					return;
				}

				MSG msg;
				while(PeekMessage(&msg, _hWnd, 0, 0, PM_REMOVE) != 0)
				{
					DispatchMessage(&msg);
					TranslateMessage(&msg);
				}
			}

		public:
			virtual bool OpenWindow(const utility::MString& title, MInt32 windowWidth, MInt32 windowHeight, bool fullScreen);
			virtual void ShutdownWindow();
			virtual void Resize(MInt32 newWidth, MInt32 newHeight);
			
			inline virtual bool IsOpened() const
			{
				return _hWnd != 0;
			}

			inline virtual void Show() const
			{
				if(!IsOpened())
				{
					return;
				}

				ShowWindow(_hWnd, SW_SHOW);
			}

			inline virtual void Hide() const
			{
				if(!IsOpened())
				{
					return;
				}
				ShowWindow(_hWnd, SW_HIDE);
			}

			inline virtual void SetIsFullscreen(bool fullScreen)
			{
				//XOR - if different
				if(_fullScreen ^ fullScreen)
				{
					MInt32 width = fullScreen ? GetSystemMetrics(SM_CXSCREEN) : _windowWidth;
					MInt32 height = fullScreen ? GetSystemMetrics(SM_CYSCREEN) : _windowHeight;
					SetWindowLongPtr(_hWnd, GWL_STYLE, fullScreen ? WS_POPUP | WS_VISIBLE : WS_OVERLAPPEDWINDOW);
					SetWindowPos(_hWnd, HWND_TOP, 0, 0, width, height, SWP_SHOWWINDOW);
				}
				_fullScreen = fullScreen;
			}

			inline virtual bool IsFullscreen() const
			{
				return _fullScreen;
			}

			inline virtual MInt32 GetWidth() const
			{
				return _fullScreen ? GetSystemMetrics(SM_CXSCREEN) : _windowWidth;
			}

			inline virtual MInt32 GetHeight() const
			{
				return _fullScreen ? GetSystemMetrics(SM_CYSCREEN) : _windowHeight;
			}

			inline HWND GetHWND() const { return _hWnd; }

			inline HINSTANCE GetHInstance() const { return _hInstance; }
		};

		inline input::EKeyCode VKtoKeyCode(WPARAM virtualKey, MUint16 scanCode, MUint16 flags)
		{
			switch(virtualKey)
			{
			case VK_BACK:
				return input::EKeyCode::BACKSPACE;
			case VK_TAB:
				return input::EKeyCode::TAB;
			case VK_RETURN:
				return input::EKeyCode::ENTER;
			case VK_SHIFT:
				if(scanCode == 0x36)
				{
					return input::EKeyCode::RIGHT_SHIFT;
				}
				else //scanCode == 0x2a
				{
					return input::EKeyCode::LEFT_SHIFT;
				}
			case VK_CONTROL:
				if(flags & 0b10)
				{
					return input::EKeyCode::RIGHT_CONTROL;
				}
				else
				{
					return input::EKeyCode::LEFT_CONTROL;
				}
			case VK_MENU:
				if(flags & 0b10)
				{
					return input::EKeyCode::RIGHT_ALT;
				}
				else
				{
					return input::EKeyCode::LEFT_ALT;
				}
			case VK_CAPITAL:
				return input::EKeyCode::CAPS_LOCK;
			case VK_ESCAPE:
				return input::EKeyCode::ESCAPE;
			case VK_SPACE:
				return input::EKeyCode::SPACE;
			case VK_LEFT:
				return input::EKeyCode::LEFT_ARROW;
			case VK_RIGHT:
				return input::EKeyCode::RIGHT_ARROW;
			case VK_UP:
				return input::EKeyCode::UP_ARROW;
			case VK_DOWN:
				return input::EKeyCode::DOWN_ARROW;
			case VK_DELETE:
				return input::EKeyCode::DEL;
			case 0x30:
				return input::EKeyCode::ALPHA_0;
			case 0x31:
				return input::EKeyCode::ALPHA_1;
			case 0x32:
				return input::EKeyCode::ALPHA_2;
			case 0x33:
				return input::EKeyCode::ALPHA_3;
			case 0x34:
				return input::EKeyCode::ALPHA_4;
			case 0x35:
				return input::EKeyCode::ALPHA_5;
			case 0x36:
				return input::EKeyCode::ALPHA_6;
			case 0x37:
				return input::EKeyCode::ALPHA_7;
			case 0x38:
				return input::EKeyCode::ALPHA_8;
			case 0x39:
				return input::EKeyCode::ALPHA_9;
			case 0x41:
				return input::EKeyCode::A;
			case 0x42:
				return input::EKeyCode::B;
			case 0x43:
				return input::EKeyCode::C;
			case 0x44:
				return input::EKeyCode::D;
			case 0x45:
				return input::EKeyCode::E;
			case 0x46:
				return input::EKeyCode::F;
			case 0x47:
				return input::EKeyCode::G;
			case 0x48:
				return input::EKeyCode::H;
			case 0x49:
				return input::EKeyCode::I;
			case 0x4A:
				return input::EKeyCode::J;
			case 0x4B:
				return input::EKeyCode::K;
			case 0x4C:
				return input::EKeyCode::L;
			case 0x4D:
				return input::EKeyCode::M;
			case 0x4E:
				return input::EKeyCode::N;
			case 0x4F:
				return input::EKeyCode::O;
			case 0x50:
				return input::EKeyCode::P;
			case 0x51:
				return input::EKeyCode::Q;
			case 0x52:
				return input::EKeyCode::R;
			case 0x53:
				return input::EKeyCode::S;
			case 0x54:
				return input::EKeyCode::T;
			case 0x55:
				return input::EKeyCode::U;
			case 0x56:
				return input::EKeyCode::V;
			case 0x57:
				return input::EKeyCode::W;
			case 0x58:
				return input::EKeyCode::X;
			case 0x59:
				return input::EKeyCode::Y;
			case 0x5A:
				return input::EKeyCode::Z;
			case VK_NUMPAD0:
				return input::EKeyCode::NUMPAD_0;
			case VK_NUMPAD1:
				return input::EKeyCode::NUMPAD_1;
			case VK_NUMPAD2:
				return input::EKeyCode::NUMPAD_2;
			case VK_NUMPAD3:
				return input::EKeyCode::NUMPAD_3;
			case VK_NUMPAD4:
				return input::EKeyCode::NUMPAD_4;
			case VK_NUMPAD5:
				return input::EKeyCode::NUMPAD_5;
			case VK_NUMPAD6:
				return input::EKeyCode::NUMPAD_6;
			case VK_NUMPAD7:
				return input::EKeyCode::NUMPAD_7;
			case VK_NUMPAD8:
				return input::EKeyCode::NUMPAD_8;
			case VK_NUMPAD9:
				return input::EKeyCode::NUMPAD_9;
			case VK_F1:
				return input::EKeyCode::F1;
			case VK_F2:
				return input::EKeyCode::F2;
			case VK_F3:
				return input::EKeyCode::F3;
			case VK_F4:
				return input::EKeyCode::F4;
			case VK_F5:
				return input::EKeyCode::F5;
			case VK_F6:
				return input::EKeyCode::F6;
			case VK_F7:
				return input::EKeyCode::F7;
			case VK_F8:
				return input::EKeyCode::F8;
			case VK_F9:
				return input::EKeyCode::F9;
			case VK_F10:
				return input::EKeyCode::F10;
			case VK_F11:
				return input::EKeyCode::F11;
			case VK_F12:
				return input::EKeyCode::F12;
			case VK_OEM_COMMA:
				return input::EKeyCode::COMMA;
			case VK_OEM_PERIOD:
				return input::EKeyCode::DOT;
			case VK_OEM_1:
				return input::EKeyCode::SEMICOLON;
			case VK_OEM_2:
				return input::EKeyCode::SLASH;
			case VK_OEM_3:
				return input::EKeyCode::TILDE;
			case VK_OEM_4:
				return input::EKeyCode::LEFT_BRACKET;
			case VK_OEM_5:
				return input::EKeyCode::BACKSLASH;
			case VK_OEM_6:
				return input::EKeyCode::RIGHT_BRACKET;
			case VK_OEM_7:
				return input::EKeyCode::SINGLE_QUOTE;
			default:
				return 0;
			}
		}
	}
}

#endif
#pragma once

#include "Window.h"

#include "core/MessageSystem.h"

namespace morphEngine
{
	namespace renderer
	{
		//Editor window class (used for editor scene view representation only)
		//And for initializing DirectX in MorphEngine as well
		class WindowEditor : public Window
		{
		protected:
			HINSTANCE _hInstance;
			HWND _hWnd;
			MInt32 _windowWidth;
			MInt32 _windowHeight;

		public:
			inline WindowEditor(HWND hWnd, HINSTANCE hInstance, MInt32 width, MInt32 height) : Window(), _hWnd(hWnd), _hInstance(hInstance), _windowWidth(width), _windowHeight(height)
			{

			}

			//Opens window (does not show window)
			inline virtual bool OpenWindow(const utility::MString& title, MInt32 windowWidth, MInt32 windowHeight, bool fullScreen)
			{
				//Returns true cause it's managed by editor app
				return true;
			}

			//Shutdowns window (if one is opened)
			inline virtual void ShutdownWindow()
			{
				//It's managed by editor app
			}

			//Resizes window to given values
			//NOTE: This function has no effect if window is in fullscreen mode
			inline virtual void Resize(MInt32 newWidth, MInt32 newHeight)
			{
				_windowWidth = newWidth;
				_windowHeight = newHeight;
				core::MessageSystem::OnWindowResize(newWidth, newHeight);
			}

			//Returns true if window is opened
			inline virtual bool IsOpened() const
			{
				return _hWnd != 0;
			}

			//Shows window
			inline virtual void Show() const
			{
				if(IsOpened())
				{
					ShowWindow(_hWnd, SW_SHOW);
				}
			}

			//Hides window
			inline virtual void Hide() const
			{
				if(IsOpened())
				{
					ShowWindow(_hWnd, SW_HIDE);
				}
			}

			//Sets fullscreen/windowed mode of window
			inline virtual void SetIsFullscreen(bool fullscreen)
			{
				//Do nothing since it's editor window and it's size etc. is managed by editor application
			}

			//Returns true if window is in fullscreen mode
			inline virtual bool IsFullscreen() const
			{
				return false;
			}

			//Returns current window width (if fullscreen mode is on then will return screen width)
			inline virtual MInt32 GetWidth() const
			{
				return _windowWidth;
			}

			//Returns current window height (if fullscreen mode is on then will return screen height)
			inline virtual MInt32 GetHeight() const
			{
				return _windowHeight;
			}

			inline HWND GetHWND() const
			{
				return _hWnd;
			}

			inline HINSTANCE GetHInstance() const
			{
				return _hInstance;
			}
		};

	}
}
#pragma once

#include "utility/MString.h"

namespace morphEngine
{
	namespace renderer
	{
		class WindowEditor;

		/// <summary>
		/// Abstract class.
		/// </summary>
		class Window
		{
		public:
			static WindowEditor* EditorWindow;

		public:
			//Opens window (does not show window)
			virtual bool OpenWindow(const utility::MString& title, MInt32 windowWidth, MInt32 windowHeight, bool fullScreen) = 0;
			//Shutdowns window (if one is opened)
			virtual void ShutdownWindow() = 0;
			//Resizes window to given values
			//NOTE: This function has no effect if window is in fullscreen mode
			virtual void Resize(MInt32 newWidth, MInt32 newHeight) = 0;

			//Returns true if window is opened
			inline virtual bool IsOpened() const = 0;
			//Shows window
			inline virtual void Show() const = 0;
			//Hides window
			inline virtual void Hide() const = 0;
			//Sets fullscreen/windowed mode of window
			inline virtual void SetIsFullscreen(bool fullscreen) = 0;
			//Returns true if window is in fullscreen mode
			inline virtual bool IsFullscreen() const = 0;
			//Returns current window width (if fullscreen mode is on then will return screen width)
			inline virtual MInt32 GetWidth() const = 0;
			//Returns current window height (if fullscreen mode is on then will return screen height)
			inline virtual MInt32 GetHeight() const = 0;

			//Returns NEW window instance depending on platform
			static Window* GetWindowInstance();
		};

	}
}
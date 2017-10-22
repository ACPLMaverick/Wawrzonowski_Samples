#include "Window.h"

#include "WindowEditor.h"

#ifdef PLATFORM_WINDOWS
#include "WindowWin32.h"
#endif

namespace morphEngine
{
	namespace renderer
	{
		WindowEditor* Window::EditorWindow = nullptr;

		Window* Window::GetWindowInstance()
		{
			if(EditorWindow != nullptr)
			{
				return EditorWindow;
			}

#ifdef PLATFORM_WINDOWS
			Window* window = new WindowWin32();
#endif
			return window;
		}

	}
}
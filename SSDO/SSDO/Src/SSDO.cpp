// SSDO.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "SSDO.h"
#include "System.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int APIENTRY wWinMain(_In_ HINSTANCE _hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	System* system = System::GetInstance();
	system->Initialize(_hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	system->Run();
	system->Shutdown();
	System::DestroyInstance();
    return 0;
}
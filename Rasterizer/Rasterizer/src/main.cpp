// LittleGame.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "System.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	System::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow);

	System::GetInstance()->Run();

	System::GetInstance()->Shutdown();

	System::DestroyInstance();

    return 0;
}
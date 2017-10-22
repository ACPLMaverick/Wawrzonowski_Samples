// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "Math.h"

// TODO: reference additional headers your program requires here

#include <iostream>
#include "SystemSettings.h"
#include "Float3.h"
#include "Color32.h"

// DEFINES FOR RENDERER VERSION
// ONLY ONE AT A TIME CAN BE DEFINED

#define RENDERER_FGK
//#define RENDERER_MAV
//#define RENDERER_MAJSTER

#include "Optimizations.h"
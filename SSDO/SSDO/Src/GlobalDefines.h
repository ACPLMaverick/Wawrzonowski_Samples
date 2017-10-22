#pragma once

#include <cstdint>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace std;

#include "System.h"

#define ZERO(arg) ZeroMemory(&arg, sizeof(arg))
#define ZEROM(argPtr, sizeBytes) ZeroMemory(argPtr, sizeBytes)

#define DWZ(type, name) \
type name; \
ZERO(name);

#define CLAMP(val, vmin, vmax) min(max(val, vmin), vmax)

#define CRASH() \
*(reinterpret_cast<int32_t*>(0)) = 0xF0BA2;

#ifdef _DEBUG

#define ASSERT_D(expression, message) \
if(!(expression)) { MessageBox(System::GetInstance()->GetHWND(), message, L"Assertion error!", MB_ICONERROR | MB_OK); CRASH(); } 

#define ASSERT(expression) ASSERT_D(expression, L"Assertion error!")

#define ASSERT_X(func) ASSERT(func == S_OK)

#define DEBUG(text) \
std::wstring d_text(text + L"\n"); \
OutputDebugString(d_text.c_str());

#define ZERO_ON_DEBUG(arg) ZERO(arg)

#else

#define ASSERT_D(expression, message)

#define ASSERT(expression)

#define ASSERT_X(func) func

#define DEBUG(text)

#define ZERO_ON_DEBUG(arg)

#endif
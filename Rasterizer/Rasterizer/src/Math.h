#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>

#define piDiv180 ((float)M_PI / 180.0f)
#define n180DivPi (180.0f / (float)M_PI)

#define Clamp(x, a, b) min(max((x),(a)), (b))
#define FloatLerp(x, y, s) (x)*(1.0f - (s)) + (y)*(s)
#define Float2Dot(ax, ay, bx, by) ((ax)*(bx) + (ay)*(by))
#define DegToRad(x) (x * piDiv180)
#define RadToDeg(x) (x * n180DivPi)
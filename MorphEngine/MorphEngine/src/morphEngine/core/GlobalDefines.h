#pragma once

#pragma region SystemDefinitions

#define PLATFORM_WINDOWS

#pragma endregion

//**********************************************

#ifdef PLATFORM_WINDOWS

#pragma region Types

typedef __int8 MInt8;
typedef __int16 MInt16;
typedef __int32 MInt32;
typedef __int64 MInt64;
typedef unsigned __int8 MUint8;
typedef unsigned __int16 MUint16;
typedef unsigned __int32 MUint32;
typedef unsigned __int64 MUint64;
typedef float MFloat32;
typedef double MDouble64;
typedef long double MLongDouble64;
typedef MUint64 MUptr;
typedef size_t MSize;

#pragma endregion

#pragma region externalHeaders

#include <Windows.h>

#pragma endregion

#pragma region Platform dependent defines

#ifdef _DEBUG

#define ME_DEPRECATED_MSG(msg) __declspec(deprecated(msg))
#define ME_DEPRECATED() ME_DEPRECATED_MSG("This is deprecated")

#else

#define ME_DEPRECATED_MSG(msg) //Do nothing
#define ME_DEPRECATED() //Do nothing

#endif
#pragma endregion

#endif

#pragma region Platform independent defines

#define USE_SIMD 0

#define STRING(arg) #arg

#define MarkMemoryUsable(DST, SIZ) memset((DST), 0xCC, (SIZ));

#define EnumClassOperations(EnumType) \
constexpr EnumType operator &(const EnumType selfValue, const EnumType inValue) \
{ \
	return (EnumType) (MUint32(selfValue) & MUint32(inValue)); \
} \
constexpr EnumType operator |(const EnumType selfValue, const EnumType inValue) \
{ \
	return (EnumType) ((MUint32)selfValue | (MUint32)inValue); \
}

#pragma endregion

#pragma region Global Functions

#pragma region Helpers

#define Kilobytes(howMuch) 1024L * howMuch
#define Megabytes(howMuch) 1024L * Kilobytes(howMuch)
#define Gigabytes(howMuch) 1024L * Megabytes(howMuch)
#define Terrabytes(howMuch) 1024L * Gigabytes(howMuch)

#pragma endregion

#pragma endregion

#pragma region internalHeaders

#include "debugging/GlobalDefines.h"

// to wszystko wypierdala budowanie w kosmos bo robi siê milion circular dependencies

/*
#include "../utility/MArray.h"
#include "../utility/MDictionary.h"
#include "../utility/MFixedArray.h"
#include "../utility/MLinkedList.h"
#include "../utility/MMath.h"
#include "../utility/MMatrix.h"
#include "../utility/MPriorityQueue.h"
#include "../utility/MPriorityQueueAsc.h"
#include "../utility/MQuaternion.h"
#include "../utility/MQueue.h"
#include "../utility/MStack.h"
#include "../utility/MString.h"
#include "../utility/MVector.h"
using namespace morphEngine::utility;
*/

/*
#include "../reflection/Type.h"
template <typename T> morphEngine::memoryManagement::Handle<T> NewObject(const morphEngine::gom::ObjectInitializer& initializer)
{
	return T::Type.CreateInstance(initializer);
}
*/

#pragma endregion
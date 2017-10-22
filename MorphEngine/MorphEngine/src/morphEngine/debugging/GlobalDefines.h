#pragma once

#include "core/GlobalDefines.h"
#include <iostream>
#include <conio.h>

#ifdef PLATFORM_WINDOWS

#ifdef _DEBUG

#define ME_ASSERT(cond, msg) \
if(!(cond)) \
{ \
	/*MessageBox(msg)*/ \
	std::cout << "Assertion Error: " << (msg) << std::endl << "Press key to quit application..." << std::endl; \
	*(reinterpret_cast<MUint8*>(0)) = 4; \
	exit(-1); \
}

#define ME_ASSERT_S(cond) ME_ASSERT(cond, "Assertion error!")

#define ME_WARNING(cond, msg) \
if(!(cond)) \
{ \
	/*MessageBox(msg)*/ \
	std::cout << "Assertion Warning: " << (msg) << std::endl; \
}

#define ME_WARNING_S(cond) ME_WARNING(cond, "Assertion warning!")

#define ME_WARNING_RETURN_STATEMENT(cond, msg)\
if(!(cond)) \
{ \
	/*MessageBox(msg)*/ \
	std::cout << "Assertion Warning: " << (msg) << std::endl; \
	return; \
}

#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#else //if NDEBUG

#define ME_ASSERT(cond, msg) \
if(!(cond)) \
{ \
	exit(-1); \
}

#define ME_ASSERT_S(cond)

#define ME_WARNING(cond, msg) //do nothing;

#define ME_WARNING_S(cond)

#define ME_WARNING_RETURN_STATEMENT(cond, msg) //do nothing

#endif // _DEBUG

#endif // PLATFORM_WINDOWS
#include "stdafx.h"
#include "Timer.h"


Timer::Timer()
{
}


Timer::~Timer()
{
}

void Timer::Initialize()
{
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));

	_startTime = GetNowTime();
	_totalTime = 0.0f;
	_prevTotalTime = 0.0f;
	_deltaTime = 0.0f;
}

void Timer::Run()
{
	float currentTime = GetNowTime();
	_prevTotalTime = _totalTime;
	_totalTime = currentTime - _startTime;
	_deltaTime = _totalTime - _prevTotalTime;

	//DEBUG(L"TotalTime: " + to_wstring(_totalTime) + L" | DeltaTime: " + to_wstring(_deltaTime));
}

void Timer::Shutdown()
{
}

double Timer::GetNowTimeDouble() const
{
	LARGE_INTEGER counts;
	bool bb = QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&counts));
	ASSERT(bb);

	return static_cast<double>(counts.QuadPart) / static_cast<double>(_frequency);
}

float Timer::GetNowTime() const
{
	LARGE_INTEGER counts;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&counts));

	return static_cast<float>(static_cast<double>(counts.QuadPart) / static_cast<double>(_frequency));
}

int64_t Timer::GetNowTicks() const
{
	LARGE_INTEGER counts;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&counts));
	return counts.QuadPart;
}

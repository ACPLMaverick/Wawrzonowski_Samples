#pragma once

#include "stdafx.h"
#include "Singleton.h"

class Timer : public Singleton<Timer>
{
protected:

#pragma region Protected

	int64_t _frequency = 0;
	float _startTime = 0.0f;
	float _totalTime = 0.0f;
	float _prevTotalTime = 0.0f;
	float _deltaTime = 0.0f;

#pragma endregion

#pragma region Functions Protected

#pragma endregion

public:

#pragma region Functions Public

	Timer();
	~Timer();

	void Initialize();
	void Run();
	void Shutdown();

	double GetNowTimeDouble() const;
	float GetNowTime() const;
	int64_t GetNowTicks() const;
	inline int64_t GetFrequency() const { return _frequency; }
	inline float GetTotalTime() const { return _totalTime; }
	inline float GetDeltaTime() const { return _deltaTime; }
	inline float GetFPS() const { return 1.0f / _deltaTime; }

#pragma endregion
};


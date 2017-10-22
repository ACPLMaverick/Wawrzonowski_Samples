#pragma once

/*
	Handles time management. Time is given in SECONDS (as it is standard SI unit).
*/

#include "Singleton.h"

class Timer :
	public Singleton<Timer>
{
	friend class Singleton<Timer>;
protected:

	double _freq = 1.0;

	double _startTime = 0.0;
	double _currentTime = 0.0;
	double _deltaTime = 0.0;
	double _fps = 0.0;
	double _fpsAcum = 0.0;
	double _frameCount = 0.0;

	Timer();

	virtual inline double GetCurrentTimeS();
public:

	~Timer();

	virtual void Initialize();
	virtual void Shutdown();
	virtual void Update();

#pragma region Accessors

	double GetActualTime() { return _currentTime; }
	double GetDeltaTime() { return _deltaTime; }
	double GetFPS() { return _fps; }
	double GetFPSAverage() { return _fpsAcum / _frameCount; }

#pragma endregion

};


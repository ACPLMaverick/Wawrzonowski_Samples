#pragma once

#include "GlobalDefines.h"
#include <vector>

namespace Scenes
{
	class Scene;
}

class Timer;
class Text;

class AveragedTime
{
public:

	typedef float (Timer::*UpdateFuncPtr)() const;

protected:

	Timer* _timer;
	UpdateFuncPtr _funcPtr;
	float _averaged;
	float _averagePeriod;
	float _averageTemp;
	float _averageAccumulator;
	float _averageDivisor;

	inline void UpdateInternal(float dt);

public:

	AveragedTime(Timer* timer, UpdateFuncPtr funcPtr, float averagePeriod = 0.5f);
	~AveragedTime();

	void Update();
	void UpdatePeriodBegin();
	void UpdatePeriodEnd();

	inline float GetAveragedValue() { return _averaged; }
};

class Profiler
{
protected:

	static const size_t TEXT_COUNT = 8;

	AveragedTime _baseTime;
	AveragedTime _postprocessTime;

	float _totalAverageAccumulator = 0.0f;
	float _totalAverageDivisor = 0.0f;

	bool _bHasAveragedFPS = false;

	std::vector<std::string> _postprocessNames;

	Scenes::Scene* _scene;

	Text* _allTexts[TEXT_COUNT];
	Text* _tLblFPS = nullptr;
	Text* _tValFPS = nullptr;
	Text* _tLblMsGlobal = nullptr;
	Text* _tValMsGlobal = nullptr;
	Text* _tLblMsPostprocess = nullptr;
	Text* _tValMsPostprocess = nullptr;
	Text* _tLblPostprocessName = nullptr;
	Text* _tValPostprocessName = nullptr;

public:
	Profiler(Scenes::Scene* scene);
	~Profiler();

	void Initialize();
	void Update();
	void UpdatePostprocessBegin();
	void UpdatePostprocessEnd();
	void Shutdown();

	void RegisterPostprocessName(const std::string& name);
	void SwitchPostprocessName(size_t index);
};


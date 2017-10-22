#pragma once

#include "GlobalDefines.h"

namespace Scenes
{
	class Scene;
}

namespace Postprocesses
{
	class Postprocess;
}

class Profiler;

class Controller
{
protected:

	Profiler* _profiler;

	Scenes::Scene* _scene;
	size_t _postprocessCounter;

	float _cameraSpeed;
	float _cameraBoost;
	float _cameraRotateSpeedX;
	float _cameraRotateSpeedY;

	bool _bRotateLight;

	inline void SwitchDirectionalLight();
	inline void PerformOneLightRotation();

public:
	Controller(Scenes::Scene* scene);
	~Controller();

	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	inline Profiler* GetProfiler() { return _profiler; }
};


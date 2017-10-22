#pragma once

#include "GlobalDefines.h"
#include <random>

class Random
{
protected:

	std::random_device _device;
	std::default_random_engine _engine;
	std::uniform_real_distribution<float> _dist;

public:
	Random();
	~Random();

	int32_t GetInt(int32_t min, int32_t max);
	float GetFloat(float min, float max);
};


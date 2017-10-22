#include "stdafx.h"
#include "Random.h"

Random::Random() : 
	_engine(_device()),
	_dist(0.0f, 1.0f)
{
}


Random::~Random()
{
}

int32_t Random::GetInt(int32_t min, int32_t max)
{
	return min + _device() % (max - min);
}

float Random::GetFloat(float min, float max)
{
	return min + _dist(_engine) * (max - min);
}

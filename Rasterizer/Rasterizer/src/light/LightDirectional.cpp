#include "LightDirectional.h"


namespace light
{
	LightDirectional::LightDirectional() :
		LightAmbient(),
		_direction(0.0f, -1.0f, 0.0f)
	{
	}

	LightDirectional::LightDirectional(const Color32 * col, const math::Float3 * direction) :
		LightAmbient(col),
		_direction(*direction)
	{
		math::Float3::Normalize(_direction);
	}


	LightDirectional::~LightDirectional()
	{
	}

}
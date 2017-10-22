#include "LightPoint.h"


namespace light
{
	LightPoint::LightPoint() : 
		LightAmbient(),
		_position(0.0f, 0.0f, 0.0f),
		_attenuationConstant(0.0f),
		_attenuationLinear(0.25f),
		_attenuationQuadratic(0.25f)
	{
	}

	LightPoint::LightPoint(const Color32 * col, const math::Float3 * position, float attConstant, float attLinear, float attQuadratic) :
		LightAmbient(col),
		_position(*position),
		_attenuationConstant(attConstant),
		_attenuationLinear(attLinear),
		_attenuationQuadratic(attQuadratic)
	{
	}


	LightPoint::~LightPoint()
	{
	}
}
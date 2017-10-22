#include "stdafx.h"
#include "LightAmbient.h"

namespace Lights
{
	LightAmbient::LightAmbient(const XMFLOAT4A color) :
		_color(color)
	{
	}

	LightAmbient::~LightAmbient()
	{
	}

	void LightAmbient::ApplyToShader(const LightAmbient & other, const Camera & camera)
	{
		_color = other._color;
	}
}
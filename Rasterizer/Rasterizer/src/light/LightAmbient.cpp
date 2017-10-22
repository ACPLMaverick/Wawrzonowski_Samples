#include "LightAmbient.h"


namespace light
{
	LightAmbient::LightAmbient() : 
		_color(0xFF000000)
	{
	}

	LightAmbient::LightAmbient(const Color32 * color)
	{
		_color = *color;
	}

	LightAmbient::~LightAmbient()
	{
	}

}
#include "LightAmbient.h"


namespace morphEngine
{
	namespace renderer
	{
		LightAmbient::LightAmbient(const gom::ObjectInitializer& initializer) :
			Light(initializer)
		{
			RegisterProperties();
		}

		LightAmbient::LightAmbient(const LightAmbient & copy, bool bDeepCopy) :
			Light(copy, bDeepCopy),
			_color(copy._color)
		{
			RegisterProperties();
		}

		LightAmbient::~LightAmbient()
		{
		}

		void LightAmbient::RegisterProperties()
		{
			RegisterProperty("Color,", &_color);
		}
	}
}
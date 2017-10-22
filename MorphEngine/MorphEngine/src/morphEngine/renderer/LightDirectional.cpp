#include "LightDirectional.h"


namespace morphEngine
{
	namespace renderer
	{
		LightDirectional::LightDirectional(const gom::ObjectInitializer& initializer) :
			Light(initializer)
		{
			RegisterProperties();
		}

		LightDirectional::LightDirectional(const LightDirectional & copy, bool bDeepCopy) :
			Light(copy, bDeepCopy),
			_color(copy._color)
		{
			RegisterProperties();
		}


		void LightDirectional::RegisterProperties()
		{
			RegisterProperty("Color", &_color);
		}

		LightDirectional::~LightDirectional()
		{
		}

	}
}
#include "Light.h"

namespace morphEngine
{
	namespace renderer
	{
		Light::Light(const gom::ObjectInitializer& initializer) :
			renderer::OctTreeElement(initializer)
		{
		}


		Light::Light(const Light & copy, bool bDeepCopy) :
			renderer::OctTreeElement(copy, bDeepCopy)
		{
		}

		void Light::UpdateBounds()
		{
			SetForcePlaceAtRoot(true);
		}

		void Light::Initialize()
		{
			OctTreeElement::Initialize();
			UpdateBounds();
		}

		Light::~Light()
		{
		}

	}
}
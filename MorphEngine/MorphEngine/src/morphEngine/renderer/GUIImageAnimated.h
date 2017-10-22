#pragma once

#include "renderer/GUIImage.h"

namespace morphEngine
{
	namespace renderer
	{

		class GUIImageAnimated :
			public GUIImage
		{
		protected:
			GUIImageAnimated(const gom::ObjectInitializer& initializer);
		public:
			~GUIImageAnimated();
		};

	}
}
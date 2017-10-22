#pragma once

#include "renderer\Renderer.h"

namespace morphEngine
{
	namespace renderer
	{

		class RendererTrail :
			public Renderer
		{
		protected:

			RendererTrail(const gom::ObjectInitializer& initializer);

		public:
			~RendererTrail();
		};

	}
}
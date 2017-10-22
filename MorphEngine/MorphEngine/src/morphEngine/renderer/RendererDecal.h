#pragma once

#include "renderer\Renderer.h"

namespace morphEngine
{
	namespace renderer
	{

		class RendererDecal :
			public Renderer
		{
		protected:

			RendererDecal(const gom::ObjectInitializer& initializer);

		public:
			~RendererDecal();
		};

	}
}
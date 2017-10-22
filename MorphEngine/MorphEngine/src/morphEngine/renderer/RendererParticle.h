#pragma once

#include "renderer\Renderer.h"

namespace morphEngine
{
	namespace renderer
	{

		class RendererParticle :
			public Renderer
		{
		protected:
			RendererParticle(const gom::ObjectInitializer& initializer);
		public:
			~RendererParticle();
		};

	}
}
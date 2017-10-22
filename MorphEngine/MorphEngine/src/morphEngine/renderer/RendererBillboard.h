#pragma once

#include "renderer/Renderer.h"

namespace morphEngine
{
	namespace renderer
	{

		class RendererBillboard :
			public Renderer
		{
		protected:

			RendererBillboard(const gom::ObjectInitializer& initializer);

		public:
			~RendererBillboard();
		};

	}
}
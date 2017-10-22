#pragma once

#include "renderer/RendererBillboard.h"

namespace morphEngine
{
	namespace renderer
	{

		class RendererFlare :
			public RendererBillboard
		{
		protected:
			RendererFlare(const gom::ObjectInitializer& initializer);
		public:
			~RendererFlare();
		};

	}
}
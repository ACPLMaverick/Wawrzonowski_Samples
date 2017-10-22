#pragma once

#include "gom/GameObject.h"

namespace morphEngine
{
	namespace renderer
	{

		class DebugDrawObject : public gom::GameObject
		{
		protected:
			DebugDrawObject(const gom::ObjectInitializer& initializer);
		public:
			~DebugDrawObject();
		};

	}
}
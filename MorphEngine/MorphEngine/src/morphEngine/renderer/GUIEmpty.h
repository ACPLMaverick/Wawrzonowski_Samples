#pragma once
#include "renderer/GUIImage.h"

namespace morphEngine
{
	namespace renderer
	{
		class GUIEmpty :
			public GUIImage
		{
			ME_TYPE
		protected:
			GUIEmpty(const gom::ObjectInitializer& initializer) : GUIImage(initializer) { }

			inline virtual void AssignDefaultMesh() override { }
			inline virtual void AssignDefaultMaterial() override { }
		public:

			GUIEmpty(const GUIEmpty& c, bool bDeepCopy) : GUIImage(c, bDeepCopy) { }
			inline virtual ~GUIEmpty() { }

			virtual inline void Draw() const override { }
		};
	}
}
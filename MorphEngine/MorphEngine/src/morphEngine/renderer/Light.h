#pragma once

#include "renderer/OctTree.h"
#include "gom/Transform.h"

namespace morphEngine
{
	namespace gom
	{
		class Camera;
	}

	namespace renderer
	{
		class Renderer;

		/// <summary>
		/// Abstract class.
		/// </summary>
		class Light :
			public renderer::OctTreeElement
		{
			ME_TYPE

		protected:

#pragma region Functions Protected

			Light(const gom::ObjectInitializer& initializer);
			Light(const Light& copy, bool bDeepCopy = true);

			inline Light& operator=(const Light& copy)
			{
				renderer::OctTreeElement::operator=(copy);
				return *this;
			}

			virtual void UpdateBounds();

#pragma endregion
		public:

#pragma region Functions Public

			virtual void Initialize() override;

			virtual ~Light();

#pragma endregion
		};

	}
}
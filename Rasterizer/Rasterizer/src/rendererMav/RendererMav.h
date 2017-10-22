#pragma once

#include "../IRenderer.h"
#include "../SpecificObjectFactory.h"
#include "GraphicsDevice.h"

namespace rendererMav
{
	class RendererMav :
		public IRenderer
	{
		friend class SpecificObjectFactory;
	protected:

#pragma region Functions Protected

		GraphicsDevice _device;

		RendererMav(SystemSettings* settings);

#pragma endregion

	public:

#pragma region Functions Public

		~RendererMav();

		virtual void Draw(Scene* scene);
		virtual Buffer<float>* GetDepthBuffer();
		virtual GraphicsDevice* GetGraphicsDevice();

#pragma endregion
	};

}

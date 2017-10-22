#pragma once
#include "ShaderFullscreen.h"
#include "renderer/LightAmbient.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			struct ShaderDeferredLightAmbientCustomData : public ShaderLocalData
			{
				const LightAmbient* Light;
			};

			class ShaderDeferredLightAmbient :
				public ShaderFullscreen
			{
			public:

				static const char* TYPE_NAME;

			protected:

				__declspec(align(16))
				struct LightAmbientData
				{
					utility::MColor Color;
				};

				device::ConstantBufferResource* _cAmbient;

				virtual inline void GetPixelShaderName(utility::MString& name) override
				{
					name = "DeferredLightAmbient";
				}

			public:
				ShaderDeferredLightAmbient();
				virtual ~ShaderDeferredLightAmbient();

				virtual void Initialize() override;
				virtual void Shutdown() override;

				virtual void SetLocal(const ShaderLocalData& customData) const override;
			};

		}
	}
}
#pragma once
#include "ShaderFullscreen.h"
#include "renderer/LightDirectional.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			struct ShaderDeferredLightDirectionalCustomData : public ShaderLocalData
			{
				const LightDirectional* Light;
			};

			class ShaderDeferredLightDirectional :
				public ShaderFullscreen
			{
			public:

				static const char* TYPE_NAME;

			protected:

				__declspec(align(16))
					struct LightDirectionalData
				{
					utility::MColor Color;
					utility::MVector3 Direction;
				};

				device::ConstantBufferResource* _cDirectional;

				virtual inline void GetPixelShaderName(utility::MString& name) override
				{
					name = "DeferredLightDirectional";
				}

			public:
				ShaderDeferredLightDirectional();
				virtual ~ShaderDeferredLightDirectional();

				virtual void Initialize() override;
				virtual void Shutdown() override;

				virtual void SetLocal(const ShaderLocalData& customData) const override;
			};

		}
	}
}
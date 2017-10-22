#pragma once
#include "ShaderFullscreen.h"
#include "renderer/LightSpot.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			struct ShaderDeferredLightSpotCustomData : ShaderLocalData
			{
				const LightSpot* Light;
			};

			class ShaderDeferredLightSpot :
				public ShaderFullscreen
			{
			public:

				static const char* TYPE_NAME;

			protected:

				__declspec(align(16))
					struct LightSpotData
				{
					utility::MColor Color;
					utility::MVector3 Position;
					MFloat32 pd1;
					utility::MVector3 Direction;
					MFloat32 pd2;
					utility::MVector3 RangeAngleSmooth;
				};

				device::ConstantBufferResource* _cSpot;

				virtual inline void GetPixelShaderName(utility::MString& name) override
				{
					name = "DeferredLightSpot";
				}

			public:
				ShaderDeferredLightSpot();
				~ShaderDeferredLightSpot();

				virtual void Initialize() override;
				virtual void Shutdown() override;

				virtual void SetLocal(const ShaderLocalData& customData) const override;
			};
		}
	}
}

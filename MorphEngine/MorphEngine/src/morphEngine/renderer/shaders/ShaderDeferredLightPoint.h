#pragma once
#include "ShaderFullscreen.h"
#include "renderer/LightPoint.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			struct ShaderDeferredLightPointCustomData : ShaderLocalData
			{
				const LightPoint* Light;
			};

			class ShaderDeferredLightPoint :
				public ShaderFullscreen
			{
			public:

				static const char* TYPE_NAME;

			protected:

				__declspec(align(16))
					struct LightPointData
				{
					utility::MColor Color;
					utility::MVector3 Position;
					MFloat32 Range;
				};

				device::ConstantBufferResource* _cPoint;

				virtual inline void GetPixelShaderName(utility::MString& name) override
				{
					name = "DeferredLightPoint";
				}

			public:

				ShaderDeferredLightPoint();
				~ShaderDeferredLightPoint();

				virtual void Initialize() override;
				virtual void Shutdown() override;

				virtual void SetLocal(const ShaderLocalData& customData) const override;
			};
		}
	}
}
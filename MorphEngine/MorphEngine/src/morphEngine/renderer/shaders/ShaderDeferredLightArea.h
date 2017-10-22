#pragma once
#include "ShaderFullscreen.h"
#include "renderer/LightArea.h"
namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			struct ShaderDeferredLightAreaCustomData : ShaderLocalData
			{
				const LightArea* Light;
			};

			class ShaderDeferredLightArea :
				public ShaderFullscreen
			{
			public:

				static const char* TYPE_NAME;

			protected:

				__declspec(align(16))
					struct LightAreaData
				{
					utility::MColor Color;
					utility::MVector3 Position;
					MFloat32 pd1;
					utility::MVector3 Direction;
					MFloat32 pd2;
					utility::MVector3 Right;
					MFloat32 pd3;
					utility::MVector4 SizeRangeSmooth;
				};

				device::ConstantBufferResource* _cArea;

				virtual inline void GetPixelShaderName(utility::MString& name) override
				{
					name = "DeferredLightArea";
				}

			public:

				ShaderDeferredLightArea();
				~ShaderDeferredLightArea();

				virtual void Initialize() override;
				virtual void Shutdown() override;

				virtual void SetLocal(const ShaderLocalData& customData) const override;
			};

		}
	}
}
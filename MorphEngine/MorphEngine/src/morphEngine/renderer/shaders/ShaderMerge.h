#pragma once
#include "renderer/shaders/ShaderFullscreen.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MTexture2D;
	}
	namespace renderer
	{
		namespace shaders
		{
			struct ShaderMergeData : public ShaderLocalData
			{
				const assetLibrary::MTexture2D* TextureA;
				const assetLibrary::MTexture2D* TextureB;
			};

			class ShaderMerge :
				public ShaderFullscreen
			{
			public:

				static const char* TYPE_NAME;

			protected:

				virtual inline void GetPixelShaderName(utility::MString& name) override
				{
					name = "Merge";
				}

			public:
				ShaderMerge();
				virtual ~ShaderMerge();

				virtual void SetLocal(const ShaderLocalData& customData) const override;
			};

		}
	}
}
#pragma once

#include "renderer/shaders/ShaderObject.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			class ShaderBlank :
				public ShaderObject
			{
			public:

				static const char* TYPE_NAME;

			protected:

				device::PixelShaderResource* _ps;

			public:
				ShaderBlank();
				~ShaderBlank();

				// Inherited via ShaderObject
				virtual void Initialize() override;
				virtual void Shutdown() override;
				virtual void Set(MUint8 passIndex = 0) const override;
				virtual void Set(const ShaderGlobalData & globalData, const ShaderLocalData & localData, MUint8 passIndex = 0) const override;
				virtual void SetGlobal(const ShaderGlobalData & globalData, MUint8 passIndex = 0) const override;
				virtual void SetupMaterialInterface(assetLibrary::MMaterial & mat) override;
				virtual void SetLocal(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh) const override;
				virtual void SetLocalDeferred(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh) const override;
			};

		}
	}
}
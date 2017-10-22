#pragma once

#include "ShaderObject.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			class ShaderGUI :
				public ShaderObject
			{
			public:

				static const char* TYPE_NAME;

			protected:


#pragma region Structs Protected

				__declspec(align(16))
					struct BufferDataVS
				{
					utility::MMatrix _guiMatrix;
				};

				__declspec(align(16))
					struct BufferDataPS
				{
					utility::MColor _baseColor;
					utility::MColor _baseColor1;
					utility::MColor _baseColor2;
					utility::MColor _baseColor3;
					MUint64 _indices;
					MFloat32 _lerp;
				};

#pragma endregion

#pragma region Protected

				device::PixelShaderResource* _ps;
				device::ConstantBufferResource* _cBufferPS;

#pragma endregion

				virtual void VertexShaderSetup() override;
				inline virtual void SkinSetup(const assetLibrary::MMesh& mesh, const animation::AnimationFrame& frame) const override { }

			public:
				ShaderGUI() { }
				virtual ~ShaderGUI() { }

				// Inherited via Shader
				virtual void Initialize() override;
				virtual void Shutdown() override;
				virtual void SetupMaterialInterface(assetLibrary::MMaterial & mat) override;
				virtual void SetGlobal(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override;
				virtual void SetGlobalDeferred(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override;
				virtual void SetGlobalSkinned(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override;
				virtual void SetGlobalSkinnedDeferred(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override;
				virtual void SetLocal(const assetLibrary::MMaterial& mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh& mesh) const override;


				// Inherited via ShaderObject
				virtual void Set(MUint8 passIndex = 0) const override;
				virtual void Set(const ShaderGlobalData & globalData, const ShaderLocalData & localData, MUint8 passIndex = 0) const override;

			};
		}
	}
}
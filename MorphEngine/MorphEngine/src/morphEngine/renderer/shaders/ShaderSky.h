#pragma once
#include "ShaderObject.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			class ShaderSky :
				public ShaderObject
			{
			protected:
#pragma region Structs Protected

				__declspec(align(16))
				struct BufferDataVS
				{
					utility::MMatrix _matWVP;
				};

				__declspec(align(16))
				struct BufferDataPS
				{
					utility::MColor _color;
					MFloat32 _contrast;
				};

#pragma endregion

#pragma region Protected

				device::PixelShaderResource* _ps = nullptr;
				device::ConstantBufferResource* _cBufferPS = nullptr;

#pragma endregion

#pragma region Functions Protected

				virtual void VertexShaderSetup();

#pragma endregion

			public:

#pragma region Functions Public

				ShaderSky();
				virtual ~ShaderSky();

				virtual void SetupMaterialInterface(assetLibrary::MMaterial& mat);

				virtual void Initialize() override;
				virtual void Shutdown() override;

				// Sets shader resources and global (per-frame) parameters.
				virtual void SetGlobal(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override;

				virtual void UnsetGlobal() const override;

				virtual void SetGlobalSkinned(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const;

				virtual void SetMaterialWise(const assetLibrary::MMaterial& mat) const;

				virtual void SetLocal(const assetLibrary::MMaterial& mat, const gom::Camera& camera, const gom::Transform& transform, const assetLibrary::MMesh& mesh) const;
				virtual void SetLocalSkinned(const assetLibrary::MMaterial& mat, const gom::Camera& camera, const gom::Transform& transform, const assetLibrary::MMesh& mesh, const animation::AnimationFrame& frame) const;

#pragma endregion

				// Inherited via ShaderObject
				virtual void Set(MUint8 passIndex = 0) const override;
				virtual void Set(const ShaderGlobalData & globalData, const ShaderLocalData & localData, MUint8 passIndex = 0) const override;
			};
		}
	}
}
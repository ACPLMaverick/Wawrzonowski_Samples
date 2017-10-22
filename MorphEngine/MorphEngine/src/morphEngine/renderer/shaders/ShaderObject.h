#pragma once

#include "renderer/shaders/Shader.h"

namespace morphEngine
{
	namespace animation
	{
		class AnimationFrame;
	}

	namespace renderer
	{
		namespace shaders
		{
			// This class is a base abstraction of shader in materials, used to render single or multiple meshes.
			class ShaderObject :
				public Shader
			{
			public:

				static const char* TYPE_NAME;

			protected:

				static const MUint32 BONES_PER_MESH = 96;

#pragma region Structs Protected

				__declspec(align(16))
					struct BufferDataVS
				{
					utility::MMatrix Wvp;
					utility::MMatrix W;
					utility::MMatrix WInvTrans;
				};

				__declspec(align(16))
					struct BufferSkinnedDataVS
				{
					utility::MMatrix SkinnedTransforms[BONES_PER_MESH];
				};

#pragma endregion

#pragma region Protected

				device::VertexShaderResource* _vs = nullptr;
				device::VertexShaderResource* _vsSkin = nullptr;
				device::ConstantBufferResource* _cBufferVS = nullptr;
				device::ConstantBufferResource* _cBufferSkinVS = nullptr;

#pragma endregion

#pragma region Functions Protected

				virtual void VertexShaderSetup();
				inline virtual void SkinSetup(const assetLibrary::MMesh& mesh, const animation::AnimationFrame& frame) const;

#pragma endregion


			public:

#pragma region Functions Public

				ShaderObject();
				virtual ~ShaderObject();

				virtual void Initialize();
				virtual void Shutdown();

				virtual void SetupMaterialInterface(assetLibrary::MMaterial& mat) = 0;

				// Sets shader resources and global (per-frame) parameters.
				virtual void SetGlobal(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override;
				virtual void SetGlobalDeferred(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const override { SetGlobal(globalData, passIndex); }

				virtual void UnsetGlobal() const override;

				virtual void SetGlobalSkinned(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const;
				// If shader does not support deferred rendering, this call is the same as SetGlobal.
				virtual void SetGlobalSkinnedDeferred(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const { SetGlobalSkinned(globalData, passIndex); }

				// Sets only parameters that can change material-wise and are not specific to mesh or single object.
				virtual void SetMaterialWise(const assetLibrary::MMaterial& mat) const;
				virtual void SetMaterialWiseDeferred(const assetLibrary::MMaterial& mat) const { SetMaterialWise(mat); }
				virtual void SetMaterialWiseSkinned(const assetLibrary::MMaterial& mat) const { SetMaterialWise(mat); }
				virtual void SetMaterialWiseSkinnedDeferred(const assetLibrary::MMaterial& mat) const { SetMaterialWiseDeferred(mat); }

				// Sets only parameters that can change mesh-wise.
				// Please use this function for rendering static geometry.
				virtual void SetLocal(const assetLibrary::MMaterial& mat, const gom::Camera& camera, const gom::Transform& transform, const assetLibrary::MMesh& mesh) const;

				// Sets only parameters that can change mesh-wise.
				// Please use this function for rendering static geometry.
				virtual void SetLocalDeferred(const assetLibrary::MMaterial& mat, const gom::Camera& camera, const gom::Transform& transform, const assetLibrary::MMesh& mesh) const 
				{ SetLocal(mat, camera, transform, mesh); }

				// Please use this function for rendering skinned geometry. 
				// Note that on shaders which don't support skinned geometry this function will ignore animation and skin.
				virtual void SetLocalSkinned(const assetLibrary::MMaterial& mat, const gom::Camera& camera, const gom::Transform& transform, const assetLibrary::MMesh& mesh, const animation::AnimationFrame& frame) const;

				// Please use this function for rendering skinned geometry. 
				// Note that on shaders which don't support skinned geometry this function will ignore animation and skin.
				virtual void SetLocalSkinnedDeferred(const assetLibrary::MMaterial& mat, const gom::Camera& camera, const gom::Transform& transform, const assetLibrary::MMesh& mesh, const animation::AnimationFrame& frame) const;

#pragma endregion

			};
		}
	}
}
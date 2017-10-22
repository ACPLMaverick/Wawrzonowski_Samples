#pragma once

#include "renderer/RendererStaticMesh.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MSkeleton;
	}

	namespace animation
	{
		class Animator;
	}

	namespace renderer
	{

		class RendererSkinnedMesh :
			public RendererStaticMesh
		{
			ME_TYPE
		protected:

#pragma region Protected

			memoryManagement::Handle<animation::Animator> _animator;
			assetLibrary::MSkeleton* _skeleton;

#pragma endregion

#pragma region Functions Protected

			RendererSkinnedMesh(const gom::ObjectInitializer& initializer);

			virtual void RegisterProperties() override;

#pragma endregion

		public:

#pragma region Functions Public

			RendererSkinnedMesh(const RendererSkinnedMesh& c, bool bDeepCopy = true);
			~RendererSkinnedMesh();

			virtual void Initialize() override;

			inline assetLibrary::MSkeleton* GetSkeleton() const { return _skeleton; }
			inline memoryManagement::Handle<animation::Animator> GetAnimator() const { return _animator; }
			inline void SetSkeleton(assetLibrary::MSkeleton* skeleton) { _skeleton = skeleton; }
			inline void SetAnimator(memoryManagement::Handle<animation::Animator> animator) { _animator = animator; }

			virtual void SetMaterialShaderPass(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0, MUint8 passIndex = 0) const override;
			virtual void SetMaterialShaderDeferredPass(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0, MUint8 passIndex = 0) const override;

			virtual void SetMaterialData(MSize index = 0, MUint8 passIndex = 0) const override;
			virtual void SetMaterialDataDeferred(MSize index = 0, MUint8 passIndex = 0) const override;

			// Draws single submesh with its corresponding material.
			virtual void DrawSingle(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0) const override;
			virtual void DrawSingleDeferred(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0) const override;

			virtual void DrawSingleWithExternMaterial(const memoryManagement::Handle<gom::Camera> camera, const assetLibrary::MMaterial* mat, MSize index = 0) const override;
			virtual void DrawSingleDeferredWithExternMaterial(const memoryManagement::Handle<gom::Camera> camera, const assetLibrary::MMaterial* mat, MSize index = 0) const override;

#pragma endregion

		};

	}
}
#pragma once
#include "core/GlobalDefines.h"
#include "gom/GameObjectComponent.h"
#include "utility/MMatrix.h"

namespace morphEngine
{
	namespace gom
	{
		class Transform;
	}

	namespace renderer
	{
		class RendererSkinnedMesh;
	}

	namespace animation
	{

		class Animator;
		class Bone;

		class Socket
		{
		protected:

#pragma region Protected

			utility::MMatrix _finalMatrix = utility::MMatrix::Identity;
			utility::MMatrix _cachedSkeletonMatrix = utility::MMatrix::Identity;
			memoryManagement::Handle<gom::Transform> _transform;
			memoryManagement::Handle<renderer::RendererSkinnedMesh> _renderer;
			MSize _boneIndex = 0;

#pragma endregion

#pragma region Functions Protected

			inline void ObtainCachedMatrix();
			inline void CalculateFinalMatrix();

#pragma endregion

		public:

#pragma region Functions Public

			Socket();
			Socket(const Socket& c);
			~Socket();

			Socket& operator=(const Socket& c);

			void Initialize(memoryManagement::Handle<gom::Transform> transform,
				memoryManagement::Handle<renderer::RendererSkinnedMesh> renderer,
				MSize boneIndex);
			void Shutdown();

			void Update();

			inline const utility::MMatrix& GetMatrix() const { return _finalMatrix; }
			inline memoryManagement::Handle<gom::Transform> GetTransform() const { return _transform; }
			inline memoryManagement::Handle<renderer::RendererSkinnedMesh> GetRenderer() const { return _renderer; }
			inline MSize GetBoneIndex() const { return _boneIndex; }
			Bone* GetBone() const;
			inline bool GetIsInitialized() const { return _renderer.IsValid(); }

			void SetRenderer(memoryManagement::Handle<renderer::RendererSkinnedMesh> renderer);
			void SetBoneIndex(MSize index);
			void SetBone(Bone* bone);

#pragma endregion
		};

	}
}
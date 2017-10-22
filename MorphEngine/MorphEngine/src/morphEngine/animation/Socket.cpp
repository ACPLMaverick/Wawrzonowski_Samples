#include "Socket.h"
#include "gom/Transform.h"
#include "renderer/RendererSkinnedMesh.h"
#include "animation/Animator.h"

namespace morphEngine
{
	namespace animation
	{
		Socket::Socket()
		{
		}

		Socket::Socket(const Socket & c) :
			_finalMatrix(c._finalMatrix),
			_cachedSkeletonMatrix(c._cachedSkeletonMatrix),
			_transform(c._transform),
			_renderer(c._renderer),
			_boneIndex(c._boneIndex)
		{
		}


		Socket::~Socket()
		{
		}

		Socket & Socket::operator=(const Socket & c)
		{
			_finalMatrix = c._finalMatrix;
			_cachedSkeletonMatrix = c._cachedSkeletonMatrix;
			_transform = c._transform;
			_renderer = c._renderer;
			_boneIndex = c._boneIndex;
			return *this;
		}

		void Socket::Initialize(memoryManagement::Handle<gom::Transform> transform, memoryManagement::Handle<renderer::RendererSkinnedMesh> renderer, MSize boneIndex)
		{
			_transform = transform;
			_renderer = renderer;
			_boneIndex = boneIndex;

			ObtainCachedMatrix();
			CalculateFinalMatrix();
		}

		void Socket::Shutdown()
		{
			_transform.Invalidate();
			_renderer.Invalidate();
			_boneIndex = 0;
			_finalMatrix = MMatrix::Identity;
			_cachedSkeletonMatrix = MMatrix::Identity;
		}

		void Socket::Update()
		{
			CalculateFinalMatrix();
		}

		Bone * Socket::GetBone() const
		{
			return _renderer->GetSkeleton()->GetBone(_boneIndex);
		}

		void Socket::SetRenderer(memoryManagement::Handle<renderer::RendererSkinnedMesh> renderer)
		{
			_renderer = renderer;
			ObtainCachedMatrix();
			CalculateFinalMatrix();
		}

		void Socket::SetBoneIndex(MSize index)
		{
			_boneIndex = index;
			ObtainCachedMatrix();
			CalculateFinalMatrix();
		}

		void Socket::SetBone(Bone * bone)
		{
			SetBoneIndex(bone->GetFlatIndex());
		}

		inline void Socket::ObtainCachedMatrix()
		{
			if (!GetIsInitialized())
				return;

			// this is inverse of inverse bindpose, so multiplied by baked animation matrix should give proper result.
			_cachedSkeletonMatrix = _renderer->GetSkeleton()->GetBone(_boneIndex)->GetWorldTransform();
		}

		inline void Socket::CalculateFinalMatrix()
		{
			if (!GetIsInitialized())
				return;

			_finalMatrix = _renderer->GetAnimator()->GetCurrentFrame().GetData()[_boneIndex];
			_finalMatrix = _cachedSkeletonMatrix * _finalMatrix;
		}
	}
}
#include "RendererSkinnedMesh.h"
#include "animation/Animator.h"

namespace morphEngine
{
	namespace renderer
	{
		RendererSkinnedMesh::RendererSkinnedMesh(const gom::ObjectInitializer& initializer) :
			RendererStaticMesh(initializer)
		{
		}

		void RendererSkinnedMesh::RegisterProperties()
		{
			RegisterProperty("Skeleton", &_skeleton);
		}

		RendererSkinnedMesh::RendererSkinnedMesh(const RendererSkinnedMesh & c, bool bDeepCopy) : RendererStaticMesh(c, bDeepCopy), _skeleton(c._skeleton), _animator(c._animator)
		{
		}

		RendererSkinnedMesh::~RendererSkinnedMesh()
		{
		}

		void RendererSkinnedMesh::Initialize()
		{
			RendererStaticMesh::Initialize();

			//for safety
			MVector3 mn(_bounds.MinLocal);
			MVector3 mx(_bounds.MaxLocal);
			MFloat32 absX = MMath::Abs(mn.X - mx.X);
			MFloat32 absY = MMath::Abs(mn.Y - mx.Y);
			MFloat32 absZ = MMath::Abs(mn.Z - mx.Z);
			MFloat32 mnF = MMath::Min(MMath::Min(-absX, -absY), -absZ);
			MFloat32 mxF = MMath::Max(MMath::Max(absX, absY), absZ);
			MVector3 offset(0.0f, absY * 0.5f, 0.0f);
			_bounds.MinLocal = (MVector3(mnF, mnF, mnF) + offset) * 0.75f;
			_bounds.MaxLocal = (MVector3(mxF, mxF, mxF) + offset) * 0.75f;
				
			//_bounds.ExternalScale = 1.5f;

			_animator = _owner->GetComponent<animation::Animator>();
			ME_ASSERT(_animator.IsValid(), "An object with skinned mesh renderer has no animator!");
		}

		void RendererSkinnedMesh::SetMaterialShaderPass(const memoryManagement::Handle<gom::Camera> camera, MSize index, MUint8 passIndex) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize(), "RendererStaticMesh: Bad index.");
			_materials[index]->SetPassSkinned(*camera, passIndex);
		}

		void RendererSkinnedMesh::SetMaterialShaderDeferredPass(const memoryManagement::Handle<gom::Camera> camera, MSize index, MUint8 passIndex) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize(), "RendererStaticMesh: Bad index.");
			_materials[index]->SetPassSkinnedDeferred(*camera, passIndex);
		}

		void RendererSkinnedMesh::SetMaterialData(MSize index, MUint8 passIndex) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize(), "RendererStaticMesh: Bad index.");
			_materials[index]->SetDataSkinned(passIndex);
		}

		void RendererSkinnedMesh::SetMaterialDataDeferred(MSize index, MUint8 passIndex) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize(), "RendererStaticMesh: Bad index.");
			_materials[index]->SetDataSkinnedDeferred(passIndex);
		}

		void RendererSkinnedMesh::DrawSingle(const memoryManagement::Handle<gom::Camera> camera, MSize index) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize() && index < _mesh->GetSubmeshCount(), "RendererStaticMesh: Bad index.");
			Handle<morphEngine::gom::Transform> tr = _owner->GetTransform();
			_materials[index]->DrawMeshSkinned(*camera, *tr, *_mesh, _animator->GetCurrentFrame(), index);
		}

		void RendererSkinnedMesh::DrawSingleDeferred(const memoryManagement::Handle<gom::Camera> camera, MSize index) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize() && index < _mesh->GetSubmeshCount(), "RendererStaticMesh: Bad index.");
			Handle<morphEngine::gom::Transform> tr = _owner->GetTransform();
			_materials[index]->DrawMeshSkinnedDeferred(*camera, *tr, *_mesh, _animator->GetCurrentFrame(), index);
		}

		void RendererSkinnedMesh::DrawSingleWithExternMaterial(const memoryManagement::Handle<gom::Camera> camera, const assetLibrary::MMaterial * mat, MSize index) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize() && index < _mesh->GetSubmeshCount(), "RendererStaticMesh: Bad index.");
			Handle<morphEngine::gom::Transform> tr = _owner->GetTransform();
			mat->DrawMeshSkinned(*camera, *tr, *_mesh, _animator->GetCurrentFrame(), index);
		}

		void RendererSkinnedMesh::DrawSingleDeferredWithExternMaterial(const memoryManagement::Handle<gom::Camera> camera, const assetLibrary::MMaterial * mat, MSize index) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize() && index < _mesh->GetSubmeshCount(), "RendererStaticMesh: Bad index.");
			Handle<morphEngine::gom::Transform> tr = _owner->GetTransform();
			mat->DrawMeshSkinnedDeferred(*camera, *tr, *_mesh, _animator->GetCurrentFrame(), index);
		}

	}
}
#include "RendererStaticMesh.h"
#include "gom/Transform.h"

namespace morphEngine
{
	namespace renderer
	{
		RendererStaticMesh::RendererStaticMesh(const gom::ObjectInitializer& initializer) : 
			Renderer(initializer)
		{
			RegisterProperties();
		}

		RendererStaticMesh::RendererStaticMesh(const RendererStaticMesh& c, bool bDeepCopy) :
			Renderer(c, bDeepCopy),
			_mesh(c._mesh),
			_materials(c._materials)
		{
			RegisterProperties();
		}

		void RendererStaticMesh::RegisterProperties()
		{
			RegisterProperty("Mesh", &_mesh);
			RegisterProperty("Materials", &_materials);
		}


		RendererStaticMesh::~RendererStaticMesh()
		{
		}

		void RendererStaticMesh::Initialize()
		{
			Renderer::Initialize();

			if (_materials.GetSize() == 0 && _material != nullptr)
			{
				_materials.Add(const_cast<assetLibrary::MMaterial*>(_material));
			}
		}

		void RendererStaticMesh::Shutdown()
		{
			Renderer::Shutdown();
		}

		void RendererStaticMesh::SetMaterialShaderPass(const memoryManagement::Handle<gom::Camera> camera, MSize index, MUint8 passIndex) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize(), "RendererStaticMesh: Bad index.");
			_materials[index]->SetPass(*camera, passIndex);
		}

		void RendererStaticMesh::SetMaterialShaderDeferredPass(const memoryManagement::Handle<gom::Camera> camera, MSize index, MUint8 passIndex) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize(), "RendererStaticMesh: Bad index.");
			_materials[index]->SetPassDeferred(*camera, passIndex);
		}

		void RendererStaticMesh::UnsetMaterialShaderPass(MSize index, MUint8 passIndex) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize(), "RendererStaticMesh: Bad index.");
			_materials[index]->UnsetPass(passIndex);
		}

		void RendererStaticMesh::SetMaterialData(MSize index, MUint8 passIndex) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize(), "RendererStaticMesh: Bad index.");
			_materials[index]->SetData(passIndex);
		}

		void RendererStaticMesh::SetMaterialDataDeferred(MSize index, MUint8 passIndex) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize(), "RendererStaticMesh: Bad index.");
			_materials[index]->SetDataDeferred(passIndex);
		}

		void RendererStaticMesh::DrawSingle(const memoryManagement::Handle<gom::Camera> camera, MSize index) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize() && index < _mesh->GetSubmeshCount(), "RendererStaticMesh: Bad index.");
			Handle<morphEngine::gom::Transform> tr = _owner->GetTransform();
			_materials[index]->DrawMesh(*camera, *tr, *_mesh, index);
		}

		void RendererStaticMesh::DrawSingleDeferred(const memoryManagement::Handle<gom::Camera> camera, MSize index) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize() && index < _mesh->GetSubmeshCount(), "RendererStaticMesh: Bad index.");
			Handle<morphEngine::gom::Transform> tr = _owner->GetTransform();
			_materials[index]->DrawMeshDeferred(*camera, *tr, *_mesh, index);
		}

		void RendererStaticMesh::DrawSingleWithExternMaterial(const memoryManagement::Handle<gom::Camera> camera, const assetLibrary::MMaterial * mat, MSize index) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize() && index < _mesh->GetSubmeshCount(), "RendererStaticMesh: Bad index.");
			Handle<morphEngine::gom::Transform> tr = _owner->GetTransform();
			mat->DrawMesh(*camera, *tr, *_mesh, index);
		}

		void RendererStaticMesh::DrawSingleDeferredWithExternMaterial(const memoryManagement::Handle<gom::Camera> camera, const assetLibrary::MMaterial * mat, MSize index) const
		{
			ME_WARNING_RETURN_STATEMENT(index < _materials.GetSize() && index < _mesh->GetSubmeshCount(), "RendererStaticMesh: Bad index.");
			Handle<morphEngine::gom::Transform> tr = _owner->GetTransform();
			mat->DrawMeshDeferred(*camera, *tr, *_mesh, index);
		}

		void RendererStaticMesh::SetMesh(assetLibrary::MMesh * mesh)
		{
			if (_mesh == mesh)
				return;

			if (_mesh != nullptr)
			{
				_mesh->EvtVerticesChanged -= new core::Event<void, assetLibrary::MMesh*>::ClassDelegate<RendererStaticMesh>(_this, &RendererStaticMesh::UpdateBounds);
			}

			_mesh = mesh;
			_mesh->EvtVerticesChanged += new core::Event<void, assetLibrary::MMesh*>::ClassDelegate<RendererStaticMesh>(_this, &RendererStaticMesh::UpdateBounds);

			// first init
			UpdateBounds(_mesh);
		}

		void RendererStaticMesh::UpdateBounds(assetLibrary::MMesh * mesh)
		{
			mesh->UpdateBounds(_bounds); 
			UpdateBoundsWorld(_owner->GetTransform());
		}
	}
}
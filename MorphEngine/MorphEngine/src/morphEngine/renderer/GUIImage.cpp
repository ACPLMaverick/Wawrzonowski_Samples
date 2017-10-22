#include "GUIImage.h"
#include "gom/GUITransform.h"
#include "renderer/GUIBatch.h"
#include "assetLibrary/MMesh.h"
#include "assetLibrary/MMaterial.h"
#include "resourceManagement/ResourceManager.h"

namespace morphEngine
{
	using namespace assetLibrary;
	using namespace resourceManagement;
	using namespace core;
	using namespace gom;
	using namespace utility;

	namespace renderer
	{

		GUIImage::GUIImage(const gom::ObjectInitializer& initializer) :
			gom::GameObjectComponent(initializer),
			_drawMesh(nullptr),
			_drawMaterial(nullptr)
		{
			RegisterProperties();
		}


		GUIImage::GUIImage(const GUIImage & copy, bool bDeepCopy) : gom::GameObjectComponent(copy, bDeepCopy), _drawMesh(copy._drawMesh), _drawMaterial(copy._drawMaterial)
		{
			RegisterProperties();
		}

		GUIImage::~GUIImage()
		{
		}

		void GUIImage::Initialize()
		{
			if(_drawMesh == nullptr)
				AssignDefaultMesh();

			if(_drawMaterial == nullptr)
				AssignDefaultMaterial();

			_owner->SwitchToGUITransform();

			Handle<gom::GameObject> parentBatchObject = static_cast<Handle<GUITransform>>(_owner->GetTransform())->GetBatchGameObject();
			ME_WARNING_RETURN_STATEMENT(parentBatchObject.IsValid(), "GUIImage: Could not find parent batch.");
			_parentBatch = parentBatchObject->GetComponent<GUIBatch>();
			ME_WARNING_RETURN_STATEMENT(_parentBatch.IsValid(), "GUIImage: Could not find parent batch.");
			_parentBatch->RegisterGUIImage(static_cast<Handle<GUIImage>>(_this));
		}

		void GUIImage::Shutdown()
		{
			GameObjectComponent::Shutdown();

#if _DEBUG
			if (_parentBatch.IsValid())
			{
#endif // _DEBUG

				_parentBatch->DeregisterGUIImage(static_cast<Handle<GUIImage>>(_this));

#if _DEBUG
			}
#endif // _DEBUG

			if (_drawMaterial != nullptr && _drawMaterial->GetIsInstance())
			{
				ResourceManager::GetInstance()->DestroyMaterialInstance(_drawMaterial);
			}
		}

		void GUIImage::Draw() const
		{
			ME_WARNING_RETURN_STATEMENT(_drawMaterial != nullptr && _drawMesh != nullptr && _parentBatch.IsValid(), "Drawing a GUIImage without assigned mesh or material.");

			_drawMaterial->DrawMesh(*_parentBatch->GetRenderCamera(), *_owner->GetTransform(), *_drawMesh);
		}

		void GUIImage::SetMesh(assetLibrary::MMesh * mesh)
		{
			if (_drawMesh != nullptr && _drawMesh->GetIsInstance())
			{
				ResourceManager::GetInstance()->DestroyMeshInstance(_drawMesh);
			}
			_drawMesh = mesh;
		}

		void GUIImage::SetMaterial(assetLibrary::MMaterial * material)
		{
			if (_drawMaterial != nullptr && _drawMaterial->GetIsInstance())
			{
				ResourceManager::GetInstance()->DestroyMaterialInstance(_drawMaterial);
			}
			_drawMaterial = material;
		}

		void GUIImage::RegisterProperties()
		{
			//RegisterProperty("Mesh", &(_drawMesh->GetPath()));
			//RegisterProperty("Material", &(_drawMaterial->GetPath()));
			RegisterProperty("Mesh", &(_drawMesh));
			RegisterProperty("Material", &(_drawMaterial));
		}

		void GUIImage::AssignDefaultMesh()
		{
			if(_drawMesh == nullptr) _drawMesh = ResourceManager::GetInstance()->GetMesh("GUIMeshPlane");
		}

		void GUIImage::AssignDefaultMaterial()
		{
			if(_drawMaterial == nullptr) _drawMaterial = ResourceManager::GetInstance()->CreateMaterialInstance("DefaultGUIMaterial");
		}
	}
}
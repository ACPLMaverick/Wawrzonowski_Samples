#include "Skybox.h"
#include "resourceManagement/ResourceManager.h"
#include "gom/Camera.h"

namespace morphEngine
{
	using namespace resourceManagement;
	using namespace memoryManagement;
	using namespace gom;

	namespace renderer
	{
		void Skybox::Initialize()
		{
			GameObjectComponent::Initialize();

			if (!_camera.IsValid())
			{
				_camera = _owner->GetComponent<Camera>();
				ME_WARNING(_camera.IsValid(), "Added Skybox component but there is no camera associated.");
				if (_camera.IsValid())
				{
					_camera->SetSkybox(static_cast<Handle<Skybox>>(_this));
				}
			}

			if (_material == nullptr)
			{
				_material = GetDefaultMaterial();
			}

			if (_mesh == nullptr)
			{
				_mesh = GetDefaultMesh();
			}

			if (_reflectionTexture == nullptr)
			{
				_reflectionTexture = GenerateReflectionTexture();
			}
		}

		void Skybox::Shutdown()
		{
			if (_reflectionTexture != nullptr)
			{
				delete _reflectionTexture;
				_reflectionTexture = nullptr;
			}

			GameObjectComponent::Shutdown();
		}

		void Skybox::Draw(const memoryManagement::Handle<gom::Camera> camera)
		{
			_material->SetPass(*camera.GetPointer(), 0);
			_material->SetData(0);
			_material->DrawMesh(*camera.GetPointer(), *_owner->GetTransform(), *_mesh);
		}

		void Skybox::RegisterProperties()
		{
			RegisterProperty("Mesh", &_mesh);
			RegisterProperty("Material", &_material);
			RegisterProperty("ReflectionCubemapSize", &_reflectionTextureSize);
		}

		assetLibrary::MMaterial * Skybox::GetDefaultMaterial()
		{
			return ResourceManager::GetInstance()->GetMaterial("SkyboxDefaultMaterial");
		}

		assetLibrary::MMesh * Skybox::GetDefaultMesh()
		{
			return ResourceManager::GetInstance()->GetMesh("MeshBoxFlipped");
		}

		assetLibrary::MTextureCube * Skybox::GenerateReflectionTexture()
		{
			// TODO: implement
			return nullptr;
		}
	}
}
#pragma once
#include "gom/GameObjectComponent.h"

namespace morphEngine
{
	namespace renderer
	{
		class Skybox :
			public gom::GameObjectComponent
		{
			ME_TYPE
		protected:

#pragma region Protected

			assetLibrary::MMaterial* _material = nullptr;
			assetLibrary::MMesh* _mesh = nullptr;
			assetLibrary::MTextureCube* _reflectionTexture = nullptr;
			MUint16 _reflectionTextureSize = 256;

			memoryManagement::Handle<gom::Camera> _camera;

#pragma endregion

#pragma region Functions Protected

			Skybox(const gom::ObjectInitializer& initializer) : gom::GameObjectComponent(initializer) { RegisterProperties(); }

			virtual void RegisterProperties() override;
			virtual assetLibrary::MMaterial* GetDefaultMaterial();
			virtual assetLibrary::MMesh* GetDefaultMesh();

			assetLibrary::MTextureCube* GenerateReflectionTexture();

#pragma endregion

		public:
			Skybox(const Skybox& c, bool bDeepCopy = true) : gom::GameObjectComponent(c, bDeepCopy),
				_mesh(c._mesh), _reflectionTexture(c._reflectionTexture), _reflectionTextureSize(c._reflectionTextureSize),
				_camera(c._camera)
			{ 
				RegisterProperties(); 
			}

			virtual ~Skybox() { }

			virtual void Initialize() override;
			virtual void Shutdown() override;

			virtual void Draw(const memoryManagement::Handle<gom::Camera> camera);


			inline assetLibrary::MTextureCube* GetReflectionTexture() const { return _reflectionTexture; }
			inline MUint16 GetReflectionTextureSize() const { return _reflectionTextureSize; }
			inline memoryManagement::Handle<gom::Camera> GetCamera() const { return _camera; }
			virtual inline assetLibrary::MMaterial* GetMaterial(MSize index = 0) const { return _material; }

			virtual inline void SetMaterial(assetLibrary::MMaterial* mat) { _material = mat; }
			inline void SetCamera(memoryManagement::Handle<gom::Camera> cam) 
			{ 
				_camera = cam; 
			}

			inline void SetReflectionTextureSize(MUint16 size)
			{
				if (_reflectionTextureSize != size)
				{
					_reflectionTextureSize = size;



				}
			}
		};

	}
}
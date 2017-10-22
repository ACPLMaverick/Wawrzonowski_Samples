#pragma once

#include "renderer/Renderer.h"
#include "utility/MArray.h"
#include "../assetLibrary/MMesh.h"
#include "BoundsBox.h"

namespace morphEngine
{
	namespace renderer
	{

		class RendererStaticMesh :
			public Renderer
		{
			ME_TYPE
		protected:

#pragma region Protected

			assetLibrary::MMesh* _mesh = nullptr;
			utility::MArray<assetLibrary::MMaterial*> _materials;

#pragma endregion

#pragma region Functions Protected

			RendererStaticMesh(const gom::ObjectInitializer& initializer);

			virtual void RegisterProperties() override;

			void UpdateBounds(assetLibrary::MMesh* mesh);

#pragma endregion


		public:

#pragma region Functions Public

			RendererStaticMesh(const RendererStaticMesh& copy, bool bDeepCopy = true);
			~RendererStaticMesh();

			virtual void Initialize() override;
			virtual void Shutdown() override;

			virtual void SetMaterialShaderPass(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0, MUint8 passIndex = 0) const override;
			virtual void SetMaterialShaderDeferredPass(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0, MUint8 passIndex = 0) const override;

			virtual void UnsetMaterialShaderPass(MSize index = 0, MUint8 passIndex = 0) const override;

			virtual void SetMaterialData(MSize index = 0, MUint8 passIndex = 0) const override;
			virtual void SetMaterialDataDeferred(MSize index = 0, MUint8 passIndex = 0) const override;

			// Draws single submesh with its corresponding material.
			virtual void DrawSingle(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0) const override;
			virtual void DrawSingleDeferred(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0) const override;

			virtual void DrawSingleWithExternMaterial(const memoryManagement::Handle<gom::Camera> camera, const assetLibrary::MMaterial* mat, MSize index = 0) const override;
			virtual void DrawSingleDeferredWithExternMaterial(const memoryManagement::Handle<gom::Camera> camera, const assetLibrary::MMaterial* mat, MSize index = 0) const override;

#pragma region Accessors

			virtual void SetMesh(assetLibrary::MMesh* mesh);
			virtual inline const assetLibrary::MMesh* GetMesh() const { return _mesh; }
			
			virtual inline void AddMaterial(assetLibrary::MMaterial* mat) { if (_materials.IsEmpty()) _material = mat; _materials.Add(mat); }
			virtual inline void RemoveMaterial(assetLibrary::MMaterial* mat) { _materials.Remove(mat); if (_materials.IsEmpty()) _material = GetDefaultMaterial(); }
			virtual inline void RemoveMaterial(MSize index) { _materials.RemoveAt(index); if (_materials.IsEmpty()) _material = GetDefaultMaterial(); }
			virtual inline void SetMaterial(assetLibrary::MMaterial* mat) override 
			{ 
				Renderer::SetMaterial(mat); 
				_materials.IsEmpty() ? _materials.Add(mat) : _materials[0] = mat; 
			}
			virtual inline assetLibrary::MMaterial* GetMaterial(MSize index = 0) const override { return _materials[index]; }
			virtual inline MSize GetMaterialCount() const override { return _mesh != nullptr ? MMath::Min(_mesh->GetSubmeshCount(), _materials.GetSize()) : 0; }
			virtual inline MSize GetSubmeshCount() const { return _mesh != nullptr ? _mesh->GetSubmeshCount() : 0; }

#pragma endregion

#pragma endregion
		};

	}
}
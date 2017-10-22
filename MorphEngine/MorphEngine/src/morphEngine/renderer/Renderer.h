#pragma once

#include "renderer/OctTree.h"
#include "../assetLibrary/MMaterial.h"

#include "BoundsBox.h"

namespace morphEngine
{
	namespace renderer
	{
		/// <summary>
		/// Abstract class.
		/// </summary>
		class Renderer :
			public OctTreeElement
		{
			ME_TYPE
		protected:

			assetLibrary::MMaterial* _material = nullptr;

			Renderer(const gom::ObjectInitializer& initializer);

			virtual assetLibrary::MMaterial* GetDefaultMaterial();
		
		public:
			Renderer(const Renderer& copy, bool bDeepCopy = true) : OctTreeElement(copy, bDeepCopy),
				_material(copy._material) { }
			~Renderer();

			virtual void Initialize() override;
			virtual void Shutdown() override;

			virtual void SetMaterialShaderPass(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0, MUint8 passIndex = 0) const { }
			virtual void SetMaterialShaderDeferredPass(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0, MUint8 passIndex = 0) const { }

			virtual void UnsetMaterialShaderPass(MSize index = 0, MUint8 passIndex = 0) const { }

			virtual void SetMaterialData(MSize index = 0, MUint8 passIndex = 0) const { }
			virtual void SetMaterialDataDeferred(MSize index = 0, MUint8 passIndex = 0) const { }

			// Draws single submesh with its corresponding material.
			virtual void DrawSingle(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0) const { }
			virtual void DrawSingleDeferred(const memoryManagement::Handle<gom::Camera> camera, MSize index = 0) const { }

			virtual void DrawSingleWithExternMaterial(const memoryManagement::Handle<gom::Camera> camera, const assetLibrary::MMaterial* mat, MSize index = 0) const { }
			virtual void DrawSingleDeferredWithExternMaterial(const memoryManagement::Handle<gom::Camera> camera, const assetLibrary::MMaterial* mat, MSize index = 0) const { }

			virtual inline void SetMaterial(assetLibrary::MMaterial* mat) { _material = mat; }
			virtual inline assetLibrary::MMaterial* GetMaterial(MSize index = 0) const { return _material; }
			virtual inline MSize GetMaterialCount() const { return _material != nullptr ? 1 : 0; }
			virtual inline MSize GetSubmeshCount() const { return 0; }
		};

	}
}
#pragma once

#include "gom/GameObjectComponent.h"

namespace morphEngine
{
	namespace gom
	{
		class GUITransform;
	}

	namespace renderer
	{
		class GUIBatch;

		// A GameObject should not have more than one GUIImage component.
		class GUIImage : public gom::GameObjectComponent
		{
			ME_TYPE
		protected:

#pragma region Protected

			memoryManagement::Handle<GUIBatch> _parentBatch;

			assetLibrary::MMesh* _drawMesh;		// Default: Plane
			assetLibrary::MMaterial* _drawMaterial;		// Default: GUIDefaultMat (input: color, transform matrix, texture)

#pragma endregion

#pragma region Functions Protected

			GUIImage(const gom::ObjectInitializer& initializer);

			virtual void RegisterProperties() override;

			virtual void AssignDefaultMesh();
			virtual void AssignDefaultMaterial();

#pragma endregion

		public:

#pragma region Functions Public

			GUIImage(const GUIImage& copy, bool bDeepCopy = true);
			virtual ~GUIImage();

			virtual void Initialize() override;
			virtual void Shutdown() override;

			// Assuming render target was already set by parenting GUIBatch.
			virtual void Draw() const;

			virtual void SetMesh(assetLibrary::MMesh* mesh);
			virtual void SetMaterial(assetLibrary::MMaterial* material);

			inline virtual assetLibrary::MMesh* GetMesh() const { return _drawMesh; }
			inline virtual assetLibrary::MMaterial* GetMaterial() const { return _drawMaterial; }

			inline memoryManagement::Handle<gom::GUITransform> GetGUITransform() { return _owner->GetTransform(); }

#pragma endregion

		};

	}
}
#pragma once

#include "renderer/GUIImage.h"
#include "renderer/meshes/MeshText.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MFont;
	}

	namespace renderer
	{
		class GUIText : public GUIImage
		{
			ME_TYPE
		protected:

#pragma region Protected

#pragma endregion

#pragma region Functions Protected

			GUIText(const gom::ObjectInitializer& initializer);

			virtual void AssignDefaultMesh() override;
			virtual void AssignDefaultMaterial() override;

			inline void FitSizeToText();

#pragma endregion


		public:

#pragma region Functions Public

			GUIText(const GUIText& copy, bool bDeepCopy = true);
			~GUIText();

			virtual void Initialize() override;
			virtual void Shutdown() override;

			inline virtual void SetMesh(assetLibrary::MMesh* mesh) override { ME_WARNING_RETURN_STATEMENT(false, "Cannot set mesh of a GUIText."); }
			virtual void SetMaterial(assetLibrary::MMaterial* material) override;

			inline virtual assetLibrary::MMesh* GetMesh() const { ME_WARNING(false, "Cannot modify mesh of a GUIText."); return nullptr; }

			void SetText(const utility::MString& text);
			const utility::MString& GetText();

			void SetFont(assetLibrary::MFont* font);
			assetLibrary::MFont* GetFont();

			void SetAlignment(renderer::meshes::MeshText::Alignment alignment);
			renderer::meshes::MeshText::Alignment GetAlignment() const;

#pragma endregion

		};

	}
}
#pragma once

#include "renderer/shaders/ShaderGUI.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			class ShaderGUIButton :
				public ShaderGUI
			{
			public:

				static const char* TYPE_NAME;

			public:
				ShaderGUIButton();
				virtual ~ShaderGUIButton();

				virtual void SetupMaterialInterface(assetLibrary::MMaterial & mat) override;
				virtual void SetLocal(const assetLibrary::MMaterial& mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh& mesh) const override;
			};
		}
	}
}
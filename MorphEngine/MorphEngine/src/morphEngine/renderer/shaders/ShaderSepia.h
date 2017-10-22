#pragma once
#include "ShaderFullscreen.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			class ShaderSepia :
				public ShaderFullscreen
			{
			public:

				static const char* TYPE_NAME;

			protected:

				virtual void GetPixelShaderName(utility::MString & name) { name = "Sepia"; }

			public:
				ShaderSepia();
				~ShaderSepia();

				// Inherited via ShaderFullscreen
				virtual void SetLocal(const ShaderLocalData & localData) const override;
			};
		}
	}
}
#include "ShaderSepia.h"
#include "renderer/RenderTarget2D.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			const char* ShaderSepia::TYPE_NAME = "ShaderSepia";

			ShaderSepia::ShaderSepia()
			{
			}


			ShaderSepia::~ShaderSepia()
			{
			}

			void ShaderSepia::SetLocal(const ShaderLocalData & localData) const
			{
				const ShaderPostprocessSimpleData& data = reinterpret_cast<const ShaderPostprocessSimpleData&>(localData);
				data.TextureInput->SetAsInputPS(4);
			}
		}
	}
}
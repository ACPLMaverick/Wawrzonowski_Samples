#include "ShaderMerge.h"
#include "resourceManagement/ResourceManager.h"
#include "renderer/RenderTarget2D.h"

namespace morphEngine
{
	using namespace assetLibrary;
	using namespace resourceManagement;
	using namespace utility;
	namespace renderer
	{
		namespace shaders
		{
			const char* ShaderMerge::TYPE_NAME = "ShaderMerge";

			ShaderMerge::ShaderMerge()
			{
			}


			ShaderMerge::~ShaderMerge()
			{
			}

			void ShaderMerge::SetLocal(const ShaderLocalData & customData) const
			{
				const ShaderMergeData& data = reinterpret_cast<const ShaderMergeData&>(customData);
				data.TextureA->SetAsInputPS(0);
				data.TextureB->SetAsInputPS(1);
			}
		}
	}
}
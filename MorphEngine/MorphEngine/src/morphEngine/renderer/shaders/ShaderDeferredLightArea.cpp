#include "ShaderDeferredLightArea.h"
#include "gom/Transform.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			const char* ShaderDeferredLightArea::TYPE_NAME = "ShaderDeferredLightArea";

			ShaderDeferredLightArea::ShaderDeferredLightArea()
			{
			}


			ShaderDeferredLightArea::~ShaderDeferredLightArea()
			{
			}

			void ShaderDeferredLightArea::Initialize()
			{
				ShaderFullscreen::Initialize();

				_cArea = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_cArea->Initialize(nullptr, sizeof(LightAreaData));
			}

			void ShaderDeferredLightArea::Shutdown()
			{
				ShaderFullscreen::Shutdown();
				_cArea->Shutdown();
				delete _cArea;
			}

			void ShaderDeferredLightArea::SetLocal(const ShaderLocalData & customData) const
			{
				const ShaderDeferredLightAreaCustomData& data = reinterpret_cast<const ShaderDeferredLightAreaCustomData&>(customData);

				LightAreaData *lData;
				_cArea->Map(reinterpret_cast<void**>(&lData));
				lData->Color = data.Light->GetColor();
				lData->Direction = -data.Light->GetDirection();
				lData->Position = data.Light->GetPosition();
				lData->Right = data.Light->GetRight();
				lData->SizeRangeSmooth.X = data.Light->GetSize().X;
				lData->SizeRangeSmooth.Y = data.Light->GetSize().Y;
				lData->SizeRangeSmooth.Z = data.Light->GetRange();
				lData->SizeRangeSmooth.W = 1.0f / MMath::Max(data.Light->GetSmooth(), 0.01f);
				_cArea->Unmap();

				_cArea->SetPS(0);
			}
		}
	}
}
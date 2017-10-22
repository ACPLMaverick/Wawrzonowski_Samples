#include "ShaderDeferredLightSpot.h"
#include "gom/Transform.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			const char* ShaderDeferredLightSpot::TYPE_NAME = "ShaderDeferredLightSpot";

			ShaderDeferredLightSpot::ShaderDeferredLightSpot()
			{
			}


			ShaderDeferredLightSpot::~ShaderDeferredLightSpot()
			{
			}

			void ShaderDeferredLightSpot::Initialize()
			{
				ShaderFullscreen::Initialize();

				_cSpot = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_cSpot->Initialize(nullptr, sizeof(LightSpotData));
			}

			void ShaderDeferredLightSpot::Shutdown()
			{
				ShaderFullscreen::Shutdown();
				_cSpot->Shutdown();
				delete _cSpot;
			}

			void ShaderDeferredLightSpot::SetLocal(const ShaderLocalData & customData) const
			{
				const ShaderDeferredLightSpotCustomData& data = reinterpret_cast<const ShaderDeferredLightSpotCustomData&>(customData);

				LightSpotData *lData;
				_cSpot->Map(reinterpret_cast<void**>(&lData));
				lData->Color = data.Light->GetColor();
				lData->Direction = -data.Light->GetDirection();
				lData->Position = data.Light->GetPosition();
				lData->RangeAngleSmooth.X = data.Light->GetRange();
				lData->RangeAngleSmooth.Y = MMath::Cos(data.Light->GetConeAngle());
				lData->RangeAngleSmooth.Z = data.Light->GetSmooth();
				_cSpot->Unmap();

				_cSpot->SetPS(0);
			}
		}
	}
}
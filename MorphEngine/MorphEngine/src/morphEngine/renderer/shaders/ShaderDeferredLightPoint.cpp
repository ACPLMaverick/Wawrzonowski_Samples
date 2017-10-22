#include "ShaderDeferredLightPoint.h"
#include "gom/Transform.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			const char* ShaderDeferredLightPoint::TYPE_NAME = "ShaderDeferredLightPoint";

			ShaderDeferredLightPoint::ShaderDeferredLightPoint()
			{
			}


			ShaderDeferredLightPoint::~ShaderDeferredLightPoint()
			{
			}

			void ShaderDeferredLightPoint::Initialize()
			{
				ShaderFullscreen::Initialize();

				_cPoint = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_cPoint->Initialize(nullptr, sizeof(LightPointData));
			}

			void ShaderDeferredLightPoint::Shutdown()
			{
				ShaderFullscreen::Shutdown();
				_cPoint->Shutdown();
				delete _cPoint;
			}

			void ShaderDeferredLightPoint::SetLocal(const ShaderLocalData & customData) const
			{
				const ShaderDeferredLightPointCustomData& data = reinterpret_cast<const ShaderDeferredLightPointCustomData&>(customData);

				LightPointData *lData;
				_cPoint->Map(reinterpret_cast<void**>(&lData));
				lData->Color = data.Light->GetColor();
				lData->Position = data.Light->GetPosition();
				lData->Range = data.Light->GetRange();
				_cPoint->Unmap();

				_cPoint->SetPS(0);
			}
		}
	}
}
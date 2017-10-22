#include "ShaderDeferredLightDirectional.h"


namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			const char* ShaderDeferredLightDirectional::TYPE_NAME = "ShaderDeferredLightDirectional";

			ShaderDeferredLightDirectional::ShaderDeferredLightDirectional()
			{
			}


			ShaderDeferredLightDirectional::~ShaderDeferredLightDirectional()
			{
			}

			void ShaderDeferredLightDirectional::Initialize()
			{
				ShaderFullscreen::Initialize();

				_cDirectional = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_cDirectional->Initialize(nullptr, sizeof(LightDirectionalData));
			}

			void ShaderDeferredLightDirectional::Shutdown()
			{
				ShaderFullscreen::Shutdown();
				_cDirectional->Shutdown();
				delete _cDirectional;
			}

			void ShaderDeferredLightDirectional::SetLocal(const ShaderLocalData & customData) const
			{
				const ShaderDeferredLightDirectionalCustomData& data = reinterpret_cast<const ShaderDeferredLightDirectionalCustomData&>(customData);

				LightDirectionalData *lData;
				_cDirectional->Map(reinterpret_cast<void**>(&lData));
				lData->Color = data.Light->GetColor();
				lData->Direction = -data.Light->GetDirection();
				_cDirectional->Unmap();

				_cDirectional->SetPS(0);
			}
		}
	}
}
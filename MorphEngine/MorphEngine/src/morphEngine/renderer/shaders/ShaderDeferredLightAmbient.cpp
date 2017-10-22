#include "ShaderDeferredLightAmbient.h"


namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			const char* ShaderDeferredLightAmbient::TYPE_NAME = "ShaderDeferredLightAmbient";

			ShaderDeferredLightAmbient::ShaderDeferredLightAmbient()
			{
			}


			ShaderDeferredLightAmbient::~ShaderDeferredLightAmbient()
			{
			}

			void ShaderDeferredLightAmbient::Initialize()
			{
				ShaderFullscreen::Initialize();

				_cAmbient = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_cAmbient->Initialize(nullptr, sizeof(LightAmbientData));
			}

			void ShaderDeferredLightAmbient::Shutdown()
			{
				ShaderFullscreen::Shutdown();
				_cAmbient->Shutdown();
				delete _cAmbient;
			}

			void ShaderDeferredLightAmbient::SetLocal(const ShaderLocalData & customData) const
			{
				LightAmbientData* data;
				_cAmbient->Map(reinterpret_cast<void**>(&data));
				data->Color = reinterpret_cast<const ShaderDeferredLightAmbientCustomData&>(customData).Light->GetColor();
				_cAmbient->Unmap();

				_cAmbient->SetPS(0);
			}

		}
	}
}
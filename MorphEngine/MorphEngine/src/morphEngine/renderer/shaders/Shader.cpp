#include "Shader.h"

#include "ShaderLegacy.h"
#include "ShaderGameObjectID.h"
#include "ShaderGUI.h"
#include "ShaderGUIButton.h"
#include "ShaderDeferredLightAmbient.h"
#include "ShaderDeferredLightArea.h"
#include "ShaderDeferredLightDirectional.h"
#include "ShaderDeferredLightPoint.h"
#include "ShaderDeferredLightSpot.h"
#include "ShaderMerge.h"
#include "ShaderSepia.h"
#include "ShaderBlank.h"
#include "ShaderBounds.h"
#include "ShaderSky.h"
#include "gom/Camera.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			Shader::Shader()
			{
			}


			Shader::~Shader()
			{
			}

			void Shader::SetupGlobalCBuffer(const ShaderGlobalData& globalData, device::ConstantBufferResource * cb, MUint32 slot) const
			{
				BufferDataGlobal* gData;
				cb->Map(reinterpret_cast<void**>(&gData));
				gData->CameraPosition = globalData.Camera->GetPosition();
				cb->Unmap();
				cb->Set(slot);
			}

			Shader * Shader::CreateShaderByName(const utility::MString & name)
			{
				if (name == ShaderLegacy::TYPE_NAME)
				{
					return new ShaderLegacy();
				}
				else if (name == ShaderSky::TYPE_NAME)
				{
					return new ShaderSky();
				}
				else if (name == ShaderGameObjectID::TYPE_NAME)
				{
					return new ShaderGameObjectID();
				}
				else if (name == ShaderGUI::TYPE_NAME)
				{
					return new ShaderGUI();
				}
				else if (name == ShaderGUIButton::TYPE_NAME)
				{
					return new ShaderGUIButton();
				}
				else if (name == ShaderDeferredLightAmbient::TYPE_NAME)
				{
					return new ShaderDeferredLightAmbient();
				}
				else if (name == ShaderDeferredLightDirectional::TYPE_NAME)
				{
					return new ShaderDeferredLightDirectional();
				}
				else if (name == ShaderDeferredLightPoint::TYPE_NAME)
				{
					return new ShaderDeferredLightPoint();
				}
				else if (name == ShaderDeferredLightSpot::TYPE_NAME)
				{
					return new ShaderDeferredLightSpot();
				}
				else if (name == ShaderDeferredLightArea::TYPE_NAME)
				{
					return new ShaderDeferredLightArea();
				}
				else if (name == ShaderMerge::TYPE_NAME)
				{
					return new ShaderMerge();
				}
				else if (name == ShaderSepia::TYPE_NAME)
				{
					return new ShaderSepia();
				}
				else if (name == ShaderBlank::TYPE_NAME)
				{
					return new ShaderBlank();
				}
				else if (name == ShaderBounds::TYPE_NAME)
				{
					return new ShaderBounds();
				}
				else
				{
					ME_ASSERT(false, "Shader: Wrong name passed to CreateShaderByName func.");
					return nullptr;
				}
			}

			void Shader::BuildShaderPath(const char * typeName, const char* suffix, utility::MString & outPath, bool deferred)
			{
				//$(OutDir)/Resources/Shaders/%(Filename)/DX11/%(Filename).cso
				outPath = SHADER_PATH_PREFIX;
				outPath += typeName;
				outPath += SHADER_PATH_MIDDLEFIX;
				outPath += typeName;
				if(deferred) outPath += SHADER_PATH_SUFFIX_DEFERRED;
				outPath += suffix;
			}
		}

	}
}
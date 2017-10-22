#include "ShaderFullscreen.h"
#include "resourceManagement/ResourceManager.h"
#include "renderer/RenderTarget2D.h"

namespace morphEngine
{
	using namespace assetLibrary;
	using namespace resourceManagement;
	using namespace utility;
	namespace renderer
	{
		using namespace device;
		namespace shaders
		{
			ShaderFullscreen::ShaderFullscreen()
			{
			}

			ShaderFullscreen::~ShaderFullscreen()
			{
			}

			void ShaderFullscreen::Initialize()
			{
				MString vsName = "DeferredFullscreen";
				MString psName;
				GetPixelShaderName(psName);

				MString pathVS, pathPS;
				BuildShaderPath(vsName, SHADER_PATH_SUFFIX_VS, pathVS);
				BuildShaderPath(psName, SHADER_PATH_SUFFIX_PS, pathPS);

				MShaderBytecode* bcVS = ResourceManager::GetInstance()->GetShaderBytecode(pathVS);
				MShaderBytecode* bcPS = ResourceManager::GetInstance()->GetShaderBytecode(pathPS);

				_vs = RenderingManager::GetInstance()->GetDevice()->CreateVertexShaderInstance();
				_ps = RenderingManager::GetInstance()->GetDevice()->CreatePixelShaderInstance();

				_vs->Initialize(bcVS->GetBytecode().GetDataPointer(), bcVS->GetBytecode().GetSize(), 0, nullptr, nullptr, nullptr);
				_ps->Initialize(bcPS->GetBytecode().GetDataPointer(), bcPS->GetBytecode().GetSize());

				_cGlobal = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_cGlobal->Initialize(nullptr, sizeof(LightGlobalData));
			}

			void ShaderFullscreen::Shutdown()
			{
				_vs->Shutdown();
				_ps->Shutdown();
				_cGlobal->Shutdown();
				delete _vs;
				delete _ps;
				delete _cGlobal;
			}

			void ShaderFullscreen::Set(MUint8 passIndex) const
			{
				_vs->Set();
				_ps->Set();
			}

			void ShaderFullscreen::Set(const ShaderGlobalData & globalData, const ShaderLocalData & localData, MUint8 passIndex) const
			{
				ME_WARNING(false, "TODO: Implement.");
			}

			void ShaderFullscreen::SetGlobal(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				const ShaderFullscreenGlobalData& data = reinterpret_cast<const ShaderFullscreenGlobalData&>(globalData);
				_vs->Set();
				_ps->Set();
				
				SetupGlobalCBuffer(globalData, _cGlobal, 1);

				data.TextureColorDepth->SetAsInputWithDepth(0, 1);
				data.TextureNormal->SetAsInputPS(2);
				data.TexturePosition->SetAsInputPS(3);
			}
			void ShaderFullscreen::SetLocal(const RenderTarget2D * input) const
			{
				input->SetAsInputPS(4);
			}

			void ShaderFullscreen::UnsetGlobal() const
			{
				RenderingManager::GetInstance()->GetDevice()->ClearBoundShaderVS();
				RenderingManager::GetInstance()->GetDevice()->ClearBoundShaderPS();
			}
		}
	}
}
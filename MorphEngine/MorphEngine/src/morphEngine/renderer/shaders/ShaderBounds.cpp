#include "ShaderBounds.h"
#include "renderer/RenderingManager.h"
#include "resourceManagement/ResourceManager.h"

namespace morphEngine
{
	using namespace assetLibrary;
	using namespace resourceManagement;
	using namespace utility;

	namespace renderer
	{
		namespace shaders
		{
			const char* ShaderBounds::TYPE_NAME = "ShaderBounds";

			ShaderBounds::ShaderBounds()
			{
			}


			ShaderBounds::~ShaderBounds()
			{
			}

			void ShaderBounds::Initialize()
			{
				_vs = RenderingManager::GetInstance()->GetDevice()->CreateVertexShaderInstance();
				_gs = RenderingManager::GetInstance()->GetDevice()->CreateGeometryShaderInstance();
				_ps = RenderingManager::GetInstance()->GetDevice()->CreatePixelShaderInstance();

				MString strVS, strGS, strPS;
				BuildShaderPath(TYPE_NAME, SHADER_PATH_SUFFIX_VS, strVS);
				BuildShaderPath(TYPE_NAME, SHADER_PATH_SUFFIX_GS, strGS);
				BuildShaderPath(TYPE_NAME, SHADER_PATH_SUFFIX_PS, strPS);

				MShaderBytecode* bcVS = ResourceManager::GetInstance()->GetShaderBytecode(strVS);
			
				MString name = "POSITION";
				MUint8 index = 0;
				device::GraphicDataFormat format = device::GraphicDataFormat::FLOAT_R32G32B32;

				_vs->Initialize(bcVS->GetBytecode().GetDataPointer(), bcVS->GetBytecode().GetSize(), 1, &name, &index, &format);

				MShaderBytecode* bcGS = ResourceManager::GetInstance()->GetShaderBytecode(strGS);
				_gs->Initialize(bcGS->GetBytecode().GetDataPointer(), bcGS->GetBytecode().GetSize());

				MShaderBytecode* bcPS = ResourceManager::GetInstance()->GetShaderBytecode(strPS);
				_ps->Initialize(bcPS->GetBytecode().GetDataPointer(), bcPS->GetBytecode().GetSize());

				_cBufferVS = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_cBufferVS->Initialize(nullptr, sizeof(MMatrix));
			}

			void ShaderBounds::Shutdown()
			{
				if (_vs != nullptr)
				{
					_vs->Shutdown();
					delete _vs;
					_vs = nullptr;
				}
				if (_gs != nullptr)
				{
					_gs->Shutdown();
					delete _gs;
					_gs = nullptr;
				}
				if (_ps != nullptr)
				{
					_ps->Shutdown();
					delete _ps;
					_ps = nullptr;
				}
				if (_cBufferVS != nullptr)
				{
					_cBufferVS->Shutdown();
					delete _cBufferVS;
					_cBufferVS = nullptr;
				}
			}

			void ShaderBounds::Set(MUint8 passIndex) const
			{
				_vs->Set();
				_gs->Set();
				_ps->Set();
			}

			void ShaderBounds::Set(const ShaderGlobalData & globalData, const ShaderLocalData & localData, MUint8 passIndex) const
			{
			}

			void ShaderBounds::SetGlobal(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
			}

			void ShaderBounds::UnsetGlobal() const
			{
				RenderingManager::GetInstance()->GetDevice()->ClearBoundShaderVS();
				RenderingManager::GetInstance()->GetDevice()->ClearBoundShaderGS();
				RenderingManager::GetInstance()->GetDevice()->ClearBoundShaderPS();
			}

			void ShaderBounds::SetLocal(const utility::MMatrix& tr, const assetLibrary::MMesh& mesh) const
			{
				mesh.GetVertexPositionsResource()->Set(0);
				mesh.GetIndicesResouce()->Set();

				MMatrix* ptr;
				_cBufferVS->Map(reinterpret_cast<void**>(&ptr));
				(*ptr) = tr;
				_cBufferVS->Unmap();
				_cBufferVS->Set();
			}
		}
	}
}
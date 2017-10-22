#include "ShaderGUI.h"
#include "assetLibrary/MMaterial.h"
#include "assetLibrary/MShaderBytecode.h"
#include "resourceManagement/ResourceManager.h"
#include "renderer/RenderingManager.h"
#include "gom/GUITransform.h"

namespace morphEngine
{
	using namespace utility;
	using namespace assetLibrary;
	using namespace resourceManagement;

	namespace renderer
	{
		using namespace device;
		namespace shaders
		{
			const char* ShaderGUI::TYPE_NAME = "ShaderGUI";

			void ShaderGUI::Initialize()
			{
				ShaderObject::Initialize();

				_ps = RenderingManager::GetInstance()->GetDevice()->CreatePixelShaderInstance();
				_cBufferPS = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();

				MString strPS;
				BuildShaderPath(TYPE_NAME, SHADER_PATH_SUFFIX_PS, strPS);

				MShaderBytecode* bcPS = ResourceManager::GetInstance()->GetShaderBytecode(strPS);
				_ps->Initialize(bcPS->GetBytecode().GetDataPointer(), bcPS->GetBytecode().GetSize());
				_cBufferPS->Initialize(nullptr, sizeof(BufferDataPS));
			}

			void ShaderGUI::Shutdown()
			{
				if (_ps != nullptr)
				{
					_ps->Shutdown();
					delete _ps;
					_ps = nullptr;
				}
				if (_cBufferPS != nullptr)
				{
					_cBufferPS->Shutdown();
					delete _cBufferPS;
					_cBufferPS = nullptr;
				}

				ShaderObject::Shutdown();
			}

			void ShaderGUI::SetupMaterialInterface(assetLibrary::MMaterial & mat)
			{
				mat.AddSlotColor();
				mat.AddSlotTexture2D();
				mat.AddSlotUint();
			}

			void ShaderGUI::SetGlobal(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				_vs->Set();
				_ps->Set();
			}

			void ShaderGUI::SetGlobalDeferred(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				_vs->Set();
				_ps->Set();
			}

			void ShaderGUI::SetGlobalSkinned(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				_vs->Set();
				_ps->Set();
			}

			void ShaderGUI::SetGlobalSkinnedDeferred(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				_vs->Set();
				_ps->Set();
			}

			void ShaderGUI::SetLocal(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh) const
			{
				mesh.GetVertexPositionsResource()->Set(0);
				mesh.GetVertexUVsResource()->Set(1);
				mesh.GetIndicesResouce()->Set();

				BufferDataVS* cbPtr;
				_cBufferVS->Map(reinterpret_cast<void**>(&cbPtr));
				cbPtr->_guiMatrix = reinterpret_cast<const gom::GUITransform&>(transform).GetGUIMatrix();
				_cBufferVS->Unmap();

				_cBufferVS->SetVS(0);

				BufferDataPS* buf;
				_cBufferPS->Map(reinterpret_cast<void**>(&buf));
				buf->_baseColor = mat.GetColor(0);
				buf->_indices = mat.GetUint(0);
				_cBufferPS->Unmap();

				_cBufferPS->SetPS(0);

				mat.GetTexture2D(0)->SetAsInputPS(0);
			}

			void ShaderGUI::Set(MUint8 passIndex) const
			{
			}

			void ShaderGUI::Set(const ShaderGlobalData & globalData, const ShaderLocalData & localData, MUint8 passIndex) const
			{
			}

			void ShaderGUI::VertexShaderSetup()
			{
				_vs = RenderingManager::GetInstance()->GetDevice()->CreateVertexShaderInstance();
				_cBufferVS = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				MString strVS;
				BuildShaderPath(TYPE_NAME, SHADER_PATH_SUFFIX_VS, strVS);
				MShaderBytecode* bcVS = ResourceManager::GetInstance()->GetShaderBytecode(strVS);

				MString layoutNames[2]{ "POSITION", "TEXCOORD" };
				MUint8 layoutIndices[2]{ 0, 0 };
				GraphicDataFormat formats[2]{ GraphicDataFormat::FLOAT_R32G32B32, GraphicDataFormat::FLOAT_R32G32 };

				_vs->Initialize(bcVS->GetBytecode().GetDataPointer(), bcVS->GetBytecode().GetSize(), 2, layoutNames, layoutIndices, formats);
				_cBufferVS->Initialize(nullptr, sizeof(BufferDataVS));
			}
		}
	}
}
#include "ShaderGameObjectID.h"
#include "assetLibrary/MMaterial.h"
#include "assetLibrary/MShaderBytecode.h"
#include "resourceManagement/ResourceManager.h"
#include "renderer/RenderingManager.h"

namespace morphEngine
{
	using namespace utility;
	using namespace assetLibrary;
	using namespace resourceManagement;

	namespace renderer
	{
		namespace shaders
		{
			const char* ShaderGameObjectID::TYPE_NAME = "ShaderGameObjectID";

			void ShaderGameObjectID::Initialize()
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

			void ShaderGameObjectID::Shutdown()
			{
				ShaderObject::Shutdown();
				_ps->Shutdown();
				_cBufferPS->Shutdown();
				delete _ps;
				delete _cBufferPS;
			}

			void ShaderGameObjectID::SetupMaterialInterface(assetLibrary::MMaterial & mat)
			{
				mat.AddSlotUint();
			}

			void ShaderGameObjectID::SetGlobal(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				ShaderObject::SetGlobal(globalData, passIndex);
				_ps->Set();
			}

			void ShaderGameObjectID::SetGlobalDeferred(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				ShaderObject::SetGlobalDeferred(globalData, passIndex);
				_ps->Set();
			}

			void ShaderGameObjectID::SetGlobalSkinned(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				ShaderObject::SetGlobalSkinned(globalData, passIndex);
				_ps->Set();
			}

			void ShaderGameObjectID::SetGlobalSkinnedDeferred(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				ShaderObject::SetGlobalSkinnedDeferred(globalData, passIndex);
				_ps->Set();
			}

			void ShaderGameObjectID::SetLocal(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh) const
			{
				ShaderObject::SetLocal(mat, camera, transform, mesh);

				BufferDataPS* buf;
				_cBufferPS->Map(reinterpret_cast<void**>(&buf));
				buf->_id = mat.GetUint(0);
				_cBufferPS->Unmap();

				_cBufferPS->SetPS(0);
			}
			void ShaderGameObjectID::Set(MUint8 passIndex) const
			{
			}
			void ShaderGameObjectID::Set(const ShaderGlobalData & globalData, const ShaderLocalData & localData, MUint8 passIndex) const
			{
			}
		}
	}
}
#include "ShaderBlank.h"
#include "renderer/RenderingManager.h"
#include "resourceManagement/ResourceManager.h"

namespace morphEngine
{
	using namespace utility;
	using namespace assetLibrary;
	using namespace resourceManagement;

	namespace renderer
	{
		namespace shaders
		{
			const char* ShaderBlank::TYPE_NAME = "ShaderBlank";

			ShaderBlank::ShaderBlank()
			{
			}


			ShaderBlank::~ShaderBlank()
			{
			}

			void ShaderBlank::Initialize()
			{
				ShaderObject::Initialize();

				_ps = RenderingManager::GetInstance()->GetDevice()->CreatePixelShaderInstance();
				
				MString strPS;
				BuildShaderPath(TYPE_NAME, SHADER_PATH_SUFFIX_PS, strPS);

				MShaderBytecode* bcPS = ResourceManager::GetInstance()->GetShaderBytecode(strPS);
				_ps->Initialize(bcPS->GetBytecode().GetDataPointer(), bcPS->GetBytecode().GetSize());
			}

			void ShaderBlank::Shutdown()
			{
				_ps->Shutdown();
				delete _ps;
				_ps = nullptr;

				ShaderObject::Shutdown();
			}

			void ShaderBlank::Set(MUint8 passIndex) const
			{
				//ShaderObject::Set(passIndex);
				//_ps->Set();
			}

			void ShaderBlank::Set(const ShaderGlobalData & globalData, const ShaderLocalData & localData, MUint8 passIndex) const
			{
				//ShaderObject::Set(globalData, localData, passIndex);
				//_ps->Set();
			}

			void ShaderBlank::SetGlobal(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				ShaderObject::SetGlobal(globalData, passIndex);
				_ps->Set();// HAHA let's try it like this
			}

			void ShaderBlank::SetupMaterialInterface(assetLibrary::MMaterial & mat)
			{
				// does not setup anything welp
			}

			void ShaderBlank::SetLocal(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh) const
			{
				ShaderObject::SetLocal(mat, camera, transform, mesh);
			}

			void ShaderBlank::SetLocalDeferred(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh) const
			{
				ShaderObject::SetLocalDeferred(mat, camera, transform, mesh);
			}
		}
	}
}
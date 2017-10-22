#include "ShaderSky.h"
#include "assetLibrary/MMaterial.h"
#include "assetLibrary/MShaderBytecode.h"
#include "resourceManagement/ResourceManager.h"
#include "renderer/RenderingManager.h"
#include "gom/Transform.h"
#include "gom/Camera.h"

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
			const char* ShaderSky::TYPE_NAME = "ShaderSky";

			ShaderSky::ShaderSky()
			{
			}


			ShaderSky::~ShaderSky()
			{
			}

			void ShaderSky::SetupMaterialInterface(assetLibrary::MMaterial & mat)
			{
				// sky color
				mat.AddSlotColor();
				// sky contrast
				mat.AddSlotFloat();
				// sky texture
				mat.AddSlotTextureCube();
			}

			void ShaderSky::Initialize()
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

			void ShaderSky::Shutdown()
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

			void ShaderSky::SetGlobal(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				_vs->Set();
				_ps->Set();
			}

			void ShaderSky::UnsetGlobal() const
			{
				RenderingManager::GetInstance()->GetDevice()->ClearBoundShaderVS();
				RenderingManager::GetInstance()->GetDevice()->ClearBoundShaderPS();
			}

			void ShaderSky::SetGlobalSkinned(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				SetGlobal(globalData, passIndex);
			}

			void ShaderSky::SetMaterialWise(const assetLibrary::MMaterial & mat) const
			{
				BufferDataPS* cbPtr = nullptr;
				_cBufferPS->Map(reinterpret_cast<void**>(&cbPtr));
				cbPtr->_color = mat.GetColor(0);
				cbPtr->_contrast = mat.GetFloat(0);
				_cBufferPS->Unmap();

				_cBufferPS->SetPS();

				mat.GetTextureCube(0)->GetResource()->SetPS(0);
			}

			void ShaderSky::SetLocal(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh) const
			{
				mesh.GetVertexPositionsResource()->Set(0);
				mesh.GetVertexUVsResource()->Set(1);
				mesh.GetIndicesResouce()->Set();

				BufferDataVS* cbPtr;

				MMatrix transMat(MMatrix::Scale(camera.GetFarPlane() * 0.9f));

				_cBufferVS->Map(reinterpret_cast<void**>(&cbPtr));

				cbPtr->_matWVP = transMat * camera.GetMatViewProj();

				_cBufferVS->Unmap();

				_cBufferVS->SetVS(0);
			}

			void ShaderSky::SetLocalSkinned(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh, const animation::AnimationFrame & frame) const
			{
				SetLocal(mat, camera, transform, mesh);
			}

			void ShaderSky::Set(MUint8 passIndex) const
			{
			}

			void ShaderSky::Set(const ShaderGlobalData & globalData, const ShaderLocalData & localData, MUint8 passIndex) const
			{
			}

			void ShaderSky::VertexShaderSetup()
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
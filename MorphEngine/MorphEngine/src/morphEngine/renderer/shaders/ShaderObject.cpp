#include "ShaderObject.h"
#include "renderer/RenderingManager.h"
#include "resourceManagement/ResourceManager.h"
#include "gom/Transform.h"
#include "gom/Camera.h"

namespace morphEngine
{
	using namespace utility;
	using namespace resourceManagement;
	using namespace assetLibrary;

	namespace renderer
	{
		using namespace device;

		namespace shaders
		{
			ShaderObject::ShaderObject()
			{
			}


			ShaderObject::~ShaderObject()
			{
			}

			void ShaderObject::Initialize()
			{
				VertexShaderSetup();
			}

			void ShaderObject::Shutdown()
			{
				if (_vs != nullptr)
				{
					_vs->Shutdown();
					delete _vs;
					_vs = nullptr;
				}
				if (_cBufferVS != nullptr)
				{
					_cBufferVS->Shutdown();
					delete _cBufferVS;
					_cBufferVS = nullptr;
				}
				if (_vsSkin != nullptr)
				{
					_vsSkin->Shutdown();
					delete _vsSkin;
					_vsSkin = nullptr;
				}
				if (_cBufferSkinVS != nullptr)
				{
					_cBufferSkinVS->Shutdown();
					delete _cBufferSkinVS;
					_cBufferSkinVS = nullptr;
				}
			}

			void ShaderObject::SetGlobal(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				_vs->Set();
			}

			void ShaderObject::UnsetGlobal() const
			{
				RenderingManager::GetInstance()->GetDevice()->ClearBoundShaderVS();
				RenderingManager::GetInstance()->GetDevice()->ClearBoundShaderPS();
			}

			void ShaderObject::SetGlobalSkinned(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				_vsSkin->Set();
			}

			void ShaderObject::SetMaterialWise(const assetLibrary::MMaterial & mat) const
			{
			}

			void ShaderObject::SetLocal(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh) const
			{
				mesh.GetVertexPositionsResource()->Set(0);
				mesh.GetVertexNormalsResource()->Set(1);
				mesh.GetVertexUVsResource()->Set(2);
				mesh.GetIndicesResouce()->Set();

				BufferDataVS* cbPtr;

				MMatrix wTrans = transform.GetMatWorld().Transposed();

				MMatrix wvp = wTrans * camera.GetMatViewProj();

				_cBufferVS->Map(reinterpret_cast<void**>(&cbPtr));

				_cBufferVS->ApplyMatrix(&cbPtr->Wvp, &wvp);
				_cBufferVS->ApplyMatrix(&cbPtr->W, &wTrans);
				_cBufferVS->ApplyMatrix(&cbPtr->WInvTrans, &transform.GetMatWorldInverseTranspose().Transposed());

				_cBufferVS->Unmap();

				_cBufferVS->SetVS(0);
			}

			void ShaderObject::SetLocalSkinned(const assetLibrary::MMaterial& mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh, const animation::AnimationFrame& frame) const
			{
				SetLocal(mat, camera, transform, mesh);
				SkinSetup(mesh, frame);
			}

			void ShaderObject::SetLocalSkinnedDeferred(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh, const animation::AnimationFrame& frame) const
			{
				SetLocalDeferred(mat, camera, transform, mesh);
				SkinSetup(mesh, frame);
			}

			void ShaderObject::VertexShaderSetup()
			{
				_vs = RenderingManager::GetInstance()->GetDevice()->CreateVertexShaderInstance();
				_vsSkin = RenderingManager::GetInstance()->GetDevice()->CreateVertexShaderInstance();
				_cBufferVS = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_cBufferSkinVS = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();

				MString strVS, strVSSkin;
				BuildShaderPath("ObjectVertexShader", SHADER_PATH_SUFFIX_VS, strVS);
				BuildShaderPath("ObjectVertexShader_Skin", SHADER_PATH_SUFFIX_VS, strVSSkin);
				MShaderBytecode* bcVS = ResourceManager::GetInstance()->GetShaderBytecode(strVS);
				MShaderBytecode* bcVSSkin = ResourceManager::GetInstance()->GetShaderBytecode(strVSSkin);

				MString layoutNames[3]{ "POSITION", "NORMAL", "TEXCOORD" };
				MUint8 layoutIndices[3]{ 0, 0, 0 };
				GraphicDataFormat formats[3]{ GraphicDataFormat::FLOAT_R32G32B32, GraphicDataFormat::FLOAT_R32G32B32, GraphicDataFormat::FLOAT_R32G32 };

				_vs->Initialize(bcVS->GetBytecode().GetDataPointer(), bcVS->GetBytecode().GetSize(), 3, layoutNames, layoutIndices, formats);
				_cBufferVS->Initialize(nullptr, sizeof(BufferDataVS));

				MString layoutNamesSkin[5]{ "POSITION", "NORMAL", "TEXCOORD", "TEXCOORD", "TEXCOORD" };
				MUint8 layoutIndicesSkin[5]{ 0, 0, 0, 1, 2 };
				GraphicDataFormat formatsSkin[5]{ 
					GraphicDataFormat::FLOAT_R32G32B32, 
					GraphicDataFormat::FLOAT_R32G32B32, 
					GraphicDataFormat::FLOAT_R32G32,
					GraphicDataFormat::FLOAT_R32G32B32A32,
					GraphicDataFormat::SINT_R32G32B32A32 };

				_vsSkin->Initialize(bcVSSkin->GetBytecode().GetDataPointer(), bcVSSkin->GetBytecode().GetSize(), 5, layoutNamesSkin, layoutIndicesSkin, formatsSkin);
				_cBufferSkinVS->Initialize(nullptr, sizeof(BufferSkinnedDataVS));
			}

			inline void ShaderObject::SkinSetup(const assetLibrary::MMesh & mesh, const animation::AnimationFrame & frame) const
			{
				mesh.GetVertexBoneWeightsResource()->Set(3);
				mesh.GetVertexBoneIndicesResource()->Set(4);

				BufferSkinnedDataVS* cbPtr;
				_cBufferSkinVS->Map(reinterpret_cast<void**>(&cbPtr));

				MSize i = 0;
				for (auto it = frame.GetData().GetBegin(); it.IsValid() && i < BONES_PER_MESH; ++it, ++i)
				{
					cbPtr->SkinnedTransforms[i] = (*it);
				}

				_cBufferSkinVS->Unmap();

				_cBufferSkinVS->SetVS(1);
			}
		}
	}
}
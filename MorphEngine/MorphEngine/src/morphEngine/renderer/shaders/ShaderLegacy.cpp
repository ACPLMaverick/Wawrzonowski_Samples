#include "ShaderLegacy.h"
#include "renderer/RenderingManager.h"
#include "renderer/LightAmbient.h"
#include "renderer/LightDirectional.h"
#include "renderer/LightPoint.h"
#include "renderer/LightSpot.h"
#include "renderer/LightArea.h"
#include "assetLibrary/MShaderBytecode.h"
#include "assetLibrary/MMaterial.h"
#include "resourceManagement/ResourceManager.h"
#include "gom/Camera.h"
#include "gom/Transform.h"
#include "utility/MMath.h"
#include "animation/AnimationFrame.h"

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
			const char* ShaderLegacy::TYPE_NAME = "ShaderLegacy";

			ShaderLegacy::ShaderLegacy()
			{
			}


			ShaderLegacy::~ShaderLegacy()
			{
			}

			void ShaderLegacy::Initialize()
			{
				ShaderObject::Initialize();

				_ps = RenderingManager::GetInstance()->GetDevice()->CreatePixelShaderInstance();
				_psDef = RenderingManager::GetInstance()->GetDevice()->CreatePixelShaderInstance();
				_cBufferPS = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_gBufferPS = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_glBuffer = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_plBuffer = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_slBuffer = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();
				_alBuffer = RenderingManager::GetInstance()->GetDevice()->CreateConstantBufferInstance();

				MString strPS, strPSDef;
				BuildShaderPath(TYPE_NAME, SHADER_PATH_SUFFIX_PS, strPS);
				BuildShaderPath(TYPE_NAME, SHADER_PATH_SUFFIX_PS, strPSDef, true);

				MShaderBytecode* bcPS = ResourceManager::GetInstance()->GetShaderBytecode(strPS);
				MShaderBytecode* bcPSDef = ResourceManager::GetInstance()->GetShaderBytecode(strPSDef);

				
				_ps->Initialize(bcPS->GetBytecode().GetDataPointer(), bcPS->GetBytecode().GetSize());
				_psDef->Initialize(bcPSDef->GetBytecode().GetDataPointer(), bcPSDef->GetBytecode().GetSize());
				_cBufferPS->Initialize(nullptr, sizeof(BufferDataPS));
				_gBufferPS->Initialize(nullptr, sizeof(BufferDataGlobal));
				_glBuffer->Initialize(nullptr, sizeof(LightGlobalBufferData));
				_plBuffer->Initialize(nullptr, sizeof(LightPointBufferData));
				_slBuffer->Initialize(nullptr, sizeof(LightSpotBufferData));
				_alBuffer->Initialize(nullptr, sizeof(LightAreaBufferData));

			}

			void ShaderLegacy::Shutdown()
			{
				ShaderObject::Shutdown();

				_ps->Shutdown();
				_psDef->Shutdown();
				_cBufferPS->Shutdown();
				_gBufferPS->Shutdown();
				_glBuffer->Shutdown();
				_plBuffer->Shutdown();
				_slBuffer->Shutdown();
				_alBuffer->Shutdown();
				delete _ps;
				delete _psDef;
				delete _cBufferPS;
				delete _gBufferPS;
				delete _glBuffer;
				delete _plBuffer;
				delete _slBuffer;
				delete _alBuffer;
			}

			void ShaderLegacy::SetupMaterialInterface(assetLibrary::MMaterial & mat)
			{
				mat.AddSlotColor();
				mat.AddSlotColor();
				mat.AddSlotFloat();

				mat.AddSlotTexture2D();
				mat.AddSlotTexture2D();
			}

			void ShaderLegacy::Set(const ShaderGlobalData & globalData, const ShaderLocalData & localData, MUint8 passIndex) const
			{
				ME_WARNING(false, "TODO: Implement.");
			}

			void ShaderLegacy::SetGlobal(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				ShaderObject::SetGlobal(globalData, passIndex);
				_ps->Set();

				SetupGlobalCBuffer(globalData, _gBufferPS, 1);

				SetupLightsForward();
			}

			void ShaderLegacy::SetGlobalDeferred(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				ShaderObject::SetGlobalDeferred(globalData, passIndex);
				_psDef->Set();

				SetupGlobalCBuffer(globalData, _gBufferPS, 1);
			}

			void ShaderLegacy::SetGlobalSkinned(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				ShaderObject::SetGlobalSkinned(globalData, passIndex);
				_ps->Set();

				SetupGlobalCBuffer(globalData, _gBufferPS, 1);

				SetupLightsForward();
			}

			void ShaderLegacy::SetGlobalSkinnedDeferred(const ShaderGlobalData & globalData, MUint8 passIndex) const
			{
				ShaderObject::SetGlobalSkinnedDeferred(globalData, passIndex);
				_psDef->Set();

				SetupGlobalCBuffer(globalData, _gBufferPS, 1);
			}

			void ShaderLegacy::SetMaterialWise(const assetLibrary::MMaterial & mat) const
			{
				BufferDataPS* cpPtr;
				_cBufferPS->Map(reinterpret_cast<void**>(&cpPtr));

				cpPtr->_colBase = mat.GetColor(0);
				cpPtr->_colSpec = mat.GetColor(1);
				cpPtr->_gloss = mat.GetFloat(0);

				_cBufferPS->Unmap();

				_cBufferPS->SetPS(0);

				mat.GetTexture2D(0)->GetResource()->SetPS(0);
				mat.GetTexture2D(1)->GetResource()->SetPS(1);
			}

			void ShaderLegacy::SetMaterialWiseDeferred(const assetLibrary::MMaterial & mat) const
			{
				SetMaterialWise(mat);
			}

			void ShaderLegacy::SetLocal(const MMaterial& mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh& mesh) const
			{
				ShaderObject::SetLocal(mat, camera, transform, mesh);
			}

			// only set material data as we draw without light
			void ShaderLegacy::SetLocalDeferred(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh) const
			{
				ShaderObject::SetLocalDeferred(mat, camera, transform, mesh);
			}

			inline void ShaderLegacy::SetupLightsForward() const
			{
				// Setting up lights.
				// For now, all existing lights in memory are taken into account.
				// Later, when scene mechanisms will exist, visibility and/or range checking will occur here.
				// TODO: Make light data include its gameobject transform.

				// GLOBAL LIGHTS

				LightGlobalBufferData* lgPtr;
				_glBuffer->Map(reinterpret_cast<void**>(&lgPtr));

				auto itAmbient = LightAmbient::GetType()->GetIterator();
				if (itAmbient.IsValid() && itAmbient.Get<LightAmbient>()->GetEnabled() &&
					itAmbient.Get<LightAmbient>()->GetVisible())
				{
					lgPtr->_colAmbient = itAmbient.Get<LightAmbient>()->GetColor();
				}

				auto itDirectional = LightDirectional::GetType()->GetIterator();
				MUint32 iDirectional = 0;
				for (;
					iDirectional < COUNT_LIGHT_DIRECTIONAL && itDirectional.IsValid();
					++itDirectional)
				{
					LightDirectional* lDir = itDirectional.Get<LightDirectional>();
					if (lDir->GetEnabled() && lDir->GetVisible())
					{
						lgPtr->_colDirectional[iDirectional] = lDir->GetColor();
						lgPtr->_dirDirectional[iDirectional] = -lDir->GetDirection();
						++iDirectional;
					}
				}
				lgPtr->_numDirectional = iDirectional;

				_glBuffer->Unmap();
				_glBuffer->SetPS(2);

				// POINT LIGHTS

				LightPointBufferData* lpPtr;
				_plBuffer->Map(reinterpret_cast<void**>(&lpPtr));

				auto itPoint = LightPoint::GetType()->GetIterator();
				MUint32 iPoint = 0;
				for (;
					iPoint < COUNT_LIGHT_POINT && itPoint.IsValid();
					++itPoint)
				{
					LightPoint* light = itPoint.Get<LightPoint>();
					if (light->GetEnabled() && light->GetVisible())
					{
						lpPtr->_colPoint[iPoint] = light->GetColor();
						lpPtr->_posPoint[iPoint] = light->GetPosition();
						lpPtr->_rangePoint[iPoint] = light->GetRange();
						++iPoint;
					}
				}
				lpPtr->_numPoint = iPoint;

				_plBuffer->Unmap();
				_plBuffer->SetPS(3);

				// SPOT LIGHTS

				LightSpotBufferData* lsPtr;
				_slBuffer->Map(reinterpret_cast<void**>(&lsPtr));

				auto itSpot = LightSpot::GetType()->GetIterator();
				MUint32 iSpot = 0;
				for (;
					iSpot < COUNT_LIGHT_SPOT && itSpot.IsValid();
					++itSpot)
				{
					LightSpot* light = itSpot.Get<LightSpot>();
					if (light->GetEnabled() && light->GetVisible())
					{
						lsPtr->_colSpot[iSpot] = light->GetColor();
						lsPtr->_posSpot[iSpot] = light->GetPosition();
						lsPtr->_dirSpot[iSpot] = -light->GetDirection();
						lsPtr->_rangeSpot[iSpot] = light->GetRange();
						lsPtr->_angleCosSpot[iSpot] = MMath::Cos(light->GetConeAngle());
						lsPtr->_smoothSpot[iSpot] = /*1.0f / MMath::Max(*/light->GetSmooth()/*, 0.0001f)*/;
						++iSpot;
					}
				}
				lsPtr->_numSpot = iSpot;

				_slBuffer->Unmap();
				_slBuffer->SetPS(4);

				// AREA LIGHTS

				LightAreaBufferData* laPtr;
				_alBuffer->Map(reinterpret_cast<void**>(&laPtr));

				auto itArea = LightArea::GetType()->GetIterator();
				MUint32 iArea = 0;
				for (;
					iArea < COUNT_LIGHT_AREA && itArea.IsValid();
					++itArea)
				{
					LightArea* light = itArea.Get<LightArea>();
					if (light->GetEnabled() && light->GetVisible())
					{
						laPtr->_colArea[iArea] = light->GetColor();
						laPtr->_posArea[iArea] = light->GetPosition();
						laPtr->_dirArea[iArea] = -light->GetDirection();
						laPtr->_rightArea[iArea] = light->GetRight();
						laPtr->_sizeRangeSmoothArea[iArea].X = light->GetSize().X;
						laPtr->_sizeRangeSmoothArea[iArea].Y = light->GetSize().Y;
						laPtr->_sizeRangeSmoothArea[iArea].Z = light->GetRange();
						laPtr->_sizeRangeSmoothArea[iArea].W = 1.0f / MMath::Max(light->GetSmooth(), 0.01f);
						++iArea;
					}
				}
				laPtr->_numArea = iArea;

				_alBuffer->Unmap();
				_alBuffer->SetPS(5);
			}
		}
	}
}
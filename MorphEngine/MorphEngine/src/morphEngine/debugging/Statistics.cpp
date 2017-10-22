#include "Statistics.h"
#include "core/Engine.h"
#include "core/ITime.h"
#include "gom/Transform.h"
#include "gom/GUITransform.h"
#include "renderer/GUIBatch.h"
#include "renderer/GUIText.h"
#include "assetLibrary/MMaterial.h"
#include "renderer/RenderingManager.h"

namespace morphEngine
{
	using namespace gom;
	using namespace memoryManagement;
	using namespace resourceManagement;
	using namespace renderer;
	using namespace core;

	namespace debugging
	{
		Statistics::Statistics(const gom::ObjectInitializer & initializer) :
			gom::GameObjectComponent(initializer),
			_textColor(MColor::Green),
			_basePosition(MVector2(60.0f, -48.0f)),
			_baseAnchor(MVector2(-1.0f, 1.0f)),
			_baseOrigin(MVector2(-1.0f, 1.0f)),
			_valOffsetX(96.0f),
			_baseSizeY(32.0f),
			_timeUpdateInterval(0.25f),
			_timeUpdateTimer(0.0f)
		{
		}

		Statistics::Statistics(const Statistics & c, bool bDeepCopy) : 
			gom::GameObjectComponent(c, bDeepCopy),
			_textColor(c._textColor),
			_basePosition(c._basePosition),
			_baseAnchor(c._baseAnchor),
			_baseOrigin(c._baseOrigin),
			_valOffsetX(c._valOffsetX),
			_baseSizeY(c._baseSizeY),
			_timeUpdateInterval(c._timeUpdateInterval),
			_timeUpdateTimer(c._timeUpdateTimer),
			_data(c._data),
			_batch(c._batch),
			_descriptions(c._descriptions),
			_valFpsCount(c._valFpsCount),
			_valMsCount(c._valMsCount),
			_valSetGlobalCount(c._valSetGlobalCount),
			_valSetMaterialCount(c._valSetMaterialCount),
			_valDrawCallCount(c._valDrawCallCount),
			_valVertexCount(c._valVertexCount),
			_valTriangleCount(c._valTriangleCount)
		{

		}

		Statistics::~Statistics()
		{
		}

		void Statistics::Initialize()
		{
			// create new components and game objects only if they are not loaded from Serialization

			if(!_batch.IsValid())
				_batch = _owner->AddComponentGet<GUIBatch>();

			if (!_descriptions.IsValid())
			{
				Handle<GameObject> descriptionsObj;
				GameObject::Create(_owner->GetName() + "_descriptions", true, true, descriptionsObj);
				descriptionsObj->GetTransform()->SetParent(_owner->GetTransform());
				_descriptions = descriptionsObj->AddComponentGet<GUIText>();
			}

			if (!_valFpsCount.IsValid())
			{
				Handle<GameObject> fpsCountObj;
				GameObject::Create(_owner->GetName() + "_fpsCount", true, true, fpsCountObj);
				fpsCountObj->GetTransform()->SetParent(_owner->GetTransform());
				_valFpsCount = fpsCountObj->AddComponentGet<GUIText>();
			}

			if (!_valMsCount.IsValid())
			{
				Handle<GameObject> msCountObj;
				GameObject::Create(_owner->GetName() + "_msCount", true, true, msCountObj);
				msCountObj->GetTransform()->SetParent(_owner->GetTransform());
				_valMsCount = msCountObj->AddComponentGet<GUIText>();
			}

			if (!_valSetGlobalCount.IsValid())
			{
				Handle<GameObject> setGlobalCountObj;
				GameObject::Create(_owner->GetName() + "_setGlobalCount", true, true, setGlobalCountObj);
				setGlobalCountObj->GetTransform()->SetParent(_owner->GetTransform());
				_valSetGlobalCount = setGlobalCountObj->AddComponentGet<GUIText>();
			}

			if (!_valSetMaterialCount.IsValid())
			{
				Handle<GameObject> setMaterialCountObj;
				GameObject::Create(_owner->GetName() + "_setMaterialCount", true, true, setMaterialCountObj);
				setMaterialCountObj->GetTransform()->SetParent(_owner->GetTransform());
				_valSetMaterialCount = setMaterialCountObj->AddComponentGet<GUIText>();
			}

			if (!_valDrawCallCount.IsValid())
			{
				Handle<GameObject> drawCallCountObj;
				GameObject::Create(_owner->GetName() + "_drawCallCount", true, true, drawCallCountObj);
				drawCallCountObj->GetTransform()->SetParent(_owner->GetTransform());
				_valDrawCallCount = drawCallCountObj->AddComponentGet<GUIText>();
			}

			if (!_valVertexCount.IsValid())
			{
				Handle<GameObject> vertexCountObj;
				GameObject::Create(_owner->GetName() + "_vertexCount", true, true, vertexCountObj);
				vertexCountObj->GetTransform()->SetParent(_owner->GetTransform());
				_valVertexCount = vertexCountObj->AddComponentGet<GUIText>();
			}

			if (!_valTriangleCount.IsValid())
			{
				Handle<GameObject> triangleCountObj;
				GameObject::Create(_owner->GetName() + "_triangleCount", true, true, triangleCountObj);
				triangleCountObj->GetTransform()->SetParent(_owner->GetTransform());
				_valTriangleCount = triangleCountObj->AddComponentGet<GUIText>();
			}

			_descriptions->SetText
			(
				"FPS: \nDelta time: \nSetGlobal calls: \nSetMaterial calls: \nDraw calls: \nVertices: \nTriangles: "
			);
			_valFpsCount->SetText("00.00");
			_valMsCount->SetText("00.00");
			_valSetGlobalCount->SetText("0");
			_valSetMaterialCount->SetText("0");
			_valDrawCallCount->SetText("0");
			_valVertexCount->SetText("0");
			_valTriangleCount->SetText("0");

			_texts[0] = _descriptions;
			_texts[1] = _valFpsCount;
			_texts[2] = _valMsCount;
			_texts[3] = _valSetGlobalCount;
			_texts[4] = _valSetMaterialCount;
			_texts[5] = _valDrawCallCount;
			_texts[6] = _valVertexCount;
			_texts[7] = _valTriangleCount;

			MVector2 valPosition(MVector2(_valOffsetX, _descriptions->GetGUITransform()->GetSize().Y * 0.21f) + _basePosition);
			for (MSize i = 0; i < TEXT_COUNT; ++i)
			{
				_texts[i]->SetAlignment(meshes::MeshText::Alignment::LEFT);
				_texts[i]->GetGUITransform()->SetOrigin(_baseOrigin);
				_texts[i]->GetGUITransform()->SetAnchor(_baseAnchor);
				_texts[i]->GetMaterial()->SetColor(_textColor, 0);
				_texts[i]->GetGUITransform()->SetSizeProportionalY(_baseSizeY);

				if (i == 0)
				{
					_texts[i]->GetGUITransform()->SetPosition(_basePosition);
				}
				else
				{
					_texts[i]->GetGUITransform()->SetPosition(valPosition);
				}

				valPosition.Y -= _baseSizeY * 0.5f;
			}

			RenderingManager::GetInstance()->RegisterRenderStatisticsComponent(static_cast<Handle<Statistics>>(_this));
		}

		void Statistics::Shutdown()
		{

		}

		void Statistics::Update()
		{
			// data is externally set

			if (_timeUpdateTimer < _timeUpdateInterval)
				_timeUpdateTimer += Engine::GetInstance()->GetGlobalTime().GetUnscaledDeltaTime();
			else
			{
				_timeUpdateTimer = 0.0f;
				_valFpsCount->SetText(MString::FromFloat(_data.Fps));
				_valMsCount->SetText(MString::FromFloat(_data.Ms));
			}

			_valSetGlobalCount->SetText(MString::FromInt(_data.SetGlobalCount));
			_valSetMaterialCount->SetText(MString::FromInt(_data.SetMaterialCount));
			_valDrawCallCount->SetText(MString::FromInt(_data.DrawCallCount));
			_valVertexCount->SetText(MString::FromInt(_data.VertexCount));
			_valTriangleCount->SetText(MString::FromInt(_data.TriangleCount));

			_data.Clear();
		}
	}
}
#pragma once
#include "core/GlobalDefines.h"
#include "gom/GameObjectComponent.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MFont;
		class MMesh;
	}

	namespace renderer
	{
		class RenderingManager;
		class GUIBatch;
		class GUIText;
	}

	namespace debugging
	{
		class Statistics : public gom::GameObjectComponent
		{
			ME_TYPE

			friend class renderer::RenderingManager;
		public:

			struct Data
			{
				MFloat32 Fps = 0.0f;
				MFloat32 Ms = 0.0f;
				MSize SetGlobalCount = 0;
				MSize SetMaterialCount = 0;
				MSize DrawCallCount = 0;
				MSize VertexCount = 0;
				MSize TriangleCount = 0;

				void Clear()
				{
					Fps = 0.0f;
					Ms = 0.0f;
					SetGlobalCount = 0;
					SetMaterialCount = 0;
					DrawCallCount = 0;
					VertexCount = 0;
					TriangleCount = 0;
				}
			};

		protected:

#pragma region Protected

			static const MSize TEXT_COUNT = 8;

			utility::MColor _textColor;
			utility::MVector2 _basePosition;
			utility::MVector2 _baseAnchor;
			utility::MVector2 _baseOrigin;
			MFloat32 _valOffsetX;
			MFloat32 _baseSizeY;
			MFloat32 _timeUpdateInterval;
			MFloat32 _timeUpdateTimer;

			Data _data;

			memoryManagement::Handle<renderer::GUIBatch> _batch;
			memoryManagement::Handle<renderer::GUIText> _descriptions;
			memoryManagement::Handle<renderer::GUIText> _valFpsCount;
			memoryManagement::Handle<renderer::GUIText> _valMsCount;
			memoryManagement::Handle<renderer::GUIText> _valSetGlobalCount;
			memoryManagement::Handle<renderer::GUIText> _valSetMaterialCount;
			memoryManagement::Handle<renderer::GUIText> _valDrawCallCount;
			memoryManagement::Handle<renderer::GUIText> _valVertexCount;
			memoryManagement::Handle<renderer::GUIText> _valTriangleCount;

			memoryManagement::Handle<renderer::GUIText> _texts[TEXT_COUNT];

#pragma endregion

#pragma region Functions Protected

			Statistics(const gom::ObjectInitializer& initializer);

#pragma endregion

		public:

#pragma region Functions Public

			Statistics(const Statistics& c, bool bDeepCopy = true);
			virtual ~Statistics();

			virtual void Initialize() override;
			virtual void Shutdown() override;

			virtual void Update() override;

			inline void SetData(const Data& data) { _data = data; }
			inline const Data& GetData() const { return _data; }

#pragma endregion

		};
	}
}
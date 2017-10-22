#pragma once
#include "gom/GameObjectComponent.h"
#include "gom/Scene.h"
#include "assetLibrary/MMaterial.h"

namespace morphEngine
{
	namespace renderer
	{
		class RenderTarget2D;

		namespace device
		{
			struct StagingTexture2DResource;
		}

		namespace shaders
		{
			class ShaderGameObjectID;
		}
	}

	namespace gom
	{
		class ObjectClickEvaluator :
			public GameObjectComponent
		{
			ME_TYPE
		protected:

#pragma region Static Const Protected

			static const MInt32 RT_SIZE_DIVISOR = 4;

#pragma endregion

#pragma region Protected

			renderer::RenderTarget2D* _renderTarget;
			renderer::device::StagingTexture2DResource* _stagingTexture;
			renderer::shaders::ShaderGameObjectID* _shader;
			assetLibrary::MMaterial _tempMat;

			bool _bHasDrawnThisFrame = false;

#pragma endregion

#pragma region Functions Protected

#pragma endregion

			ObjectClickEvaluator(const ObjectInitializer& initializer) : GameObjectComponent(initializer) { }
			
			void DrawGameObjects(RenderSetCameraPair & camera);
			MUint64 GetClickedID(RenderSet& set, utility::MVector2 mousePosImageSpace);

		public:

#pragma region Functions Public

#pragma endregion


			ObjectClickEvaluator(const ObjectClickEvaluator& c, bool bDeepCopy = true);
			ObjectClickEvaluator& operator=(const ObjectClickEvaluator& c);
			virtual ~ObjectClickEvaluator() { }

			virtual void Initialize() override;
			virtual void Shutdown() override;

			// For testing and clearing obsolete click data.
			virtual void Update() override;

			Handle<GameObject> GetClickedGameObject(RenderSetCameraPair& camera, utility::MVector2 mousePosImageSpace);
		};

	}
}


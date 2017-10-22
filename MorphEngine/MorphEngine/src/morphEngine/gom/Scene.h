#pragma once

#include "GameObject.h"
#include "memoryManagement/Handle.h"
#include "renderer/OctTree.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MMaterial;
	}

	namespace renderer
	{
		class Renderer;
		class RendererStaticMesh;
		class GUIBatch;
		class LightAmbient;
		class LightDirectional;
		class LightPoint;
		class LightSpot;
		class LightArea;
	}

	namespace gom
	{
		class Camera;

		struct RenderSet
		{
			struct RendererIndexPair
			{
				renderer::Renderer* RendererPtr;
				MSize MaterialIndex;

				RendererIndexPair() : RendererPtr(nullptr), MaterialIndex(-1) { }
				RendererIndexPair(renderer::Renderer* ptr, MSize index) : RendererPtr(ptr), MaterialIndex(index) { }
				RendererIndexPair(const RendererIndexPair& c) : RendererPtr(c.RendererPtr), MaterialIndex(c.MaterialIndex) { }
				RendererIndexPair& operator=(const RendererIndexPair& c)
				{
					RendererPtr = c.RendererPtr;
					MaterialIndex = c.MaterialIndex;
					return *this;
				}

				bool operator==(const RendererIndexPair& o) const { return RendererPtr == o.RendererPtr && MaterialIndex == o.MaterialIndex; }
				bool operator!=(const RendererIndexPair& o) const { return !operator==(o); }

				inline operator renderer::Renderer*() { return RendererPtr; }
				inline operator const renderer::Renderer*() const { return RendererPtr; }
			};

			MDictionary<MUint64, memoryManagement::Handle<gom::GameObject>> GameObjectsPerID;

			// per-shader, per-material, renderer-index-pair
			MArray<MArray<MArray<RendererIndexPair>>> RenderersOpaqueGrouped;

			MArray<RendererIndexPair> RenderersOpaqueFrontToBack;
			MArray<RendererIndexPair> RenderersTransparentBackToFront;
			MArray<renderer::GUIBatch*> GUIBatchesBeforePostprocessing;
			MArray<renderer::GUIBatch*> GUIBatchesAfterPostprocessing;
			renderer::Renderer* Skybox;

			// Handle<renderer::Skybox> Skyb;
			renderer::LightAmbient* LightAmb;
			MArray<renderer::LightDirectional*> LightsDirectional;
			MArray<renderer::LightPoint*> LightsPoint;
			MArray<renderer::LightSpot*> LightsSpot;
			MArray<renderer::LightArea*> LightsArea;

			bool operator==(const RenderSet& o) const
			{
				ME_ASSERT(false, "Not implmented, should not be called.");
				return false;
			}

			bool operator!=(const RenderSet& o) const
			{
				ME_ASSERT(false, "Not implmented, should not be called.");
				return false;
			}

			void Clear()
			{
				GameObjectsPerID.Clear();
				RenderersOpaqueGrouped.Clear();
				RenderersOpaqueFrontToBack.Clear();
				RenderersTransparentBackToFront.Clear();
				GUIBatchesBeforePostprocessing.Clear();
				GUIBatchesAfterPostprocessing.Clear();
				LightAmb = nullptr;
				LightsDirectional.Clear();
				LightsPoint.Clear();
				LightsSpot.Clear();
				LightsArea.Clear();
			}
		};

		struct RenderSetCameraPair
		{
			RenderSet Set;
			Handle<Camera> Cam;

			RenderSetCameraPair() { }
			RenderSetCameraPair(Handle<Camera> cam) : Cam(cam) { }

			bool operator==(const RenderSetCameraPair o) const
			{
				return Cam == o.Cam;
			}

			bool operator!=(const RenderSetCameraPair o) const
			{
				return Cam != o.Cam;
			}
		};

		class Scene
		{
			friend class SceneManager;

		protected:

#pragma region Protected

			renderer::OctTree _octTree;

			MString _path;
			MString _name;
			MFixedArray<MUint8> _rawData;

			MArray<RenderSetCameraPair> _cameras;
			MArray<Handle<GameObject>> _gameObjects;
			MArray<Handle<GameObject>> _gameObjectsDeferredInit;
			MArray<Handle<renderer::OctTreeElement>> _octTreeElementsDeferredInit;

			// octrree goes here, consisiting of some IQuadtreeable objects having bool IsVisible(CameraFrustum) or sth like that

			bool _bIsInitializationInProgress = false;
			bool _bIsInitialized = false;

#pragma endregion

#pragma region Functions Protected

			inline void LoadDeferredGameObjects();
			inline void GenerateRenderSets();
			inline void GenerateRenderSet(RenderSetCameraPair& pair);

			static MInt8 CompareByDistanceToCameraFrontToBack(RenderSet::RendererIndexPair* a, RenderSet::RendererIndexPair* b, void* cameraInstancePtr);
			static MInt8 CompareByDistanceToCameraBackToFront(RenderSet::RendererIndexPair* a, RenderSet::RendererIndexPair* b, void* cameraInstancePtr);

#pragma endregion

		public:
#pragma region Public functions

			Scene(const MString& scenePath);
			Scene(const MString& scenePath, const MFixedArray<MUint8>& rawData);
			~Scene();

			void Shutdown();

			void Update();
			void Draw();

			void AddGameObject(Handle<GameObject> gameObject);
			void RemoveGameObject(Handle<GameObject> gameObject);
			void SaveScene();
			void LoadScene();
			// Test only
			void LoadFromGameObjects(MArray<Handle<GameObject>>& gameObjects);

#pragma region RegisterComponents

			void RegisterCamera(Handle<Camera> cam);
			void RegisterCameraAsMain(Handle<Camera> cam);
			void RegisterOctTreeElement(Handle<renderer::OctTreeElement> element);


			void DeregisterCamera(Handle<Camera> cam);
			void DeregisterOctTreeElement(Handle<renderer::OctTreeElement> element);

#pragma endregion

			inline RenderSetCameraPair GetMainCameraPair() { return _cameras[0]; }
			inline const MArray<Handle<GameObject>>& GetGameObjects() const
			{
				return _gameObjects;
			}

			inline const MString& GetName() const { return _name; }
			inline bool GetIsInitialized() const { return _bIsInitialized; }
			inline bool GetIsInitializationInProgress() const { return _bIsInitializationInProgress; }

			void* operator new(MSize size);
			void operator delete(void* p, MSize size);

#pragma endregion
		};

	}
}
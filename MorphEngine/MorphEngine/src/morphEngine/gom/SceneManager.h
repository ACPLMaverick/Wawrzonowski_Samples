#pragma once
#include <vector>

#include "Scene.h"
#include "core\Config.h"
#include "core\Singleton.h"
#include "utility\Collections.h"

namespace morphEngine
{
	namespace gom
	{
		class Camera;


		class SceneManager :
			public core::Singleton<SceneManager>
		{
		protected:
#pragma region Protected
			
			core::Config _sceneConfig;
			Scene* _currentScene;

			memoryManagement::Handle<gom::GameObject> _editorObject;
			RenderSetCameraPair _editorCamera;

#pragma endregion
		public:
#pragma region Public functions

			SceneManager();
			~SceneManager();

			void Initialize();
			void Shutdown();

			void Update();
			void Draw();

			void InitializeEditor();
			void ShutdownEditor();

			void UpdateEditor();
			void DrawEditor();

			void OpenScene(const MString& scenePath);
			void OpenScene(const MString& scenePath, const MFixedArray<MUint8>& rawData);
			// Test only
			void OpenSceneFromGameObjects(utility::MArray<Handle<GameObject>>& gameObjects, const utility::MString& path = "NoPath");
			void CloseCurrentScene();

			inline Scene* GetScene() const
			{
				return _currentScene;
			}

			inline memoryManagement::Handle<Camera> GetEditorCamera() { return _editorCamera.Cam; }

#define GetCurrentScene() GetScene()

#pragma endregion
		};

	}
}
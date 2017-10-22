#include "SceneManager.h"
#include "Camera.h"
#include "input/InputComponent.h"
#include "gom/EditorCameraController.h"
#include "renderer\RenderingManager.h"

#define SCENE_MANAGER_CONFIG_PATH "Configs/scenes.conf"
#define SCENE_MANAGER_DEFAULT_SCENE_KEY "DefaultScene"

using namespace morphEngine::core;
using namespace morphEngine::renderer;
using namespace morphEngine::utility;

namespace morphEngine
{
	using namespace memoryManagement;
	using namespace input;

	namespace gom
	{

		SceneManager::SceneManager() : _currentScene(nullptr)
		{
		}


		SceneManager::~SceneManager()
		{
		}

		void SceneManager::Initialize()
		{
			_sceneConfig.Read(SCENE_MANAGER_CONFIG_PATH);
			Config::ValueType values = _sceneConfig.GetValues(SCENE_MANAGER_DEFAULT_SCENE_KEY);
			if(values.GetSize() > 0)
			{
				OpenScene(values[0]);
			}
		}

		void SceneManager::Shutdown()
		{
			CloseCurrentScene();
		}

		void SceneManager::Update()
		{
			if (_currentScene != nullptr)
			{
				_currentScene->Update();
			}
		}

		void SceneManager::Draw()
		{
			if (_currentScene != nullptr)
			{
				_currentScene->Draw();
			}
		}

		void SceneManager::InitializeEditor()
		{
			// for now
			Initialize();

			// initialize editor-controller game object
			ObjectInitializer initializer(true);
			GameObject::GetType()->CreateInstance(initializer, _editorObject);
			_editorObject->SetName("EditorObject");
			_editorObject->SetEnabled(true);
			_editorObject->SetVisible(true);

			_editorCamera.Cam = _editorObject->AddComponentGetManualInitialize<Camera>();
			_editorCamera.Cam->SetRenderMode(CameraRenderMode::DEFERRED);
			_editorCamera.Cam->SetIsMain(true);
			_editorCamera.Cam->SetWidthHeight(640, 480);		// where do I get these?
			_editorCamera.Cam->SetResizesWithWindow(true);
			_editorCamera.Cam->Initialize();

			_editorObject->AddComponent<EditorCameraController>();

			_editorObject->GetTransform()->SetPosition(MVector3(5.0f, 5.0f, -5.0f));
			_editorObject->GetTransform()->SetDirection(MVector3(-1.0f, -0.5f, 1.0f));

			_editorObject->Initialize();
		}

		void SceneManager::ShutdownEditor()
		{
			//_editorObject->Shutdown();
			//_editorObject->Type.DestroyInstance(_editorObject);
			
			// for now
			Shutdown();
		}

		void SceneManager::UpdateEditor()
		{
			// update editor camera gameobject only in a classic way (not using type iterators)
			const MArray<Handle<GameObjectComponent>>& comps = _editorObject->GetComponents();
			for (MSize i = 0; i < comps.GetSize(); ++i)
			{
				comps[i]->Update();
			}

			_editorObject->GetTransform()->Update();

			// Generate render set for editor camera
			if (_currentScene != nullptr)
			{
				_currentScene->GenerateRenderSet(_editorCamera);
			}

			// Any necessary editor logic needs to be solved in EditorCameraController's Update
		}

		void SceneManager::DrawEditor()
		{
			_editorCamera.Cam->DrawRenderers(_editorCamera.Set);
			RenderingManager::GetInstance()->GetDevice()->CopyToBackBuffer(_editorCamera.Cam->GetRenderTarget()->GetResource());
		}

		void SceneManager::OpenScene(const MString& scenePath)
		{
			if(_currentScene != nullptr)
			{
				_currentScene->Shutdown();
				delete _currentScene;
			}

			_currentScene = new Scene(scenePath);
			if(_currentScene)
			{
				_currentScene->LoadScene();
			}
		}

		void SceneManager::OpenScene(const MString& scenePath, const MFixedArray<MUint8>& rawData)
		{
			if(_currentScene != nullptr)
			{
				_currentScene->Shutdown();
				delete _currentScene;
			}

			_currentScene = new Scene(scenePath, rawData);
			if(_currentScene)
			{
				_currentScene->LoadScene();
			}
		}

		void SceneManager::OpenSceneFromGameObjects(MArray<Handle<GameObject>>& gameObjects, const utility::MString& path)
		{
			if (_currentScene != nullptr)
			{
				_currentScene->Shutdown();
				delete _currentScene;
			}

			_currentScene = new Scene(path);
			_currentScene->LoadFromGameObjects(gameObjects);
		}

		void SceneManager::CloseCurrentScene()
		{
			if(_currentScene != nullptr)
			{
				_currentScene->Shutdown();
				delete _currentScene;
				_currentScene = 0;
			}
		}
	}
}
#include "Scene.h"
#include "resourceManagement/fileSystem/FileSystem.h"
#include "core/Engine.h"
#include "debugging/Debug.h"
#include "reflection/Type.h"
#include "gom/GameObjectComponent.h"
#include "gom/Camera.h"
#include "renderer/RenderingManager.h"
#include "renderer/Renderer.h"
#include "renderer/GUIBatch.h"
#include "renderer/LightAmbient.h"
#include "renderer/LightDirectional.h"
#include "renderer/LightPoint.h"
#include "renderer/LightSpot.h"
#include "renderer/LightArea.h"
#include "physics\PhysicsManager.h"

using namespace morphEngine::resourceManagement::fileSystem;
using namespace morphEngine::core;
using namespace morphEngine::debugging;
using namespace morphEngine::reflection;

namespace morphEngine
{
	using namespace renderer;

	namespace gom
	{
		Scene::Scene(const MString& scenePath) : _path(MString::Replace(scenePath, '/', '\\'))
		{
			MInt32 lastDotIndex = _path.FindLast('.');
			MInt32 lastSlashIndex = _path.FindLast('\\');
			if(lastDotIndex >= 0)
			{
				if(lastSlashIndex >= 0 && lastSlashIndex < lastDotIndex)
				{
					_name = _path.Substring(lastSlashIndex + 1, lastDotIndex - lastSlashIndex - 1);
				}
				else
				{
					_name = _path.Substring(0, lastDotIndex);
				}
			}
			else
			{
				_name = _path;
			}
			const char* n = _name;
			int i = 0;
		}

		Scene::Scene(const MString& scenePath, const MFixedArray<MUint8>& rawData) : Scene(scenePath)
		{
			MInt32 i = _path.FindLast("\\Resources");
			if(i == -1)
			{
				_path = "Scenes\\" + _name + ".msc";
			}
			else
			{
				MString sub = _path.Substring(i + 1);
				const char* s = sub;
				MInt32 afterRes = sub.FindFirst('\\');
				if(afterRes > -1)
				{
					_path = sub.Substring(afterRes + 1);
				}
			}
			const char* p = _path;

			_rawData = rawData;
		}

		Scene::~Scene()
		{
		}

		void Scene::Shutdown()
		{
			MArray<Handle<GameObject>>::MIteratorArray it = _gameObjects.GetBegin();
			for(MSize i = 0; it.IsValid(); ++it, ++i)
			{
				MSize p = _gameObjects.GetSize();
				(*it)->Shutdown();
				(*it).GetType()->DestroyInstance((*it));
			}
			_gameObjects.Shutdown();

			_octTree.Shutdown();
			
			_bIsInitialized = false;
		}

		void Scene::Update()
		{
			// update components
			for (auto compIterator = GameObjectComponent::GetType()->GetIteratorPolymorphic(); compIterator.IsValid(); ++compIterator)
			{
				compIterator.Get<GameObjectComponent>()->Update();
			}

			// generate render sets
			GenerateRenderSets();
		}

		void Scene::Draw()
		{
			ME_WARNING_RETURN_STATEMENT(!_cameras.IsEmpty(), "There does not exist any camera!");
			// draw only from main camera, rest will be rendered when material they're using is need to be drawn
			GetMainCameraPair().Cam->DrawRenderers(GetMainCameraPair().Set);

			// copy main camera render target data to backbuffer
			RenderingManager::GetInstance()->GetDevice()->CopyToBackBuffer(GetMainCameraPair().Cam->GetRenderTarget()->GetResource());
		}

		void Scene::AddGameObject(Handle<GameObject> gameObject)
		{
			if(gameObject.IsValid())
			{
				if (_bIsInitializationInProgress)
				{
					_gameObjectsDeferredInit.Add(gameObject);
				}
				else
				{
					_gameObjects.Add(gameObject);
					gameObject->Initialize();
				}
			}
		}

		void Scene::RemoveGameObject(Handle<GameObject> gameObject)
		{
			if(gameObject.IsValid())
			{
				if (_bIsInitializationInProgress)
				{
					if (_gameObjectsDeferredInit.Contains(gameObject))
					{
						_gameObjectsDeferredInit.Remove(gameObject);
					}
					else
					{
						_gameObjects.Remove(gameObject);
					}
				}
				else
				{
					_gameObjects.Remove(gameObject);
				}


				MArray<Handle<Transform>>::MIteratorArray it = gameObject->GetTransform()->GetChildrenIterator();
				for(; it.IsValid(); ++it)
				{
					RemoveGameObject((*it)->GetOwner());
				}

				gameObject->Shutdown();
				gameObject.GetType()->DestroyInstance(gameObject);
			}
		}

		void Scene::SaveScene()
		{
			if(Engine::GetInstance() == nullptr)
			{
				return;
			}

			FileSystem& fs = Engine::GetInstance()->GetFileSystem();
			if(!fs.DoesFileExist(_path))
			{
				if(!fs.CreateFile(_path))
				{
					Debug::Log(ELogType::ERR, "Scene file does not exists: " + _path);
					return;
				}
			}

			MArray<Handle<GameObject>>::MIteratorArray it = _gameObjects.GetBegin();
			MArray<MFixedArray<MUint8>> serializedDatas;
			MSize wholeSize = 0;
			for(; it.IsValid(); ++it)
			{
				MFixedArray<MUint8> typeData;
				
				const MString& typeName = (*it)->GetType()->GetName();
				MSize stringLength = typeName.Length();
				typeData.Allocate(sizeof(MSize) + stringLength);
				memcpy(typeData.GetDataPointer(), &stringLength, sizeof(MSize));
				memcpy(typeData.GetDataPointer() + sizeof(MSize), (const char*)typeName, stringLength);

				MFixedArray<MUint8> itData;
				(*it)->Serialize(itData);

				serializedDatas.Add(typeData);
				serializedDatas.Add(itData);
				
				wholeSize += itData.GetSize() + typeData.GetSize();
			}

			if(wholeSize <= 0)
			{
				return;
			}
			MFixedArray<MUint8> wholeData(wholeSize);
			MSize offset = 0;
			MArray<MFixedArray<MUint8>>::MIteratorArray dataIt = serializedDatas.GetBegin();
			MSize dataSize = 0;
			for(; dataIt.IsValid(); ++dataIt)
			{
				dataSize = (*dataIt).GetSize();
				memcpy(wholeData.GetDataPointer() + offset, (*dataIt).GetDataPointer(), dataSize);
				offset += dataSize;
			}

			File& f = fs.OpenFile(_path, FileAccessMode::WRITE);
			if(f.IsOpened())
			{
				f.Write(wholeData, 0);
				fs.CloseFile(f);
			}
		}

		void Scene::LoadScene()
		{
			_bIsInitializationInProgress = true;

			if(Engine::GetInstance() == nullptr)
			{
				return;
			}

			MFixedArray<MUint8> fileData;

			if(_rawData.GetSize() == 0)
			{
				FileSystem& fs = Engine::GetInstance()->GetFileSystem();
				if(!fs.DoesFileExist(_path))
				{
					Debug::Log(ELogType::ERR, "Scene file does not exists.");
					return;
				}

				File& f = fs.OpenFile(_path, FileAccessMode::READ);
				if(!f.IsOpened())
				{
					Debug::Log(ELogType::ERR, "Cannot open scene file.");
					return;
				}

				f.Read(fileData);
				fs.CloseFile(f);
			}
			else
			{
				fileData = _rawData;
			}

			MArray<Handle<GameObject>>::MIteratorArray firstIt = _gameObjects.GetBegin();
			for(; firstIt.IsValid(); ++firstIt)
			{
				if((*firstIt).IsValid())
				{
					(*firstIt)->Shutdown();
					(*firstIt).GetType()->DestroyInstance(*firstIt);
				}
			}
			_gameObjects.Clear();

			MSize index = 0;
			MSize stringLength = 0;
			MString typeName;
			MArray<char> typeChars;
			while(index < fileData.GetSize())
			{
				memcpy(&stringLength, fileData.GetDataPointer() + index, sizeof(MSize));
				typeChars.Resize(stringLength);
				index += sizeof(MSize);
				memcpy(typeChars.GetDataPointer(), fileData.GetDataPointer() + index, stringLength);
				index += stringLength;
				typeName = typeChars;
			
				const Type* t = Assembly::GetInstance()->GetType(typeName);
				if(t != nullptr)
				{
					Handle<GameObject> handle;
					t->CreateInstance(ObjectInitializer(true), handle);
					_gameObjects.Add(handle);
					handle->Deserialize(fileData, index);
					handle->Initialize();
				}
			}

			LoadDeferredGameObjects();

			MArray<Handle<GameObject>>::MIteratorArray it = _gameObjects.GetBegin();
			for(; it.IsValid(); ++it)
			{
				(*it)->PostDeserialize();
			}

			_octTree.Initialize(_octTreeElementsDeferredInit);
			_octTreeElementsDeferredInit.Clear();

			_bIsInitializationInProgress = false;
			_bIsInitialized = true;
		}

		void Scene::LoadFromGameObjects(MArray<Handle<GameObject>>& gameObjects)
		{
			_bIsInitializationInProgress = true;
			for (auto it = gameObjects.GetBegin(); it.IsValid(); ++it)
			{
				_gameObjects.Add((*it));
			}

			for (auto it = gameObjects.GetBegin(); it.IsValid(); ++it)
			{
				(*it)->Initialize();
			}

			LoadDeferredGameObjects();

			_octTree.Initialize(_octTreeElementsDeferredInit, utility::MVector3(100.0f, 100.0f, 100.0f));
			_octTreeElementsDeferredInit.Clear();

			_bIsInitializationInProgress = false;
			_bIsInitialized = true;
		}

		void Scene::RegisterCamera(Handle<Camera> cam)
		{
			_cameras.Add(RenderSetCameraPair(cam));
		}

		void Scene::RegisterOctTreeElement(Handle<renderer::OctTreeElement> element)
		{
			if (!_bIsInitialized)
			{
				_octTreeElementsDeferredInit.Add(element);
			}
			else
			{
				_octTree.Add(element);
			}
		}

		void Scene::RegisterCameraAsMain(Handle<Camera> cam)
		{
			if (_cameras.IsEmpty())
			{
				_cameras.Add(RenderSetCameraPair(cam));
			}
			else if (_cameras[0].Cam->GetIsMain())
			{
				_cameras.Add(RenderSetCameraPair(cam));
			}
			else
			{
				RenderSetCameraPair currentFirst = _cameras[0];
				_cameras[0] = RenderSetCameraPair(cam);
				_cameras.Add(currentFirst);
			}
		}

		void Scene::DeregisterCamera(Handle<Camera> cam)
		{
			_cameras.Remove(RenderSetCameraPair(cam));
		}

		void Scene::DeregisterOctTreeElement(Handle<renderer::OctTreeElement> element)
		{
			if (!_bIsInitialized)
			{
				_octTreeElementsDeferredInit.Remove(element);
			}
			else
			{
				_octTree.Remove(element);
			}
		}

		void* Scene::operator new(MSize size)
		{
			return MemoryManager::GetInstance()->GetSceneAllocator()->Allocate(size);
		}

		void Scene::operator delete(void* p, MSize size)
		{
			MemoryManager::GetInstance()->GetSceneAllocator()->Deallocate(p);
		}

		inline void Scene::LoadDeferredGameObjects()
		{
			while (!_gameObjectsDeferredInit.IsEmpty())
			{
				Handle<GameObject> go = _gameObjectsDeferredInit[0];
				go->Initialize();
				_gameObjects.Add(go);
				_gameObjectsDeferredInit.Remove(go);
			}
		}

		inline void Scene::GenerateRenderSets()
		{
			for (auto it = _cameras.GetBegin(); it.IsValid(); ++it)
			{
				GenerateRenderSet((*it));
			}
		}

		inline void Scene::GenerateRenderSet(RenderSetCameraPair & pair)
		{
			pair.Set.Clear();

			// THIS IMPLMENTATION IS ABSOLUTELY TEMPORARY
			// Not anymore :E

			MArray<Handle<Renderer>> rndrsVisible;
			_octTree.FrustumCull(pair.Cam, pair.Set);

			// perform sorting on both renderer arrays
			//pair.Set.RenderersOpaqueFrontToBack.Sort(&Scene::CompareByDistanceToCameraFrontToBack, pair.Cam.GetPointer());	// this is turned off until occlusion culling is implemented
			pair.Set.RenderersTransparentBackToFront.Sort(&Scene::CompareByDistanceToCameraBackToFront, pair.Cam.GetPointer());

			for (auto it = GUIBatch::GetType()->GetIteratorPolymorphic(); it.IsValid(); ++it)
			{
				GUIBatch* batch = it.Get<GUIBatch>();
				if (batch->GetDrawBeforePostprocessing())
				{
					pair.Set.GUIBatchesBeforePostprocessing.Add(batch);
				}
				else
				{
					pair.Set.GUIBatchesAfterPostprocessing.Add(batch);
				}
			}
		}

		MInt8 Scene::CompareByDistanceToCameraFrontToBack(RenderSet::RendererIndexPair * a, RenderSet::RendererIndexPair * b, void * cameraInstancePtr)
		{
			Camera* cam = reinterpret_cast<Camera*>(cameraInstancePtr);

			MFloat32 distA = (a->RendererPtr->GetOwner()->GetTransform()->GetPosition() - cam->GetPosition()).LengthSquared();
			MFloat32 distB = (b->RendererPtr->GetOwner()->GetTransform()->GetPosition() - cam->GetPosition()).LengthSquared();

			if (distA > distB)
			{
				return 1;
			}
			else if (distA < distB)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}

		MInt8 Scene::CompareByDistanceToCameraBackToFront(RenderSet::RendererIndexPair * a, RenderSet::RendererIndexPair * b, void * cameraInstancePtr)
		{
			Camera* cam = reinterpret_cast<Camera*>(cameraInstancePtr);

			MFloat32 distA = (a->RendererPtr->GetOwner()->GetTransform()->GetPosition() - cam->GetPosition()).LengthSquared();
			MFloat32 distB = (b->RendererPtr->GetOwner()->GetTransform()->GetPosition() - cam->GetPosition()).LengthSquared();

			if (distA > distB)
			{
				return -1;
			}
			else if (distA < distB)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
}
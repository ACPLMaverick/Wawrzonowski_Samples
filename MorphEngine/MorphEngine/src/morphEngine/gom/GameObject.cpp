#include "GameObject.h"
#include "memoryManagement/MemoryManager.h"
#include "gom/GameObjectComponent.h"
#include "gom/Transform.h"
#include "gom/GUITransform.h"
#include "reflection/Type.h"

#include "gom/Scene.h"
#include "gom/SceneManager.h"

namespace morphEngine
{
	namespace gom
	{

		GameObject::GameObject(const ObjectInitializer& initializer) :
			BaseObject(initializer)
		{
			if (!_transform.IsValid())
			{
				if(initializer.IsFakeInitializer())
				{
					Transform::GetType()->CreateInstance(GameObjectComponentInitializer(true, static_cast<memoryManagement::Handle<GameObject>>(_this)), _transform);
				}
				else
				{
					Transform::GetType()->CreateInstance(GameObjectComponentInitializer(static_cast<memoryManagement::Handle<GameObject>>(_this)), _transform);
				}
				_transform->Initialize();
			}
		}

		GameObject::GameObject(const GameObject& c, bool bDeepCopy) :
			BaseObject(c, bDeepCopy),
			_transform(c._transform),
			_components(c._components)
		{
			if (bDeepCopy)
			{
				c._transform.GetType()->CloneInstance(c._transform, _transform);
				_transform->SetOwner(_this);

				MSize size = c._components.GetSize();
				//_components.Resize(size);
				for(MSize i = 0; i < size; ++i)
				{
					c._components[i].GetType()->CloneInstance(c._components[i], _components[i]);
					_components[i]->SetOwner(_this);
				}
			}
		}


		GameObject::~GameObject()
		{
		}

		void GameObject::Initialize()
		{
			BaseObject::Initialize();

			_transform->Initialize();

			MArray<Handle<GameObjectComponent>>::MIteratorArray it = _components.GetBegin();
			for(; it.IsValid(); ++it)
			{
				(*it)->Initialize();
			}
		}

		void GameObject::Shutdown()
		{
			MArray<Handle<GameObjectComponent>>::MIteratorArray it = _components.GetBegin();
			for(; it.IsValid(); ++it)
			{
				if((*it).IsValid())
				{
					(*it)->Shutdown();
					(*it).GetType()->DestroyInstance(*it);
				}
			}

			if(_transform.IsValid())
			{
				_transform->Shutdown();
				const reflection::Type* type = _transform.GetType();
				type->DestroyInstance(static_cast<Handle<BaseObject>>(_transform));
			}

			BaseObject::Shutdown();
		}
		
		void GameObject::Serialize(MFixedArray<MUint8>& outData)
		{
			MArray<MFixedArray<MUint8>> datas;
			MSize wholeDataSize = 0;

			MFixedArray<MUint8> transformData;
			_transform->Serialize(transformData);
			datas.Add(transformData);
			wholeDataSize += transformData.GetSize();

			MFixedArray<MUint8> componentsSizeData(sizeof(MSize));
			MSize componentsSize = _components.GetSize();
			memcpy(componentsSizeData.GetDataPointer(), &componentsSize, sizeof(MSize));
			datas.Add(componentsSizeData);
			wholeDataSize += componentsSizeData.GetSize();

			MArray<Handle<GameObjectComponent>>::MIteratorArray componentsIt = _components.GetBegin();
			for(; componentsIt.IsValid(); ++componentsIt)
			{
				MFixedArray<MUint8> componentInfoData;
				const MString& typeName = (*componentsIt).GetType()->GetName();
				const char* tmp = typeName;
				MSize stringLength = typeName.Length();
				componentInfoData.Allocate(sizeof(MSize) + stringLength);
				memcpy(componentInfoData.GetDataPointer(), &stringLength, sizeof(MSize));
				memcpy(componentInfoData.GetDataPointer() + sizeof(MSize), (const char*)typeName, stringLength);
				datas.Add(componentInfoData);
				wholeDataSize += componentInfoData.GetSize();

				MFixedArray<MUint8> data;
				(*componentsIt)->Serialize(data);
				datas.Add(data);
				wholeDataSize += data.GetSize();
			}

			MFixedArray<MUint8> propertiesData;
			BaseObject::Serialize(propertiesData);
			datas.Add(propertiesData);
			wholeDataSize += propertiesData.GetSize();

			outData.Allocate(wholeDataSize);
			MSize currentOffset = 0;
			MSize dataSize = 0;
			MArray<MFixedArray<MUint8>>::MIteratorArray datasIt = datas.GetBegin();
			for(; datasIt.IsValid(); ++datasIt)
			{
				dataSize = (*datasIt).GetSize();
				memcpy(outData.GetDataPointer() + currentOffset, (*datasIt).GetDataPointer(), dataSize);
				currentOffset += dataSize;
			}
		}

		void GameObject::Deserialize(MFixedArray<MUint8>& serializedData, MSize& index)
		{
			_transform->Deserialize(serializedData, index);
			MSize componentsSize;
			memcpy(&componentsSize, serializedData.GetDataPointer() + index, sizeof(MSize));
			index += sizeof(MSize);
			MemoryManager* m = MemoryManager::GetInstance();
			_components.Clear();

			MString typeName;
			MSize stringLength;
			MArray<char> typeChars;
			for(MSize i = 0; i < componentsSize; ++i)
			{
				memcpy(&stringLength, serializedData.GetDataPointer() + index, sizeof(MSize));
				typeChars.Resize(stringLength);
				index += sizeof(MSize);
				memcpy(typeChars.GetDataPointer(), serializedData.GetDataPointer() + index, stringLength);
				index += stringLength;
				typeName = typeChars;
			
				const char* tmp = *(typeName);
				const reflection::Type* t = reflection::Assembly::GetInstance()->GetType(typeName);
				if(t != nullptr)
				{
					const char* tmp = typeName;
					Handle<GameObjectComponent> handle;
					t->CreateInstance(GameObjectComponentInitializer(true, static_cast<Handle<GameObject>>(_this)), handle);
					handle->Deserialize(serializedData, index);
					_components.Add(handle);
				}
			}

			BaseObject::Deserialize(serializedData, index);
		}

		void GameObject::PostDeserialize()
		{
			_transform->PostDeserialize();
			_transform->SetOwner(_this);

			MArray<Handle<GameObjectComponent>>::MIteratorArray it = _components.GetBegin();
			for(; it.IsValid(); ++it)
			{
				(*it)->PostDeserialize();
				(*it)->SetOwner(_this);
			}

			BaseObject::PostDeserialize();
		}		

		void GameObject::Create(const utility::MString & name, bool enabled, bool visible, memoryManagement::Handle<GameObject>& ret, bool bAddToScene)
		{
			GameObject::Type.CreateInstance(ObjectInitializer(name, enabled, visible), ret);
			if (bAddToScene && SceneManager::GetInstance()->GetScene() != nullptr)
			{
				SceneManager::GetInstance()->GetScene()->AddGameObject(ret);
			}
		}

		GameObject& GameObject::operator=(const GameObject& c)
		{
			BaseObject::operator=(c);
			_components = c._components;
			//ME_WARNING(false, "GameObject: Creating a copy of GameObject - need to copy transform itself");
			_transform = c._transform;
			return *this;
		}

		void GameObject::AddComponent(Handle<GameObjectComponent> component)
		{
			_components.Add(component);
			component->Initialize();
		}

		void GameObject::RemoveComponent(Handle<GameObjectComponent> component)
		{
			if(_components.Contains(component))
			{
				component->Shutdown();
				_components.Remove(component);
			}
		}
		void GameObject::SwitchToGUITransform()
		{
			if (!_transform.GetType()->IsA(gom::GUITransform::GetType()))
			{

				Handle<GUITransform> guiTransform;
				GUITransform::GetType()->CloneInstance(_transform, guiTransform, false);
				_transform->Shutdown();
				_transform->GetType()->DestroyInstance(_transform);
				_transform = guiTransform;
				_transform->Initialize();

				//_transform->Shutdown();
				//_transform->Type.DestroyInstance(_transform);

				//gom::GUITransform::Type.CreateInstance(GameObjectComponentInitializer(_uniqueID != NULL_ID, static_cast<memoryManagement::Handle<GameObject>>(_this)), _transform);
				//_transform->Initialize();
			}
		}
	}
}
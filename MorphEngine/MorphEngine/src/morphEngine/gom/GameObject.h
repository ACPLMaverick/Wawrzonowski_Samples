#pragma once

#include "gom/BaseObject.h"
#include "core/GlobalDefines.h"
#include "core\Delegate.h"

#include <utility/MArray.h>

using namespace morphEngine::memoryManagement;

namespace morphEngine
{
	namespace physics
	{
		class CollisionInfo;
		class TriggerInfo;
		class Rigidbody;
	}
	namespace gom
	{
		class GameObjectComponent;
		class Transform;

		class GameObject :
			public BaseObject
		{
			ME_TYPE

		protected:

#pragma region Protected

			utility::MArray<Handle<GameObjectComponent>> _components;
			
			Handle<Transform> _transform;

#pragma endregion

		public:

#pragma region Public

			core::Event<void, Handle<GameObject>, const physics::CollisionInfo&> OnCollisionStart;
			core::Event<void, Handle<GameObject>, const physics::CollisionInfo&> OnCollisionStay;
			core::Event<void, Handle<GameObject>, const physics::CollisionInfo&> OnCollisionEnd;
			core::Event<void, Handle<GameObject>> OnTriggerStart;
			core::Event<void, Handle<GameObject>> OnTriggerStay;
			core::Event<void, Handle<GameObject>> OnTriggerEnd;

#pragma endregion

		protected:

#pragma region Functions Protected

			GameObject(const ObjectInitializer& initializer);

#pragma endregion


		public:

#pragma region Functions Public

			GameObject(const GameObject& c, bool bDeepCopy = true);
			virtual ~GameObject();

#pragma region BaseObject functions overloaded

			virtual void Initialize();
			virtual void Shutdown();

			virtual void Serialize(MFixedArray<MUint8>& outData);
			virtual void Deserialize(MFixedArray<MUint8>& serializedData, MSize& index);
			virtual void PostDeserialize();

			// Commented out and unusable until further notice.

			//inline static Handle<GameObject> Create(const MString& name = "NoName", bool enabled = true, bool visible = true)
			//{
			//	Handle<GameObject> ret;
			//	GameObject::Type.CreateInstance(ObjectInitializer(name, enabled, visible), ret);
			//	return ret;
			//}

			// Automatically adds GameObject to scene if it is created (unless specified by parameter) (does not have to be fully initialized).
			// If scene is not fully initialized (i.e. this is called during initialization), Initialization of this GameObject will be deferred until
			// all other objects are created and initialized.
			static void Create(const utility::MString& name, bool enabled, bool visible, memoryManagement::Handle<GameObject>& ret, bool bAddToScene = true);

#pragma endregion

#pragma region Operators

			GameObject& operator=(const GameObject& c);

#pragma endregion

#pragma region Accessors

			inline Handle<Transform> GetTransform() const { return _transform; }
			template<typename T>
			inline const Handle<T> GetComponent()
			{
				MArray<Handle<GameObjectComponent>>::MIteratorArray& it = _components.GetBegin();
				for(; it.IsValid(); ++it)
				{
					if((*it).GetType()->IsA(T::GetType()))
					{
						return (*it);
					}
				}

				return Handle<T>();
			}
			inline const utility::MArray<Handle<GameObjectComponent>>& GetComponents() const { return _components; }

			inline void SetTransform(Handle<Transform> transform) { _transform = transform; }
			
			//Be careful! This function should not be used in game!
			//This function exists so editor can add existing (created) component to game object
			void AddComponent(Handle<GameObjectComponent> component);

			template<typename T>
			inline Handle<T> AddComponentGet()
			{
				const morphEngine::reflection::Type* t = T::GetType();
				Handle<T> outObject;
				if(t == nullptr)
				{
					return outObject;
				}
				if (_uniqueID != NULL_ID)
				{
					t->CreateInstance(GameObjectComponentInitializer(_this), outObject);
				}
				else
				{
					t->CreateInstance(GameObjectComponentInitializer(true, _this), outObject);
				}
				_components.Add(outObject);
				outObject->Initialize();
				return outObject;
			}

			template<typename T>
			inline Handle<T> AddComponentGetManualInitialize()
			{
				const morphEngine::reflection::Type* t = T::GetType();
				Handle<T> outObject;
				if(t == nullptr)
				{
					return outObject;
				}

				if (_uniqueID != NULL_ID)
				{
					t->CreateInstance(GameObjectComponentInitializer(_this), outObject);
				}
				else
				{
					t->CreateInstance(GameObjectComponentInitializer(true, _this), outObject);
				}
				_components.Add(outObject);
				return outObject;
			}

			template<typename T>
			inline void AddComponent()
			{
				const morphEngine::reflection::Type* t = T::GetType();
				if(t == nullptr)
				{
					return;
				}
				Handle<T> object;
				if (_uniqueID != NULL_ID)
				{
					t->CreateInstance(GameObjectComponentInitializer(_this), object);
				}
				else
				{
					t->CreateInstance(GameObjectComponentInitializer(true, _this), object);
				}
				_components.Add(object);
				object->Initialize();
			}

			void RemoveComponent(Handle<GameObjectComponent> component);

			void SwitchToGUITransform();

#pragma endregion

#pragma endregion

		};

	}
}
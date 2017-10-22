#pragma once

#include "gom/GameObject.h"

namespace morphEngine
{
	namespace gom
	{
		class Camera;

		class GameObjectComponentInitializer : public ObjectInitializer
		{
			friend class Scene;
			friend class SceneManager;
			friend class GameObject;

		protected:
			GameObjectComponentInitializer(bool bFakeInitializer, memoryManagement::Handle<GameObject> owner) : ObjectInitializer(bFakeInitializer), _owner(owner)
			{

			}

		public:
			memoryManagement::Handle<GameObject> _owner;
		
		public:
			GameObjectComponentInitializer(memoryManagement::Handle<GameObject> owner, const utility::MString& name = "NoName", bool enabled = true, bool visible = true) :
				ObjectInitializer(name, enabled, visible),
				_owner(owner)
			{

			}
		};

		class GameObjectComponent :
			public BaseObject
		{
			ME_TYPE

		protected:

			memoryManagement::Handle<GameObject> _owner;

			inline GameObjectComponent(const ObjectInitializer& initializer) :
				BaseObject(initializer),
				_owner(reinterpret_cast<const GameObjectComponentInitializer&>(initializer)._owner)
			{
				//if(_owner.IsValid() && !_owner.IsStoringID() && _this.IsValid())
				//	_owner->AddComponent(static_cast<memoryManagement::Handle<GameObjectComponent>>(_this));
			}

			virtual void RegisterComponent() { }
			virtual void DeregisterComponent() { }

		public:

#pragma region Functions Public
			inline GameObjectComponent(const GameObjectComponent& c, bool bDeepCopy = true) : BaseObject(c, bDeepCopy), _owner(c._owner) { }
			inline virtual ~GameObjectComponent() { }

			// Called post-constructor and post-initialize-properties
			virtual void Initialize()
			{
				BaseObject::Initialize();
				RegisterComponent();
			}
			// Called pre-deallocation
			virtual void Shutdown() {
				DeregisterComponent();
				BaseObject::Shutdown();
			}

			virtual void Update();
			virtual void Draw(const memoryManagement::Handle<gom::Camera> camera) const;

			virtual GameObjectComponent& operator=(const GameObjectComponent& c) { return reinterpret_cast<GameObjectComponent&>(BaseObject::operator=(c)); }

#pragma region Accessors

			inline memoryManagement::Handle<GameObject> GetOwner() const { return _owner; }
			inline void SetOwner(Handle<GameObject> owner) { _owner = owner; }

			virtual inline bool GetEnabled() override { return _bEnabled && _owner->GetEnabled(); }
			virtual inline bool GetVisible() override { return _bVisible && _owner->GetVisible(); }

#pragma endregion

#pragma endregion
		};

	}
}
#include "Rigidbody.h"

#include "PhysicsManager.h"
#include "gom\Transform.h"

using namespace morphEngine::gom;
using namespace physx;

namespace morphEngine
{
	namespace physics
	{

		void Rigidbody::PreSimulate()
		{
			if(_physxActor != nullptr)
			{
				_physxActor->setGlobalPose(_owner->GetTransform()->GetPhysxTransform());
			}
		}

		void Rigidbody::PostSimulate()
		{
			if(_physxActor != nullptr)
			{
				_owner->GetTransform()->SetPhysxTransform(_physxActor->getGlobalPose());
			}
		}

		void Rigidbody::OnCollisionStart(Handle<GameObject> otherGameObject, const CollisionInfo& collisionInfo)
		{
			_owner->OnCollisionStart(otherGameObject, collisionInfo);
		}

		void Rigidbody::OnCollisionStay(Handle<GameObject> otherGameObject, const CollisionInfo& collisionInfo)
		{
			_owner->OnCollisionStay(otherGameObject, collisionInfo);
		}

		void Rigidbody::OnCollisionEnd(Handle<GameObject> otherGameObject, const CollisionInfo& collisionInfo)
		{
			_owner->OnCollisionEnd(otherGameObject, collisionInfo);
		}

		void Rigidbody::OnTriggerStart(Handle<GameObject> otherGameObject)
		{
			_owner->OnTriggerStart(otherGameObject);
		}

		void Rigidbody::OnTriggerStay(Handle<GameObject> otherGameObject)
		{
			_owner->OnTriggerStay(otherGameObject);
		}

		void Rigidbody::OnTriggerEnd(Handle<GameObject> otherGameObject)
		{
			_owner->OnTriggerEnd(otherGameObject);
		}

		void Rigidbody::AddToScene()
		{
			if(!_bWasAddedToScene)
			{
				_bWasAddedToScene = true;
				PhysicsManager::GetInstance()->AddStaticRigidbody(_this);
			}
		}

		Rigidbody::Rigidbody(const ObjectInitializer& initializer) : GameObjectComponent(initializer), _physxActor(nullptr), _physxActorData(static_cast<Handle<Rigidbody>>(_this)), _bWasAddedToScene(false)
		{
		
		}
		
		Rigidbody::Rigidbody(const Rigidbody& other, bool bDeepCopy) : GameObjectComponent(other, bDeepCopy), _physxActor(nullptr), _physxActorData(static_cast<Handle<Rigidbody>>(_this)), _bWasAddedToScene(false)
		{
		
		}

		void Rigidbody::Initialize()
		{
			GameObjectComponent::Initialize();

			if(_physxActor == nullptr)
			{
				_physxActor = PhysicsManager::GetInstance()->CreateStaticRigidbody(_owner->GetTransform()->GetPhysxTransform());
				if(_physxActor)
				{
					_physxActorData.Actor = _this;
					_physxActor->userData = &_physxActorData;
				}
			}
		}

		void Rigidbody::Shutdown()
		{
			if(_physxActor != nullptr)
			{
				PhysicsManager::GetInstance()->RemoveStaticRigidbody(_this);
				_physxActor->release();
			}

			GameObjectComponent::Shutdown();
		}

		void Rigidbody::Sleep()
		{

		}

		void Rigidbody::WakeUp()
		{
		
		}
	}
}
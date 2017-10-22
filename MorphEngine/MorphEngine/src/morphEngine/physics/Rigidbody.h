#pragma once

#include "gom\GameObjectComponent.h"

#include "Collider.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "MeshCollider.h"
#include "FixedJoint.h"
#include "HingeJoint.h"

namespace morphEngine
{
	namespace physics
	{
		class CollisionInfo;
		class TriggerInfo;

		class PhysxActorData
		{
		public:
			Handle<Rigidbody> Actor;

			inline PhysxActorData(Handle<Rigidbody> actor) : Actor(actor) { }
		};

		class Rigidbody : public gom::GameObjectComponent
		{
			ME_TYPE

			friend class PhysicsManager;
			friend class CollisionInfo;
			friend class TriggerInfo;
			friend class ConstraintBreakInfo;
			friend class PhysxInfo;
			friend class SleepInfo;
			friend class WakeInfo;
			friend class Collider;

		private:
			physx::PxRigidStatic* _physxActor;
			PhysxActorData _physxActorData;

			bool _bWasAddedToScene;

		private:
			void AddToScene();

		protected:
			virtual inline physx::PxActor* GetPhysxActor() { return _physxActor; }
			virtual void PreSimulate();
			virtual void PostSimulate();

			virtual void OnCollisionStart(Handle<gom::GameObject> otherGameObject, const CollisionInfo& collisionInfo);
			virtual void OnCollisionStay(Handle<gom::GameObject> otherGameObject, const CollisionInfo& collisionInfo);
			virtual void OnCollisionEnd(Handle<gom::GameObject> otherGameObject, const CollisionInfo& collisionInfo);

			virtual void OnTriggerStart(Handle<gom::GameObject> otherGameObject);
			virtual void OnTriggerStay(Handle<gom::GameObject> otherGameObject);
			virtual void OnTriggerEnd(Handle<gom::GameObject> otherGameObject);

		public:
			Rigidbody(const gom::ObjectInitializer& initializer);
			Rigidbody(const Rigidbody& other, bool bDeepCopy = true);
			inline virtual ~Rigidbody() { }

			virtual void Initialize();
			virtual void Shutdown();

			virtual void Sleep();
			virtual void WakeUp();
		};
	}
}
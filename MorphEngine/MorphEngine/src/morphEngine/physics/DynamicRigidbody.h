#pragma once

#include "Rigidbody.h"
#include "Constraints.h"

namespace morphEngine
{
	namespace physics
	{
		class Collider;

		class DynamicRigidbody : public Rigidbody
		{
			ME_TYPE

			friend class PhysicsManager;

		private:
			void RegisterProperties();

		protected:
			physx::PxRigidDynamic* _physxActor;
			physx::PxD6Joint* _constraintJoint;
			PhysxActorData _physxActorData;

			Constraints _constraints;
			MFloat32 _mass;
			bool _bIsKinematic;
			bool _bUseGravity;
			bool _bContinousCollision;

			virtual inline physx::PxActor* GetPhysxActor() { return _physxActor; }
			virtual void PreSimulate();
			virtual void PostSimulate();

		public:
			DynamicRigidbody(const gom::ObjectInitializer& initializer);
			DynamicRigidbody(const DynamicRigidbody& other, bool bDeepCopy = true);

			inline utility::MVector3 GetVelocity() const { return _physxActor ? utility::MVector3(_physxActor->getLinearVelocity()) : utility::MVector3::Zero; }
			inline Constraints GetConstraints() const { return _constraints; }
			inline MFloat32 GetMass() const { return _mass; }
			inline bool GetIsKinematic() const { return _bIsKinematic; }
			inline bool GetUseGravity() const { return _bUseGravity; }
			inline bool GetContinousCollision() const { return _bContinousCollision; }

			void SetVelocity(const utility::MVector3& newVelocity);
			void SetConstraints(Constraints constraints);
			void SetMass(MFloat32 newMass);
			void SetIsKinematic(bool bNewIsKinematic);
			void SetUseGravity(bool bNewUseGravity);
			void SetContinousCollision(bool bNewContinousCollision);

			virtual void Initialize();
			virtual void Shutdown();

			virtual void Sleep();
			virtual void WakeUp();

			void AddForce(const utility::MVector3& force);
			void AddTorque(const utility::MVector3& torque);
		};
	}
}
#include "DynamicRigidbody.h"

#include "PhysicsManager.h"
#include "gom\Transform.h"

using namespace morphEngine::gom;
using namespace physx;

namespace morphEngine
{
	namespace physics
	{

		void DynamicRigidbody::RegisterProperties()
		{
			RegisterProperty("Constraints", &_constraints);
			RegisterProperty("Mass", &_mass);
			RegisterProperty("Use gravity", &_bUseGravity);
			RegisterProperty("Is kinematic", &_bIsKinematic);
			RegisterProperty("Use continous collision", &_bContinousCollision);
		}

		void DynamicRigidbody::PreSimulate()
		{
			if(_physxActor != nullptr)
			{
				const PxTransform& transform = _owner->GetTransform()->GetPhysxTransform();
				if(_bIsKinematic)
				{
					_physxActor->setKinematicTarget(transform);
				}
				else
				{
					_physxActor->setGlobalPose(transform);
				}
			}
		}

		void DynamicRigidbody::PostSimulate()
		{
			if(_physxActor != nullptr)
			{
				_owner->GetTransform()->SetPhysxTransform(_physxActor->getGlobalPose());
			}
		}

		DynamicRigidbody::DynamicRigidbody(const ObjectInitializer& initializer) : Rigidbody(initializer), _physxActor(nullptr), _mass(1.0f), _bIsKinematic(false), _bUseGravity(true), _bContinousCollision(false), _constraints(Constraints::FREE), _physxActorData(static_cast<Handle<Rigidbody>>(_this))
		{
			RegisterProperties();
		}

		DynamicRigidbody::DynamicRigidbody(const DynamicRigidbody& other, bool bDeepCopy) : Rigidbody(other, bDeepCopy), _physxActor(nullptr), _mass(other._mass), _bIsKinematic(other._bIsKinematic), _bUseGravity(other._bUseGravity), _bContinousCollision(other._bContinousCollision), _constraints(other._constraints), _physxActorData(static_cast<Handle<Rigidbody>>(_this))
		{
			RegisterProperties();
		}

		void DynamicRigidbody::SetVelocity(const utility::MVector3& newVelocity)
		{
			if(_physxActor != nullptr)
			{
				_physxActor->setLinearVelocity(newVelocity.PhysXVector);
			}
		}

		void DynamicRigidbody::SetConstraints(Constraints constraints)
		{
			if(_constraintJoint != nullptr)
			{
				if(_constraints != constraints)
				{
					_constraints = constraints;
					PhysicsManager::GetInstance()->Constraint(_constraintJoint, _constraints);
				}
			}
		}

		void DynamicRigidbody::SetMass(MFloat32 newMass)
		{
			if(_physxActor != nullptr)
			{
				_mass = newMass;
				_physxActor->setMass(newMass);
			}
		}

		void DynamicRigidbody::SetIsKinematic(bool bNewIsKinematic)
		{
			if(_physxActor != nullptr)
			{
				_bIsKinematic = bNewIsKinematic;
				_physxActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, _bIsKinematic);
			}
		}

		void DynamicRigidbody::SetUseGravity(bool bNewUseGravity)
		{
			if(_physxActor != nullptr)
			{
				_bUseGravity = bNewUseGravity;
				_physxActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !_bUseGravity);
			}
		}

		void DynamicRigidbody::SetContinousCollision(bool bNewContinousCollision)
		{
			if(_physxActor != nullptr)
			{
				_bContinousCollision = bNewContinousCollision;
				_physxActor->setRigidDynamicFlag(PxRigidDynamicFlag::eENABLE_CCD, _bContinousCollision);
			}
		}

		void DynamicRigidbody::Initialize()
		{
			GameObjectComponent::Initialize();

			if(_physxActor == nullptr)
			{
				_physxActor = PhysicsManager::GetInstance()->CreateDynamicRigidbody(_owner->GetTransform()->GetPhysxTransform(), &_constraintJoint);
				if(_physxActor != nullptr)
				{
					_physxActorData.Actor = _this;
					_physxActor->userData = &_physxActorData;
					PhysicsManager::GetInstance()->Constraint(_constraintJoint, _constraints);
					PhysicsManager::GetInstance()->AddDynamicRigidbody(_this);
				}
			}
		}

		void DynamicRigidbody::Shutdown()
		{
			if(_physxActor != nullptr)
			{
				PhysicsManager::GetInstance()->RemoveDynamicRigidbody(_this);
				_physxActor->release();
			}

			GameObjectComponent::Shutdown();
		}

		void DynamicRigidbody::Sleep()
		{
			if(_physxActor != nullptr && !_physxActor->isSleeping())
			{
				_physxActor->putToSleep();
			}
		}

		void DynamicRigidbody::WakeUp()
		{
			if(_physxActor != nullptr && _physxActor->isSleeping())
			{
				_physxActor->wakeUp();
			}
		}

		void DynamicRigidbody::AddForce(const utility::MVector3& force)
		{
			if(_physxActor != nullptr)
			{
				_physxActor->addForce(force.PhysXVector);
			}
		}

		void DynamicRigidbody::AddTorque(const utility::MVector3& torque)
		{
			if(_physxActor != nullptr)
			{
				_physxActor->addTorque(torque.PhysXVector);
			}
		}
	}
}
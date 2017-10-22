#include "CapsuleCollider.h"

#include "PhysicsManager.h"

using namespace morphEngine::gom;

namespace morphEngine
{
	namespace physics
	{
		DEFINE_MENUM(CapsuleMainAxis);

		void CapsuleCollider::RegisterProperties()
		{
			RegisterProperty("Half height", &_halfHeight);
			RegisterProperty("Radius", &_radius);
			RegisterProperty("Main axis", &_mainAxis);
		}

		void CapsuleCollider::SetCapsuleGeometry()
		{
			if(_physxShape != nullptr)
			{
				physx::PxCapsuleGeometry capsule;
				if(_physxShape->getCapsuleGeometry(capsule))
				{
					capsule.halfHeight = _halfHeight;
					capsule.radius = _radius;
					_physxShape->setGeometry(capsule);
				}
				MQuaternion quaternion;
				switch(_mainAxis)
				{
				case CapsuleMainAxis::X:
					break;
				case CapsuleMainAxis::Y:
					quaternion = MQuaternion::FromEuler(90.0f, 0.0f, 0.0f);
					break;
				case CapsuleMainAxis::Z:
					quaternion = MQuaternion::FromEuler(0.0f, 90.0f, 0.0f);
					break;
				}
				physx::PxTransform localPose(quaternion.PhysxQuaternion);
				_physxShape->setLocalPose(localPose);
			}
		}

		CapsuleCollider::CapsuleCollider(const ObjectInitializer& initializer) : Collider(initializer), _radius(0.5f), _halfHeight(1.0f), _mainAxis(CapsuleMainAxis::X)
		{
			RegisterProperties();
		}

		CapsuleCollider::CapsuleCollider(const CapsuleCollider& other, bool bDeepCopy) : Collider(other, bDeepCopy), _radius(other._radius), _halfHeight(other._halfHeight), _mainAxis(other._mainAxis)
		{
			RegisterProperties();
		}

		CapsuleCollider::~CapsuleCollider()
		{
		
		}

		void CapsuleCollider::Initialize()
		{
			Collider::Initialize();

			Handle<Rigidbody> rigidbody = _owner->GetComponent<Rigidbody>();
			if(!rigidbody.IsValid())
			{
				rigidbody = _owner->AddComponentGet<Rigidbody>();
			}
			else if(!rigidbody->GetInitialized())
			{
				rigidbody->Initialize();
			}

			if(_physxShape == nullptr && _physxMaterial != nullptr)
			{
				_physxShape = PhysicsManager::GetInstance()->CreateCapsuleShape(rigidbody, _radius, _halfHeight, _mainAxis, *_physxMaterial);
			}

			SetFlags();

			if(!rigidbody->GetType()->IsA(DynamicRigidbody::GetType()))
			{
				AddRigidbodyToScene(rigidbody);
			}
		}
	}
}
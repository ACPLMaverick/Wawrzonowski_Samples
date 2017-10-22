#include "Collider.h"

#include "DynamicRigidbody.h"
#include "PhysicsManager.h"

using namespace morphEngine::gom;
using namespace physx;

namespace morphEngine
{
	namespace physics
	{

		void Collider::RegisterProperties()
		{
			RegisterProperty("Static friction", &(_materialInfo.StaticFriction));
			RegisterProperty("Dynamic friction", &(_materialInfo.DynamicFriction));
			RegisterProperty("Restitution", &(_materialInfo.Restitution));
			RegisterProperty("Trigger", &_bIsTrigger);
		}

		Collider::Collider(const ObjectInitializer& initializer) : GameObjectComponent(initializer), _physxShape(nullptr), _physxMaterial(nullptr), _bIsTrigger(false), _materialInfo()
		{
			RegisterProperties();
		}

		void Collider::SetFlags()
		{
			if(_physxShape != nullptr)
			{
				_physxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !_bIsTrigger);
				_physxShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, _bIsTrigger);
			}
		}

		void Collider::AddRigidbodyToScene(Handle<Rigidbody> rigidbody) const
		{
			if(rigidbody.IsValid())
			{
				rigidbody->AddToScene();
			}
		}

		Collider::Collider(const Collider& other, bool bDeepCopy) : GameObjectComponent(other, bDeepCopy), _bIsTrigger(other._bIsTrigger), _materialInfo(other._materialInfo), _physxShape(nullptr), _physxMaterial(nullptr)
		{
			RegisterProperties();
		}

		Collider::~Collider()
		{
		}

		void Collider::Initialize()
		{
			GameObjectComponent::Initialize();
			if(_physxMaterial == nullptr)
			{
				_physxMaterial = PhysicsManager::GetInstance()->CreateMaterial(_materialInfo.StaticFriction, _materialInfo.DynamicFriction, _materialInfo.Restitution);
			}
		}

		void Collider::Shutdown()
		{
			if(_physxMaterial != nullptr)
			{
				_physxMaterial->release();
				_physxMaterial = nullptr;
			}

			GameObjectComponent::Shutdown();
		}

		void Collider::UpdateMaterial()
		{
			if(_physxMaterial != nullptr)
			{
				_physxMaterial->release();
				_physxMaterial = nullptr;
			}

			_physxMaterial = PhysicsManager::GetInstance()->CreateMaterial(_materialInfo.StaticFriction, _materialInfo.DynamicFriction, _materialInfo.Restitution);
		}
	}
}
#include "SphereCollider.h"

#include "PhysicsManager.h"

using namespace morphEngine::gom;

namespace morphEngine
{
	namespace physics
	{

		void SphereCollider::RegisterProperties()
		{
			RegisterProperty("Radius", &_radius);
		}

		SphereCollider::SphereCollider(const ObjectInitializer& initializer) : Collider(initializer), _radius(0.5f)
		{
			RegisterProperties();
		}

		SphereCollider::SphereCollider(const SphereCollider& other, bool bDeepCopy) : Collider(other, bDeepCopy), _radius(other._radius)
		{
			RegisterProperties();
		}

		SphereCollider::~SphereCollider()
		{
		
		}

		void SphereCollider::Initialize()
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
				_physxShape = PhysicsManager::GetInstance()->CreateSphereShape(rigidbody, _radius, *_physxMaterial);
			}

			SetFlags();

			if(!rigidbody->GetType()->IsA(DynamicRigidbody::GetType()))
			{
				AddRigidbodyToScene(rigidbody);
			}
		}
	}
}
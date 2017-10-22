#include "BoxCollider.h"

#include "PhysicsManager.h"

using namespace morphEngine::gom;

namespace morphEngine
{
	namespace physics
	{

		void BoxCollider::RegisterProperties()
		{
			RegisterProperty("Size", &_size);
		}

		BoxCollider::BoxCollider(const ObjectInitializer& initializer) : Collider(initializer), _size(1.0f, 1.0f, 1.0f)
		{
			RegisterProperties();
		}

		BoxCollider::BoxCollider(const BoxCollider& other, bool bDeepCopy) : Collider(other, bDeepCopy), _size(other._size)
		{
			RegisterProperties();
		}

		BoxCollider::~BoxCollider()
		{

		}

		void BoxCollider::Initialize()
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
				_physxShape = PhysicsManager::GetInstance()->CreateBoxShape(rigidbody, _size * 0.5f, *_physxMaterial);
			}

			SetFlags();

			if(!rigidbody.GetType()->IsA(DynamicRigidbody::GetType()))
			{
				AddRigidbodyToScene(rigidbody);
			}
		}
	}
}
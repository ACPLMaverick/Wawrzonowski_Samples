#include "MeshCollider.h"

using namespace morphEngine::gom;

namespace morphEngine
{
	namespace physics
	{

		MeshCollider::MeshCollider(const ObjectInitializer& initializer) : Collider(initializer)
		{
		
		}

		MeshCollider::MeshCollider(const MeshCollider& other, bool bDeepCopy) : Collider(other, bDeepCopy)
		{
		
		}

		MeshCollider::~MeshCollider()
		{
		
		}
	}
}
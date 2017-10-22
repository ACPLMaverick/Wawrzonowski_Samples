#pragma once

#include "Collider.h"

namespace morphEngine
{
	namespace physics
	{
		class MeshCollider : public Collider
		{
			friend class PhysicsUtility;

			ME_TYPE

		public:
			MeshCollider(const gom::ObjectInitializer& initializer);
			MeshCollider(const MeshCollider& other, bool bDeepCopy = true);
			virtual ~MeshCollider();
		};
	}
}

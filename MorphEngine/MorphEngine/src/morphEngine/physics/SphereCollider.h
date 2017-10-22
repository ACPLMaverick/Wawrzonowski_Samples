#pragma once

#include "Collider.h"

namespace morphEngine
{
	namespace physics
	{
		class SphereCollider : public Collider
		{
			friend class PhysicsUtility;

			ME_TYPE

		protected:
			MFloat32 _radius;

		private:
			void RegisterProperties();

		public:
			SphereCollider(const gom::ObjectInitializer& initializer);
			SphereCollider(const SphereCollider& other, bool bDeepCopy = true);
			virtual ~SphereCollider();

			virtual void Initialize();

			inline MFloat32 GetRadius() const { return _radius; }

			inline void SetRadius(MFloat32 newRadius)
			{
				_radius = newRadius;
				if(_physxShape != nullptr)
				{
					physx::PxSphereGeometry sphere;
					if(_physxShape->getSphereGeometry(sphere))
					{
						sphere.radius = _radius;
						_physxShape->setGeometry(sphere);
					}
				}
			}
		};
	}
}
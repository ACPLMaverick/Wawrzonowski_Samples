#pragma once

#include "Collider.h"

namespace morphEngine
{
	namespace physics
	{
		class BoxCollider : public Collider
		{
			friend class PhysicsUtility;

			ME_TYPE

		protected:
			utility::MVector3 _size;

		private:
			void RegisterProperties();

		public:
			BoxCollider(const gom::ObjectInitializer& initializer);
			BoxCollider(const BoxCollider& other, bool bDeepCopy = true);
			virtual ~BoxCollider();

			virtual void Initialize();

			inline const utility::MVector3& GetSize() const { return _size; }
			inline void SetSize(const utility::MVector3& newSize)
			{
				_size = newSize;
				if(_physxShape != nullptr)
				{
					physx::PxBoxGeometry box;
					if(_physxShape->getBoxGeometry(box))
					{
						box.halfExtents = _size.PhysXVector * 0.5f;
						_physxShape->setGeometry(box);
					}
				}
			}
		};
	}
}

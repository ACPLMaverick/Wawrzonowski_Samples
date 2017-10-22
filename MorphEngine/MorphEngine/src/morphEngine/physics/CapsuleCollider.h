#pragma once

#include "Collider.h"

namespace morphEngine
{
	namespace physics
	{
		DECLARE_MENUM(CapsuleMainAxis, X, Y, Z)

		class CapsuleCollider : public Collider
		{
			friend class PhysicsUtility;

			ME_TYPE

		protected:
			MFloat32 _radius;
			MFloat32 _halfHeight;
			CapsuleMainAxis _mainAxis;

		private:
			void RegisterProperties();

			void SetCapsuleGeometry();

		public:
			CapsuleCollider(const gom::ObjectInitializer& initializer);
			CapsuleCollider(const CapsuleCollider& other, bool bDeepCopy = true);
			virtual ~CapsuleCollider();

			virtual void Initialize();

			inline MFloat32 GetRadius() const { return _radius; }
			inline MFloat32 GetHalfHeight() const { return _halfHeight; }
			inline CapsuleMainAxis GetMainAxis() const { return _mainAxis; }

			inline void SetMainAxis(CapsuleMainAxis newMainAxis)
			{
				_mainAxis = newMainAxis;
				SetCapsuleGeometry();
			}

			inline void SetRadius(MFloat32 newRadius) 
			{
				_radius = newRadius;
				SetCapsuleGeometry();
			}

			inline void SetHalfHeight(MFloat32 newHalfHeight)
			{
				_halfHeight = newHalfHeight;
				SetCapsuleGeometry();
			}
		};
	}
}

#pragma once

#include "PhysX\PxPhysicsAPI.h"
#include "gom\GameObjectComponent.h"

#define STATIC_FRICTION_DEFAULT 0.1f
#define DYNAMIC_FRICTION_DEFAULT 0.05f
#define RESTITUTION_DEFAULT 0.0f

namespace morphEngine
{
	namespace physics
	{
		struct PhysicsMaterialInfo
		{
		public:
			MFloat32 StaticFriction;
			MFloat32 DynamicFriction;
			MFloat32 Restitution;

		public:
			inline PhysicsMaterialInfo() : StaticFriction(STATIC_FRICTION_DEFAULT), DynamicFriction(DYNAMIC_FRICTION_DEFAULT), Restitution(RESTITUTION_DEFAULT) { }
			inline PhysicsMaterialInfo(const PhysicsMaterialInfo& other) : StaticFriction(other.StaticFriction), DynamicFriction(other.DynamicFriction), Restitution(other.Restitution) { }
		};

		class Collider : public gom::GameObjectComponent
		{
			ME_TYPE

			friend class Rigidbody;
			friend class DynamicRigidbody;
			friend class PhysicsUtility;

		protected:
			physx::PxShape* _physxShape;
			physx::PxMaterial* _physxMaterial;
			PhysicsMaterialInfo _materialInfo;
			bool _bIsTrigger;

		private:
			void RegisterProperties();

		protected:
			void SetFlags();
			void AddRigidbodyToScene(Handle<Rigidbody> rigidbody) const;

		public:
			Collider(const gom::ObjectInitializer& initializer);
			Collider(const Collider& other, bool bDeepCopy = true);
			virtual ~Collider();

			virtual void Initialize();
			virtual void Shutdown();

			void UpdateMaterial();

			inline bool GetIsTrigger() const { return _bIsTrigger; }
			inline void SetIsTrigger(bool bNewIsTrigger)
			{
				if(bNewIsTrigger ^ _bIsTrigger)
				{
					_bIsTrigger = bNewIsTrigger;
					SetFlags();
				} 
			}
		};
	}
}
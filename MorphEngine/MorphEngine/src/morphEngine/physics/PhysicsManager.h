#pragma once
#include "core/Singleton.h"

#include "PhysX\PxPhysicsAPI.h"

#include "debugging\Debug.h"
#include "utility\MVector.h"
#include "core\Config.h"
#include "DynamicRigidbody.h"

#if _DEBUG
#pragma comment(lib,"PhysX3DEBUG_x64.lib")
#pragma comment(lib,"PhysX3CharacterKinematicDEBUG_x64.lib")
#pragma comment(lib,"PhysX3CommonDEBUG_x64.lib")
#pragma comment(lib,"PhysX3CookingDEBUG_x64.lib")
#pragma comment(lib,"PhysXProfileSDKDEBUG.lib")
#pragma comment(lib,"PxTaskDEBUG.lib")
#pragma comment(lib,"PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib,"PhysXVisualDebuggerSDKDEBUG.lib")
#else
#pragma comment(lib,"PhysX3_x64.lib")
#pragma comment(lib,"PhysX3CharacterKinematic_x64.lib")
#pragma comment(lib,"PhysX3Common_x64.lib")
#pragma comment(lib,"PhysX3Cooking_x64.lib")
#pragma comment(lib,"PhysXProfileSDK.lib")
#pragma comment(lib,"PxTask.lib")
#pragma comment(lib,"PhysX3Extensions.lib")
#endif

namespace morphEngine
{
	namespace physics
	{

#pragma region Collision, trigger and other simulation events info classes

		class PhysxInfo
		{
		protected:
			PhysxActorData* _firstData;
			PhysxActorData* _secondData;

		public:
			inline PhysxInfo(PhysxActorData* first = nullptr, PhysxActorData* second = nullptr) : _firstData(first), _secondData(second)
			{}
			inline virtual ~PhysxInfo()
			{
				_firstData = nullptr;
				_secondData = nullptr;
			}

			virtual void Report() = 0;

			inline void* operator new(MSize size)
			{
				return MemoryManager::GetInstance()->GetPhysicsAllocator()->Allocate(size);
			}
			inline void operator delete(void* p, MSize size)
			{
				MemoryManager::GetInstance()->GetPhysicsAllocator()->Deallocate(p);
			}
		};

		enum ContactState
		{
			BEGIN,
			PERSIST,
			END,

			NONE
		};

		class ContactPoint
		{
			friend class CollisionInfo;

		public:
			//The position of the contact point between two rigidbodies (WORLD SPACE)
			MVector3 Position;
			//The normal of the colliding surfaces at the contact point
			MVector3 Normal;
			//Impulse applied at the contact point. Divide by the simulation time step to get a force value
			MVector3 Impulse;
			//Separation of the rigidbodies at the contact point. A negative separation denotes a penetration
			MVector3 Separation;

		protected:
			inline ContactPoint() { }

		public:
			inline ContactPoint(const MVector3& position, const MVector3& normal, const MVector3& impulse, const MVector3& separation) : Impulse(impulse), Position(position), Normal(normal), Separation(separation) { }
			inline ContactPoint(const ContactPoint& other) : Position(other.Position), Normal(other.Normal), Impulse(other.Impulse), Separation(other.Separation) { }

			inline ContactPoint& operator=(const ContactPoint& other)
			{
				Position = other.Position;
				Normal = other.Normal;
				Impulse = other.Impulse;
				Separation = other.Separation;
				return *this;
			}
		};

		class CollisionInfo : public PhysxInfo
		{
		protected:
			ContactState _state;

		public:
			ContactPoint* ContactPoints;
			MSize ContactPointsCount;

		public:
			virtual void Report();

			void SetContactPoint(MSize index, const ContactPoint& contactPoint)
			{
				if(index >= ContactPointsCount)
				{
					return;
				}
				ContactPoints[index] = contactPoint;
			}

			inline CollisionInfo(ContactState state, PhysxActorData* first = nullptr, PhysxActorData* second = nullptr, MSize contactPointsCount = 0) : PhysxInfo(first, second), _state(state), ContactPointsCount(contactPointsCount), ContactPoints(nullptr)
			{
				ContactPoints = new ContactPoint[contactPointsCount];
			}

			inline virtual ~CollisionInfo()
			{
				if(ContactPoints != nullptr)
				{
					delete[] ContactPoints;
					ContactPoints = nullptr;
				}
			}
		};

		class TriggerInfo : public PhysxInfo
		{
		protected:
			ContactState _state;

		public:
			inline TriggerInfo(ContactState state, PhysxActorData* first = nullptr, PhysxActorData* second = nullptr) : PhysxInfo(first, second), _state(state)
			{}

			virtual void Report();
		};

		class ConstraintBreakInfo : public PhysxInfo
		{
		public:
			inline ConstraintBreakInfo(PhysxActorData* first = nullptr, PhysxActorData* second = nullptr) : PhysxInfo(first, second)
			{}

			virtual void Report();
		};

#pragma endregion

		/// <summary>
		/// Singleton.
		/// </summary>
		class PhysicsManager : public core::Singleton<PhysicsManager>
		{
			friend class Rigidbody;
			friend class DynamicRigidbody;
			friend class BoxCollider;
			friend class CapsuleCollider;
			friend class SphereCollider;
			friend class MeshCollider;
			friend class Collider;
			friend class PhysicsUtility;

		protected:

#pragma region Physx derived classes

			class PhysxAllocator : public physx::PxAllocatorCallback
			{
			public:
				void* allocate(size_t size, const char* typeName, const char* filename, int line);
				void deallocate(void* ptr);
			};

			class PhysxSimulationEventCallback : public physx::PxSimulationEventCallback
			{
			protected:
				PhysicsManager* _physics;

			public:
				//This is called when a breakable constraint breaks.
				virtual void onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count);
				//This is called with the actors which have just been woken up
				virtual void onWake(physx::PxActor **actors, physx::PxU32 count);
				//This is called with the actors which have just been put to sleep.
				virtual void onSleep(physx::PxActor **actors, physx::PxU32 count);
				//This is called when certain contact events occur.
				virtual void onContact(const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs);
				//This is called with the current trigger pair events.
				virtual void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count);

				inline void SetPhysics(PhysicsManager* physics) { _physics = physics; }
				inline virtual ~PhysxSimulationEventCallback() { _physics = 0; }
			};

			class PhysxErrorCallback : public physx::PxErrorCallback
			{
			public:
				virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file,
										 int line);

				inline virtual ~PhysxErrorCallback() { }
			};

#pragma endregion

			physx::PxFoundation* _foundation;
			physx::PxPhysics* _sdk;
			physx::PxProfileZoneManager* _profileZoneManager;
			physx::PxCpuDispatcher* _cpuDispatcher;
			physx::PxVisualDebuggerConnection* _pvdConnection;

			physx::PxScene* _scene;

			physx::PxTolerancesScale _scale;
			bool _bTrackAllocations;

			PhysxAllocator _physxAllocator;
			PhysxErrorCallback _physxErrorCallback;
			PhysxSimulationEventCallback _physxSimulationCallback;

			core::Config _physicsConfig;
			utility::MVector3 _gravity;

			utility::MArray<Handle<Rigidbody>> _allRigidbodies;

			utility::MArray<PhysxInfo*> _events;

		protected:
			static physx::PxFilterFlags FilterShader(physx::PxFilterObjectAttributes attributes0,
													 physx::PxFilterData filterData0,
													 physx::PxFilterObjectAttributes attributes1,
													 physx::PxFilterData filterData1,
													 physx::PxPairFlags& pairFlags,
													 const void* constantBlock,
													 physx::PxU32 constantBlockSize);

			inline physx::PxRigidStatic* CreateStaticRigidbody(const physx::PxTransform& transform) const
			{
				return _sdk ? _sdk->createRigidStatic(transform) : nullptr;
			}

			inline physx::PxRigidDynamic* CreateDynamicRigidbody(const physx::PxTransform& transform, physx::PxD6Joint** constraintJoint) const
			{
				physx::PxRigidDynamic* actor = _sdk ? _sdk->createRigidDynamic(transform) : nullptr;
				if(actor && constraintJoint != nullptr)
				{
					(*constraintJoint) = physx::PxD6JointCreate(*_sdk, nullptr, physx::PxTransform::createIdentity(), actor, actor->getGlobalPose());
					(*constraintJoint)->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eFREE);
					(*constraintJoint)->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eFREE);
					(*constraintJoint)->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eFREE);
					(*constraintJoint)->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eFREE);
					(*constraintJoint)->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eFREE);
					(*constraintJoint)->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);
				}
				return actor;
			}

			inline physx::PxMaterial* CreateMaterial(MFloat32 staticFriction = STATIC_FRICTION_DEFAULT, MFloat32 dynamicFriction = DYNAMIC_FRICTION_DEFAULT, MFloat32 restitution = RESTITUTION_DEFAULT) const
			{
				return _sdk ? _sdk->createMaterial(staticFriction, dynamicFriction, restitution) : nullptr;
			}

			physx::PxShape* CreateBoxShape(Handle<Rigidbody> rigidbody, MFloat32 halfSizeX, MFloat32 halfSizeY, MFloat32 halfSizeZ, const physx::PxMaterial& material) const;
			physx::PxShape* CreateBoxShape(Handle<Rigidbody> rigidbody, const utility::MVector3& halfSize, const physx::PxMaterial& material) const;
			physx::PxShape* CreateBoxShape(Handle<Rigidbody> rigidbody, const physx::PxShape* shape, const physx::PxMaterial& material) const;

			physx::PxShape* CreateCapsuleShape(Handle<Rigidbody> rigidbody, MFloat32 radius, MFloat32 halfHeight, CapsuleMainAxis mainAxis, const physx::PxMaterial& material) const;
			physx::PxShape* CreateCapsuleShape(Handle<Rigidbody> rigidbody, const physx::PxShape* shape, const physx::PxMaterial& material) const;

			physx::PxShape* CreateSphereShape(Handle<Rigidbody> rigidbody, MFloat32 radius, const physx::PxMaterial& material) const;
			physx::PxShape* CreateSphereShape(Handle<Rigidbody> rigidbody, const physx::PxShape* shape, const physx::PxMaterial& material) const;

			physx::PxShape* CreateMeshShape(Handle<Rigidbody> rigidbody, const physx::PxMaterial& material) const;
			physx::PxShape* CreateMeshShape(Handle<Rigidbody> rigidbody, const physx::PxShape* shape, const physx::PxMaterial& material) const;

			void Constraint(physx::PxD6Joint* constraintJoint, Constraints constraints);

		public:
			inline PhysicsManager() : _foundation(nullptr), _sdk(nullptr), _profileZoneManager(nullptr), _cpuDispatcher(nullptr), _scene(nullptr), _pvdConnection(nullptr){ }
			inline ~PhysicsManager() { }

			void Initialize();
			void Shutdown();

			void Update(MFloat32 fixedDeltaTime);
			void ResolveEvents();

			void Clear();

			inline const utility::MVector3& GetGravity() const { return _gravity; }
			inline void SetGravity(const utility::MVector3& gravity) { _gravity = gravity; }

			void AddDynamicRigidbody(Handle<DynamicRigidbody> rigidbody);
			void AddStaticRigidbody(Handle<Rigidbody> rigidbody);
			void RemoveDynamicRigidbody(Handle<DynamicRigidbody> rigidbody);
			void RemoveStaticRigidbody(Handle<Rigidbody> rigidbody);
		};
	}
}
#include "PhysicsManager.h"

#include "memoryManagement\MemoryManager.h"

#define PHYSICS_MANAGER_CONFIG_PATH "Configs/physics.conf"
#define PHYSICS_MANAGER_GRAVITY_KEY "Gravity"

using namespace physx;
using namespace morphEngine::core;
using namespace morphEngine::debugging;
using namespace morphEngine::utility;
using namespace morphEngine::memoryManagement;

#define LOG_ERROR_CREATION(className) Debug::Log(ELogType::ERR, MString("Error! Cannot create ") + className + " class instance!")

namespace morphEngine
{
	namespace physics
	{
		void CollisionInfo::Report()
		{
			if(_firstData == nullptr || _secondData == nullptr)
			{
				return;
			}

			if(!_firstData->Actor.IsValid() || !_secondData->Actor.IsValid() || !_firstData->Actor->GetOwner().IsValid() || !_secondData->Actor->GetOwner().IsValid())
			{
				return;
			}

			switch(_state)
			{
			case ContactState::BEGIN:
				_firstData->Actor->OnCollisionStart(_firstData->Actor->GetOwner(), *this);
				_secondData->Actor->OnCollisionStart(_firstData->Actor->GetOwner(), *this);
				break;
			case ContactState::END:
				_firstData->Actor->OnCollisionEnd(_secondData->Actor->GetOwner(), *this);
				_secondData->Actor->OnCollisionEnd(_firstData->Actor->GetOwner(), *this);
				break;
			case ContactState::PERSIST:
				_firstData->Actor->OnCollisionStay(_secondData->Actor->GetOwner(), *this);
				_secondData->Actor->OnCollisionStay(_firstData->Actor->GetOwner(), *this);
				break;
			}
		}

		void TriggerInfo::Report()
		{
			if(_firstData == nullptr || _secondData == nullptr)
			{
				return;
			}

			if(!_firstData->Actor.IsValid() || !_secondData->Actor.IsValid() || !_firstData->Actor->GetOwner().IsValid() || !_secondData->Actor->GetOwner().IsValid())
			{
				return;
			}

			switch(_state)
			{
			case ContactState::BEGIN:
				_firstData->Actor->OnTriggerStart(_secondData->Actor->GetOwner());
				_secondData->Actor->OnTriggerStart(_firstData->Actor->GetOwner());
				break;
			case ContactState::END:
				_firstData->Actor->OnTriggerEnd(_secondData->Actor->GetOwner());
				_secondData->Actor->OnTriggerEnd(_firstData->Actor->GetOwner());
				break;
			case ContactState::PERSIST:
				_firstData->Actor->OnTriggerStay(_secondData->Actor->GetOwner());
				_secondData->Actor->OnTriggerStay(_firstData->Actor->GetOwner());
				break;
			}
		}

		void ConstraintBreakInfo::Report()
		{
			Debug::Log(ELogType::LOG, "ContraintBreak here");
		}

		void* PhysicsManager::PhysxAllocator::allocate(size_t size, const char* typeName, const char* filename, int line)
		{
			return MemoryManager::GetInstance()->GetPhysicsAllocator()->Allocate(size, 16);
		}

		void PhysicsManager::PhysxAllocator::deallocate(void* ptr)
		{
			if(ptr != nullptr)
			{
				MemoryManager::GetInstance()->GetPhysicsAllocator()->Deallocate(ptr);
			}
		}

		void PhysicsManager::PhysxSimulationEventCallback::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count)
		{
			//We do not care about constraints breaking (for now ofc, maybe later we'll need this)
		}

		void PhysicsManager::PhysxSimulationEventCallback::onWake(PxActor** actors, PxU32 count)
		{
			//Do nothing since we don't care about waking a rigidbody up
		}

		void PhysicsManager::PhysxSimulationEventCallback::onSleep(PxActor** actors, PxU32 count)
		{
			//Do nothing since we don't care about putting a rigidbody to sleep
		}

		void PhysicsManager::PhysxSimulationEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
		{
			physx::PxRigidActor* const* actors = pairHeader.actors;

			PhysxActorData* firstActorData = nullptr;
			PhysxActorData* secondActorData = nullptr;
			if(actors[0] != nullptr)
			{
				firstActorData = (PhysxActorData*)actors[0]->userData;
			}
			if(actors[1] != nullptr)
			{
				secondActorData = (PhysxActorData*)actors[1]->userData;
			}

			if(firstActorData == nullptr || secondActorData == nullptr)
			{
				return;
			}

			for(PxU32 i = 0; i < nbPairs; ++i)
			{
				const PxContactPair& contact = pairs[i];
				ContactState collisionState = ContactState::NONE;
				if(contact.events | PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					collisionState = ContactState::BEGIN;
				}
				else if(contact.events | PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
				{
					collisionState = ContactState::PERSIST;
				}
				else if(contact.events | PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					collisionState = ContactState::END;
				}

				CollisionInfo* collisionInfo = new CollisionInfo(collisionState, firstActorData, secondActorData, contact.contactCount);
				PxContactPairPoint* contacts = new PxContactPairPoint[contact.contactCount];
				contact.extractContacts(contacts, contact.contactCount);
				for(PxU32 j = 0; j < contact.contactCount; ++j)
				{
					PxContactPairPoint& contactPairPoint = contacts[j];
					collisionInfo->SetContactPoint(j, ContactPoint(contactPairPoint.position, contactPairPoint.normal, contactPairPoint.impulse, contactPairPoint.separation));
				}
				delete[] contacts;
				_physics->_events.Add(collisionInfo);
			}
		}

		void PhysicsManager::PhysxSimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
		{
			for(PxU32 i = 0; i < count; ++i)
			{
				PxTriggerPair& pair = pairs[i];
				PhysxActorData* firstActorData = nullptr;
				PhysxActorData* secondActorData = nullptr;
				if(pair.triggerActor != nullptr)
				{
					firstActorData = (PhysxActorData*)pair.triggerActor->userData;
				}
				if(pair.otherActor != nullptr)
				{
					secondActorData = (PhysxActorData*)pair.otherActor->userData;
				}

				if(firstActorData == nullptr || secondActorData == nullptr)
				{
					continue;
				}

				ContactState triggerState = ContactState::NONE;
				if(pair.status | PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					triggerState = ContactState::BEGIN;
				}
				else if(pair.status | PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					triggerState = ContactState::END;
				}

				_physics->_events.Add(new TriggerInfo(triggerState, firstActorData, secondActorData));
			}
		}

#if _DEBUG
		void PhysicsManager::PhysxErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
		{
			MString msg = message;
			msg += ", in file: ";
			msg += file;
			msg += ":";
			msg += MString::FromInt(line);
			if(code == PxErrorCode::eABORT || code == PxErrorCode::eINTERNAL_ERROR || code == PxErrorCode::eINVALID_OPERATION || PxErrorCode::eINVALID_PARAMETER || code == PxErrorCode::eOUT_OF_MEMORY)
			{
				Debug::Log(ELogType::ERR, msg);
			}
			else if(code == PxErrorCode::eDEBUG_WARNING || code == PxErrorCode::ePERF_WARNING)
			{
				Debug::Log(ELogType::WARNING, msg);
			}
			else if(code == PxErrorCode::eDEBUG_INFO)
			{
				Debug::Log(ELogType::LOG, msg);
			}
		}
#else
		void PhysicsManager::PhysxErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
		{
			if(code == PxErrorCode::eABORT || code == PxErrorCode::eINTERNAL_ERROR || code == PxErrorCode::eINVALID_OPERATION || PxErrorCode::eINVALID_PARAMETER || code == PxErrorCode::eOUT_OF_MEMORY)
			{
				ME_ASSERT(false, "PhysX error!");
			}
			else if(code == PxErrorCode::eDEBUG_WARNING || code == PxErrorCode::ePERF_WARNING)
			{
				ME_WARNING(false, "PhysX warning!");
			}
		}
#endif

		PxFilterFlags PhysicsManager::FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
		{
			bool isTrigger = PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1);
			pairFlags = PxPairFlag::eSOLVE_CONTACT;
			pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;
			pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;

			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
			if(!isTrigger)
			{
				pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
				pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
			}

			return PxFilterFlag::eDEFAULT;
		}

		PxShape* PhysicsManager::CreateBoxShape(Handle<Rigidbody> rigidbody, MFloat32 halfSizeX, MFloat32 halfSizeY, MFloat32 halfSizeZ, const PxMaterial& material) const
		{
			PxActor* actor = rigidbody->GetPhysxActor();
			PxRigidStatic* rigidStatic = actor->is<PxRigidStatic>();
			if(rigidStatic)
			{
				return rigidStatic->createShape(PxBoxGeometry(halfSizeX, halfSizeY, halfSizeZ), material, PxTransform::createIdentity());
			}
			PxRigidDynamic* rigidDynamic = actor->is<PxRigidDynamic>();
			if(rigidDynamic)
			{
				return rigidDynamic->createShape(PxBoxGeometry(halfSizeX, halfSizeY, halfSizeZ), material, PxTransform::createIdentity());
			}

			return nullptr;
		}

		PxShape* PhysicsManager::CreateBoxShape(Handle<Rigidbody> rigidbody, const utility::MVector3& halfSize, const PxMaterial& material) const
		{
			return CreateBoxShape(rigidbody, halfSize.X, halfSize.Y, halfSize.Z, material);
		}

		PxShape* PhysicsManager::CreateBoxShape(Handle<Rigidbody> rigidbody, const PxShape* shape, const PxMaterial& material) const
		{
			if(shape == nullptr)
			{
				return nullptr;
			}

			PxBoxGeometry box;
			if(!shape->getBoxGeometry(box))
			{
				return nullptr;
			}

			PxActor* actor = rigidbody->GetPhysxActor();
			PxRigidStatic* rigidStatic = actor->is<PxRigidStatic>();
			if(rigidStatic)
			{
				rigidStatic->createShape(PxBoxGeometry(box), material, PxTransform::createIdentity());
			}
			PxRigidDynamic* rigidDynamic = actor->is<PxRigidDynamic>();
			if(rigidDynamic)
			{
				rigidDynamic->createShape(PxBoxGeometry(box), material, PxTransform::createIdentity());
			}

			return nullptr;
		}

		PxShape* PhysicsManager::CreateCapsuleShape(Handle<Rigidbody> rigidbody, MFloat32 radius, MFloat32 halfHeight, CapsuleMainAxis mainAxis, const PxMaterial& material) const
		{
			PxActor* actor = rigidbody->GetPhysxActor();
			PxRigidStatic* rigidStatic = actor->is<PxRigidStatic>();
			MQuaternion quaternion;
			switch(mainAxis)
			{
			case CapsuleMainAxis::X:
				break;
			case CapsuleMainAxis::Y:
				quaternion = MQuaternion::FromEuler(90.0f, 0.0f, 0.0f);
				break;
			case CapsuleMainAxis::Z:
				quaternion = MQuaternion::FromEuler(0.0f, 90.0f, 0.0f);
				break;
			}
			physx::PxTransform localPose(quaternion.PhysxQuaternion);
			if(rigidStatic)
			{
				return rigidStatic->createShape(PxCapsuleGeometry(radius, halfHeight), material, localPose);
			}
			PxRigidDynamic* rigidDynamic = actor->is<PxRigidDynamic>();
			if(rigidDynamic)
			{
				return rigidDynamic->createShape(PxCapsuleGeometry(radius, halfHeight), material, localPose);
			}

			return nullptr;
		}

		PxShape* PhysicsManager::CreateCapsuleShape(Handle<Rigidbody> rigidbody, const PxShape* shape, const PxMaterial& material) const
		{
			if(shape == nullptr)
			{
				return nullptr;
			}

			PxCapsuleGeometry capsule;
			if(!shape->getCapsuleGeometry(capsule))
			{
				return nullptr;
			}

			PxActor* actor = rigidbody->GetPhysxActor();
			PxRigidStatic* rigidStatic = actor->is<PxRigidStatic>();
			if(rigidStatic)
			{
				rigidStatic->createShape(PxCapsuleGeometry(capsule), material, shape->getLocalPose());
			}
			PxRigidDynamic* rigidDynamic = actor->is<PxRigidDynamic>();
			if(rigidDynamic)
			{
				rigidDynamic->createShape(PxCapsuleGeometry(capsule), material, shape->getLocalPose());
			}

			return nullptr;
		}

		PxShape* PhysicsManager::CreateSphereShape(Handle<Rigidbody> rigidbody, MFloat32 radius, const PxMaterial& material) const
		{
			PxActor* actor = rigidbody->GetPhysxActor();
			PxRigidStatic* rigidStatic = actor->is<PxRigidStatic>();
			if(rigidStatic)
			{
				return rigidStatic->createShape(PxSphereGeometry(radius), material, PxTransform::createIdentity());
			}
			PxRigidDynamic* rigidDynamic = actor->is<PxRigidDynamic>();
			if(rigidDynamic)
			{
				return rigidDynamic->createShape(PxSphereGeometry(radius), material, PxTransform::createIdentity());
			}

			return nullptr;
		}

		PxShape* PhysicsManager::CreateSphereShape(Handle<Rigidbody> rigidbody, const PxShape* shape, const PxMaterial& material) const
		{
			if(shape == nullptr)
			{
				return nullptr;
			}

			PxSphereGeometry sphere;
			if(!shape->getSphereGeometry(sphere))
			{
				return nullptr;
			}

			PxActor* actor = rigidbody->GetPhysxActor();
			PxRigidStatic* rigidStatic = actor->is<PxRigidStatic>();
			if(rigidStatic)
			{
				rigidStatic->createShape(PxSphereGeometry(sphere), material, PxTransform::createIdentity());
			}
			PxRigidDynamic* rigidDynamic = actor->is<PxRigidDynamic>();
			if(rigidDynamic)
			{
				rigidDynamic->createShape(PxSphereGeometry(sphere), material, PxTransform::createIdentity());
			}

			return nullptr;
		}

		PxShape* PhysicsManager::CreateMeshShape(Handle<Rigidbody> rigidbody, const PxMaterial& material) const
		{
			ME_ASSERT(false, "Mesh Colliders are not supported now!");
			return nullptr;
		}

		PxShape* PhysicsManager::CreateMeshShape(Handle<Rigidbody> rigidbody, const PxShape* shape, const PxMaterial& material) const
		{
			if(shape == nullptr)
			{
				return nullptr;
			}

			PxConvexMeshGeometry mesh;
			if(!shape->getConvexMeshGeometry(mesh))
			{
				return nullptr;
			}
			
			PxActor* actor = rigidbody->GetPhysxActor();
			PxRigidStatic* rigidStatic = actor->is<PxRigidStatic>();
			if(rigidStatic)
			{
				rigidStatic->createShape(PxConvexMeshGeometry(mesh), material, PxTransform::createIdentity());
			}
			PxRigidDynamic* rigidDynamic = actor->is<PxRigidDynamic>();
			if(rigidDynamic)
			{
				rigidDynamic->createShape(PxConvexMeshGeometry(mesh), material, PxTransform::createIdentity());
			}

			return nullptr;
		}

		void PhysicsManager::Constraint(physx::PxD6Joint* constraintJoint, Constraints constraints)
		{
			if(constraintJoint == nullptr)
			{
				return;
			}
			
			constraintJoint->setMotion(PxD6Axis::eX, constraints.HasDefined(Constraints::POSITION_X) ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
			constraintJoint->setMotion(PxD6Axis::eY, constraints.HasDefined(Constraints::POSITION_Y) ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
			constraintJoint->setMotion(PxD6Axis::eZ, constraints.HasDefined(Constraints::POSITION_Z) ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
			constraintJoint->setMotion(PxD6Axis::eSWING2, constraints.HasDefined(Constraints::ROTATION_X) ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
			constraintJoint->setMotion(PxD6Axis::eSWING1, constraints.HasDefined(Constraints::ROTATION_Y) ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
			constraintJoint->setMotion(PxD6Axis::eTWIST, constraints.HasDefined(Constraints::ROTATION_Z) ? PxD6Motion::eLOCKED : PxD6Motion::eFREE);
		}

		void PhysicsManager::Initialize()
		{
			_scale = PxTolerancesScale();
#if _DEBUG
			_bTrackAllocations = true;
#else
			_bTrackAllocations = false;
#endif

			_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _physxAllocator, _physxErrorCallback);
			if(!_foundation)
			{
				LOG_ERROR_CREATION("PxFoundation");
				return;
			}

			_profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(_foundation);
			if(!_profileZoneManager)
			{
				LOG_ERROR_CREATION("PxProfileZoneManager");
				return;
			}

			_sdk = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, _scale, _bTrackAllocations, _profileZoneManager);
			if(!_sdk)
			{
				LOG_ERROR_CREATION("PxPhysics");
				return;
			}

			if(!PxInitExtensions(*_sdk))
			{
				Debug::Log(ELogType::ERR, "Cannot initialize PhysX extensions");
				return;
			}

			_cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
			if(!_cpuDispatcher)
			{
				LOG_ERROR_CREATION("PxCpuDispatcher");
				return;
			}

			_physxSimulationCallback.SetPhysics(this);

			_gravity = MVector3(0.0f, -9.81f, 0.0f);
			_physicsConfig.Read(PHYSICS_MANAGER_CONFIG_PATH);
			Config::ValueType gravityValues = _physicsConfig.GetValues(PHYSICS_MANAGER_GRAVITY_KEY);
			if(gravityValues.GetSize() > 0)
			{
				//Get only first gravity data
				Config::ValueElement gravity = gravityValues[0];
				MArray<MString> gravityComponents;
				MArray<char> chars;
				chars.Add(' ');
				MString::SplitNCS(gravity, chars, gravityComponents);
				_gravity.X = MString::ToFloat32(gravityComponents[0]);
				_gravity.Y = MString::ToFloat32(gravityComponents[1]);
				_gravity.Z = MString::ToFloat32(gravityComponents[2]);
			}

			PxSceneDesc sceneDesc(_scale);
			sceneDesc.gravity = _gravity;
			sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
			sceneDesc.cpuDispatcher = _cpuDispatcher;
			sceneDesc.filterShader = PhysicsManager::FilterShader;
			sceneDesc.simulationEventCallback = &_physxSimulationCallback;

			_scene = _sdk->createScene(sceneDesc);
			if(!_scene)
			{
				LOG_ERROR_CREATION("PxScene");
				return;
			}

#if _DEBUG
			PxVisualDebuggerConnectionManager* pvdConnectionManager = _sdk->getPvdConnectionManager();
			if(pvdConnectionManager)
			{
				PxVisualDebuggerConnectionFlags pvdConnectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
				_pvdConnection = PxVisualDebuggerExt::createConnection(pvdConnectionManager, "127.0.0.1", 5425, 100, pvdConnectionFlags);
			}
			_sdk->getVisualDebugger()->setVisualDebuggerFlags(PxVisualDebuggerFlag::eTRANSMIT_CONSTRAINTS | PxVisualDebuggerFlag::eTRANSMIT_CONTACTS | PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES);
#endif
		}

		void PhysicsManager::Shutdown()
		{
			Clear();

			_physicsConfig.Clear();
			MString gravity = MString::FromFloat(_gravity.X) + " " + MString::FromFloat(_gravity.Y) + " " + MString::FromFloat(_gravity.Z);
			_physicsConfig.SetValue(PHYSICS_MANAGER_GRAVITY_KEY, gravity);
			_physicsConfig.Write();

			_allRigidbodies.Clear();

#if _DEBUG
			if(_pvdConnection)
			{
				_pvdConnection->release();
				_pvdConnection = nullptr;
			}
#endif

			if(_scene)
			{
				_scene->release();
				_scene = nullptr;
			}

			if(_cpuDispatcher)
			{
				delete _cpuDispatcher;
				_cpuDispatcher = nullptr;
			}

			PxCloseExtensions();
			if(_sdk)
			{
				_sdk->release();
				_sdk = nullptr;
			}

			if(_profileZoneManager)
			{
				_profileZoneManager->release();
				_profileZoneManager = nullptr;
			}

			if(_foundation)
			{
				_foundation->release();
				_foundation = nullptr;
			}
		}

		void PhysicsManager::Update(MFloat32 fixedDeltaTime)
		{
			if(_scene != nullptr)
			{
				MArray<Handle<Rigidbody>>::MIteratorArray it = _allRigidbodies.GetBegin();
				for(; it.IsValid(); ++it)
				{
					if((*it).IsValid())
					{
						(*it)->PreSimulate();
					}
				}
				
				_scene->simulate(fixedDeltaTime);
				_scene->fetchResults(true);

				MArray<Handle<Rigidbody>>::MIteratorArray it2 = _allRigidbodies.GetBegin();
				for(; it.IsValid(); ++it)
				{
					if((*it2).IsValid())
					{
						(*it2)->PostSimulate();
					}
				}
			}
		}

		void PhysicsManager::ResolveEvents()
		{
			MArray<PhysxInfo*>::MIteratorArray it = _events.GetBegin();
			for(; it.IsValid(); ++it)
			{
				(*it)->Report();
			}

			Clear();
		}

		void PhysicsManager::Clear()
		{
			MArray<PhysxInfo*>::MIteratorArray destroyIt = _events.GetBegin();
			for(; destroyIt.IsValid(); ++destroyIt)
			{
				delete (*destroyIt);
			}

			_events.Clear();
		}

		void PhysicsManager::AddDynamicRigidbody(Handle<DynamicRigidbody> rigidbody)
		{
			if(_scene != nullptr && rigidbody.IsValid())
			{
				_scene->addActor(*rigidbody->GetPhysxActor());
				_allRigidbodies.Add(rigidbody);
			}
		}
		
		void PhysicsManager::AddStaticRigidbody(Handle<Rigidbody> rigidbody)
		{
			if(_scene != nullptr && rigidbody.IsValid())
			{
				_scene->addActor(*rigidbody->GetPhysxActor());
				_allRigidbodies.Add(rigidbody);
			}
		}

		void PhysicsManager::RemoveDynamicRigidbody(Handle<DynamicRigidbody> rigidbody)
		{
			if(_scene != nullptr && rigidbody.IsValid())
			{
				_scene->removeActor(*rigidbody->GetPhysxActor());
				_allRigidbodies.Remove(rigidbody);
			}
		}
		
		void PhysicsManager::RemoveStaticRigidbody(Handle<Rigidbody> rigidbody)
		{
			if(_scene != nullptr && rigidbody.IsValid())
			{
				_scene->removeActor(*rigidbody->GetPhysxActor());
				_allRigidbodies.Remove(rigidbody);
			}
		}
	}
}
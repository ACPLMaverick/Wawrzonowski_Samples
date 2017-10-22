#include "PhysicsUtility.h"

using namespace morphEngine::gom;
using namespace morphEngine::utility;
using namespace physx;

namespace morphEngine
{
	namespace physics
	{
		bool PhysicsUtility::Overlaps(PxScene* physxScene, const PxTransform& transform, const PxGeometry& geometry, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags)
		{
			static const MUint32 touchesMaxCount = 256;
			PxOverlapHit* touches = new PxOverlapHit[touchesMaxCount];
			PxOverlapBuffer hits = PxOverlapBuffer(touches, touchesMaxCount);
			PxQueryFilterData queryFilterData(CalculateQueryDynamicsFlags(dynamicsFlags) | PxQueryFlag::eNO_BLOCK);

			bool status = physxScene->overlap(geometry, transform, hits, queryFilterData);
			if(status)
			{
				for(MUint32 i = 0; i < hits.nbTouches; ++i)
				{
					PxOverlapHit& touch = hits.touches[i];
					PhysxActorData* data = (PhysxActorData*)touch.actor->userData;

					Handle<GameObject> hitObject = Handle<GameObject>();
					if(data != nullptr && data->Actor.IsValid() && data->Actor->GetEnabled() && data->Actor->GetOwner().IsValid() && data->Actor->GetOwner()->GetEnabled())
					{
						hitObject = data->Actor->GetOwner();
					}

					outHits.Add(
						QueryHit(
							MVector3::Zero,
							MVector3::Up,
							0.0f,
							hitObject
						)
					);
				}
			}

			delete[] touches;

			return status;
		}

		bool PhysicsUtility::RaycastSingle(const MVector3& start, const MVector3& direction, MFloat32 maxDistance, QueryHit& outHit, QueryDynamicsFlags dynamicsFlags)
		{
			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}

			maxDistance = MMath::Clamp(maxDistance, 0.0f, FLT_MAX);

			static const MUint32 touchesMaxCount = 8;
			PxRaycastHit* touches = new PxRaycastHit[touchesMaxCount];
			PxRaycastBuffer hits(touches, touchesMaxCount);
			PxQueryFilterData queryFilterData(CalculateQueryDynamicsFlags(dynamicsFlags) | PxQueryFlag::eNO_BLOCK);

			bool status = physxScene->raycast(start.PhysXVector, direction.Normalized().PhysXVector, maxDistance, hits, PxHitFlag::eDISTANCE | PxHitFlag::eMTD | PxHitFlag::eNORMAL | PxHitFlag::ePOSITION, queryFilterData);
			
			if(status)
			{
				status = false;
				outHit.HitDistance = FLT_MAX;
				for(MUint32 i = 0; i < hits.nbTouches; ++i)
				{
					PxRaycastHit& touch = hits.touches[i];
					if(!touch.hadInitialOverlap())
					{
						PhysxActorData* data = (PhysxActorData*)touch.actor->userData;
						status = true;
						if(touch.distance < outHit.HitDistance)
						{
							outHit.HitPosition = touch.position;
							outHit.HitNormal = touch.normal;
							outHit.HitDistance = touch.distance;
							if(data != nullptr && data->Actor.IsValid() && data->Actor->GetEnabled() && data->Actor->GetOwner().IsValid() && data->Actor->GetOwner()->GetEnabled())
							{
								outHit.GameObject = data->Actor->GetOwner();
							}
							else
							{
								outHit.GameObject = Handle<GameObject>();
							}
						}
					}
				}
			}

			delete[] touches;
			return status;
		}

		bool PhysicsUtility::RaycastMulti(const MVector3& start, const MVector3& direction, MFloat32 maxDistance, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags)
		{
			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}

			maxDistance = MMath::Clamp(maxDistance, 0.0f, FLT_MAX);

			static const MUint32 touchesMaxCount = 256;
			PxRaycastHit* touches = new PxRaycastHit[touchesMaxCount];
			PxRaycastBuffer hits(touches, touchesMaxCount);
			PxQueryFilterData queryFilterData(CalculateQueryDynamicsFlags(dynamicsFlags) | PxQueryFlag::eNO_BLOCK);

			bool status = physxScene->raycast(start.PhysXVector, direction.Normalized().PhysXVector, maxDistance, hits, PxHitFlag::eDISTANCE | PxHitFlag::eMTD | PxHitFlag::eNORMAL | PxHitFlag::ePOSITION, queryFilterData);
			
			if(status)
			{
				status = false;
				for(MUint32 i = 0; i < hits.nbTouches; ++i)
				{
					PxRaycastHit& touch = hits.touches[i];
					if(!touch.hadInitialOverlap())
					{
						PhysxActorData* data = (PhysxActorData*)touch.actor->userData;
						status = true;
						
						Handle<GameObject> hitObject = Handle<GameObject>();
						if(data != nullptr && data->Actor.IsValid() && data->Actor->GetEnabled() && data->Actor->GetOwner().IsValid() && data->Actor->GetOwner()->GetEnabled())
						{
							hitObject = data->Actor->GetOwner();
						}

						outHits.Add(
							QueryHit(
								touch.position,
								touch.normal,
								touch.distance,
								hitObject
							)
						);
					}
				}
			}

			delete[] touches;
			return status;
		}

		bool PhysicsUtility::SweepSingle(const MVector3& startPosition, const MQuaternion& startRotation, const MVector3& direction, MFloat32 maxDistance, QueryGeometryData& geometryData, QueryHit& outHit, QueryDynamicsFlags dynamicsFlags)
		{
			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}
			
			PxGeometry* sweepGeometry = nullptr;

			switch(geometryData.Geometry)
			{
			case QueryGeometryType::BOX:
				sweepGeometry = new PxBoxGeometry(geometryData.Data.BoxHalfSize.PhysXVector);
				break;
			case QueryGeometryType::CAPSULE:
				sweepGeometry = new PxCapsuleGeometry(geometryData.Data.CapsuleRadius, geometryData.Data.CapsuleHalfHeight);
				break;
			case QueryGeometryType::SPHERE:
				sweepGeometry = new PxSphereGeometry(geometryData.Data.SphereRadius);
				break;
			}

			if(sweepGeometry == nullptr)
			{
				return false;
			}

			maxDistance = MMath::Clamp(maxDistance, 0.0f, FLT_MAX);

			static const MUint32 touchesMaxCount = 8;
			PxSweepHit* touches = new PxSweepHit[touchesMaxCount];
			PxSweepBuffer hits(touches, touchesMaxCount);
			PxQueryFilterData queryFilterData(CalculateQueryDynamicsFlags(dynamicsFlags) | PxQueryFlag::eNO_BLOCK);

			bool status = physxScene->sweep(*sweepGeometry, PxTransform(startPosition.PhysXVector, startRotation.PhysxQuaternion), direction.Normalized().PhysXVector, maxDistance, hits, PxHitFlag::eDISTANCE | PxHitFlag::eMTD | PxHitFlag::eNORMAL | PxHitFlag::ePOSITION, queryFilterData);

			if(status)
			{
				status = false;
				outHit.HitDistance = FLT_MAX;
				for(MUint32 i = 0; i < hits.nbTouches; ++i)
				{
					PxSweepHit& touch = hits.touches[i];
					if(!touch.hadInitialOverlap())
					{
						PhysxActorData* data = (PhysxActorData*)touch.actor->userData;
						status = true;
						if(touch.distance < outHit.HitDistance)
						{
							outHit.HitPosition = touch.position;
							outHit.HitNormal = touch.normal;
							outHit.HitDistance = touch.distance;
							if(data != nullptr && data->Actor.IsValid() && data->Actor->GetEnabled() && data->Actor->GetOwner().IsValid() && data->Actor->GetOwner()->GetEnabled())
							{
								outHit.GameObject = data->Actor->GetOwner();
							}
							else
							{
								outHit.GameObject = Handle<GameObject>();
							}
						}
					}
				}
			}

			delete sweepGeometry;
			delete[] touches;

			return status;
		}

		bool PhysicsUtility::SweepSingle(Handle<Transform> startTransform, const MVector3& direction, MFloat32 maxDistance, QueryGeometryData& geometryData, QueryHit& outHit, QueryDynamicsFlags dynamicsFlags)
		{
			if(!startTransform.IsValid())
			{
				return false;
			}

			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}

			PxGeometry* sweepGeometry = nullptr;

			switch(geometryData.Geometry)
			{
			case QueryGeometryType::BOX:
				sweepGeometry = new PxBoxGeometry(geometryData.Data.BoxHalfSize.PhysXVector);
				break;
			case QueryGeometryType::CAPSULE:
				sweepGeometry = new PxCapsuleGeometry(geometryData.Data.CapsuleRadius, geometryData.Data.CapsuleHalfHeight);
				break;
			case QueryGeometryType::SPHERE:
				sweepGeometry = new PxSphereGeometry(geometryData.Data.SphereRadius);
				break;
			}

			if(sweepGeometry == nullptr)
			{
				return false;
			}

			maxDistance = MMath::Clamp(maxDistance, 0.0f, FLT_MAX);

			static const MUint32 touchesMaxCount = 8;
			PxSweepHit* touches = new PxSweepHit[touchesMaxCount];
			PxSweepBuffer hits(touches, touchesMaxCount);
			PxQueryFilterData queryFilterData(CalculateQueryDynamicsFlags(dynamicsFlags) | PxQueryFlag::eNO_BLOCK);

			bool status = physxScene->sweep(*sweepGeometry, startTransform->GetPhysxTransform(), direction.Normalized().PhysXVector, maxDistance, hits, PxHitFlag::eDISTANCE | PxHitFlag::eMTD | PxHitFlag::eNORMAL | PxHitFlag::ePOSITION, queryFilterData);

			if(status)
			{
				status = false;
				outHit.HitDistance = FLT_MAX;
				for(MUint32 i = 0; i < hits.nbTouches; ++i)
				{
					PxSweepHit& touch = hits.touches[i];
					if(!touch.hadInitialOverlap())
					{
						PhysxActorData* data = (PhysxActorData*)touch.actor->userData;
						status = true;
						if(touch.distance < outHit.HitDistance)
						{
							outHit.HitPosition = touch.position;
							outHit.HitNormal = touch.normal;
							outHit.HitDistance = touch.distance;
							if(data != nullptr && data->Actor.IsValid() && data->Actor->GetEnabled() && data->Actor->GetOwner().IsValid() && data->Actor->GetOwner()->GetEnabled())
							{
								outHit.GameObject = data->Actor->GetOwner();
							}
							else
							{
								outHit.GameObject = Handle<GameObject>();
							}
						}
					}
				}
			}

			delete sweepGeometry;
			delete[] touches;

			return status;
		}

		bool PhysicsUtility::SweepMulti(const MVector3& startPosition, const MQuaternion& startRotation, const MVector3& direction, MFloat32 maxDistance, QueryGeometryData& geometryData, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags)
		{
			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}

			PxGeometry* sweepGeometry = nullptr;

			switch(geometryData.Geometry)
			{
			case QueryGeometryType::BOX:
				sweepGeometry = new PxBoxGeometry(geometryData.Data.BoxHalfSize.PhysXVector);
				break;
			case QueryGeometryType::CAPSULE:
				sweepGeometry = new PxCapsuleGeometry(geometryData.Data.CapsuleRadius, geometryData.Data.CapsuleHalfHeight);
				break;
			case QueryGeometryType::SPHERE:
				sweepGeometry = new PxSphereGeometry(geometryData.Data.SphereRadius);
				break;
			}

			if(sweepGeometry == nullptr)
			{
				return false;
			}

			maxDistance = MMath::Clamp(maxDistance, 0.0f, FLT_MAX);

			static const MUint32 touchesMaxCount = 256;
			PxSweepHit* touches = new PxSweepHit[touchesMaxCount];
			PxSweepBuffer hits(touches, touchesMaxCount);
			PxQueryFilterData queryFilterData(CalculateQueryDynamicsFlags(dynamicsFlags) | PxQueryFlag::eNO_BLOCK);

			bool status = physxScene->sweep(*sweepGeometry, PxTransform(startPosition.PhysXVector, startRotation.PhysxQuaternion), direction.Normalized().PhysXVector, maxDistance, hits, PxHitFlag::eDISTANCE | PxHitFlag::eMTD | PxHitFlag::eNORMAL | PxHitFlag::ePOSITION, queryFilterData);

			if(status)
			{
				status = false;
				for(MUint32 i = 0; i < hits.nbTouches; ++i)
				{
					PxSweepHit& touch = hits.touches[i];
					if(!touch.hadInitialOverlap())
					{
						PhysxActorData* data = (PhysxActorData*)touch.actor->userData;
						status = true;

						Handle<GameObject> hitObject = Handle<GameObject>();
						if(data != nullptr && data->Actor.IsValid() && data->Actor->GetEnabled() && data->Actor->GetOwner().IsValid() && data->Actor->GetOwner()->GetEnabled())
						{
							hitObject = data->Actor->GetOwner();
						}

						outHits.Add(
							QueryHit(
								touch.position,
								touch.normal,
								touch.distance,
								hitObject
							)
						);
					}
				}
			}

			delete sweepGeometry;
			delete[] touches;

			return status;
		}

		bool PhysicsUtility::SweepMulti(Handle<Transform> startTransform, const MVector3& direction, MFloat32 maxDistance, QueryGeometryData& geometryData, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags)
		{
			if(!startTransform.IsValid())
			{
				return false;
			}

			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}

			PxGeometry* sweepGeometry = nullptr;

			switch(geometryData.Geometry)
			{
			case QueryGeometryType::BOX:
				sweepGeometry = new PxBoxGeometry(geometryData.Data.BoxHalfSize.PhysXVector);
				break;
			case QueryGeometryType::CAPSULE:
				sweepGeometry = new PxCapsuleGeometry(geometryData.Data.CapsuleRadius, geometryData.Data.CapsuleHalfHeight);
				break;
			case QueryGeometryType::SPHERE:
				sweepGeometry = new PxSphereGeometry(geometryData.Data.SphereRadius);
				break;
			}

			if(sweepGeometry == nullptr)
			{
				return false;
			}

			maxDistance = MMath::Clamp(maxDistance, 0.0f, FLT_MAX);

			static const MUint32 touchesMaxCount = 256;
			PxSweepHit* touches = new PxSweepHit[touchesMaxCount];
			PxSweepBuffer hits(touches, touchesMaxCount);
			PxQueryFilterData queryFilterData(CalculateQueryDynamicsFlags(dynamicsFlags) | PxQueryFlag::eNO_BLOCK);

			bool status = physxScene->sweep(*sweepGeometry, startTransform->GetPhysxTransform(), direction.Normalized().PhysXVector, maxDistance, hits, PxHitFlag::eDISTANCE | PxHitFlag::eMTD | PxHitFlag::eNORMAL | PxHitFlag::ePOSITION, queryFilterData);

			if(status)
			{
				status = false;
				for(MUint32 i = 0; i < hits.nbTouches; ++i)
				{
					PxSweepHit& touch = hits.touches[i];
					if(!touch.hadInitialOverlap())
					{
						PhysxActorData* data = (PhysxActorData*)touch.actor->userData;
						status = true;

						Handle<GameObject> hitObject = Handle<GameObject>();
						if(data != nullptr && data->Actor.IsValid() && data->Actor->GetEnabled() && data->Actor->GetOwner().IsValid() && data->Actor->GetOwner()->GetEnabled())
						{
							hitObject = data->Actor->GetOwner();
						}

						outHits.Add(
							QueryHit(
								touch.position,
								touch.normal,
								touch.distance,
								hitObject
							)
						);
					}
				}
			}

			delete sweepGeometry;
			delete[] touches;

			return status;
		}

		bool PhysicsUtility::Overlaps(Handle<BoxCollider> collider, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags, bool bIncludeSelf)
		{
			if(!collider.IsValid())
			{
				return false;
			}

			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}

			PxTransform transform = PxTransform::createIdentity();
			if(!collider->GetOwner().IsValid() || !collider->GetOwner()->GetTransform().IsValid())
			{
				return false;
			}
			transform = collider->GetOwner()->GetTransform()->GetPhysxTransform();

			PxShape* physxShape = collider->_physxShape;
			if(physxShape == nullptr)
			{
				return false;
			}

			PxBoxGeometry box;
			if(!physxShape->getBoxGeometry(box))
			{
				return false;
			}

			bool status = Overlaps(physxScene, transform, box, outHits, dynamicsFlags);

			if(status && !bIncludeSelf)
			{
				//Remove collider owner from outHits
				for(MSize i = 0; i < outHits.GetSize(); ++i)
				{
					QueryHit& qh = outHits[i];
					if(qh.GameObject == collider->GetOwner())
					{
						outHits.RemoveAt(i);
						break;
					}
				}
			}

			return status;
		}

		bool PhysicsUtility::Overlaps(Handle<SphereCollider> collider, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags, bool bIncludeSelf)
		{
			if(!collider.IsValid())
			{
				return false;
			}

			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}

			PxTransform transform = PxTransform::createIdentity();
			if(!collider->GetOwner().IsValid() || !collider->GetOwner()->GetTransform().IsValid())
			{
				return false;
			}
			transform = collider->GetOwner()->GetTransform()->GetPhysxTransform();

			PxShape* physxShape = collider->_physxShape;
			if(physxShape == nullptr)
			{
				return false;
			}

			PxSphereGeometry sphere;
			if(!physxShape->getSphereGeometry(sphere))
			{
				return false;
			}

			bool status = Overlaps(physxScene, transform, sphere, outHits, dynamicsFlags);

			if(status && !bIncludeSelf)
			{
				//Remove collider owner from outHits
				for(MSize i = 0; i < outHits.GetSize(); ++i)
				{
					QueryHit& qh = outHits[i];
					if(qh.GameObject == collider->GetOwner())
					{
						outHits.RemoveAt(i);
						break;
					}
				}
			}

			return status;
		}

		bool PhysicsUtility::Overlaps(Handle<CapsuleCollider> collider, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags, bool bIncludeSelf)
		{
			if(!collider.IsValid())
			{
				return false;
			}

			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}

			PxTransform transform = PxTransform::createIdentity();
			if(!collider->GetOwner().IsValid() || !collider->GetOwner()->GetTransform().IsValid())
			{
				return false;
			}
			transform = collider->GetOwner()->GetTransform()->GetPhysxTransform();

			PxShape* physxShape = collider->_physxShape;
			if(physxShape == nullptr)
			{
				return false;
			}
			PxTransform localPose = physxShape->getLocalPose();
			transform = localPose * transform;

			PxCapsuleGeometry capsule;
			if(!physxShape->getCapsuleGeometry(capsule))
			{
				return false;
			}

			bool status = Overlaps(physxScene, transform, capsule, outHits, dynamicsFlags);

			if(status && !bIncludeSelf)
			{
				//Remove collider owner from outHits
				for(MSize i = 0; i < outHits.GetSize(); ++i)
				{
					QueryHit& qh = outHits[i];
					if(qh.GameObject == collider->GetOwner())
					{
						outHits.RemoveAt(i);
						break;
					}
				}
			}

			return status;
		}

		bool PhysicsUtility::Overlaps(Handle<MeshCollider> collider, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags, bool bIncludeSelf)
		{
			ME_ASSERT(false, "Mesh colliders are not supported in MorphEngine");

			if(!collider.IsValid())
			{
				return false;
			}

			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}

			PxTransform transform = PxTransform::createIdentity();
			if(!collider->GetOwner().IsValid() || !collider->GetOwner()->GetTransform().IsValid())
			{
				return false;
			}
			transform = collider->GetOwner()->GetTransform()->GetPhysxTransform();

			PxShape* physxShape = collider->_physxShape;
			if(physxShape == nullptr)
			{
				return false;
			}

			PxConvexMeshGeometry mesh;
			if(!physxShape->getConvexMeshGeometry(mesh))
			{
				return false;
			}

			bool status = Overlaps(physxScene, transform, mesh, outHits, dynamicsFlags);

			if(status && !bIncludeSelf)
			{
				//Remove collider owner from outHits
				for(MSize i = 0; i < outHits.GetSize(); ++i)
				{
					QueryHit& qh = outHits[i];
					if(qh.GameObject == collider->GetOwner())
					{
						outHits.RemoveAt(i);
						break;
					}
				}
			}

			return status;
		}

		bool PhysicsUtility::Overlaps(const QueryGeometryData& geometryData, Handle<gom::Transform> transform, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags)
		{
			if(!transform.IsValid())
			{
				return false;
			}

			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}

			PxGeometry* geometry = nullptr;

			switch(geometryData.Geometry)
			{
			case QueryGeometryType::BOX:
				geometry = new PxBoxGeometry(geometryData.Data.BoxHalfSize.PhysXVector);
				break;
			case QueryGeometryType::CAPSULE:
				geometry = new PxCapsuleGeometry(geometryData.Data.CapsuleRadius, geometryData.Data.CapsuleHalfHeight);
				break;
			case QueryGeometryType::SPHERE:
				geometry = new PxSphereGeometry(geometryData.Data.SphereRadius);
				break;
			}

			if(geometry == nullptr)
			{
				return false;
			}

			return Overlaps(physxScene, transform->GetPhysxTransform(), *geometry, outHits, dynamicsFlags);
		}

		bool PhysicsUtility::Overlaps(const QueryGeometryData& geometryData, const MVector3& startPosition, const MQuaternion& startRotation, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags)
		{
			PxScene* physxScene = PhysicsManager::GetInstance()->_scene;
			if(physxScene == nullptr)
			{
				return false;
			}

			PxGeometry* geometry = nullptr;

			switch(geometryData.Geometry)
			{
			case QueryGeometryType::BOX:
				geometry = new PxBoxGeometry(geometryData.Data.BoxHalfSize.PhysXVector);
				break;
			case QueryGeometryType::CAPSULE:
				geometry = new PxCapsuleGeometry(geometryData.Data.CapsuleRadius, geometryData.Data.CapsuleHalfHeight);
				break;
			case QueryGeometryType::SPHERE:
				geometry = new PxSphereGeometry(geometryData.Data.SphereRadius);
				break;
			}

			if(geometry == nullptr)
			{
				return false;
			}

			return Overlaps(physxScene, PxTransform(startPosition.PhysXVector, startRotation.PhysxQuaternion), *geometry, outHits, dynamicsFlags);
		}
	}
}
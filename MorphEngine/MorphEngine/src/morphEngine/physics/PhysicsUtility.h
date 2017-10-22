#pragma once

#include "PhysicsManager.h"
#include "gom\Transform.h"

namespace morphEngine
{
	namespace physics
	{
		enum class QueryDynamicsFlags
		{
			//Only static rigidbodies will be taken into account in queries
			STATIC_ONLY = 0x01,
			//Only dynamic rigidbodies will be taken into account in queries
			DYNAMIC_ONLY = 0x10,
			//All rigidbodies will be taken into account in queries
			ALL = STATIC_ONLY | DYNAMIC_ONLY,
		};
		EnumClassOperations(QueryDynamicsFlags);

		enum class QueryGeometryType
		{
			BOX = 0,
			SPHERE = 1,
			CAPSULE = 2,
		};

		struct QueryGeometryData
		{
		public:
			QueryGeometryType Geometry;
			union GeometryData
			{
				MVector3 BoxHalfSize;
				struct
				{
					MFloat32 BoxHalfSizeX;
					MFloat32 BoxHalfSizeY;
					MFloat32 BoxHalfSizeZ;
				};
				struct
				{
					MFloat32 CapsuleHalfHeight;
					MFloat32 CapsuleRadius;
					MFloat32 CapsulePadding;
				};
				struct
				{
					MFloat32 SphereRadius;
					MFloat32 SpherePadding1;
					MFloat32 SpherePadding2;
				};

				inline GeometryData() { }
			} Data;

		public:
			inline QueryGeometryData() : Geometry(QueryGeometryType::BOX) {}
			inline QueryGeometryData(QueryGeometryType geometry) : Geometry(geometry) { }
		};

		//Struct containing hit data such as hit position, normal, distance and hit game object
		struct QueryHit
		{
			friend class PhysicsUtility;

		public:
			memoryManagement::Handle<gom::GameObject> GameObject;
			utility::MVector3 HitPosition;
			utility::MVector3 HitNormal;
			MFloat32 HitDistance;

		protected:
			QueryHit(const MVector3& position, const MVector3& normal, MFloat32 distance, memoryManagement::Handle<gom::GameObject> gameObject) : GameObject(gameObject), HitPosition(position), HitNormal(normal), HitDistance(distance)
			{

			}

		public:
			inline QueryHit() : HitPosition(MVector3::Zero), HitNormal(MVector3::Up), HitDistance(FLT_MAX), GameObject()
			{

			}

			inline bool operator==(const QueryHit& other) const
			{
				return GameObject == other.GameObject && MMath::Approximately(HitDistance, other.HitDistance) && HitPosition == other.HitPosition && HitNormal == other.HitNormal;
			}

			inline bool operator!=(const QueryHit& other) const
			{
				return GameObject != other.GameObject || !MMath::Approximately(HitDistance, other.HitDistance) || HitPosition != other.HitPosition || HitNormal == other.HitNormal;
			}

			inline bool operator<(const QueryHit& other) const
			{
				return HitDistance < other.HitDistance;
			}

			inline bool operator<=(const QueryHit& other) const
			{
				return HitDistance <= other.HitDistance;
			}

			inline bool operator>(const QueryHit& other) const
			{
				return HitDistance > other.HitDistance;
			}

			inline bool operator>=(const QueryHit& other) const
			{
				return HitDistance >= other.HitDistance;
			}
		};

		//Static class for some physics related stuff like queries (raycasting, sweepcasting, overlapping)
		class PhysicsUtility
		{
		protected:
			inline static physx::PxQueryFlags CalculateQueryDynamicsFlags(QueryDynamicsFlags dynamicsFlags)
			{
				switch(dynamicsFlags)
				{
				case QueryDynamicsFlags::ALL:
					return physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC;
				case QueryDynamicsFlags::DYNAMIC_ONLY:
					return physx::PxQueryFlag::eDYNAMIC;
				case QueryDynamicsFlags::STATIC_ONLY:
					return physx::PxQueryFlag::eSTATIC;
				}

				ME_ASSERT(false, "One does not simply reach this code! PhysicsUtility::CalculateQueryDynamicsFlags received invalid dynamicsFlags");
			}

			static bool Overlaps(physx::PxScene* physxScene, const physx::PxTransform& transform, const physx::PxGeometry& geometry, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags);

		public:
			//Returns true if there was any hit
			//outHit will be filled with first hit encountered from start to start + direction * maxDistance
			//Does not include shape in which it's starting (i.e. if raycast starts from the shape then that shape and its actor will not be included in out hit)
			static bool RaycastSingle(const MVector3& start, const MVector3& direction, MFloat32 maxDistance, QueryHit& outHit, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL);
			//Returns true if there was any hit
			//outHits will be filled with all hits in maxDistance from start in direction
			//Does not include shape in which it's starting (i.e. if raycast starts from the shape then that shape and its actor will not be included in out hit)
			static bool RaycastMulti(const MVector3& start, const MVector3& direction, MFloat32 maxDistance, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL);

			//Returns true if there was any hit
			//outHit will be filled with first hit encountered from startPosition (and startRotation) to start + direction * maxDistance
			//Does not include shape in which it's starting (i.e. if sweep starts from the shape then that shape and its actor will not be included in out hit)
			static bool SweepSingle(const MVector3& startPosition, const MQuaternion& startRotation, const MVector3& direction, MFloat32 maxDistance, QueryGeometryData& geometryData, QueryHit& outHit, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL);
			//Returns true if there was any hit
			//outHit will be filled with first hit encountered from startTransform to startTransform position + direction * maxDistance
			//Does not include shape in which it's starting (i.e. if sweep starts from the shape then that shape and its actor will not be included in out hit)
			static bool SweepSingle(Handle<gom::Transform> startTransform, const MVector3& direction, MFloat32 maxDistance, QueryGeometryData& geometryData, QueryHit& outHit, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL);
			//Returns true if there was any hit
			//outHits will be filled with all hits in maxDistance from startPosition and startRotation in direction
			//Does not include shape in which it's starting (i.e. if sweep starts from the shape then that shape and its actor will not be included in out hit)
			static bool SweepMulti(const MVector3& startPosition, const MQuaternion& startRotation, const MVector3& direction, MFloat32 maxDistance, QueryGeometryData& geometryData, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL);
			//Returns true if there was any hit
			//outHits will be filled with all hits in maxDistance from startTransform in direction
			//Does not include shape in which it's starting (i.e. if sweep starts from the shape then that shape and its actor will not be included in out hit)
			static bool SweepMulti(Handle<gom::Transform> startTransform, const MVector3& direction, MFloat32 maxDistance, QueryGeometryData& geometryData, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL);

			//Returns true if overlap test found something that is overlapped by given collider (self overlapping may be controlled by bIncludeSelf parameter)
			//outHits will be filled with all hits in overlapped region
			//NOTE: Overlaps will fill only Actor data (no position, normal etc.)
			static bool Overlaps(Handle<BoxCollider> collider, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL, bool bIncludeSelf = false);
			//Returns true if overlap test found something that is overlapped by given collider (self overlapping may be controlled by bIncludeSelf parameter)
			//outHits will be filled with all hits in overlapped region
			//NOTE: Overlaps will fill only Actor data (no position, normal etc.)
			static bool Overlaps(Handle<SphereCollider> collider, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL, bool bIncludeSelf = false);
			//Returns true if overlap test found something that is overlapped by given collider (self overlapping may be controlled by bIncludeSelf parameter)
			//outHits will be filled with all hits in overlapped region
			//NOTE: Overlaps will fill only Actor data (no position, normal etc.)
			static bool Overlaps(Handle<CapsuleCollider> collider, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL, bool bIncludeSelf = false);
			//Returns true if overlap test found something that is overlapped by given collider (self overlapping may be controlled by bIncludeSelf parameter)
			//outHits will be filled with all hits in overlapped region
			//NOTE: Overlaps will fill only Actor data (no position, normal etc.)
			static bool Overlaps(Handle<MeshCollider> collider, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL, bool bIncludeSelf = false);
			//Returns true if overlap test found something that is overlapped by given geometry
			//outHits will be filled with all hits in overlapped region
			//NOTE: Overlaps will fill only Actor data (no position, normal etc.)
			static bool Overlaps(const QueryGeometryData& geometryData, Handle<gom::Transform> transform, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL);
			//Returns true if overlap test found something that is overlapped by given geometry
			//outHits will be filled with all hits in overlapped region
			//NOTE: Overlaps will fill only Actor data (no position, normal etc.)
			static bool Overlaps(const QueryGeometryData& geometryData, const MVector3& startPosition, const MQuaternion& startRotation, MArray<QueryHit>& outHits, QueryDynamicsFlags dynamicsFlags = QueryDynamicsFlags::ALL);
		};
	}
}
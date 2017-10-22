#pragma once
#include "Bounds.h"

namespace morphEngine
{
	namespace renderer
	{

		class BoundsSphere : public Bounds
		{
		public:

#pragma region Public

			utility::MVector3 CenterLocal = utility::MVector3::Zero;
			MFloat32 RadiusLocal = 1.0f;

#pragma endregion

		protected:

#pragma region Protected

			utility::MVector3 _centerWorld = CenterLocal;
			MFloat32 _radiusWorld = RadiusLocal;

#pragma endregion

		public:

#pragma region Functions Public

			inline BoundsSphere()
			{
			}

			inline BoundsSphere(const BoundsSphere& c) : Bounds(c),
				CenterLocal(c.CenterLocal),
				RadiusLocal(c.RadiusLocal),
				_centerWorld(c._centerWorld),
				_radiusWorld(c._radiusWorld)
			{
			}

			inline virtual ~BoundsSphere()
			{
			}

			inline virtual void Draw(memoryManagement::Handle<gom::Camera> camera) const override
			{

			}

			inline virtual void UpdateToWorld(memoryManagement::Handle<gom::Transform> transform) override
			{
				_centerWorld = transform->GetMatWorld() * MVector4(CenterLocal, 1.0f);
				_radiusWorld = (transform->GetMatWorld() * MVector4(RadiusLocal * ExternalScale, 0.0f, 0.0f, 0.0f)).Length();
			}

			inline virtual void UpdateToWorldIdentity() override
			{
				_centerWorld = CenterLocal;
				_radiusWorld = RadiusLocal * ExternalScale;
			}

			inline virtual bool IntersectsWith(const BoundsSphere& other) const override
			{
				MFloat32 radiusSum = _radiusWorld + other._radiusWorld;
				return (_centerWorld - other._centerWorld).LengthSquared() <= (radiusSum * radiusSum);
			}

			virtual bool IntersectsWith(const BoundsBox& other) const override;
			virtual bool IntersectsWith(const BoundsFrustum& other) const override;

			inline virtual bool IsInside(const BoundsSphere& other) const override
			{
				MFloat32 distanceCenters = (_centerWorld - other._centerWorld).Length();
				MFloat32 radiusDiffs = (_radiusWorld) - (other._radiusWorld);
				return radiusDiffs <= distanceCenters;
			}

			virtual bool IsInside(const BoundsBox& other) const override;
			virtual bool IsInside(const BoundsFrustum& other) const override;

			const utility::MVector3& GetCenterWorld() const { return _centerWorld; }
			MFloat32 GetRadiusWorld() const { return _radiusWorld; }

#pragma endregion


		};
	}
}
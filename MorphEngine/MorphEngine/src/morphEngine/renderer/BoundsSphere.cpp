#include "BoundsSphere.h"
#include "BoundsBox.h"
#include "BoundsFrustum.h"

namespace morphEngine
{
	namespace renderer
	{
		bool BoundsSphere::IntersectsWith(const BoundsBox & other) const
		{
			return other.IntersectsWith(*this);
		}
		bool BoundsSphere::IntersectsWith(const BoundsFrustum & other) const
		{
			return other.IntersectsWith(*this);
		}

		bool BoundsSphere::IsInside(const BoundsBox & other) const
		{
			// TODO: Take all vertices into consideration
			MFloat32 rSquared = GetRadiusWorld() * GetRadiusWorld();
			return (other.GetMinWorld(), other.GetCenterWorld()).LengthSquared() > rSquared && (other.GetMaxWorld(), other.GetCenterWorld()).LengthSquared() > rSquared;
		}
		bool BoundsSphere::IsInside(const BoundsFrustum & other) const
		{
			return false;
		}
	}
}
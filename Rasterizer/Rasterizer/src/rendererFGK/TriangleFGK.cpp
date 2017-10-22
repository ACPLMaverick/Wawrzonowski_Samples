#include "TriangleFGK.h"


namespace rendererFGK
{
	TriangleFGK::TriangleFGK(math::Float3& x, math::Float3& y, math::Float3& z,
		math::Float2& ux, math::Float2& uy, math::Float2& uz,
		math::Float3& cx, math::Float3& cy, math::Float3& cz,
		Color32& col) :
		Triangle(x, y, z, ux, uy, uz, cx, cy, cz, col),
		_plane(x, y, z),
		_bbMin
		(
			min(min(v1.x, v2.x), v3.x),
			min(min(v1.y, v2.y), v3.y),
			min(min(v1.z, v2.z), v3.z)
		),
		_bbMax
		(
			max(max(v1.x, v2.x), v3.x),
			max(max(v1.y, v2.y), v3.y),
			max(max(v1.z, v2.z), v3.z)
		)
	{
	}


	TriangleFGK::~TriangleFGK()
	{
	}

	RayHit TriangleFGK::CalcIntersect(Ray & ray)
	{
		// normal check
		if (math::Float3::Dot(ray.GetDirection(), _plane.GetNormal()) < 0.0f)
		{
			return RayHit();
		}

		// plane check
		RayHit rayHit(_plane.CalcIntersect(ray));
		if (rayHit.hit)
		{
#ifdef RENDERER_FGK_BOUNDINGBOX
			// bounding box check
			if (
				rayHit.point.GreaterEqualsEpsilon(_bbMin, 0.1f) &&
				rayHit.point.SmallerEqualsEpsilon(_bbMax, 0.1f)
				)
			{
#endif
				// barycentric check
				
				math::Float3 g0 = v2 - v1, g1 = v3 - v1, g2 = rayHit.point - v1;
				float d00 = math::Float3::Dot(g0, g0);
				float d01 = math::Float3::Dot(g0, g1);
				float d11 = math::Float3::Dot(g1, g1);
				float d20 = math::Float3::Dot(g2, g0);
				float d21 = math::Float3::Dot(g2, g1);
				float bdenom = 1.0f / (d00 * d11 - d01 * d01);
				rayHit.barycentric.x = (d11 * d20 - d01 * d21) * bdenom;
				rayHit.barycentric.y = (d00 * d21 - d01 * d20) * bdenom;
				rayHit.barycentric.z = 1.0f - rayHit.barycentric.x - rayHit.barycentric.y;

				if (
					(rayHit.barycentric.x < 0.0f) ||
					(rayHit.barycentric.y < 0.0f) ||
					(rayHit.barycentric.z < 0.0f)
					)
				{
					rayHit.hit = false;
				}
				else
				{
					// ray distance check

					if (ray.GetMaxDistance() != 0.0f)
					{
						if (math::Float3::LengthSquared(rayHit.point - ray.GetOrigin()) > ray.GetMaxDistanceSquared())
						{
							return RayHit();
						}
					}

					// calculate interpolated normal and uv
					rayHit.normal = c2 * rayHit.barycentric.x + c3 * rayHit.barycentric.y + c1 * rayHit.barycentric.z;
					math::Float3::Normalize(rayHit.normal);

					rayHit.uv = u2 * rayHit.barycentric.x + u3 * rayHit.barycentric.y + u1 * rayHit.barycentric.z;

					rayHit.hit = true;
				}
#ifdef RENDERER_FGK_BOUNDINGBOX
			}
			else
			{
				rayHit.hit = false;
			}
#endif
		}

		return rayHit;
	}

	void TriangleFGK::Draw()
	{
	}

	void TriangleFGK::SetPosition(const math::Float3 & x, const math::Float3 & y, const math::Float3 & z)
	{
		v1 = x;
		v2 = y;
		v3 = z;

		RecalculatePlane();
	}

	void TriangleFGK::SetNormals(const math::Float3 & x, const math::Float3 & y, const math::Float3 & z)
	{
		c1 = x;
		c2 = y;
		c3 = z;
	}

	void TriangleFGK::SetUvs(const math::Float2 & x, const math::Float2 & y, const math::Float2 & z)
	{
		u1 = x;
		u2 = y;
		u3 = z;
	}

	void TriangleFGK::RecalculatePlane()
	{
		_plane = Plane(v1, v2, v3);
		_bbMin = math::Float3
		(
			min(min(v1.x, v2.x), v3.x),
			min(min(v1.y, v2.y), v3.y),
			min(min(v1.z, v2.z), v3.z)
		);
		_bbMax = math::Float3
		(
			max(max(v1.x, v2.x), v3.x),
			max(max(v1.y, v2.y), v3.y),
			max(max(v1.z, v2.z), v3.z)
		);
	}
}
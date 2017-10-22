#include "Sphere.h"

#include <stdio.h>

Sphere::Sphere()
{
	this->_center = math::Float3();
	this->_radius = 1.0f;
}

Sphere::Sphere(math::Float3& c, float r)
{
	this->_center = c;
	this->_radius = r;
}

RayHit Sphere::CalcIntersect(Ray& ray)
{
	//distance check
	if (ray.GetMaxDistance() != 0.0f)
	{
		math::Float3 nearestPoint = _center - ray.GetDirection() * _radius;
		if (math::Float3::LengthSquared(nearestPoint - ray.GetOrigin()) > ray.GetMaxDistanceSquared())
		{
			return RayHit();
		}
	}

	math::Float3 ocVec = ray.GetOrigin() - _center;
	float B = -math::Float3::Dot(ray.GetDirection(), ocVec);
	float det = (B * B) - math::Float3::Dot(ocVec, ocVec) + (_radius * _radius);

	if (det > 0)
	{
		det = sqrt(det);
		float d1 = B + det;
		float d2 = B - det;

		if (d1 > 0)
		{
			if (d2 < 0)
			{
				//Ray origin inside sphere case
				math::Float3 p = math::Float3(ray.GetOrigin() + ray.GetDirection() * d1);
				if (math::Float3::LengthSquared(p - ray.GetOrigin()) > ray.GetMaxDistanceSquared())
				{
					return RayHit();
				}
				else
				{
					return RayHit(true, p);
				}
			}
			else
			{
				//Ray origin in front of sphere case
				math::Float3 p = math::Float3(ray.GetOrigin() + ray.GetDirection() * d2);
				if (math::Float3::LengthSquared(p - ray.GetOrigin()) > ray.GetMaxDistanceSquared())
				{
					return RayHit();
				}
				else
				{
					return RayHit(true, p);
				}
			}
		}
		else
		{
			return RayHit();
		}
	}
	else if (det == 0)
	{
		//Ray intersects only in one point (sphere tangent)
		math::Float3 p = math::Float3(ray.GetOrigin() + ray.GetDirection() * B);
		if (math::Float3::LengthSquared(p - ray.GetOrigin()) > ray.GetMaxDistanceSquared())
		{
			return RayHit();
		}
		else
		{
			return RayHit(true, p);
		}
	}
	else
	{
		//Ray doesn't intersect
		return RayHit();
	}
}

void Sphere::Update()
{

}

void Sphere::Draw()
{

}
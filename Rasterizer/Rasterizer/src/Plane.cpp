#include "Plane.h"

Plane::Plane()
{
	this->_point = math::Float3();
	this->_normal = math::Float3(0.0f, 0.0f, 1.0f);
}

Plane::Plane(math::Float3& p, math::Float3& n)
{
	this->_point = p;
	this->_normal = n;
}

Plane::Plane(math::Float3 & p1, math::Float3 & p2, math::Float3 & p3)
{
	this->_point = p1;
	math::Float3 d1 = p2 - p1;
	math::Float3 d2 = p3 - p1;
	math::Float3::Normalize(d1);
	math::Float3::Normalize(d2);
	_normal = -math::Float3::Cross(d1, d2);
	math::Float3::Normalize(_normal);
}

RayHit Plane::CalcIntersect(Ray& ray)
{
	float d = math::Float3::Dot(this->_normal, this->_point);
	float ndv = (math::Float3::Dot(this->_normal, ray.GetDirection()));
	float t = 0.0f;
	if(ndv > 0.0001f)
	{ 
		t = (d - math::Float3::Dot(this->_normal, ray.GetOrigin())) / ndv;
		if (t >= 0.0f)
		{
			//Ray intersects plane
			return RayHit(true, math::Float3(ray.GetOrigin() + ray.GetDirection() * t));
		}
		else
		{
			//Ray intersection is behind ray origin
			return RayHit();
		}
	}
	else
	{
		//Ray is parallel to plane
		return RayHit();
	}
}

void Plane::Update()
{

}

void Plane::Draw()
{

}
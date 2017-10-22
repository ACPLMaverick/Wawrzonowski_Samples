#pragma once

#include "stdafx.h"

struct RayHit
{
	math::Float3 point;

	/// <summary>
	/// This is only valid for ray-triangle intersection.
	/// </summary>
	math::Float3 barycentric;
	math::Float3 normal;
	math::Float2 uv;

	/// <summary>
	/// This is for teting
	/// </summary>
	int debugFlag = 0;
	bool hit;

	RayHit() :
		hit(false),
		point(math::Float3()),
		barycentric(math::Float3()),
		normal(math::Float3(0.0f, 1.0f, 0.0f)),
		uv(math::Float2(0.5f, 0.5f))
	{
	}

	RayHit(bool isHit, math::Float3& p) :
		barycentric(math::Float3()),
		normal(math::Float3(0.0f, 1.0f, 0.0f)),
		uv(math::Float2()),
		hit(isHit),
		point(p)
	{

	}

	RayHit(bool isHit, 
		float maxDist,
		math::Float3& p, 
		math::Float3& bar,
		math::Float3& nrm,
		math::Float2& uvv) :
		hit(isHit),
		point(p),
		barycentric(bar),
		normal(nrm),
		uv(uvv)
	{

	}
};

class Ray
{
protected:
	math::Float3 _origin;
	math::Float3 _dir;

	float _maxDistance;

public:
	Ray();
	Ray(const math::Float3& s, const math::Float3& dir, float maxDistance = 0.0f);

	math::Float3 GetOrigin();
	math::Float3 GetDirection();
	float GetMaxDistance();
	float GetMaxDistanceSquared();
};
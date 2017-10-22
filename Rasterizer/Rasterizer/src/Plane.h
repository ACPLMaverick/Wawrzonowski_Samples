#pragma once

#include "Primitive.h"

class Plane : public Primitive
{
protected:
	math::Float3 _point;
	math::Float3 _normal;

public:	
	Plane();
	Plane(math::Float3& p, math::Float3& n);
	Plane(math::Float3& p1, math::Float3& p2, math::Float3& p3);

	RayHit CalcIntersect(Ray& ray) override;

	void Update() override;
	void Draw() override;

	math::Float3 GetPoint() { return _point; }
	math::Float3 GetNormal() { return _normal; }
};
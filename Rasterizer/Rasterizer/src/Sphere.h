#pragma once

#include "Primitive.h"

class Sphere : public Primitive
{
protected:
	math::Float3 _center;
	float _radius;

public:
	Sphere();
	Sphere(math::Float3& c, float r);

	RayHit CalcIntersect(Ray& ray) override;

	void Update() override;
	void Draw() override;

	math::Float3 GetCenter() { return _center; }
	float GetRadius() { return _radius; }
};
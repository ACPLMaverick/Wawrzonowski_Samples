#pragma once

#include "stdafx.h"
#include "Primitive.h"
#include "Float3.h"
#include "Float2.h"
#include "Color32.h"

class Triangle :
	public Primitive
{
protected:

public:
	math::Float3 v1, v2, v3;
	math::Float3 c1, c2, c3;
	math::Float2 u1, u2, u3;
	Color32 col;

	Triangle(math::Float3& x, math::Float3& y, math::Float3& z, 
		math::Float2& ux, math::Float2& uy, math::Float2& uz,
		math::Float3& cx = math::Float3(1.0f, 1.0f, 1.0f), math::Float3& cy = math::Float3(1.0f, 1.0f, 1.0f), math::Float3& cz = math::Float3(1.0f, 1.0f, 1.0f),
		Color32& col = Color32());
	virtual ~Triangle();

	virtual RayHit CalcIntersect(Ray& ray) = 0;
	virtual void Update() override;
	virtual void Draw() = 0;
};


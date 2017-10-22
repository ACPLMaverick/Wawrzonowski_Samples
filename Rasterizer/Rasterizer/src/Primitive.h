#pragma once

#include "stdafx.h"
#include "Ray.h"
#include "Transform.h"
#include "Material.h"

class Primitive
{
protected:

#pragma region Protected

	Transform _transform;

	Material* _material;

#pragma endregion

#pragma region Functions Protected


#pragma endregion

public:

#pragma region Functions Public

	Primitive();
	Primitive
	(
		const math::Float3* pos,
		const math::Float3* rot,
		const math::Float3* scl
	);
	virtual ~Primitive();

	virtual RayHit CalcIntersect(Ray& ray) = 0;

	virtual void Update() = 0;
	virtual void Draw() = 0;

#pragma region Accessors

	Transform* GetTransform() { return &_transform; }
	Material* GetMaterialPtr() { return _material; }

	void SetMaterialPtr(Material* mat) { _material = mat; }

#pragma endregion

#pragma endregion
};


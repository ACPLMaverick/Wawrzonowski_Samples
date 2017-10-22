#pragma once

#include "Matrix4x4.h"
#include "Float3.h"

class Transform
{
protected:

#pragma region Protected

	math::Matrix4x4 _worldMatrix;
	math::Matrix4x4 _worldMatrixInverseTranspose;

	math::Float3 _position;
	math::Float3 _rotation;
	math::Float3 _scale;

#pragma endregion

#pragma region Functions Protected

	inline void GenerateWorldMatrix();

#pragma endregion

public:

#pragma region Functions Public

	Transform
	(
		const math::Float3* pos,
		const math::Float3* rot,
		const math::Float3* scl
	);
	~Transform();

#pragma region Accessors

	const math::Float3* GetPosition() const { return &_position; }
	const math::Float3* GetRotation() const { return &_rotation; }
	const math::Float3* GetScale() const { return &_scale; }

	const math::Matrix4x4* GetWorldMatrix() const { return &_worldMatrix; }
	const math::Matrix4x4* GetWorldInverseTransposeMatrix() const { return &_worldMatrixInverseTranspose; }

	void SetPosition(const math::Float3* pos) { _position = *pos; GenerateWorldMatrix(); }
	void SetRotation(const math::Float3* rot) 
	{ 
		_rotation = *rot; 
		if (_rotation.x > 360.0f)
		{
			_rotation.x -= 360.0f;
		}
		if (_rotation.y > 360.0f)
		{
			_rotation.y -= 360.0f;
		}
		if (_rotation.z > 360.0f)
		{
			_rotation.z -= 360.0f;
		}
		GenerateWorldMatrix(); 
	}
	void SetScale(const math::Float3* scl) { _scale = *scl; GenerateWorldMatrix(); }

#pragma endregion

#pragma endregion
};


#include "Transform.h"



Transform::Transform
(
	const math::Float3* pos,
	const math::Float3* rot,
	const math::Float3* scl
) :
	_position(*pos),
	_rotation(*rot),
	_scale(*scl)
{
	GenerateWorldMatrix();
}


Transform::~Transform()
{
}

void Transform::GenerateWorldMatrix()
{
	math::Matrix4x4 trans, rot, scl;
	math::Matrix4x4::Translation(&_position, &trans);
	math::Matrix4x4::Rotation(&_rotation, &rot);
	math::Matrix4x4::Scale(&_scale, &scl);

	_worldMatrix = scl * rot * trans;
	_worldMatrixInverseTranspose = _worldMatrix;
	math::Matrix4x4::Inverse(&_worldMatrixInverseTranspose, &_worldMatrixInverseTranspose);
	math::Matrix4x4 temp = _worldMatrixInverseTranspose;
	math::Matrix4x4::Transpose(&temp, &_worldMatrixInverseTranspose);
}
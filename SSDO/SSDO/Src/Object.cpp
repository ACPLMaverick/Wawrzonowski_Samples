#include "stdafx.h"
#include "Object.h"
#include "Material.h"
#include "Renderer.h"
#include "Timer.h"

Object::Object(const Mesh & mesh, const Material & material, const XMFLOAT3A & pos, const XMFLOAT3A & rot, const XMFLOAT3A & scl) :
	_mesh(mesh),
	_material(material),
	_position(pos),
	_rotation(rot),
	_scale(scl)
{
	CreateWorldMatrix();
}

Object::~Object()
{
}

void Object::Update()
{
	if (_bNeedCreateWorldMatrix)
	{
		CreateWorldMatrix();
		_bNeedCreateWorldMatrix = false;
	}
}

inline void Object::CreateWorldMatrix()
{
	XMVECTOR pos = XMLoadFloat3(&_position);
	XMVECTOR rot = XMLoadFloat3(&_rotation);
	XMVECTOR scl = XMLoadFloat3(&_scale);
	XMMATRIX mat = XMMatrixScalingFromVector(scl) * XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z) * XMMatrixTranslationFromVector(pos);
	XMMATRIX matInvTransp = XMMatrixTranspose(XMMatrixInverse(nullptr, mat));
	XMStoreFloat4x4(&_matWorld, mat);
	XMStoreFloat4x4(&_matWorldInvTransp, matInvTransp);
}

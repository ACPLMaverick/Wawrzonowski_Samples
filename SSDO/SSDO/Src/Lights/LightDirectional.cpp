#include "stdafx.h"
#include "LightDirectional.h"
#include "Camera.h"

namespace Lights
{
	LightDirectional::LightDirectional(const XMFLOAT4A & color, const XMFLOAT3A & direction) :
		_color(color),
		_direction(direction)
	{
		SetDirection(direction);
	}

	LightDirectional::~LightDirectional()
	{
	}

	void LightDirectional::ApplyToShader(const LightDirectional & other, const Camera & camera)
	{
		_color = other._color;
		XMVECTOR dir = XMLoadFloat4A(&XMFLOAT4A(other._direction.x, other._direction.y, other._direction.z, 0.0f));
		XMMATRIX v = XMLoadFloat4x4A(&camera.GetMatView());
		dir = XMVector4Transform(dir, v);
		XMStoreFloat3A(&_direction, dir);
	}

}
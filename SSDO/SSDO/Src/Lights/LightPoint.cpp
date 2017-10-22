#include "stdafx.h"
#include "LightPoint.h"
#include "Camera.h"

namespace Lights
{
	LightPoint::LightPoint(const XMFLOAT4A & color, const XMFLOAT3A & position, float range) :
		_color(color),
		_position(position),
		_range(range)
	{
	}

	LightPoint::~LightPoint()
	{
	}

	void LightPoint::ApplyToShader(const LightPoint & other, const Camera & camera)
	{
		_color = other._color;
		_range = other._range;

		XMVECTOR pos = XMLoadFloat4A(&XMFLOAT4A(other._position.x, other._position.y, other._position.z, 1.0f));
		XMMATRIX v = XMLoadFloat4x4A(&camera.GetMatView());
		pos = XMVector4Transform(pos, v);
		XMStoreFloat3A(&_position, pos);
	}
}

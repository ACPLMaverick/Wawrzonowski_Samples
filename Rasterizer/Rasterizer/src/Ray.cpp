#include "Ray.h"

Ray::Ray() :
	_origin(math::Float3()),
	_dir(math::Float3(0.0f, 0.0f, -1.0f)),
	_maxDistance(0.0f)
{
	this->_origin = math::Float3();
	this->_dir = math::Float3();
}

Ray::Ray(const math::Float3& s, const math::Float3& d, float maxDistance) :
	_origin(s),
	_dir(d),
	_maxDistance(maxDistance)
{
}

math::Float3 Ray::GetOrigin()
{
	return _origin;
}

math::Float3 Ray::GetDirection()
{
	return _dir;
}

float Ray::GetMaxDistance()
{
	return _maxDistance;
}

float Ray::GetMaxDistanceSquared()
{
	return _maxDistance * _maxDistance;
}

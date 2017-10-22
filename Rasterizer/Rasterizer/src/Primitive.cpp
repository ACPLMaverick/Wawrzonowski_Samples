#include "Primitive.h"




Primitive::Primitive() :
	_transform(&math::Float3(), &math::Float3(), &math::Float3(1.0f, 1.0f, 1.0f))
{
}

Primitive::Primitive(const math::Float3 * pos, const math::Float3 * rot, const math::Float3 * scl) :
	_transform(pos, rot, scl)
{
}


Primitive::~Primitive()
{
}
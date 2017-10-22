#include "Triangle.h"



Triangle::Triangle(math::Float3& x, math::Float3& y, math::Float3& z, 
	math::Float2& ux, math::Float2& uy, math::Float2& uz,
	math::Float3& cx, math::Float3& cy, math::Float3& cz,
	Color32& col) :
	Primitive()
{
	v1 = x;
	v2 = y;
	v3 = z;
	u1 = ux;
	u2 = uy;
	u3 = uz;
	c1 = cx;
	c2 = cy;
	c3 = cz;
	this->col = col;
}

Triangle::~Triangle()
{
}

void Triangle::Update()
{
}
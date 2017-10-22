#include "SceneTriangle.h"
#include "Camera.h"
#include "Triangle.h"
#include "SpecificObjectFactory.h"
#include "System.h"

SceneTriangle::SceneTriangle()
{
}


SceneTriangle::~SceneTriangle()
{
}

void SceneTriangle::InitializeScene()
{
	_cameras.push_back(new Camera(
		&math::Float3(0.0f, 5.0f, -10.0f),
		&math::Float3(0.0f, 0.0f, 0.0f),
		&math::Float3(0.0f, 1.0f, 0.0f),
		50.0f,
		(float)System::GetInstance()->GetSystemSettings()->GetDisplayWidth() / 
			(float)System::GetInstance()->GetSystemSettings()->GetDisplayHeight()
	));
	_primitives.push_back(SpecificObjectFactory::GetTriangle(
		math::Float3(-4.0f, 0.0f, -5.0f),
		math::Float3(4.0f, 0.0f, 5.0f),
		math::Float3(5.0f, 0.0f, -4.0f),

		math::Float2(0.0f, 0.0f),
		math::Float2(0.0f, 1.0f),
		math::Float2(1.0f, 1.0f),

		math::Float3(1.0f, 0.0f, 0.0f),
		math::Float3(0.0f, 1.0f, 0.0f),
		math::Float3(0.0f, 0.0f, 1.0f),

		Color32(0xFFFFFFFF)
	));
	//_primitives.push_back(SpecificObjectFactory::GetTriangle(
	//	math::Float3(-0.2f, 0.4f, -2.0f),
	//	math::Float3(0.8f, 0.8f, 0.0f),
	//	math::Float3(0.5f, -0.5f, 2.0f),

	//	math::Float2(0.0f, 0.0f),
	//	math::Float2(0.0f, 1.0f),
	//	math::Float2(1.0f, 1.0f),

	//	math::Float3(1.0f, 1.0f, 0.0f),
	//	math::Float3(0.0f, 1.0f, 1.0f),
	//	math::Float3(1.0f, 0.0f, 1.0f),

	//	Color32(0xAFFFFFFF)
	//));
}
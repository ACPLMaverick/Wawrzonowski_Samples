#pragma once

#include "stdafx.h"
#include "SystemSettings.h"
#include "Float2.h"

namespace light
{
	class LightAmbient;
	class LightDirectional;
	class LightSpot;
}
class Triangle;
class IRenderer;
class Mesh;

class SpecificObjectFactory
{
protected:

#pragma region Protected

#pragma endregion

#pragma region Functions Protected

	SpecificObjectFactory();

#pragma endregion

public:

#pragma region Functions Public

	virtual ~SpecificObjectFactory();

#pragma endregion

#pragma region Functions Public Static

	static IRenderer* GetRenderer(SystemSettings* ss);
	static Triangle* GetTriangle(math::Float3& x, math::Float3& y, math::Float3& z,
		math::Float2& ux, math::Float2& uy, math::Float2& uz,
		math::Float3& cx = math::Float3(1.0f, 1.0f, 1.0f), math::Float3& cy = math::Float3(1.0f, 1.0f, 1.0f), math::Float3& cz = math::Float3(1.0f, 1.0f, 1.0f),
		Color32& col = Color32());
	static Mesh* GetMesh(
		const math::Float3* pos,
		const math::Float3* rot,
		const math::Float3* scl,
		const std::string* fPath);
	static light::LightAmbient* GetLightAmbient(
		const Color32* color
	);
	static light::LightDirectional* GetLightDirectional(
		const Color32* col, const math::Float3* direction
	);
	static light::LightSpot* GetLightSpot(
		const Color32* col,
		const math::Float3* dir,
		const math::Float3* pos,
		float attC,
		float attL,
		float attQ,
		float umbra,
		float penumbra,
		float falloff
	);

#pragma endregion
};

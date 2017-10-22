#include "SpecificObjectFactory.h"

#include "System.h"

#include "rendererFGK/RendererFGK.h"
#include "rendererFGK/TriangleFGK.h"
#include "rendererFGK/MeshFGK.h"

#include "rendererMav/RendererMav.h"
#include "rendererMav/GraphicsDevice.h"
#include "rendererMav/TriangleMav.h"
#include "rendererMav/MeshMav.h"

#include "light/LightAmbient.h"
#include "light/LightDirectional.h"
#include "light/LightSpot.h"

SpecificObjectFactory::SpecificObjectFactory()
{
}


SpecificObjectFactory::~SpecificObjectFactory()
{
}

IRenderer * SpecificObjectFactory::GetRenderer(SystemSettings* ss)
{
#ifdef RENDERER_FGK

	return new rendererFGK::RendererFGK(ss);

#endif // RENDERER_FGK

#ifdef RENDERER_MAV

	return new rendererMav::RendererMav(ss);

#endif // RENDERER_MAV

#ifdef RENDERER_MAJSTER

#endif // RENDERER_MAJSTER
}

Triangle * SpecificObjectFactory::GetTriangle(math::Float3& x, math::Float3& y, math::Float3& z,
	math::Float2& ux, math::Float2& uy, math::Float2& uz,
	math::Float3& cx, math::Float3& cy, math::Float3& cz,
	Color32& col)
{
#ifdef RENDERER_FGK

	return new rendererFGK::TriangleFGK(x, y, z, ux, uy, uz, cx, cy, cz, col);

#endif // RENDERER_FGK

#ifdef RENDERER_MAV

	return new rendererMav::TriangleMav(x, y, z, ux, uy, uz, cx, cy, cz, col);

#endif // RENDERER_MAV

#ifdef RENDERER_MAJSTER

#endif // RENDERER_MAJSTER
}

Mesh * SpecificObjectFactory::GetMesh(const math::Float3 * pos, const math::Float3 * rot, const math::Float3 * scl, const std::string * fPath)
{
#ifdef RENDERER_FGK

	return new rendererFGK::MeshFGK(pos, rot, scl, fPath);

#endif // RENDERER_FGK

#ifdef RENDERER_MAV

	return new rendererMav::MeshMav(pos, rot, scl, fPath);

#endif // RENDERER_MAV

#ifdef RENDERER_MAJSTER

#endif // RENDERER_MAJSTER
}

light::LightAmbient * SpecificObjectFactory::GetLightAmbient(const Color32 * color)
{
#ifdef RENDERER_FGK

	return new light::LightAmbient(color);

#endif // RENDERER_FGK

#ifdef RENDERER_MAV

	rendererMav::GraphicsDevice* gd = ((rendererMav::RendererMav*)System::GetInstance()->GetRenderer())->GetGraphicsDevice();
	uint8_t id = gd->EnableLightAmbient(color);
	if (id != (uint8_t)-1)
	{
		return gd->GetLightAmbient(id);
	}
	else
	{
		return nullptr;
	}

#endif

#ifdef RENDERER_MAJSTER

#endif // RENDERER_MAJSTER
}

light::LightDirectional * SpecificObjectFactory::GetLightDirectional(const Color32 * col, const math::Float3 * direction)
{
#ifdef RENDERER_FGK

	return new light::LightDirectional(col, direction);

#endif // RENDERER_FGK

#ifdef RENDERER_MAV

	rendererMav::GraphicsDevice* gd = ((rendererMav::RendererMav*)System::GetInstance()->GetRenderer())->GetGraphicsDevice();
	uint8_t id = gd->EnableDirectionalLight(col, direction);
	if (id != (uint8_t)-1)
	{
		return gd->GetLightDirectional(id);
	}
	else
	{
		return nullptr;
	}

#endif

#ifdef RENDERER_MAJSTER

#endif // RENDERER_MAJSTER
}

light::LightSpot * SpecificObjectFactory::GetLightSpot(const Color32 * col, const math::Float3 * dir, const math::Float3* pos, float attC, float attL, float attQ, float umbra, float penumbra, float falloff)
{
#ifdef RENDERER_FGK

	return new light::LightSpot(col, dir, pos, attC, attL, attQ, umbra, penumbra, falloff);

#endif // RENDERER_FGK

#ifdef RENDERER_MAV

	rendererMav::GraphicsDevice* gd = ((rendererMav::RendererMav*)System::GetInstance()->GetRenderer())->GetGraphicsDevice();
	uint8_t id = gd->EnableSpotLight(col, dir, pos, attC, attL, attQ, umbra, penumbra, falloff);
	if (id != (uint8_t)-1)
	{
		return gd->GetLightSpot(id);
	}
	else
	{
		return nullptr;
	}

#endif

#ifdef RENDERER_MAJSTER

#endif // RENDERER_MAJSTER
}

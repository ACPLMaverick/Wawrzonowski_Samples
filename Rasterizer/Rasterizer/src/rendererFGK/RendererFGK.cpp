#include "RendererFGK.h"
#include "../System.h"
#include "../Scene.h"
#include "../Camera.h"
#include "../Primitive.h"

#include "../light/LightAmbient.h"
#include "../light/LightDirectional.h"
#include "../light/LightSpot.h"
#include "../light/LightPoint.h"

#include <stack>

namespace rendererFGK
{
	RendererFGK::RendererFGK(SystemSettings* settings) :
		IRenderer(settings),
		_aaMode(AntialiasingMode::NONE),
		_aaColorDistance(0.2f),
		_clearColor(0xFFAAAAAA),
		_aaDepth(4)
	{
		uint16_t w = _bufferColor.GetWidth();
		uint16_t h = _bufferColor.GetHeight();
		_halfPxSize = math::Float2(0.5f / (float)w, 0.5f / (float)h);

//#ifdef RENDERER_FGK_MULTITHREAD
//
//		InitThreads();
//
//#endif // RENDERER_FGK_MULTITHREAD
	}

	RendererFGK::~RendererFGK()
	{
#ifdef RENDERER_FGK_MULTITHREAD

		DestroyThreads();

#endif // RENDERER_FGK_MULTITHREAD
	}

	void RendererFGK::Draw(Scene * scene)
	{
		_bufferColor.Fill(0x00AAAAAA);
		_bufferDepth.Fill(FLT_MAX);

		// for each pixel, cast ray into scene
		Camera* cam = scene->GetCurrentCamera();

		uint16_t w = _bufferColor.GetWidth();
		uint16_t h = _bufferColor.GetHeight();

		float tanFovByTwo = tan(cam->GetFOVYRads() * 0.5f);
		float aspect = cam->GetAspectRatio();
		float nearPlane = cam->GetNearPlane();

		math::Float3 camOrigin = *cam->GetPosition();
		math::Float3 camDirection = *cam->GetDirection();

#ifdef RENDERER_FGK_MULTITHREAD

		// wait for all threads to set in begin position
		EnterSynchronizationBarrier(&_barrier, SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY | SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE);

		// wait for all threads to complete
		EnterSynchronizationBarrier(&_barrier, SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY | SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE);

#else

		for (uint16_t i = 0; i < h; ++i)
		{
			for (uint16_t j = 0; j < w; ++j)
			{
				ComputePixel(math::Int2(j, i), scene, cam, tanFovByTwo);
			}
		}

#endif // RENDERER_FGK_MULTITHREAD
	}

	void RendererFGK::InitThreads()
	{
		InitializeSynchronizationBarrier(&_barrier, 9, 0);

		for (size_t i = 0; i < NUM_THREADS; ++i)
		{
			_threadHandles[i] = CreateThread
			(
				NULL,
				0,
				ThreadFunc, // tu bêdzie wskaŸnik
				(LPVOID)i,
				NULL,
				NULL
			);
		}
	}

	inline void RendererFGK::DestroyThreads()
	{
		for (size_t i = 0; i < NUM_THREADS; ++i)
		{
			TerminateThread(_threadHandles[i], 0);
		}
	}

	DWORD RendererFGK::ThreadFunc(LPVOID lpParameter)
	{
		// compute pixel range
		RendererFGK* rendererObj = (RendererFGK*)System::GetInstance()->GetRenderer();
		Buffer<Color32>* prBuffer = System::GetInstance()->GetRenderer()->GetColorBuffer();
		Scene* scene = System::GetInstance()->GetCurrentScene();
		Camera* cam = scene->GetCurrentCamera();

		size_t prCount = (prBuffer->GetWidth() * prBuffer->GetHeight());
		size_t prPart = prCount / RendererFGK::NUM_THREADS;
		size_t pxBegin = prPart * (size_t)lpParameter;
		size_t pxEndExclusive = pxBegin + prPart;

		float tanFovByTwo = tan(cam->GetFOVYRads() * 0.5f);

		if ((size_t)lpParameter == RendererFGK::NUM_THREADS - 1)
		{
			// last thread get the division rest
			pxEndExclusive = prCount;
		}
		// enter loop
		while (true)
		{
			// wait for start signal
			EnterSynchronizationBarrier(&rendererObj->_barrier, SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY | SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE);

			// compute colors
			for (size_t i = pxBegin; i < pxEndExclusive; ++i)
			{
				int32_t y = i / prBuffer->GetWidth();
				int32_t x = i % prBuffer->GetWidth();
				rendererObj->ComputePixel(math::Int2(x, y), scene, cam, tanFovByTwo);
			}

			// signal end and wait for other threads to complete
			EnterSynchronizationBarrier(&rendererObj->_barrier, SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY | SYNCHRONIZATION_BARRIER_FLAGS_NO_DELETE);
		}

		return 0;
	}

	void RendererFGK::ComputePixel(math::Int2 pos, Scene* scene, Camera* cam, float tanFovByTwo)
	{
		math::Float3 camOrigin = *cam->GetPosition();
		math::Float3 camDirection = *cam->GetDirection();
		float aspect = cam->GetAspectRatio();
		math::Float3 ssPixel(GetViewSpacePosition(pos));
		Ray ray = CalculateRay(ssPixel, tanFovByTwo, aspect, cam->GetViewInvMatrix(), &camOrigin);
		//Ray ray = CalculateRayOrtho(ssPixel, aspect, cam->GetViewInvMatrix(), &camOrigin, &camDirection);

		if (_aaMode == AntialiasingMode::ADAPTIVE)
		{
			AdaptiveRays aCoords
			(
				ray,
				CalculateRay(math::Float3(ssPixel.x - _halfPxSize.x, ssPixel.y + _halfPxSize.y, 0.0f), tanFovByTwo, aspect, cam->GetViewInvMatrix(), &camOrigin),
				CalculateRay(math::Float3(ssPixel.x + _halfPxSize.x, ssPixel.y + _halfPxSize.y, 0.0f), tanFovByTwo, aspect, cam->GetViewInvMatrix(), &camOrigin),
				CalculateRay(math::Float3(ssPixel.x + _halfPxSize.x, ssPixel.y - _halfPxSize.y, 0.0f), tanFovByTwo, aspect, cam->GetViewInvMatrix(), &camOrigin),
				CalculateRay(math::Float3(ssPixel.x - _halfPxSize.x, ssPixel.y - _halfPxSize.y, 0.0f), tanFovByTwo, aspect, cam->GetViewInvMatrix(), &camOrigin)
			);

			_bufferColor.SetPixel(pos.x, pos.y, RaySampleAdaptive(aCoords, math::Float2(ssPixel.x, ssPixel.y), _halfPxSize, scene, cam->GetViewInvMatrix(), cam, pos, tanFovByTwo, aspect, 0));
		}
		else
		{
			_bufferColor.SetPixel(pos.x, pos.y, RaySample(ray, scene, cam, pos, 0));
		}
	}

	math::Float2 RendererFGK::GetViewSpacePosition(const math::Int2 & pos)
	{
		return math::Float2(
			(float)(pos.x) / _bufferColor.GetWidth() * 2.0f - 1.0f + _halfPxSize.x,
			(float)(pos.y) / _bufferColor.GetHeight() * 2.0f - 1.0f + _halfPxSize.y
			);
	}

	math::Int2 RendererFGK::GetScreenSpacePosition(const math::Float3 & pos)
	{
		return math::Int2
			(
				(int32_t)(pos.x * (float)_bufferColor.GetWidth() * 0.5f + ((float)_bufferColor.GetWidth() * 0.5f)),
				(int32_t)(pos.y * (float)_bufferColor.GetHeight() * 0.5f + ((float)_bufferColor.GetHeight() * 0.5f))
			);
	}

	Ray RendererFGK::CalculateRay(const math::Float3& px, float tanFovByTwo, float aspect, const math::Matrix4x4* vmInv, const math::Float3* camOrigin)
	{
		math::Float3 point = px * tanFovByTwo;
		point.x *= aspect;
		point.z = 1.0f;
		point = *vmInv * math::Float4(point);
		point = point - *camOrigin;
		math::Float3::Normalize(point);
		return Ray(*camOrigin, point);
	}

	Ray RendererFGK::CalculateRayOrtho(const math::Float3& px, float aspect, const math::Matrix4x4* vmInv, const math::Float3* camOrigin, const math::Float3* camDirection)
	{
		math::Float3 point = px * 5.0f;
		point.x *= aspect;
		point.z = 0.0f;
		point = *vmInv * math::Float4(point);
		return Ray(point, *camDirection);
	}

	inline Color32 RendererFGK::RaySample(Ray & ray, Scene * scene, const Camera* cam, const math::Int2 ndcPos, int recCtr)
	{
		Color32 ret = _clearColor;
		std::vector<Primitive*>* prims = scene->GetPrimitives();
		float closestDist = FLT_MAX;
		Primitive* prim = nullptr;
		bool error = false;
		RayHit hit;
		for (std::vector<Primitive*>::iterator it = prims->begin(); it != prims->end(); ++it)
		{
			RayHit cHit = (*it)->CalcIntersect(ray);
			if (cHit.hit)
			{
				float distanceToOrigin = math::Float3::LengthSquared(cHit.point - ray.GetOrigin());
				if (distanceToOrigin <= closestDist)	// depth test
				{
					closestDist = distanceToOrigin;
					prim = (*it);
					hit = cHit;
				}
			}
			if (hit.debugFlag != 0)
			{
				error = true;
			}
		}

		if (error)
		{
			ret.color = 0xFFFF0000;
		}
		else if (prim != nullptr)
		{
			Material* mat = prim->GetMaterialPtr();
			const math::Matrix4x4* worldMatrix = prim->GetTransform()->GetWorldMatrix();
			if (mat != nullptr)
			{
				// phong
				ret = Color32(0xFF000000);
				math::Float3 eyeDir = *cam->GetDirection();

				for (std::vector<light::LightDirectional*>::iterator it = scene->GetLightsDirectional()->begin();
					it != scene->GetLightsDirectional()->end(); ++it)
				{
					if (CheckPathToLight(hit.point, -*(*it)->GetDirection(), 0.0f, scene))
					{
						math::Float3 dir = *(*it)->GetDirection();
						Color32 diffuse = *(*it)->GetColor();
						Phong(*worldMatrix, hit.point, hit.normal, hit.uv, dir, ray.GetDirection(), diffuse, mat, ret);
					}
				}

				for (std::vector<light::LightSpot*>::iterator it = scene->GetLightsSpot()->begin();
					it != scene->GetLightsSpot()->end(); ++it)
				{
					math::Float3 pos = *(*it)->GetPostition();
					math::Float3 dir = pos - hit.point;
					float dirLength = math::Float3::Length(dir);
					dir = dir / dirLength;

					if (CheckPathToLight(hit.point, dir, dirLength, scene))
					{
						float spotEffect = math::Float3::Dot(*(*it)->GetDirection(), -dir);

						if (spotEffect > (*it)->GetUmbraAngleRad())
						{
							spotEffect = pow(spotEffect, (*it)->GetFalloffFactor());
							float distance = math::Float3::LengthSquared(hit.point - *(*it)->GetPostition());
							spotEffect = spotEffect / ((*it)->GetAttenuationConstant() +
								(*it)->GetAttenuationLinear() * distance * 0.25f +
								(*it)->GetAttenuationQuadratic() * distance);

							Color32 col = *(*it)->GetColor() * spotEffect;
							Phong(*worldMatrix, hit.point, hit.normal, hit.uv, -dir, ray.GetDirection(), col, mat, ret);
						}
					}
				}

				for (std::vector<light::LightPoint*>::iterator it = scene->GetLightsPoint()->begin();
					it != scene->GetLightsPoint()->end(); ++it)
				{
					math::Float3 dir = hit.point - *(*it)->GetPosition();
					float dirLength = math::Float3::Length(dir);
					dir = dir / dirLength;

					if (CheckPathToLight(hit.point, -dir, dirLength, scene))
					{
						float distance = math::Float3::LengthSquared(hit.point - *(*it)->GetPosition());
						float pointEffect = 1.0f / ((*it)->GetAttenuationConstant() +
							(*it)->GetAttenuationLinear() * distance * 0.25f +
							(*it)->GetAttenuationQuadratic() * distance);

						Color32 col = *(*it)->GetColor() * pointEffect;
						Phong(*worldMatrix, hit.point, hit.normal, hit.uv, dir, ray.GetDirection(), col, mat, ret);
					}
				}

				ret += *scene->GetLightAmbient()->GetColor();

				// reflection and refraction
				
				if (recCtr <= RAY_SAMPLE_MAX_RECURSION)
				{
					// reflection
					if (mat->GetReflectivity() > 0.0f)
					{
						Color32 reflectionColor = RaySample(Ray(hit.point + hit.normal * -0.00001f, math::Float3::Reflect(ray.GetDirection(), hit.normal)), scene, cam, ndcPos, recCtr + 1);
						ret = Color32::LerpNoAlpha(ret.color, reflectionColor, mat->GetReflectivity());
					}

					// refraction
					if (mat->GetRefractivity() > 0.0f)
					{
						Color32 refractionColor = RaySample(Ray(hit.point + hit.normal * -0.00001f, 
							math::Float3::Refract(ray.GetDirection(), hit.normal, mat->GetRefractionCoefficent())), 
							scene, cam, ndcPos, recCtr + 1);
						ret = Color32::LerpNoAlpha(ret.color, refractionColor, mat->GetRefractivity());
					}
				}
			}
			else
			{
				// no material - draw magenta
				ret.color = 0xFFFF00FF;
			}
		}
		return ret;
	}

	Color32 RendererFGK::RaySampleAdaptive(AdaptiveRays& rays, math::Float2 ssPixel, math::Float2 halfPxSize, Scene* scene,
		const math::Matrix4x4* vmInv, const Camera* cam, const math::Int2 ndcPos, float tanFovByTwo, float aspect, int ctr)
	{
		// sample four corner rays
		Color32 cols[4];
		for (size_t i = 1; i < 5; ++i)
		{
			cols[i - 1] = RaySample(rays.tab[i], scene, cam, ndcPos, 0);
		}

		// check recursion warunek
		if (ctr < _aaDepth)
		{
			// check corner ray colour
			for (size_t k = 0; k < 4; ++k)
			{
				float dist = 0.0f;
				for (size_t m = k + 1; m < k + 3; ++m)
				{
					size_t mCap = m % 4;
					dist += Color32::Distance(cols[k], cols[mCap]);
				}
				if (dist > _aaColorDistance)
				{
					// distance is bigger, so sample further this pixel
					//halfPxSize = halfPxSize * 0.5f;
					if (k == 0)
					{
						// tl
						cols[k] = RaySampleAdaptive
						(
							AdaptiveRays
							(
								CalculateRay(math::Float3(ssPixel.x - halfPxSize.x * 0.5f, ssPixel.y + halfPxSize.y * 0.5f, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition()),
								rays.tl,
								CalculateRay(math::Float3(ssPixel.x, ssPixel.y + halfPxSize.y, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition()),
								rays.center,
								CalculateRay(math::Float3(ssPixel.x - halfPxSize.x, ssPixel.y, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition())
							),
							math::Float2(ssPixel.x - halfPxSize.x * 0.5f, ssPixel.y + halfPxSize.y * 0.5f),
							halfPxSize * 0.5f,
							scene,
							vmInv,
							cam,
							ndcPos,
							tanFovByTwo,
							aspect,
							ctr + 1
						);
					}
					else if (k == 1)
					{
						// tr
						cols[k] = RaySampleAdaptive
						(
							AdaptiveRays
							(
								CalculateRay(math::Float3(ssPixel.x + halfPxSize.x * 0.5f, ssPixel.y + halfPxSize.y * 0.5f, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition()),
								CalculateRay(math::Float3(ssPixel.x, ssPixel.y + halfPxSize.y, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition()),
								rays.tr,
								CalculateRay(math::Float3(ssPixel.x + halfPxSize.x, ssPixel.y, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition()),
								rays.center
							),
							math::Float2(ssPixel.x + halfPxSize.x * 0.5f, ssPixel.y + halfPxSize.y * 0.5f),
							halfPxSize * 0.5f,
							scene,
							vmInv,
							cam,
							ndcPos,
							tanFovByTwo,
							aspect,
							ctr + 1
						);
					}
					else if (k == 2)
					{
						// br
						cols[k] = RaySampleAdaptive
						(
							AdaptiveRays
							(
								CalculateRay(math::Float3(ssPixel.x + halfPxSize.x * 0.5f, ssPixel.y - halfPxSize.y * 0.5f, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition()),
								rays.center,
								CalculateRay(math::Float3(ssPixel.x + halfPxSize.x, ssPixel.y, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition()),
								rays.br,
								CalculateRay(math::Float3(ssPixel.x, ssPixel.y - halfPxSize.y, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition())
							),
							math::Float2(ssPixel.x + halfPxSize.x * 0.5f, ssPixel.y - halfPxSize.y * 0.5f),
							halfPxSize * 0.5f,
							scene,
							vmInv,
							cam,
							ndcPos,
							tanFovByTwo,
							aspect,
							ctr + 1
						);
					}
					else if (k == 3)
					{
						// bl
						cols[k] = RaySampleAdaptive
						(
							AdaptiveRays
							(
								CalculateRay(math::Float3(ssPixel.x - halfPxSize.x * 0.5f, ssPixel.y - halfPxSize.y * 0.5f, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition()),
								CalculateRay(math::Float3(ssPixel.x - halfPxSize.x, ssPixel.y, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition()),
								rays.center,
								CalculateRay(math::Float3(ssPixel.x, ssPixel.y - halfPxSize.y, 0.0f), tanFovByTwo, aspect, vmInv, cam->GetPosition()),
								rays.bl
							),
							math::Float2(ssPixel.x - halfPxSize.x * 0.5f, ssPixel.y - halfPxSize.y * 0.5f),
							halfPxSize * 0.5f,
							scene,
							vmInv,
							cam,
							ndcPos,
							tanFovByTwo,
							aspect,
							ctr + 1
						);
					}
				}
			}
 
			return Color32::AverageFour(cols);
			//return RaySample(rays.center, scene, *camOrigin, ndcPos);
		}
		else
		{
			//return Color32::AverageFour(cols);
			return RaySample(rays.center, scene, cam, ndcPos, 0);
		}
	}

	inline bool RendererFGK::CheckPathToLight(const math::Float3& start, const math::Float3& dir, float maxDist, Scene * scene)
	{
		for (std::vector<Primitive*>::iterator it = scene->GetPrimitives()->begin(); it != scene->GetPrimitives()->end(); ++it)
		{
			RayHit cHit = (*it)->CalcIntersect(Ray(start + dir * 0.001f, dir, maxDist));
			if (cHit.hit)
			{
				return false;
			}
		}

		return true;
	}

	void RendererFGK::Phong(const math::Matrix4x4& transformMatrix, const math::Float3 & worldPosition, const math::Float3 & normal,
		const math::Float2 & uv, const math::Float3 & lightDir, const math::Float3 & eyeDir,
		const Color32 & lightColor, const  Material * mat, Color32 & actualColor)
	{
		float dot = max(math::Float3::Dot(normal, -lightDir), 0.0f);
		math::Float3 reflected = math::Float3::Reflect(-lightDir, normal);
		float spec = 10.0f * pow(max(math::Float3::Dot(eyeDir, reflected), 0.0f), mat->GetGlossiness());

		math::Matrix4x4 invTransform;
		math::Matrix4x4::Inverse(&transformMatrix, &invTransform);
		math::Float3 modelPosition(invTransform * math::Float4(worldPosition, 1.0f));
		Color32 diffuseMap = mat->GetMapDiffuse()->GetColor(uv, modelPosition);

		actualColor += (lightColor * (*mat->GetColorDiffuse() * dot + 
			*mat->GetColorSpecular() * spec * diffuseMap.GetFltA())) * diffuseMap;
	}
}
#pragma once

#include "../SpecificObjectFactory.h"
#include "../IRenderer.h"
#include "../Float2.h"
#include "../Int2.h"
#include "../Matrix4x4.h"
#include "../Ray.h"

class Camera;
class Material;

namespace rendererFGK
{
	class RendererFGK :
		public IRenderer
	{
		friend class SpecificObjectFactory;
	public:

#pragma region Enum Public

		enum AntialiasingMode
		{
			NONE,
			ADAPTIVE
		};

#pragma endregion
	protected:

#pragma region Struct Protected

		struct AdaptiveRays
		{
			union
			{
				struct
				{
					Ray center;
					Ray tl;
					Ray tr;
					Ray br;
					Ray bl;
				};
				Ray tab[5];
			};

			AdaptiveRays()
			{
				ZeroMemory(tab, 5 * sizeof(Ray));
			}

			AdaptiveRays(const AdaptiveRays& c)
			{
				this->center = c.center;
				this->tl = c.tl;
				this->tr = c.tr;
				this->br = c.br;
				this->bl = c.bl;
			}

			AdaptiveRays(const Ray& center, const Ray& tl, const Ray& tr, const Ray& br, const Ray& bl)
			{
				this->center = center;
				this->tl = tl;
				this->tr = tr;
				this->br = br;
				this->bl = bl;
			}

			~AdaptiveRays()
			{

			}
		};

#pragma endregion

#pragma region Const

		static const int32_t NUM_THREADS = 8;
		static const int32_t RAY_SAMPLE_MAX_RECURSION = 2;

#pragma endregion

#pragma region Protected

		AntialiasingMode _aaMode;
		math::Float2 _halfPxSize;
		float _aaColorDistance;
		Color32 _clearColor;
		uint8_t _aaDepth;

		HANDLE _threadHandles[NUM_THREADS];
		SYNCHRONIZATION_BARRIER _barrier;

#pragma endregion

#pragma region Functions Protected

		RendererFGK(SystemSettings* settings);

		void DestroyThreads();
		static DWORD WINAPI ThreadFunc(_In_ LPVOID lpParameter);

		inline void ComputePixel(math::Int2 pos, Scene* scene, Camera* cam, float tanFovByTwo);
		inline math::Float2 GetViewSpacePosition(const math::Int2& pos);
		inline math::Int2 GetScreenSpacePosition(const math::Float3& pos);
		inline Ray CalculateRay(const math::Float3& px, float tanFovByTwo, float aspect, const math::Matrix4x4* vmInv, const math::Float3* camOrigin);
		inline Ray RendererFGK::CalculateRayOrtho(const math::Float3& px, float aspect, const math::Matrix4x4* vmInv, const math::Float3* camOrigin, const math::Float3* camDirection);
		inline Color32 RaySample(Ray& ray, Scene* scene, const Camera* cam, const math::Int2 ndcPos, int recCtr);
		inline Color32 RaySampleAdaptive(AdaptiveRays& rays, math::Float2 ssPixel, math::Float2 halfPxSize, Scene* scene, 
			const math::Matrix4x4* vmInv, const Camera* cam, const math::Int2 ndcPos, float tanFovByTwo, float aspect, int ctr);

		inline bool CheckPathToLight(const math::Float3& start, const math::Float3& dir, float maxDist, Scene* scene);
		inline void Phong(const math::Matrix4x4& transformMatrix, const math::Float3 & worldPosition, const math::Float3& normal,
			const math::Float2& uv, const math::Float3& lightDir, const math::Float3& eyeDir, const Color32& lightColor, const Material* mat, Color32& actualColor);

#pragma endregion

	public:

#pragma region Functions Public

		~RendererFGK();

		void InitThreads();
		virtual void Draw(Scene* scene) override;

#pragma endregion

	};

}
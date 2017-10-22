#pragma once
#include "Postprocess.h"

class Texture;

namespace Postprocesses
{
	class RandomVectorsGenerator
	{
	public:
		static void Generate(Texture** tex, uint32_t width);
	};

	class SimpleSSAO :
		public Postprocess
	{
	protected:

		static const int32_t SAMPLE_COUNT = 14;

		XMFLOAT4A _offsets[SAMPLE_COUNT];
		float _maxDistance;
		float _fadeStart;
		float _epsilon;

		ID3D11Buffer* _ssaoBuffer;
		Texture* _randomVectorTexture;

		inline void FillParams(XMFLOAT4A* paramBuffer) const;

	public:
		SimpleSSAO();
		~SimpleSSAO();

		// Inherited via Postprocess
		virtual void Update() override;
		virtual void SetPass(GBuffer& gBuffer, const Camera& camera, int32_t passIndex = 0) const;
		virtual inline int GetPassCount() const { return 2; }
	};
}
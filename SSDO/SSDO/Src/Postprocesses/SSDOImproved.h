#pragma once
#include "Postprocess.h"
#include "Utility/AdaptiveLayerGenerator.h"

class Texture;
class RWTexture;

namespace Postprocesses
{
	class SSDOImproved :
		public Postprocess
	{

	protected:

#pragma region Static Const Protected

		static const uint32_t SAT_SIZE_DIVISOR = 4;

#pragma endregion

#pragma region Protected

		float _sampleBoxHalfSize;
		float _occlusionPower;
		float _occlusionFaloff;
		float _powFactor;

		ID3D11Buffer* _dataBuffer;
		RWTexture* _satColor;
		RWTexture* _satNormalDepth;

		RWTexture* _satBufferA;
		RWTexture* _satBufferB;

		// Adaptive layering data

		RWTexture* _layerIndices;
		RWTexture* _satLayerIndices;
		Utility::AdaptiveLayerGenerator::AdaptiveLayerData _adaptiveDataNormalDepth;
		Utility::AdaptiveLayerGenerator::AdaptiveLayerData _adaptiveDataColor;

		// End Adaptive layering data

		Texture* _testInput;
		RWTexture* _testBuf;
		RWTexture* _testOutput;

#pragma endregion

#pragma region Functions Protected

		static void AssignTextureParams(Texture* tex);

#pragma endregion
		
	public:

#pragma region Functions Public

		SSDOImproved();
		~SSDOImproved();

		// Inherited via Postprocess
		virtual void Update() override;
		virtual void SetPass(GBuffer& gBuffer, const Camera& camera, int32_t passIndex = 0) const override;
		virtual void AfterPass(GBuffer& gBuffer, const Camera& camera, int32_t passIndex = 0) const override;
		virtual inline int GetPassCount() const { return 1; }

#pragma endregion

	};

}


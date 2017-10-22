#pragma once
#include "Postprocess.h"
#include "Utility/DepthAwareBlur.h"

class Texture;
class RWTexture;

namespace Postprocesses
{
	class SSDOImprovedB :
		public Postprocess
	{

	protected:

#pragma region Static Const Protected

		static const uint32_t BUFFER_SIZE_DIVISOR = 4;

#pragma endregion

#pragma region Protected

		Utility::DepthAwareBlur _blurGen;

		float _sampleBoxHalfSize;
		float _occlusionPower;
		float _occlusionFaloff;
		float _powFactor;

		ID3D11Buffer* _dataBuffer;

		RWTexture* _bufColor;
		RWTexture* _bufNormalDepth;

		RWTexture* _tempA;
		RWTexture* _tempB;

#pragma endregion

#pragma region Functions Protected

		static void AssignTextureParams(Texture* tex, bool mipmapped = false);

#pragma endregion
		
	public:

#pragma region Functions Public

		SSDOImprovedB();
		~SSDOImprovedB();

		// Inherited via Postprocess
		virtual void Update() override;
		virtual void SetPass(GBuffer& gBuffer, const Camera& camera, int32_t passIndex = 0) const override;
		virtual void AfterPass(GBuffer& gBuffer, const Camera& camera, int32_t passIndex = 0) const override;
		virtual inline int GetPassCount() const { return 1; }

#pragma endregion

	};

}


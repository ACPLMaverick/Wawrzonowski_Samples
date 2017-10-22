#pragma once

#include "GlobalDefines.h"
#include "GBuffer.h"

class Texture;
class RWTexture;
class ComputeShader;

namespace Postprocesses
{
	namespace Utility
	{
		class DepthAwareBlur
		{
		protected:

#pragma region Protected

			ComputeShader* _shader;

			float _depthFilterHalfWidth;
			float _filterSampleSpacing;
			uint32_t _blurCount;

#pragma endregion

#pragma region Functions Protected

			void GenerateInternal(
				ID3D11ShaderResourceView* inputA, ID3D11ShaderResourceView* inputB,
				RWTexture* bufferA, RWTexture* bufferB,
				RWTexture* outputA, RWTexture* outputB,
				uint32_t width, uint32_t height, uint32_t level
			) const;

#pragma endregion

		public:

#pragma region Functions Public

			DepthAwareBlur(float depthFilterHalfWidth, float filterSampleSpacing, uint32_t blurCount);
			~DepthAwareBlur();

			void Generate(
				const GBuffer::RenderTarget* inputA, const GBuffer::RenderTarget* inputB,
				RWTexture* bufferA, RWTexture* bufferB,
				RWTexture* outputA, RWTexture* outputB
			) const;

			void Generate(
				const Texture* inputA, const Texture* inputB,
				RWTexture* bufferA, RWTexture* bufferB,
				RWTexture* outputA, RWTexture* outputB
			) const;

			float GetDepthFilterHalfWidth() const { return _depthFilterHalfWidth; }
			float GetFilterSampleSpacing() const { return _filterSampleSpacing; }
			uint32_t GetBlurCount() const { return _blurCount; }

#pragma endregion

		};

	}
}
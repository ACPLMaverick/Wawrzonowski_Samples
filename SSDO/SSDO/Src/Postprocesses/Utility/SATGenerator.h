#pragma once

#include "GlobalDefines.h"
#include "RWStructuredBuffer.h"
#include "GBuffer.h"
#include "Singleton.h"

class Texture;
class RWTexture;
class ComputeShader;

namespace Postprocesses
{
	namespace Utility
	{

		class SATGenerator : public Singleton<SATGenerator>
		{
		protected:

		#pragma region Structs Protected

			struct TempBufferData
			{
				XMFLOAT4 NormalDepth;
				XMFLOAT4 Color;
			};

		#pragma endregion

		#pragma region Protected

			ComputeShader* _shader = nullptr;

			inline void GenerateInternal(
				ID3D11ShaderResourceView* const inputASRV,
				ID3D11ShaderResourceView* const inputBSRV,
				ID3D11ShaderResourceView* const bufferASRV,
				ID3D11ShaderResourceView* const bufferBSRV,
				ID3D11UnorderedAccessView* bufferAUAV,
				ID3D11UnorderedAccessView* bufferBUAV,
				ID3D11UnorderedAccessView* outputAUAV,
				ID3D11UnorderedAccessView* outputBUAV,
				uint32_t inputWidth, uint32_t inputHeight, uint32_t inputLevel) const;

			inline void PrintRawData(const Texture* tex) const;

		#pragma endregion

		public:

		#pragma region Public

			SATGenerator();
			~SATGenerator();

			void Generate(
				const Texture* inputA,
				const Texture* inputB,
				RWTexture* bufferA,
				RWTexture* bufferB,
				RWTexture* outputA,
				RWTexture* outputB) const;
			void Generate(
				const GBuffer::RenderTarget* inputA,
				const GBuffer::RenderTarget* inputB,
				RWTexture* bufferA,
				RWTexture* bufferB,
				RWTexture* outputA,
				RWTexture* outputB) const;

		#pragma endregion

		};
	}
}
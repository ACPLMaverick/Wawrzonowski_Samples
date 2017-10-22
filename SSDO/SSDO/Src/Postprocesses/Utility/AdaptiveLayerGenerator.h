#pragma once

#include "GlobalDefines.h"
#include "GBuffer.h"
#include "Singleton.h"

class Texture;
class RWTexture;
class ComputeShader;

typedef void GenerateTexParamsFunc(Texture*);

namespace Postprocesses
{
	namespace Utility
	{

		class AdaptiveLayerGenerator : public Singleton<AdaptiveLayerGenerator>
		{
		public:

#pragma region Struct Public

			struct AdaptiveLayerData
			{
				RWTexture* _layer1 = nullptr;
				RWTexture* _layer2 = nullptr;
				RWTexture* _layer10 = nullptr;
				RWTexture* _layer11 = nullptr;
				RWTexture* _layer20 = nullptr;
				RWTexture* _layer21 = nullptr;
				RWTexture* _satLayer1 = nullptr;
				// _satLayer2;	// is computed diferentially while sampling
				RWTexture* _satLayer10 = nullptr;
				// _satLayer11 is computed diferentially while sampling
				RWTexture* _satLayer20 = nullptr;
				// _satLayer21 is computed diferentially while sampling

				AdaptiveLayerData(GenerateTexParamsFunc genFunc);
				~AdaptiveLayerData();
			};

#pragma endregion

		protected:

#pragma region Protected

			ComputeShader* _shader = nullptr;
			ID3D11Buffer* _buffer = nullptr;

#pragma endregion

#pragma region Functions Protected

			inline void GenerateInternal(ID3D11ShaderResourceView* baseInputA, ID3D11ShaderResourceView* baseInputB,
				const RWTexture* baseSATA, const RWTexture* baseSATB,
				RWTexture* bufferA, RWTexture* bufferB, RWTexture* layerIndices, RWTexture* satLayerIndices,
				const AdaptiveLayerData* dataA, const AdaptiveLayerData* dataB,
				uint32_t inputWidth, uint32_t inputHeight, float boxHalfSize) const;

#pragma endregion

		public:

#pragma region Functions Public

			AdaptiveLayerGenerator();
			~AdaptiveLayerGenerator();

			void Generate(const GBuffer::RenderTarget* baseInputA, const GBuffer::RenderTarget* baseInputB, const RWTexture* baseSATA, const RWTexture* baseSATB,
				RWTexture* bufferA, RWTexture* bufferB, RWTexture* layerIndices, RWTexture* satLayerIndices,
				const AdaptiveLayerData* dataA, const AdaptiveLayerData* dataB, float boxHalfSize) const;
			void Generate(const Texture* baseInputA, const Texture* baseInputB, const RWTexture* baseSATA, const RWTexture* baseSATB,
				RWTexture* bufferA, RWTexture* bufferB, RWTexture* layerIndices, RWTexture* satLayerIndices,
				const AdaptiveLayerData* dataA, const AdaptiveLayerData* dataB, float boxHalfSize) const;

#pragma endregion

		};

	}
}
#include "stdafx.h"
#include "AdaptiveLayerGenerator.h"
#include "Texture.h"
#include "RWTexture.h"
#include "ComputeShader.h"
#include "System.h"
#include "Scenes\Scene.h"
#include "SATGenerator.h"
#include "Helper.h"

namespace Postprocesses
{
	namespace Utility
	{

		AdaptiveLayerGenerator::AdaptiveLayerGenerator()
		{
			_shader = System::GetInstance()->GetScene()->LoadComputeShader(L"AdaptiveLayers");
		}


		AdaptiveLayerGenerator::~AdaptiveLayerGenerator()
		{
		}

		void AdaptiveLayerGenerator::Generate(const GBuffer::RenderTarget * baseInputA, const GBuffer::RenderTarget * baseInputB,
			const RWTexture * baseSATA, const RWTexture * baseSATB, RWTexture* bufferA, RWTexture* bufferB,
			RWTexture* layerIndices, RWTexture* satLayerIndices,
			const AdaptiveLayerData * dataA, const AdaptiveLayerData * dataB, float boxHalfSize) const
		{
			GenerateInternal(baseInputA->SRV, baseInputB->SRV, baseSATA, baseSATB, bufferA, bufferB, layerIndices, satLayerIndices, dataA, dataB,
				System::GetInstance()->GetOptions()._windowWidth,
				System::GetInstance()->GetOptions()._windowHeight, boxHalfSize);
		}

		void AdaptiveLayerGenerator::Generate(const Texture * baseInputA, const Texture * baseInputB,
			const RWTexture * baseSATA, const RWTexture * baseSATB, RWTexture* bufferA, RWTexture* bufferB,
			RWTexture* layerIndices, RWTexture* satLayerIndices,
			const AdaptiveLayerData * dataA, const AdaptiveLayerData * dataB, float boxHalfSize) const
		{
			GenerateInternal(baseInputA->GetSRV(), baseInputB->GetSRV(), baseSATA, baseSATB, bufferA, bufferB, layerIndices, satLayerIndices, dataA, dataB,
				baseInputA->GetWidth(), baseInputA->GetHeight(), boxHalfSize);
		}

		inline void AdaptiveLayerGenerator::GenerateInternal(ID3D11ShaderResourceView * baseInputA, ID3D11ShaderResourceView * baseInputB,
			const RWTexture * baseSATA, const RWTexture * baseSATB, RWTexture* bufferA, RWTexture* bufferB, RWTexture* layerIndices, RWTexture* satLayerIndices,
			const AdaptiveLayerData * dataA, const AdaptiveLayerData * dataB,
			uint32_t inputWidth, uint32_t inputHeight, float boxHalfSize) const
		{
			const float boxHalfSizeEnlarged(boxHalfSize * 4.0f);
			uint32_t inputLevel(Helper::GetTextureLevel(inputWidth, baseSATA->GetWidth()));
			inputWidth >>= inputLevel;
			inputHeight >>= inputLevel;
			ID3D11DeviceContext* deviceContext(Renderer::GetInstance()->GetDeviceContext());
			uint32_t threadGroupCountX, threadGroupCountY;

			threadGroupCountX = inputWidth / (_shader->GetThreadCountInGroupX() * 2) + 1;
			threadGroupCountY = inputHeight / _shader->GetThreadCountInGroupY();
			void* nullPtr[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

			// Split base buffers into two adaptive layers with index texture assignment.

			ComputeShader::AdaptiveLayerBuffer* buf(_shader->MapConstantBuffer<ComputeShader::AdaptiveLayerBuffer>());
			buf->SatDimensionsAndRecs.x = static_cast<float>(baseSATA->GetWidth());
			buf->SatDimensionsAndRecs.y = static_cast<float>(baseSATA->GetHeight());
			buf->SatDimensionsAndRecs.z = 1.0f / buf->SatDimensionsAndRecs.x;
			buf->SatDimensionsAndRecs.w = 1.0f / buf->SatDimensionsAndRecs.y;
			buf->BoxHalfSize = boxHalfSizeEnlarged;
			buf->InputLevel = inputLevel;
			buf->IndexOffset = 0;
			buf->InputSATIsDifferential = false;
			_shader->UnmapConstantBuffer();
			_shader->Set(1);

			ID3D11ShaderResourceView* srvPtr[6] = { baseInputA, baseInputB, baseSATA->GetSRV(), baseSATB->GetSRV(), nullptr, nullptr };
			deviceContext->CSSetShaderResources(4, 4, srvPtr);

			ID3D11UnorderedAccessView* uavPtr[5] = { dataA->_layer1->GetUAV(), dataB->_layer1->GetUAV(),
													dataA->_layer2->GetUAV(), dataB->_layer2->GetUAV(), layerIndices->GetUAV() };
			deviceContext->CSSetUnorderedAccessViews(0, 5, uavPtr, nullptr);

			ID3D11SamplerState* smpPtr[1] = { baseSATA->GetSampler() };
			deviceContext->CSSetSamplers(0, 1, smpPtr);

			_shader->Dispatch(threadGroupCountX, threadGroupCountY, 1);

			deviceContext->CSSetShaderResources(4, 4, reinterpret_cast<ID3D11ShaderResourceView**>(&nullPtr));
			deviceContext->CSSetUnorderedAccessViews(0, 5, reinterpret_cast<ID3D11UnorderedAccessView**>(&nullPtr), nullptr);

			// Generate SAT for first layer.
			// Compute second layer's SAT using differential method.

			SATGenerator::GetInstance()->Generate(dataA->_layer1, dataB->_layer1, bufferA, bufferB, dataA->_satLayer1, dataB->_satLayer1);

			// Cleanup after SATGenerator.
			deviceContext->CSSetShaderResources(4, 4, reinterpret_cast<ID3D11ShaderResourceView**>(&nullPtr));
			deviceContext->CSSetUnorderedAccessViews(0, 5, reinterpret_cast<ID3D11UnorderedAccessView**>(&nullPtr), nullptr);

			// Clear layers
			const uint32_t clearVal[4] = { 0, 0, 0, 0 };
			deviceContext->ClearUnorderedAccessViewUint(layerIndices->GetUAV(), clearVal);

			// For each second layer, compute further two adaptive layers out of them with index texture assignment.

			buf = _shader->MapConstantBuffer<ComputeShader::AdaptiveLayerBuffer>();
			buf->SatDimensionsAndRecs.x = static_cast<float>(baseSATA->GetWidth());
			buf->SatDimensionsAndRecs.y = static_cast<float>(baseSATA->GetHeight());
			buf->SatDimensionsAndRecs.z = 1.0f / buf->SatDimensionsAndRecs.x;
			buf->SatDimensionsAndRecs.w = 1.0f / buf->SatDimensionsAndRecs.y;
			buf->BoxHalfSize = boxHalfSizeEnlarged;
			buf->InputLevel = 0;
			buf->IndexOffset = 0;
			buf->InputSATIsDifferential = false;
			_shader->UnmapConstantBuffer();
			_shader->Set(1);


			// Layers 10 and 11 From Layer 1.
			srvPtr[0] = dataA->_layer1->GetSRV();
			srvPtr[1] = dataB->_layer1->GetSRV();
			srvPtr[2] = dataA->_satLayer1->GetSRV();
			srvPtr[3] = dataB->_satLayer1->GetSRV();

			uavPtr[0] = dataA->_layer10->GetUAV();
			uavPtr[1] = dataB->_layer10->GetUAV();
			uavPtr[2] = dataA->_layer11->GetUAV();
			uavPtr[3] = dataB->_layer11->GetUAV();
			uavPtr[4] = layerIndices->GetUAV();

			deviceContext->CSSetShaderResources(4, 4, srvPtr);
			deviceContext->CSSetUnorderedAccessViews(0, 5, uavPtr, nullptr);

			_shader->Dispatch(threadGroupCountX, threadGroupCountY, 1);

			deviceContext->CSSetShaderResources(4, 4, reinterpret_cast<ID3D11ShaderResourceView**>(&nullPtr));
			deviceContext->CSSetUnorderedAccessViews(0, 5, reinterpret_cast<ID3D11UnorderedAccessView**>(&nullPtr), nullptr);
			deviceContext->CSSetSamplers(0, 1, smpPtr);


			// Layers 20 and 21 From Layer 2. SAT of Layer 2 is computed differentially here.
			srvPtr[0] = dataA->_layer2->GetSRV();
			srvPtr[1] = dataB->_layer2->GetSRV();
			srvPtr[2] = dataA->_satLayer1->GetSRV();	// cause differentiality
			srvPtr[3] = dataB->_satLayer1->GetSRV();
			srvPtr[4] = baseSATA->GetSRV();
			srvPtr[5] = baseSATB->GetSRV();

			uavPtr[0] = dataA->_layer20->GetUAV();
			uavPtr[1] = dataB->_layer20->GetUAV();
			uavPtr[2] = dataA->_layer21->GetUAV();
			uavPtr[3] = dataB->_layer21->GetUAV();
			uavPtr[4] = layerIndices->GetUAV();

			buf = _shader->MapConstantBuffer<ComputeShader::AdaptiveLayerBuffer>();
			buf->SatDimensionsAndRecs.x = static_cast<float>(baseSATA->GetWidth());
			buf->SatDimensionsAndRecs.y = static_cast<float>(baseSATA->GetHeight());
			buf->SatDimensionsAndRecs.z = 1.0f / buf->SatDimensionsAndRecs.x;
			buf->SatDimensionsAndRecs.w = 1.0f / buf->SatDimensionsAndRecs.y;
			buf->BoxHalfSize = boxHalfSizeEnlarged;
			buf->InputLevel = 0;
			buf->IndexOffset = 2;
			buf->InputSATIsDifferential = true;
			_shader->UnmapConstantBuffer();

			deviceContext->CSSetShaderResources(4, 6, srvPtr);
			deviceContext->CSSetUnorderedAccessViews(0, 5, uavPtr, nullptr);

			_shader->Dispatch(threadGroupCountX, threadGroupCountY, 1);

			deviceContext->CSSetShaderResources(4, 6, reinterpret_cast<ID3D11ShaderResourceView**>(&nullPtr));
			deviceContext->CSSetUnorderedAccessViews(0, 5, reinterpret_cast<ID3D11UnorderedAccessView**>(&nullPtr), nullptr);
			deviceContext->CSSetSamplers(0, 1, smpPtr);


			// For each child first layer compute SAT, second one will be computed diferentially in SSDOImproved's shader.

			SATGenerator::GetInstance()->Generate(dataA->_layer10, dataB->_layer10, bufferA, bufferB, dataA->_satLayer10, dataB->_satLayer10);
			SATGenerator::GetInstance()->Generate(dataA->_layer20, dataB->_layer20, bufferA, bufferB, dataA->_satLayer20, dataB->_satLayer20);

			// Also compute SAT for indices. Some redundancy here because of uneven number of textures

			SATGenerator::GetInstance()->Generate(layerIndices, dataB->_layer20, bufferA, bufferB, satLayerIndices, dataB->_satLayer20);

		}


		AdaptiveLayerGenerator::AdaptiveLayerData::AdaptiveLayerData(GenerateTexParamsFunc genFunc)
		{
			_layer1 = new RWTexture();
			_layer2 = new RWTexture();
			_layer10 = new RWTexture();
			_layer11 = new RWTexture();
			_layer20 = new RWTexture();
			_layer21 = new RWTexture();
			_satLayer1 = new RWTexture();
			_satLayer10 = new RWTexture();
			_satLayer20 = new RWTexture();
			genFunc(_layer1);
			genFunc(_layer2);
			genFunc(_layer10);
			genFunc(_layer11);
			genFunc(_layer20);
			genFunc(_layer21);
			genFunc(_satLayer1);
			genFunc(_satLayer10);
			genFunc(_satLayer20);
		}

		AdaptiveLayerGenerator::AdaptiveLayerData::~AdaptiveLayerData()
		{
			delete _layer1;
			delete _layer2;
			delete _layer10;
			delete _layer11;
			delete _layer20;
			delete _layer21;
			delete _satLayer1;
			delete _satLayer10;
			delete _satLayer20;
		}
	}
}
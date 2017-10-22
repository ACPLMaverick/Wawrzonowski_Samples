#include "stdafx.h"
#include "DepthAwareBlur.h"
#include "System.h"
#include "Scenes/Scene.h"
#include "ComputeShader.h"
#include "Texture.h"
#include "RWTexture.h"
#include "Helper.h"

namespace Postprocesses
{
	namespace Utility
	{
		DepthAwareBlur::DepthAwareBlur(float depthFilterHalfWidth, float filterSampleSpacing, uint32_t blurCount) :
			_depthFilterHalfWidth(depthFilterHalfWidth),
			_filterSampleSpacing(filterSampleSpacing),
			_blurCount(blurCount)
		{
			_shader = System::GetInstance()->GetScene()->LoadComputeShader(L"DepthAwareBlur");
		}


		DepthAwareBlur::~DepthAwareBlur()
		{
		}

		void DepthAwareBlur::Generate(const GBuffer::RenderTarget * inputA, const GBuffer::RenderTarget * inputB, RWTexture * bufferA, RWTexture * bufferB, RWTexture * outputA, RWTexture * outputB) const
		{
			const uint32_t width(System::GetInstance()->GetOptions()._windowWidth);
			GenerateInternal(inputA->SRV, inputB->SRV, bufferA, bufferB, outputA, outputB,
				width, System::GetInstance()->GetOptions()._windowHeight, 
				Helper::GetTextureLevel(width, outputA->GetWidth()));
		}

		void DepthAwareBlur::Generate(const Texture * inputA, const Texture * inputB, RWTexture * bufferA, RWTexture * bufferB, RWTexture * outputA, RWTexture * outputB) const
		{
			GenerateInternal(inputA->GetSRV(), inputB->GetSRV(), bufferA, bufferB, outputA, outputB, 
				inputA->GetWidth(), inputB->GetHeight(), Helper::GetTextureLevel(inputA->GetWidth(), outputA->GetWidth()));
		}

		void DepthAwareBlur::GenerateInternal(ID3D11ShaderResourceView * inputA, ID3D11ShaderResourceView * inputB, RWTexture * bufferA, RWTexture * bufferB, 
			RWTexture * outputA, RWTexture * outputB, uint32_t width, uint32_t height, uint32_t level) const
		{
			width >>= level;
			height >>= level;
			ID3D11DeviceContext* deviceContext(Renderer::GetInstance()->GetDeviceContext());
			uint32_t threadGroupCountX, threadGroupCountY, threadGroupCountYRec;
			ComputeShader::DepthAwareBlurBuffer* buf;

			threadGroupCountX = width / (_shader->GetThreadCountInGroupX() * 2) + 1;
			threadGroupCountY = height / _shader->GetThreadCountInGroupY();
			threadGroupCountYRec = width / _shader->GetThreadCountInGroupY();

			RWTexture* ins[2] = { bufferA, bufferB };
			RWTexture* outs[2] = { outputA, outputB };
			ID3D11ShaderResourceView* inputs[2] = { inputA, inputB };
			ID3D11UnorderedAccessView* outputs[2] = { bufferA->GetUAV(), bufferB->GetUAV() };

			for (uint32_t i = 0; i < _blurCount; ++i)
			{
				buf = _shader->MapConstantBuffer<ComputeShader::DepthAwareBlurBuffer>();
				buf->Width = width;
				buf->Vertical = false;
				buf->Level = level;
				buf->FilterSampleSpacing = _filterSampleSpacing;
				buf->FilterHalfSize = _depthFilterHalfWidth;
				_shader->UnmapConstantBuffer();
				_shader->Set(1);

				deviceContext->CSSetShaderResources(0, 2, inputs);
				deviceContext->CSSetUnorderedAccessViews(0, 2, outputs, nullptr);

				_shader->Dispatch(threadGroupCountX, threadGroupCountY, 1);

				inputs[0] = ins[0]->GetSRV(); inputs[1] = ins[1]->GetSRV(); outputs[0] = outs[0]->GetUAV(); outputs[1] = outs[1]->GetUAV();


				buf = _shader->MapConstantBuffer<ComputeShader::DepthAwareBlurBuffer>();
				buf->Width = height;
				buf->Vertical = true;
				buf->Level = 0;
				buf->FilterSampleSpacing = _filterSampleSpacing;
				buf->FilterHalfSize = _depthFilterHalfWidth;
				_shader->UnmapConstantBuffer();

				deviceContext->CSSetUnorderedAccessViews(0, 2, outputs, nullptr);
				deviceContext->CSSetShaderResources(0, 2, inputs);

				_shader->Dispatch(threadGroupCountX, threadGroupCountYRec, 1);

				if (level != 0)
					level = 0;

				RWTexture* tmp = ins[0];
				ins[0] = outs[0];
				outs[0] = tmp;
				tmp = ins[1];
				ins[1] = outs[1];
				outs[1] = tmp;

				inputs[0] = ins[0]->GetSRV(); inputs[1] = ins[1]->GetSRV(); outputs[0] = outs[0]->GetUAV(); outputs[1] = outs[1]->GetUAV();

				// cleanup

				inputs[0] = nullptr; inputs[1] = nullptr; outputs[0] = nullptr; outputs[1] = nullptr;
				deviceContext->CSSetUnorderedAccessViews(0, 2, outputs, nullptr);
				deviceContext->CSSetShaderResources(0, 2, inputs);
			}


		}
	}
}
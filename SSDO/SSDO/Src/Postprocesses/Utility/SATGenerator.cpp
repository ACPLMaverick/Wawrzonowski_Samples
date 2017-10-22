#include "stdafx.h"
#include "SATGenerator.h"
#include "Texture.h"
#include "RWTexture.h"
#include "ComputeShader.h"
#include "System.h"
#include "Scenes\Scene.h"
#include "Renderer.h"
#include "Helper.h"

namespace Postprocesses
{
	namespace Utility
	{

		SATGenerator::SATGenerator()
		{
			_shader = System::GetInstance()->GetScene()->LoadComputeShader(L"SAT");
		}

		SATGenerator::~SATGenerator()
		{
		}

		void SATGenerator::Generate(
			const Texture* inputA,
			const Texture* inputB,
			RWTexture* bufferA,
			RWTexture* bufferB,
			RWTexture* outputA,
			RWTexture* outputB) const
		{
			ASSERT(inputA->GetWidth() == inputB->GetWidth() && inputA->GetHeight() == inputB->GetHeight() && inputA->GetFormat() == inputB->GetFormat());
			uint32_t inputWidth(inputA->GetWidth());
			uint32_t inputHeight(inputA->GetHeight());
			uint32_t inputLevel(Helper::GetTextureLevel(inputWidth, outputA->GetWidth()));
			GenerateInternal(inputA->GetSRV(), inputB->GetSRV(), bufferA->GetSRV(), bufferB->GetSRV(),
				bufferA->GetUAV(), bufferB->GetUAV(), outputA->GetUAV(), outputB->GetUAV(),
				inputWidth, inputHeight, inputLevel);

			// TETIN!
			/*output->AcquireDeviceData();
			PrintRawData(input);
			PrintRawData(output);

			int i = 5;
			i++;*/
		}

		void SATGenerator::Generate(
			const GBuffer::RenderTarget* inputA,
			const GBuffer::RenderTarget* inputB,
			RWTexture* bufferA,
			RWTexture* bufferB,
			RWTexture* outputA,
			RWTexture* outputB) const
		{
			uint32_t inputWidth(System::GetInstance()->GetOptions()._windowWidth);
			uint32_t inputHeight(System::GetInstance()->GetOptions()._windowHeight);
			uint32_t inputLevel(Helper::GetTextureLevel(inputWidth, outputA->GetWidth()));
			GenerateInternal(inputA->SRV, inputB->SRV, bufferA->GetSRV(), bufferB->GetSRV(),
				bufferA->GetUAV(), bufferB->GetUAV(), outputA->GetUAV(), outputB->GetUAV(),
				inputWidth, inputHeight, inputLevel);
		}

		inline void SATGenerator::GenerateInternal(
			ID3D11ShaderResourceView* const inputASRV,
			ID3D11ShaderResourceView* const inputBSRV,
			ID3D11ShaderResourceView* const bufferASRV,
			ID3D11ShaderResourceView* const bufferBSRV,
			ID3D11UnorderedAccessView* bufferAUAV,
			ID3D11UnorderedAccessView* bufferBUAV,
			ID3D11UnorderedAccessView* outputAUAV,
			ID3D11UnorderedAccessView* outputBUAV,
			uint32_t inputWidth, uint32_t inputHeight, uint32_t inputLevel) const
		{
			ID3D11DeviceContext* deviceContext(Renderer::GetInstance()->GetDeviceContext());

			inputWidth >>= inputLevel;
			inputHeight >>= inputLevel;

			ComputeShader::SATBuffer* buf = _shader->MapConstantBuffer<ComputeShader::SATBuffer>();
			buf->WidthWidthpowLevel[0] = inputWidth;
			buf->WidthWidthpowLevel[1] = Helper::GetPowerOfTwoHigherThan(inputWidth);
			buf->WidthWidthpowLevel[2] = inputLevel;
			buf->Vertical = false;
			_shader->UnmapConstantBuffer();
			_shader->Set(1);

			deviceContext->CSSetShaderResources(4, 1, &inputASRV);
			deviceContext->CSSetShaderResources(5, 1, &inputBSRV);

			deviceContext->CSSetUnorderedAccessViews(0, 1, &bufferAUAV, nullptr);
			deviceContext->CSSetUnorderedAccessViews(1, 1, &bufferBUAV, nullptr);

			uint32_t threadGroupCountX, threadGroupCountY;

			threadGroupCountX = inputWidth / (_shader->GetThreadCountInGroupX() * 2) + 1;
			threadGroupCountY = inputHeight / _shader->GetThreadCountInGroupY();

			_shader->Dispatch(threadGroupCountX, threadGroupCountY, 1);

			ID3D11UnorderedAccessView* nullik[2]{ nullptr, nullptr };
			deviceContext->CSSetUnorderedAccessViews(0, 2, nullik, nullptr);

			buf = _shader->MapConstantBuffer<ComputeShader::SATBuffer>();
			buf->WidthWidthpowLevel[0] = inputHeight;
			buf->WidthWidthpowLevel[1] = Helper::GetPowerOfTwoHigherThan(inputHeight);
			buf->WidthWidthpowLevel[2] = 0;
			buf->Vertical = true;
			_shader->UnmapConstantBuffer();

			deviceContext->CSSetShaderResources(4, 1, &bufferASRV);
			deviceContext->CSSetShaderResources(5, 1, &bufferBSRV);

			deviceContext->CSSetUnorderedAccessViews(0, 1, &outputAUAV, nullptr);
			deviceContext->CSSetUnorderedAccessViews(1, 1, &outputBUAV, nullptr);

			threadGroupCountX = inputHeight / (_shader->GetThreadCountInGroupX() * 2) + 1;
			threadGroupCountY = inputWidth / _shader->GetThreadCountInGroupY();

			_shader->Dispatch(threadGroupCountX, threadGroupCountY, 1);

			deviceContext->CSSetShaderResources(4, 1, reinterpret_cast<ID3D11ShaderResourceView**>(nullik));
			deviceContext->CSSetUnorderedAccessViews(0, 2, nullik, nullptr);
		}

		inline void SATGenerator::PrintRawData(const Texture * tex) const
		{
			PackedVector::XMHALF4* data(reinterpret_cast<PackedVector::XMHALF4*>(const_cast<Texture*>(tex)->GetRawData().GetDataPtr()));
			std::wstring dbg = L"\n";
			for (size_t i = 0; i < tex->GetHeight(); ++i)
			{
				for (size_t j = 0; j < tex->GetWidth(); ++j)
				{
					XMFLOAT4 field;
					field.x = PackedVector::XMConvertHalfToFloat(data[tex->GetWidth() * i + j].x);
					field.y = PackedVector::XMConvertHalfToFloat(data[tex->GetWidth() * i + j].y);
					field.z = PackedVector::XMConvertHalfToFloat(data[tex->GetWidth() * i + j].z);
					field.w = PackedVector::XMConvertHalfToFloat(data[tex->GetWidth() * i + j].w);
					dbg += L" [ " + to_wstring(field.x) + L" , " + to_wstring(field.y) + L" , " + to_wstring(field.z) + L" , " + to_wstring(field.w) + L" ] ";
				}
				dbg += L"\n";
			}

			OutputDebugString(dbg.c_str());
		}
	}
}
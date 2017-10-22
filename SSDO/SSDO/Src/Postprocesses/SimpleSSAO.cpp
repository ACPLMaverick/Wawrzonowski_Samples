#include "stdafx.h"
#include "SimpleSSAO.h"
#include "Shader.h"
#include "System.h"
#include "Scenes\Scene.h"
#include "Renderer.h"
#include "System.h"
#include "Random.h"
#include "Camera.h"
#include "Texture.h"

using namespace DirectX::PackedVector;
#define HALF_MIN 6.10352e-5f
#define HALF_MAX 65504.f

namespace Postprocesses
{
	SimpleSSAO::SimpleSSAO() :
		_ssaoBuffer(nullptr),
		_randomVectorTexture(nullptr),
		_maxDistance(0.5f),
		_fadeStart(0.02f),
		_epsilon(0.01f)
	{
		RandomVectorsGenerator::Generate(&_randomVectorTexture, 1024);

		_shaders.push_back(System::GetInstance()->GetScene()->LoadShader(std::wstring(L"SimpleSSAO_Base")));
		_shaders.push_back(System::GetInstance()->GetScene()->LoadShader(std::wstring(L"SimpleSSAO_BlurMerge")));

		ID3D11Device* device = Renderer::GetInstance()->GetDevice();

		D3D11_BUFFER_DESC bDesc;
		bDesc.Usage = D3D11_USAGE_DYNAMIC;
		bDesc.ByteWidth = sizeof(Shader::SSAOBasePS);
		bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bDesc.MiscFlags = 0;
		bDesc.StructureByteStride = 0;

		device->CreateBuffer(&bDesc, nullptr, &_ssaoBuffer);
		ASSERT(_ssaoBuffer != nullptr);

		// offsets

		_offsets[0] = XMFLOAT4A(1.0f, 1.0f, 1.0f, 0.0f);
		_offsets[1] = XMFLOAT4A(-1.0f, -1.0f, -1.0f, 0.0f);
		_offsets[2] = XMFLOAT4A(-1.0f, 1.0f, 1.0f, 0.0f);
		_offsets[3] = XMFLOAT4A(1.0f, -1.0f, -1.0f, 0.0f);
		_offsets[4] = XMFLOAT4A(1.0f, 1.0f, -1.0f, 0.0f);
		_offsets[5] = XMFLOAT4A(-1.0f, -1.0f, 1.0f, 0.0f);
		_offsets[6] = XMFLOAT4A(-1.0f, 1.0f, -1.0f, 0.0f);
		_offsets[7] = XMFLOAT4A(1.0f, -1.0f, 1.0f, 0.0f);
		_offsets[8] = XMFLOAT4A(-1.0f, 0.0f, 0.0f, 0.0f);
		_offsets[9] = XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f);
		_offsets[10] = XMFLOAT4A(0.0f, -1.0f, 0.0f, 0.0f);
		_offsets[11] = XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f);
		_offsets[12] = XMFLOAT4A(0.0f, 0.0f, -1.0f, 0.0f);
		_offsets[13] = XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f);

		for (int32_t i = 0; i < SAMPLE_COUNT; ++i)
		{
			float rand = System::GetInstance()->GetRandom()->GetFloat(0.25f, 1.0f);
			XMVECTOR vc = XMLoadFloat4A(&_offsets[i]);
			vc = rand * XMVector4Normalize(vc);
			XMStoreFloat4(&_offsets[i], vc);
		}
	}

	SimpleSSAO::~SimpleSSAO()
	{
		delete _randomVectorTexture;
	}

	void SimpleSSAO::Update()
	{
	}

	void SimpleSSAO::SetPass(GBuffer& gBuffer, const Camera & camera, int32_t passIndex) const
	{
		Postprocess::SetPass(gBuffer, camera, passIndex);

		if (passIndex == 0)
		{
			Shader::SSAOBasePS* buffer = reinterpret_cast<Shader::SSAOBasePS*>(_shaders[0]->MapPsBuffer(1));
			XMMATRIX proj = XMLoadFloat4x4A(&camera.GetMatProj());
			XMStoreFloat4x4A(&buffer->Proj, XMMatrixTranspose(proj));
			memcpy(&buffer->Offsets, _offsets, SAMPLE_COUNT * sizeof(XMFLOAT4A));
			FillParams(&buffer->Params);

			_shaders[0]->UnmapPsBuffer(1);

			_randomVectorTexture->Set(5);

			const int32_t inSlots[1] = { 4 };
			gBuffer.PPClearBuffersAsInput(inSlots, 1);
			const GBuffer::RenderTarget* rts[1] = { &gBuffer.PPGetBuffers()[0] };
			gBuffer.PPSetBuffersAsOutput(rts, 1, nullptr);
		}
		else if (passIndex == 1)
		{
			Shader::SSAOBlurMergePS* buffer = reinterpret_cast<Shader::SSAOBlurMergePS*>(_shaders[1]->MapPsBuffer(1));
			buffer->TexelSize = XMFLOAT2A(1.0f / ((float)System::GetInstance()->GetOptions()._windowWidth / GBuffer::PP_BUFFER_SIZE_DIVISOR), 
				1.0f / ((float)System::GetInstance()->GetOptions()._windowHeight / GBuffer::PP_BUFFER_SIZE_DIVISOR));
			_shaders[1]->UnmapPsBuffer(1);

			
			gBuffer.PPClearBuffersAsOutput(1);
			const GBuffer::RenderTarget* in[1] = { &gBuffer.PPGetBuffers()[0] };
			const int32_t inSlots[1] = { 4 };
			gBuffer.PPSetBuffersAsInput(in, inSlots, 1);
			const GBuffer::RenderTarget* rts[1] = { gBuffer.PPGetOutputBuffer() };
			gBuffer.PPSetBuffersAsOutput(rts, 1, nullptr);
		}
	}

	inline void SimpleSSAO::FillParams(XMFLOAT4A * paramBuffer) const
	{
		paramBuffer->x = _maxDistance;
		paramBuffer->y = _fadeStart;
		paramBuffer->z = _epsilon;
		paramBuffer->w = 0.0f;
	}

	void RandomVectorsGenerator::Generate(Texture ** tex, uint32_t width)
	{
		const uint32_t singlePixelSize = sizeof(HALF) * 4;

		(*tex) = new Texture();

		Buffer<uint8_t>& buf = (*tex)->GetRawData();
		const uint32_t pixelCount = width * width;
		const uint32_t allPixelsSize = pixelCount * singlePixelSize;
		buf.Allocate(allPixelsSize);
		buf.Resize(allPixelsSize);
		XMVECTOR pixelVector;
		HALF pixelHalf[4];
		ZeroMemory(pixelHalf, singlePixelSize);

		(*tex)->SetWidth(width);
		(*tex)->SetBPP(singlePixelSize);
		(*tex)->SetFormat(DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
		(*tex)->SetMipmapped(false);

		for (uint32_t i = 0; i < pixelCount; ++i)
		{
			// Create random vector (fill pixelHalf).
			for (uint32_t j = 0; j < 3; ++j)
			{
				pixelVector.m128_f32[j] = System::GetInstance()->GetRandom()->GetFloat(0.0001f, HALF_MAX);
			}
			pixelVector.m128_f32[3] = 0.0f;

			// normalize random vector
			pixelVector = XMVector3Normalize(pixelVector);

			for (uint32_t j = 0; j < 3; ++j)
			{
				pixelHalf[j] = XMConvertFloatToHalf(pixelVector.m128_f32[j]);
			}
			
			// copy to buffer data
			size_t offset = i * singlePixelSize;
			ASSERT(offset < buf.GetSizeBytes());

			memcpy(buf.GetDataPtr() + offset, pixelHalf, singlePixelSize);
		}

		(*tex)->InitResources(true);
	}
}
#include "stdafx.h"
#include "SSDOImproved.h"
#include "System.h"
#include "Renderer.h"
#include "Random.h"
#include "Camera.h"
#include "Scenes\Scene.h"
#include "SimpleSSAO.h"
#include "GBuffer.h"
#include "Lights/LightDirectional.h"
#include "RWTexture.h"
#include "Utility/SATGenerator.h"


namespace Postprocesses
{
	using namespace Utility;

	SSDOImproved::SSDOImproved() :
		_dataBuffer(nullptr),
		_sampleBoxHalfSize(0.3f),
		_occlusionPower(1.0f),
		_occlusionFaloff(1.0f),
		_powFactor(1.0f),
		_adaptiveDataNormalDepth(&SSDOImproved::AssignTextureParams),
		_adaptiveDataColor(&SSDOImproved::AssignTextureParams)
	{
		// TETIN

		const int32_t sizX(1280 / SAT_SIZE_DIVISOR);
		const int32_t sizY(720 / SAT_SIZE_DIVISOR);

		_testInput = new Texture();
		_testInput->SetWidth(sizX);
		_testInput->SetHeight(sizY);
		_testInput->SetFormat(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT);
		_testInput->SetBPP(128);
		_testInput->SetMipmapped(false);
		_testInput->AllocateRawDataToTextureSize();

		XMFLOAT4* ptr = reinterpret_cast<XMFLOAT4*>(_testInput->GetRawData().GetDataPtr());
		for (size_t i = 0; i < sizX * sizY; ++i)
		{
			ptr[i] = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
		}
		
		_testInput->InitResources(false, false);

		_testOutput = new RWTexture();
		_testOutput->SetWidth(sizX);
		_testOutput->SetHeight(sizY);
		_testOutput->SetFormat(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT);
		_testOutput->SetBPP(128);
		_testOutput->SetMipmapped(false);
		_testOutput->AllocateRawDataToTextureSize();
		_testOutput->InitResources(false, false);

		_testBuf = new RWTexture();
		_testBuf->SetWidth(sizX);
		_testBuf->SetHeight(sizY);
		_testBuf->SetFormat(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT);
		_testBuf->SetBPP(128);
		_testBuf->SetMipmapped(false);
		_testBuf->InitResources(true, false);

		// END TETIN

		_satColor = new RWTexture();
		_satNormalDepth = new RWTexture();
		_satBufferA = new RWTexture();
		_satBufferB = new RWTexture();
		_layerIndices = new RWTexture();
		_satLayerIndices = new RWTexture();
		
		AssignTextureParams(_satColor);
		AssignTextureParams(_satNormalDepth);
		AssignTextureParams(_satBufferA);
		AssignTextureParams(_satBufferB);
		AssignTextureParams(_layerIndices);
		AssignTextureParams(_satLayerIndices);

		_shaders.push_back(System::GetInstance()->GetScene()->LoadShader(std::wstring(L"SSDOImproved_Base")));

		ID3D11Device* device = Renderer::GetInstance()->GetDevice();

		D3D11_BUFFER_DESC bDesc;
		bDesc.Usage = D3D11_USAGE_DYNAMIC;
		bDesc.ByteWidth = sizeof(Shader::SSDOImprovedPS);
		bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bDesc.MiscFlags = 0;
		bDesc.StructureByteStride = 0;

		device->CreateBuffer(&bDesc, nullptr, &_dataBuffer);
		ASSERT(_dataBuffer != nullptr);
	}

	SSDOImproved::~SSDOImproved()
	{
		delete _testInput;
		delete _testBuf;
		delete _testOutput;

		_dataBuffer->Release();
		delete _dataBuffer;

		delete _satColor;
		delete _satNormalDepth;
		delete _satBufferA;
		delete _satBufferB;
		delete _layerIndices;
		delete _satLayerIndices;
	}

	void SSDOImproved::Update()
	{
	}

	void SSDOImproved::SetPass(GBuffer& gBuffer, const Camera & camera, int32_t passIndex) const
	{
		Postprocess::SetPass(gBuffer, camera, passIndex);

		ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
		ID3D11ShaderResourceView* nullik[2]{ nullptr, nullptr };
		deviceContext->PSSetShaderResources(4, 2, nullik);

		// Generate SATs.
		//_satGen.Generate(_testInput, _testBuf, _testOutput);
		SATGenerator::GetInstance()->Generate(gBuffer.GetNormalDepthBuffer(), gBuffer.GetColorBuffer(),
									_satBufferA, _satBufferB, _satNormalDepth, _satColor);

		// Generate adaptive layers.
		AdaptiveLayerGenerator::GetInstance()->Generate(gBuffer.GetNormalDepthBuffer(), gBuffer.GetColorBuffer(),
			_satNormalDepth, _satColor, _satBufferA, _satBufferB, _layerIndices, _satLayerIndices, &_adaptiveDataNormalDepth, &_adaptiveDataColor,
			_sampleBoxHalfSize);

		Shader::SSDOImprovedPS* buffer = reinterpret_cast<Shader::SSDOImprovedPS*>(_shaders[0]->MapPsBuffer(1));
		reinterpret_cast<Lights::LightDirectional*>(&buffer->LightColor)->ApplyToShader(
			*System::GetInstance()->GetScene()->GetLightsDirectional()[0],
			camera
		);
		buffer->SatDimensionsAndRecs.x = static_cast<float>(_satNormalDepth->GetWidth());
		buffer->SatDimensionsAndRecs.y = static_cast<float>(_satNormalDepth->GetHeight());
		buffer->SatDimensionsAndRecs.z = 1.0f / buffer->SatDimensionsAndRecs.x;
		buffer->SatDimensionsAndRecs.w = 1.0f / buffer->SatDimensionsAndRecs.y;
		buffer->SampleBoxHalfSize = _sampleBoxHalfSize;
		buffer->OcclusionPower = _occlusionPower;
		buffer->OcclusionFalloff = _occlusionFaloff;
		buffer->PowFactor = _powFactor;

		_shaders[0]->UnmapPsBuffer(1);

		// Set four adaptive layers for each data buffer (8 textures) and index texture (totally 9)
		// Argh argh
		_satNormalDepth->Set(4);
		_satColor->Set(5);

		_adaptiveDataNormalDepth._satLayer1->Set(6);
		_adaptiveDataNormalDepth._satLayer10->Set(7);
		_adaptiveDataNormalDepth._satLayer20->Set(8);
		_adaptiveDataColor._satLayer1->Set(9);
		_adaptiveDataColor._satLayer10->Set(10);
		_adaptiveDataColor._satLayer20->Set(11);
		_satLayerIndices->Set(12);

		const GBuffer::RenderTarget* in[1] = { gBuffer.PPGetOutputBBuffer() };
		const int32_t inSlots[1] = { 3 };
		gBuffer.PPSetBuffersAsInput(in, inSlots, 1);
		const GBuffer::RenderTarget* rts[1] = { gBuffer.PPGetOutputBuffer() };
		gBuffer.PPSetBuffersAsOutput(rts, 1, nullptr);
	}

	void SSDOImproved::AfterPass(GBuffer & gBuffer, const Camera & camera, int32_t passIndex) const
	{
		ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
		ID3D11ShaderResourceView* nullik[7]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		deviceContext->PSSetShaderResources(6, 7, nullik);
	}

	void SSDOImproved::AssignTextureParams(Texture * tex)
	{
		tex->SetWidth(System::GetInstance()->GetOptions()._windowWidth / SAT_SIZE_DIVISOR);
		tex->SetHeight(System::GetInstance()->GetOptions()._windowHeight / SAT_SIZE_DIVISOR);
		tex->SetFormat(DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT);
		tex->SetBPP(128);
		tex->SetMipmapped(false);
		tex->InitResources(false, false);
	}
}

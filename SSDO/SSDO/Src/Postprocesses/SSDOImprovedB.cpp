#include "stdafx.h"
#include "SSDOImprovedB.h"
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

	SSDOImprovedB::SSDOImprovedB() :
		_dataBuffer(nullptr),
		_sampleBoxHalfSize(0.3f),
		_occlusionPower(1.0f),
		_occlusionFaloff(1.0f),
		_powFactor(1.0f),
		_blurGen(0.3f, 0.0f, 1)
	{
		_bufColor = new RWTexture();
		_bufNormalDepth = new RWTexture();
		_tempA = new RWTexture();
		_tempB = new RWTexture();
		
		AssignTextureParams(_bufColor);
		AssignTextureParams(_bufNormalDepth, true);
		AssignTextureParams(_tempA);
		AssignTextureParams(_tempB);

		_shaders.push_back(System::GetInstance()->GetScene()->LoadShader(std::wstring(L"SSDOImprovedB_Base")));

		ID3D11Device* device = Renderer::GetInstance()->GetDevice();

		D3D11_BUFFER_DESC bDesc;
		bDesc.Usage = D3D11_USAGE_DYNAMIC;
		bDesc.ByteWidth = sizeof(Shader::SSDOImprovedBPS);
		bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bDesc.MiscFlags = 0;
		bDesc.StructureByteStride = 0;

		device->CreateBuffer(&bDesc, nullptr, &_dataBuffer);
		ASSERT(_dataBuffer != nullptr);
	}

	SSDOImprovedB::~SSDOImprovedB()
	{

		_dataBuffer->Release();
		delete _dataBuffer;

		delete _bufColor;
		delete _bufNormalDepth;
		delete _tempA;
		delete _tempB;
	}

	void SSDOImprovedB::Update()
	{
	}

	void SSDOImprovedB::SetPass(GBuffer& gBuffer, const Camera & camera, int32_t passIndex) const
	{
		Postprocess::SetPass(gBuffer, camera, passIndex);

		ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
		ID3D11ShaderResourceView* nullik[2]{ nullptr, nullptr };
		deviceContext->PSSetShaderResources(4, 2, nullik);

		// Generate blurred buffers.
		_blurGen.Generate(gBuffer.GetNormalDepthBuffer(), gBuffer.GetColorBuffer(), _tempA, _tempB, _bufNormalDepth, _bufColor);

		// Generate mips for edge fixing in pixel shader.
		deviceContext->GenerateMips(_bufNormalDepth->GetSRV());

		Shader::SSDOImprovedBPS* buffer = reinterpret_cast<Shader::SSDOImprovedBPS*>(_shaders[0]->MapPsBuffer(1));
		reinterpret_cast<Lights::LightDirectional*>(&buffer->LightColor)->ApplyToShader(
			*System::GetInstance()->GetScene()->GetLightsDirectional()[0],
			camera
		);
		buffer->SampleBoxHalfSize = _blurGen.GetDepthFilterHalfWidth();
		buffer->OcclusionPower = _occlusionPower;
		buffer->OcclusionFalloff = _occlusionFaloff;
		buffer->PowFactor = _powFactor;

		_shaders[0]->UnmapPsBuffer(1);

		_bufNormalDepth->Set(4);
		_bufColor->Set(5);

		const GBuffer::RenderTarget* in[1] = { gBuffer.PPGetOutputBBuffer() };
		const int32_t inSlots[1] = { 3 };
		gBuffer.PPSetBuffersAsInput(in, inSlots, 1);
		const GBuffer::RenderTarget* rts[1] = { gBuffer.PPGetOutputBuffer() };
		gBuffer.PPSetBuffersAsOutput(rts, 1, nullptr);
	}

	void SSDOImprovedB::AfterPass(GBuffer & gBuffer, const Camera & camera, int32_t passIndex) const
	{
		ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
		ID3D11ShaderResourceView* nullik[2]{ nullptr, nullptr };
		deviceContext->PSSetShaderResources(4, 2, nullik);
		deviceContext->PSSetSamplers(4, 2, reinterpret_cast<ID3D11SamplerState**>(nullik));
	}

	void SSDOImprovedB::AssignTextureParams(Texture * tex, bool mipmapped)
	{
		tex->SetWidth(System::GetInstance()->GetOptions()._windowWidth / BUFFER_SIZE_DIVISOR);
		tex->SetHeight(System::GetInstance()->GetOptions()._windowHeight / BUFFER_SIZE_DIVISOR);
		tex->SetFormat(DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
		tex->SetBPP(64);
		tex->SetMipmapped(mipmapped);
		tex->InitResources(false, false);
	}
}

#include "stdafx.h"
#include "GBuffer.h"
#include "System.h"
#include "Renderer.h"
#include "Camera.h"
#include "Scenes\Scene.h"
#include "Lights\LightAmbient.h"
#include "Lights\LightDirectional.h"
#include "Lights\LightPoint.h"
#include "Postprocesses\Postprocess.h"
using namespace Lights;
using namespace Postprocesses;

#define SHADER_DRAW L"DeferredDrawShader"
#define SHADER_MERGE L"DeferredLightMergeShader"
#define SHADER_L_AMBIENT L"DeferredLightAmbientShader"
#define SHADER_L_DIRECTIONAL L"DeferredLightDirectionalShader"
#define SHADER_L_POINT L"DeferredLightPointShader"

GBuffer::GBuffer(const Camera& camera) :
	_camera(camera)
{
	const uint32_t w = System::GetInstance()->GetOptions()._windowWidth;
	const uint32_t h = System::GetInstance()->GetOptions()._windowHeight;
	const DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	const DXGI_FORMAT formatNormal = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
	const DXGI_FORMAT formatDepthTexture = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
	const DXGI_FORMAT formatDepthDsv = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	const DXGI_FORMAT formatDepthSrv = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	const D3D11_SRV_DIMENSION srvDimension = Renderer::GetInstance()->DEFAULT_SAMPLE_COUNT != 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
	const D3D11_DSV_DIMENSION dsvDimension = Renderer::GetInstance()->DEFAULT_SAMPLE_COUNT != 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();

	D3D11_TEXTURE2D_DESC descTexture;
	descTexture.ArraySize = 1;
	descTexture.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	descTexture.CPUAccessFlags = 0;
	descTexture.Format = format;
	descTexture.MipLevels = 1;
	descTexture.MiscFlags = 0;
	descTexture.Usage = D3D11_USAGE_DEFAULT;
	descTexture.Width = w;
	descTexture.Height = h;
	descTexture.SampleDesc.Count = Renderer::GetInstance()->DEFAULT_SAMPLE_COUNT;
	descTexture.SampleDesc.Quality = Renderer::GetInstance()->GetSampleQuality() - 1;

	D3D11_TEXTURE2D_DESC descTextureBuffers = descTexture;
	descTextureBuffers.Width /= PP_BUFFER_SIZE_DIVISOR;
	descTextureBuffers.Height /= PP_BUFFER_SIZE_DIVISOR;

	D3D11_TEXTURE2D_DESC descMipmapped = descTexture;
	descMipmapped.MipLevels = 0;
	descMipmapped.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_TEXTURE2D_DESC descTextureNormals = descMipmapped;
	descTextureNormals.Format = formatNormal;

	D3D11_SAMPLER_DESC descSampler;
	descSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	descSampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	descSampler.MipLODBias = 0.0f;
	descSampler.MaxAnisotropy = 1;
	descSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	descSampler.BorderColor[0] = 0;
	descSampler.BorderColor[1] = 0;
	descSampler.BorderColor[2] = 0;
	descSampler.BorderColor[3] = 0;
	descSampler.MinLOD = 0;
	descSampler.MaxLOD = D3D11_FLOAT32_MAX;

	D3D11_SAMPLER_DESC descSamplerBuffers = descSampler;
	descSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	D3D11_SHADER_RESOURCE_VIEW_DESC descSrv;
	ZeroMemory(&descSrv, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	descSrv.Format = format;
	descSrv.ViewDimension = srvDimension;
	descSrv.Texture2D.MipLevels = 1;
	descSrv.Texture2D.MostDetailedMip = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC descSrvMipmapped = descSrv;
	descSrvMipmapped.Texture2D.MipLevels = -1;

	D3D11_SHADER_RESOURCE_VIEW_DESC descSrvNormals = descSrvMipmapped;
	descSrvNormals.Format = formatNormal;

	D3D11_TEXTURE2D_DESC descDs;
	descDs.Width = w;
	descDs.Height = h;
	descDs.MipLevels = 1;
	descDs.ArraySize = 1;
	descDs.Format = formatDepthTexture;
	descDs.SampleDesc.Count = Renderer::GetInstance()->DEFAULT_SAMPLE_COUNT;
	descDs.SampleDesc.Quality = Renderer::GetInstance()->GetSampleQuality() - 1;
	descDs.Usage = D3D11_USAGE_DEFAULT;
	descDs.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDs.CPUAccessFlags = 0;
	descDs.MiscFlags = 0;
	
	HRESULT hr;
	hr = device->CreateTexture2D(&descMipmapped, nullptr, &_color.Texture);
	ASSERT(hr == S_OK);
	hr = device->CreateTexture2D(&descTextureNormals, nullptr, &_normal.Texture);
	ASSERT(hr == S_OK);
	hr = device->CreateTexture2D(&descTexture, nullptr, &_outputA.Texture);
	ASSERT(hr == S_OK);
	hr = device->CreateTexture2D(&descTexture, nullptr, &_outputB.Texture);
	ASSERT(hr == S_OK);
	for (size_t i = 0; i < PP_BUFFER_COUNT; ++i)
	{
		hr = device->CreateTexture2D(&descTextureBuffers, nullptr, &_postprocessBuffers[i].Texture);
		ASSERT(hr == S_OK);
	}
	hr = device->CreateTexture2D(&descDs, nullptr, &_depth.Texture);
	ASSERT(hr == S_OK);

	hr = device->CreateRenderTargetView(_color.Texture, nullptr, &_color.View);
	ASSERT(hr == S_OK);
	hr = device->CreateRenderTargetView(_normal.Texture, nullptr, &_normal.View);
	ASSERT(hr == S_OK);
	hr = device->CreateRenderTargetView(_outputA.Texture, nullptr, &_outputA.View);
	ASSERT(hr == S_OK);
	hr = device->CreateRenderTargetView(_outputB.Texture, nullptr, &_outputB.View);
	ASSERT(hr == S_OK);
	for (size_t i = 0; i < PP_BUFFER_COUNT; ++i)
	{
		hr = device->CreateRenderTargetView(_postprocessBuffers[i].Texture, nullptr, &_postprocessBuffers[i].View);
		ASSERT(hr == S_OK);
	}

	hr = device->CreateSamplerState(&descSampler, &_color.Sampler);
	ASSERT(hr == S_OK);
	hr = device->CreateSamplerState(&descSampler, &_normal.Sampler);
	ASSERT(hr == S_OK);
	hr = device->CreateSamplerState(&descSampler, &_outputA.Sampler);
	ASSERT(hr == S_OK);
	hr = device->CreateSamplerState(&descSampler, &_outputB.Sampler);
	ASSERT(hr == S_OK);
	hr = device->CreateSamplerState(&descSampler, &_depth.Sampler);
	ASSERT(hr == S_OK);
	for (size_t i = 0; i < PP_BUFFER_COUNT; ++i)
	{
		hr = device->CreateSamplerState(&descSamplerBuffers, &_postprocessBuffers[i].Sampler);
		ASSERT(hr == S_OK);
	}

	hr = device->CreateShaderResourceView(_color.Texture, &descSrvMipmapped, &_color.SRV);
	ASSERT(hr == S_OK);
	hr = device->CreateShaderResourceView(_normal.Texture, &descSrvNormals, &_normal.SRV);
	ASSERT(hr == S_OK);
	hr = device->CreateShaderResourceView(_outputA.Texture, &descSrv, &_outputA.SRV);
	ASSERT(hr == S_OK);
	hr = device->CreateShaderResourceView(_outputB.Texture, &descSrv, &_outputB.SRV);
	ASSERT(hr == S_OK);
	for (size_t i = 0; i < PP_BUFFER_COUNT; ++i)
	{
		hr = device->CreateShaderResourceView(_postprocessBuffers[i].Texture, &descSrv, &_postprocessBuffers[i].SRV);
		ASSERT(hr == S_OK);
	}
	descSrv.Format = formatDepthSrv;
	hr = device->CreateShaderResourceView(_depth.Texture, &descSrv, &_depth.SRV);
	ASSERT(hr == S_OK);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = formatDepthDsv;
	dsvDesc.ViewDimension = dsvDimension;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(_depth.Texture, &dsvDesc, &_depth.DepthStencilView);
	ASSERT(hr == S_OK);

	D3D11_BLEND_DESC blendDesc;
	D3D11_RENDER_TARGET_BLEND_DESC rbDesc;

	rbDesc.BlendEnable = true;
	rbDesc.SrcBlend = D3D11_BLEND_ONE;
	rbDesc.DestBlend = D3D11_BLEND_ONE;
	rbDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rbDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	rbDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	rbDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rbDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	for (int i = 0; i < 8; ++i)
		blendDesc.RenderTarget[i] = rbDesc;

	device->CreateBlendState(&blendDesc, &_additiveBlendState);
	ASSERT(_additiveBlendState != nullptr);


	_shaderDraw = System::GetInstance()->GetScene()->LoadShader(std::wstring(SHADER_DRAW));
	_shaderMerge = System::GetInstance()->GetScene()->LoadShader(std::wstring(SHADER_MERGE));
	_shaderLightAmbient = System::GetInstance()->GetScene()->LoadShader(std::wstring(SHADER_L_AMBIENT));
	_shaderLightDirectional = System::GetInstance()->GetScene()->LoadShader(std::wstring(SHADER_L_DIRECTIONAL));
	_shaderLightPoint = System::GetInstance()->GetScene()->LoadShader(std::wstring(SHADER_L_POINT));


	XMFLOAT3A fpVertices[4];
	XMFLOAT2A fpUvs[4];
	uint16_t fpIndices[6];
	fpVertices[0] = (XMFLOAT3A(-1.0f, -1.0f, 0.0f));
	fpVertices[1] = (XMFLOAT3A(1.0f, -1.0f, 0.0f));
	fpVertices[2] = (XMFLOAT3A(1.0f, 1.0f, 0.0f));
	fpVertices[3] = (XMFLOAT3A(-1.0f, 1.0f, 0.0f));
	fpUvs[0] = (XMFLOAT2A(0.0f, 1.0f));
	fpUvs[1] = (XMFLOAT2A(1.0f, 1.0f));
	fpUvs[2] = (XMFLOAT2A(1.0f, 0.0f));
	fpUvs[3] = (XMFLOAT2A(0.0f, 0.0f));
	fpIndices[0] = 0;
	fpIndices[1] = 1;
	fpIndices[2] = 2;
	fpIndices[3] = 2;
	fpIndices[4] = 3;
	fpIndices[5] = 0;
	
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = static_cast<uint32_t>(sizeof(XMFLOAT3A) * 4);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &fpVertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
}


GBuffer::~GBuffer()
{
	_color.Shutdown();
	_normal.Shutdown();
	_depth.Shutdown();
	_outputA.Shutdown();
	_outputB.Shutdown();
	for (size_t i = 0; i < PP_BUFFER_COUNT; ++i)
		_postprocessBuffers[i].Shutdown();
}

void GBuffer::SetDrawMeshes()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	
	ID3D11RenderTargetView* rtPtrs[2] = { _color.View, _normal.View };
	context->OMSetRenderTargets(2, rtPtrs, _depth.DepthStencilView);

	context->ClearRenderTargetView(_color.View, Renderer::GetInstance()->DEFAULT_CLEAR_COLOR);
	context->ClearRenderTargetView(_normal.View, Renderer::GetInstance()->DEFAULT_CLEAR_COLOR);
	context->ClearRenderTargetView(_outputA.View, Renderer::GetInstance()->DEFAULT_CLEAR_COLOR);
	context->ClearRenderTargetView(_outputB.View, Renderer::GetInstance()->DEFAULT_CLEAR_COLOR);
	for (size_t i = 0; i < PP_BUFFER_COUNT; ++i)
		context->ClearRenderTargetView(_postprocessBuffers[i].View, Renderer::GetInstance()->DEFAULT_CLEAR_COLOR);
	context->ClearDepthStencilView(_depth.DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
		Renderer::GetInstance()->DEFAULT_CLEAR_DEPTH, Renderer::GetInstance()->DEFAULT_CLEAR_STENCIL);
}

void GBuffer::SetDrawLights()
{
	// cleanup after mesh drawing

	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	ID3D11RenderTargetView* nullPtrs[3] = { nullptr, nullptr, nullptr };
	context->OMSetRenderTargets(3, nullPtrs, nullptr);

	// set blend state to ADDITIVE here
	context->OMSetBlendState(_additiveBlendState, nullptr, 0xFFFFFFFF);

	// nullify vertex and index buffers
	const uintptr_t null = 0;
	context->IASetVertexBuffers(0, 1, reinterpret_cast<ID3D11Buffer *const *>(&null), reinterpret_cast<const UINT *>(&null), reinterpret_cast<const UINT *>(&null));
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
}

void GBuffer::SetDrawLightAmbient()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	_shaderLightAmbient->Set();
	SetMapData();
	context->OMSetRenderTargets(1, &_outputA.View, nullptr);
}

void GBuffer::SetDrawLightDirectional()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	_shaderLightDirectional->Set();
	SetMapData();
	Shader::LightCommonDataPS* cd = reinterpret_cast<Shader::LightCommonDataPS*>(_shaderLightDirectional->MapPsBuffer(0));
	XMMATRIX pi = XMLoadFloat4x4A(&_camera.GetMatProjInverse());
	XMStoreFloat4x4A(&cd->gProjInverse, XMMatrixTranspose(pi));
	_shaderLightDirectional->UnmapPsBuffer(0);
	context->OMSetRenderTargets(1, &_outputA.View, nullptr);
}

void GBuffer::SetDrawLightPoint()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	_shaderLightPoint->Set();
	SetMapData();
	Shader::LightCommonDataPS* cd = reinterpret_cast<Shader::LightCommonDataPS*>(_shaderLightPoint->MapPsBuffer(0));
	XMMATRIX pi = XMLoadFloat4x4A(&_camera.GetMatProjInverse());
	XMStoreFloat4x4A(&cd->gProjInverse, XMMatrixTranspose(pi));
	_shaderLightPoint->UnmapPsBuffer(0);
	context->OMSetRenderTargets(1, &_outputA.View, nullptr);
}

void GBuffer::DrawLightAmbient(const Lights::LightAmbient & lightAmbient)
{
	// assuming correct shader is set
	LightAmbient* la = reinterpret_cast<LightAmbient*>(_shaderLightAmbient->MapPsBuffer(0));
	la->ApplyToShader(lightAmbient, _camera);
	_shaderLightAmbient->UnmapPsBuffer(0);
	DrawFullscreenPlane();
}

void GBuffer::DrawLightDirectional(const Lights::LightDirectional & lightDirectional)
{
	// assuming correct shader is set
	LightDirectional* ld = reinterpret_cast<LightDirectional*>(_shaderLightDirectional->MapPsBuffer(1));
	ld->ApplyToShader(lightDirectional, _camera);
	_shaderLightDirectional->UnmapPsBuffer(1);
	DrawFullscreenPlane();
}

void GBuffer::DrawLightPoint(const Lights::LightPoint & lightPoint)
{
	// assuming correct shader is set
	LightPoint* lp = reinterpret_cast<LightPoint*>(_shaderLightPoint->MapPsBuffer(1));
	lp->ApplyToShader(lightPoint, _camera);
	_shaderLightPoint->UnmapPsBuffer(1);
	DrawFullscreenPlane();
}

void GBuffer::SetDrawPostprocesses()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	const uintptr_t null = 0;
	context->OMSetRenderTargets(1, reinterpret_cast<ID3D11RenderTargetView* const*>(&null), nullptr);

	const RenderTarget* rt[1] = { &_outputA };
	const int32_t rts[1] = { 3 };
	PPSetBuffersAsInput(rt, rts, 1);

	// generate mipmaps for normals, depth and color
	_normal.GenerateMipmaps();
	_color.GenerateMipmaps();

	// return blend state to opaque
	Renderer::GetInstance()->SetMainBlendState();
}

void GBuffer::DrawPostprocess(const Postprocesses::Postprocess & pp)
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();

	for (int i = 0; i < pp.GetPassCount(); ++i)
	{
		if (i == pp.GetPassCount() - 1)
		{
			FlipOutputs();
		}

		// Buffers are set up in postprocess itself.
		pp.SetPass(*this, _camera, i);

		DrawFullscreenPlane();

		pp.AfterPass(*this, _camera, i);
	}
}

void GBuffer::PPSetBuffersAsInput(const RenderTarget ** bufferPtrArray, const int32_t slotArray[], size_t bufferCount)
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	for (size_t i = 0; i < bufferCount; ++i)
	{
		context->PSSetShaderResources(slotArray[i], 1, &bufferPtrArray[i]->SRV);
		context->PSSetSamplers(slotArray[i], 1, &bufferPtrArray[i]->Sampler);
	}
}

void GBuffer::PPSetBuffersAsOutput(const RenderTarget ** bufferPtrArray, uint32_t bufferCount, const RenderTargetDepthBuffer* depthBufferPtr)
{
	const size_t maxBufferCount(16);
	ASSERT(bufferCount <= maxBufferCount);
	ID3D11RenderTargetView* rtvs[maxBufferCount];
	for (size_t i = 0; i < bufferCount; ++i)
	{
		rtvs[i] = bufferPtrArray[i]->View;
	}

	ID3D11DepthStencilView* dsv(depthBufferPtr != nullptr ? depthBufferPtr->DepthStencilView : nullptr);

	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	context->OMSetRenderTargets(bufferCount, rtvs, dsv);
}

void GBuffer::PPClearBuffersAsInput(const int32_t * slotArray, size_t bufferCount)
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	const size_t zero = 0;
	for (size_t i = 0; i < bufferCount; ++i)
	{
		context->PSSetShaderResources(slotArray[i], 1, reinterpret_cast<ID3D11ShaderResourceView*const*>(&zero));
		context->PSSetSamplers(slotArray[i], 1, reinterpret_cast<ID3D11SamplerState*const*>(&zero));
	}
}

void GBuffer::PPClearBuffersAsOutput(uint32_t bufferCount)
{
	const size_t maxBufferCount(16);
	ASSERT(bufferCount <= maxBufferCount);
	ID3D11RenderTargetView* rtvs[maxBufferCount];
	for (size_t i = 0; i < bufferCount; ++i)
	{
		rtvs[i] = nullptr;
	}
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	context->OMSetRenderTargets(bufferCount, rtvs, nullptr);
}

void GBuffer::SetDrawTexts()
{
	const int32_t sa[1] = { 3 };
	PPClearBuffersAsInput(sa, 1);
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	context->OMSetRenderTargets(1, &_outputA.View, nullptr);
}

void GBuffer::EndFrame()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	UnsetMapData();
}

void GBuffer::Draw() const
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();

	Renderer::GetInstance()->CopyRenderTargetToMain(_outputA.Texture);
}

void GBuffer::Merge(const GBuffer & other)
{
	FlipOutputs();
}

void GBuffer::FlipOutputs()
{
	RenderTarget vmp = _outputA;
	_outputA = _outputB;
	_outputB = vmp;
}

inline void GBuffer::SetMapData()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	const int32_t numViews = 3;
	ID3D11ShaderResourceView* srv[numViews] = { _color.SRV, _normal.SRV, _depth.SRV };
	ID3D11SamplerState* ss[numViews] = { _color.Sampler, _normal.Sampler, _depth.Sampler };
	context->PSSetShaderResources(0, numViews, srv);
	context->PSSetSamplers(0, numViews, ss);
}

inline void GBuffer::UnsetMapData()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	const int32_t numViews = 4;
	ID3D11ShaderResourceView* const nullSrv[numViews] = { nullptr, nullptr, nullptr, nullptr };
	ID3D11SamplerState* const nullSmp[numViews] = { nullptr, nullptr, nullptr, nullptr };

	context->PSSetShaderResources(0, numViews, nullSrv);
	context->PSSetSamplers(0, numViews, nullSmp);
}

inline void GBuffer::DrawFullscreenPlane()
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	context->Draw(3, 0);
}

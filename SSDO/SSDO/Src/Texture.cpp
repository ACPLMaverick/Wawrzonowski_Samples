#include "stdafx.h"
#include "Texture.h"
#include "Renderer.h"

Texture::Texture()
{
}

Texture::Texture(const std::string & fileName, bool bMakeReadOnly)
{
	ASSERT_D(false, L"Not implemented.");
}

void Texture::InitResources(bool bMakeReadOnly, bool bIsRenderTarget)
{
	Shutdown();

	ID3D11Device* device = Renderer::GetInstance()->GetDevice();
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();

	DWZ(D3D11_TEXTURE2D_DESC, desc);
	GenerateTextureResourceDesc(desc, bIsRenderTarget);

	device->CreateTexture2D(&desc, nullptr, &_fTexture);
	ASSERT_D(_fTexture != nullptr, L"GraphicsDevice: An error occured while creating texture 2D resource.");

	if(_rawData.IsAllocated())
		deviceContext->UpdateSubresource(_fTexture, 0, NULL, _rawData.GetDataPtr(), _width * (_bpp / 8), 0);

	DWZ(D3D11_SAMPLER_DESC, sDesc);
	GenerateSamplerDesc(sDesc);
	device->CreateSamplerState(&sDesc, &_fSampler);
	ASSERT_D(_fSampler != nullptr, L"GraphicsDevice: An error occured while creating sampler state for 2D texture.");

	DWZ(D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc);
	GenerateSRVDesc(srvDesc, desc);
	device->CreateShaderResourceView(_fTexture, &srvDesc, &_fSRV);
	ASSERT_D(_fSRV != nullptr, L"GraphicsDevice: An error occured while creating shader resource view for 2D texture.");

	if (_bMipmapped)
	{
		ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
		deviceContext->GenerateMips(_fSRV);
	}

	if (bMakeReadOnly)
	{
		MakeReadOnly();
	}

	_bIsRenderTarget = bIsRenderTarget;
	if (bIsRenderTarget)
	{
		device->CreateRenderTargetView(_fTexture, nullptr, &_fRTV);
		ASSERT(_fRTV != nullptr);
	}
}

void Texture::Set(int32_t slot)
{
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();

	deviceContext->PSSetShaderResources(slot, 1, &_fSRV);
	deviceContext->PSSetSamplers(slot, 1, &_fSampler);
}

void Texture::SetAsRenderTarget(int32_t slot)
{
	ASSERT(_bIsRenderTarget);

	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
	deviceContext->OMSetRenderTargets(1, &_fRTV, nullptr);
}

void Texture::Shutdown()
{
	if (_fSRV == nullptr)
	{
		return;
	}

	if (_fRTV != nullptr)
	{
		_fRTV->Release();
		_fRTV = nullptr;
	}

	_fSRV->Release();
	_fSRV = nullptr;

	_fSampler->Release();
	_fSampler = nullptr;

	_fTexture->Release();
	_fTexture = nullptr;
}

void Texture::GenerateTextureResourceDesc(D3D11_TEXTURE2D_DESC & desc, bool bIsRenderTarget)
{
	uint8_t realMipmapCount = _bMipmapped ? 0 : 1;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | ((_bMipmapped || bIsRenderTarget) ? D3D11_BIND_RENDER_TARGET : 0);
	desc.CPUAccessFlags = 0;
	desc.Format = _format;
	desc.MipLevels = realMipmapCount;
	desc.MiscFlags = _bMipmapped ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Width = _width;
	desc.Height = _height == 0 ? _width : _height;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
}

void Texture::GenerateSamplerDesc(D3D11_SAMPLER_DESC & sDesc)
{
	sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sDesc.MipLODBias = 0.0f;
	sDesc.MaxAnisotropy = 16;
	sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sDesc.BorderColor[0] = 0;
	sDesc.BorderColor[1] = 0;
	sDesc.BorderColor[2] = 0;
	sDesc.BorderColor[3] = 0;
	sDesc.MinLOD = 0;
	sDesc.MaxLOD = D3D11_FLOAT32_MAX;
}

void Texture::GenerateSRVDesc(D3D11_SHADER_RESOURCE_VIEW_DESC & srvDesc, D3D11_TEXTURE2D_DESC& infoDesc)
{
	srvDesc.Format = infoDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = _bMipmapped ? -1 : 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
}

void Texture::GenerateRTVDesc(D3D11_RENDER_TARGET_VIEW_DESC & desc)
{
}

void Texture::CreateMipmaps()
{
	if (!_bMipmapped)
		_bMipmapped = true;

	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
	deviceContext->GenerateMips(_fSRV);
}

void Texture::ClearTextureSlot(int32_t slot)
{
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
	deviceContext->PSSetShaderResources(slot, 1, nullptr);
}
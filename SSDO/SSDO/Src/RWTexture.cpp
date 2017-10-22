#include "stdafx.h"
#include "RWTexture.h"
#include "Renderer.h"

void RWTexture::InitResources(bool bMakeReadOnly, bool bIsRenderTarget)
{
	Texture::InitResources(bMakeReadOnly, bIsRenderTarget);
	DWZ(D3D11_UNORDERED_ACCESS_VIEW_DESC, uavDesc);
	GenerateUAVDesc(uavDesc);
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();
	device->CreateUnorderedAccessView(_fTexture, &uavDesc, &_fUAV);
	ASSERT(_fUAV != nullptr);

	if (!bMakeReadOnly)
	{
		DWZ(D3D11_TEXTURE2D_DESC, arDesc);
		GenerateTextureResourceDesc(arDesc, false);
		arDesc.BindFlags = 0;
		arDesc.MiscFlags = 0;
		arDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		arDesc.Usage = D3D11_USAGE_STAGING;
		
		device->CreateTexture2D(&arDesc, nullptr, &_fAccessResource);
		ASSERT(_fAccessResource != nullptr);
	}
}

void RWTexture::SetCS(int32_t slot)
{
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();

	deviceContext->CSSetShaderResources(slot, 1, &_fSRV);
	deviceContext->CSSetSamplers(slot, 1, &_fSampler);
}

void RWTexture::AcquireDeviceData()
{
	ASSERT(_fAccessResource != nullptr);

	const size_t dataSize(GetDataSizeBytes());
	if (!_rawData.IsAllocated() || _rawData.GetSizeBytes() != dataSize)
	{
		_rawData.Destroy();
		_rawData.Allocate(dataSize);
		_rawData.Resize(dataSize);
	}
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
	deviceContext->CopyResource(_fAccessResource, _fTexture);
	DWZ(D3D11_MAPPED_SUBRESOURCE, res);
	deviceContext->Map(_fAccessResource, 0, D3D11_MAP_READ, 0, &res);
	ASSERT(res.pData != nullptr);

	// Why each row of the staging texture data has this weird offset of *dataSize*? Hell if I know.
	const size_t rowSize(_width * _bpp / 8);
	const size_t rows(GetHeight());
	uint8_t* ptr = reinterpret_cast<uint8_t*>(res.pData);
	for (size_t i = 0; i < rows; ++i)
	{
		uint8_t* dest = _rawData.GetDataPtr() + i * rowSize;
		memcpy(dest, ptr, rowSize);
		ptr += res.RowPitch;
	}

	deviceContext->Unmap(_fAccessResource, 0);
}

void RWTexture::Shutdown()
{
	if (_fUAV != nullptr)
	{
		_fUAV->Release();
		delete _fUAV;
		_fUAV = nullptr;
	}
}

void RWTexture::GenerateTextureResourceDesc(D3D11_TEXTURE2D_DESC & desc, bool bIsRenderTarget)
{
	Texture::GenerateTextureResourceDesc(desc, bIsRenderTarget);
	desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
}

void RWTexture::GenerateUAVDesc(D3D11_UNORDERED_ACCESS_VIEW_DESC & desc)
{
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = GetDataSizeBytes();
	desc.Format = _format;
}

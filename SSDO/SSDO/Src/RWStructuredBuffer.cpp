#include "stdafx.h"
#include "RWStructuredBuffer.h"
#include "Renderer.h"

RWStructuredBuffer::RWStructuredBuffer(uint32_t elementCount, uint32_t oneElementSize)
{
	ID3D11Device* device(Renderer::GetInstance()->GetDevice());
	DWZ(D3D11_BUFFER_DESC, bDesc);
	bDesc.ByteWidth = elementCount * oneElementSize;
	bDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bDesc.StructureByteStride = oneElementSize;

	device->CreateBuffer(&bDesc, nullptr, &_buffer);
	ASSERT(_buffer != nullptr);

	DWZ(D3D11_UNORDERED_ACCESS_VIEW_DESC, uavDesc);
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = elementCount;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;

	device->CreateUnorderedAccessView(_buffer, &uavDesc, &_uav);
	ASSERT(_uav != nullptr);

	DWZ(D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc);
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.NumElements = elementCount;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;

	device->CreateShaderResourceView(_buffer, &srvDesc, &_srv);
	ASSERT(_srv != nullptr);


	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	device->CreateBuffer(&bDesc, nullptr, &_bufferRead);
	ASSERT(_bufferRead != nullptr);

	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	device->CreateBuffer(&bDesc, nullptr, &_bufferWrite);
	ASSERT(_bufferWrite != nullptr);
}


RWStructuredBuffer::~RWStructuredBuffer()
{
	_buffer->Release();
	delete _buffer;
	_bufferRead->Release();
	delete _bufferRead;
	_bufferWrite->Release();
	delete _bufferWrite;
	_srv->Release();
	delete _srv;
	_uav->Release();
	delete _uav;
}

void RWStructuredBuffer::Set(uint32_t slot) const
{
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
	deviceContext->CSSetUnorderedAccessViews(slot, 1, &_uav, nullptr);
}

void* RWStructuredBuffer::MapRead()
{
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
	deviceContext->CopyResource(_bufferRead, _buffer);

	D3D11_MAPPED_SUBRESOURCE sr;
	deviceContext->Map(_bufferRead, 0, D3D11_MAP_READ, 0, &sr);
	return sr.pData;
}

void* RWStructuredBuffer::MapWrite()
{
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();

	D3D11_MAPPED_SUBRESOURCE sr;
	deviceContext->Map(_bufferWrite, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
	return sr.pData;
}

void RWStructuredBuffer::UnmapRead()
{
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
	deviceContext->Unmap(_bufferRead, 0);
}

void RWStructuredBuffer::UnmapWrite()
{
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
	deviceContext->CopyResource(_buffer, _bufferWrite);
	deviceContext->Unmap(_bufferWrite, 0);
}
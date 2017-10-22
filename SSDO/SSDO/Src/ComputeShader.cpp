#include "stdafx.h"
#include "ComputeShader.h"
#include "Renderer.h"

ComputeShader::ComputeShader(const std::wstring& name, uint32_t constantBufferSize,
							uint32_t threadsInGroupX, uint32_t threadsInGroupY, uint32_t threadsInGroupZ) :
	_shader(nullptr),
	_cBuffer(nullptr),
	_threadsInGroupX(threadsInGroupX),
	_threadsInGroupY(threadsInGroupY),
	_threadsInGroupZ(threadsInGroupZ)
{
	uint8_t* vecCS;
	size_t sizeCS;

	ShaderBytecodeLoader::LoadBytecodeCS(name, &vecCS, sizeCS);

	ID3D11Device* device = Renderer::GetInstance()->GetDevice();
	device->CreateComputeShader(vecCS, sizeCS, nullptr, &_shader);
	ASSERT(_shader != nullptr);

	delete[] vecCS;

	
	DWZ(D3D11_BUFFER_DESC, bDesc);
	bDesc.Usage = D3D11_USAGE_DYNAMIC;
	bDesc.ByteWidth = constantBufferSize;
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&bDesc, nullptr, &_cBuffer);
	ASSERT(_cBuffer != nullptr);
}

ComputeShader::~ComputeShader()
{
	if (_shader != nullptr)
	{
		_shader->Release();
		delete _shader;
		_shader = nullptr;

		_cBuffer->Release();
		delete _cBuffer;
		_cBuffer = nullptr;
	}
}

void ComputeShader::Set(uint32_t constantBufferSlot)
{
	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();
	context->CSSetShader(_shader, nullptr, 0);
	context->CSSetConstantBuffers(constantBufferSlot, 1, &_cBuffer);
}

void ComputeShader::Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ)
{
	ASSERT(threadGroupCountX <= D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION);
	ASSERT(threadGroupCountY <= D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION);
	ASSERT(threadGroupCountZ <= D3D11_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION);

	ID3D11DeviceContext* context = Renderer::GetInstance()->GetDeviceContext();

	context->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

ComputeShader * ComputeShader::CreateResource(const std::wstring & name)
{
	if (name == L"SAT")
	{
		return new ComputeShader(name, sizeof(SATBuffer), 256, 1, 1);
	}
	else if (name == L"AdaptiveLayers")
	{
		return new ComputeShader(name, sizeof(AdaptiveLayerBuffer), 256, 1, 1);
	}
	else if (name == L"DepthAwareBlur")
	{
		return new ComputeShader(name, sizeof(DepthAwareBlurBuffer), 512, 1, 1);
	}
	else
	{
		ASSERT(false);
		return nullptr;
	}
}

void ComputeShader::Clear()
{
}

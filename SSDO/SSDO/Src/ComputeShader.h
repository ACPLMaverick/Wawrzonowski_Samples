#pragma once

#include "GlobalDefines.h"
#include "Shader.h"
#include "Resource.h"
#include "Renderer.h"

class ComputeShader
{
public:

#pragma region Buffer Structs Public

	__declspec(align(16))
	struct SATBuffer
	{
		uint32_t WidthWidthpowLevel[3];
		bool Vertical;
	};

	__declspec(align(16))
		struct AdaptiveLayerBuffer
	{
		XMFLOAT4 SatDimensionsAndRecs;
		float BoxHalfSize;
		uint32_t InputLevel;
		uint32_t IndexOffset;
		bool InputSATIsDifferential;
	};

	__declspec(align(16))
		struct DepthAwareBlurBuffer
	{
		uint32_t Width;
		uint32_t Level;
		float FilterHalfSize;
		float FilterSampleSpacing;
		bool Vertical;
	};

#pragma endregion

protected:

#pragma region Protected

	ID3D11ComputeShader* _shader;
	ID3D11Buffer* _cBuffer;
	uint32_t _threadsInGroupX;
	uint32_t _threadsInGroupY;
	uint32_t _threadsInGroupZ;

#pragma endregion

public:

#pragma region Functions Public

	ComputeShader(const std::wstring& name, uint32_t constantBufferSize, 
		uint32_t threadsInGroupX, uint32_t threadsInGroupY, uint32_t threadsInGroupZ);
	~ComputeShader();

	void Set(uint32_t constantBufferSlot);
	void Dispatch(uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCountZ);

	inline void* MapConstantBuffer()
	{
		ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
		D3D11_MAPPED_SUBRESOURCE sr;
		deviceContext->Map(_cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
		return sr.pData;
	}
	template <typename T> inline T* MapConstantBuffer()
	{
		return reinterpret_cast<T*>(MapConstantBuffer());
	}
	inline void UnmapConstantBuffer()
	{
		ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();
		deviceContext->Unmap(_cBuffer, 0);
	}

	inline ID3D11ComputeShader* GetResource() { return _shader; }
	inline uint32_t GetThreadCountInGroupX() { return _threadsInGroupX; }
	inline uint32_t GetThreadCountInGroupY() { return _threadsInGroupY; }
	inline uint32_t GetThreadCountInGroupZ() { return _threadsInGroupZ; }

	static ComputeShader* CreateResource(const std::wstring& name);
	static void Clear();

#pragma endregion
};


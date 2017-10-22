#pragma once

#include "GlobalDefines.h"

class RWStructuredBuffer
{
protected:

	ID3D11Buffer* _buffer = nullptr;
	ID3D11UnorderedAccessView* _uav = nullptr;
	ID3D11ShaderResourceView* _srv = nullptr;

	ID3D11Buffer* _bufferRead = nullptr;
	ID3D11Buffer* _bufferWrite = nullptr;

public:
	RWStructuredBuffer(uint32_t elementCount, uint32_t oneElementSize);
	~RWStructuredBuffer();

	void Set(uint32_t slot) const;

	void* MapRead();
	void* MapWrite();
	template <typename T> inline T* MapRead() { return reinterpret_cast<T*>(MapRead()); }
	template <typename T> inline T* MapWrite() { return reinterpret_cast<T*>(MapWrite()); }
	void UnmapRead();
	void UnmapWrite();

	inline ID3D11Buffer* GetBuffer() { return _buffer; }
	inline ID3D11UnorderedAccessView* GetUAV() { return _uav; }
	inline ID3D11ShaderResourceView* GetSRV() { return _srv; }
};


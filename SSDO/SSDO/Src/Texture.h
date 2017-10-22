#pragma once

#include "GlobalDefines.h"
#include "Buffer.h"

class Texture
{
protected:

	Buffer<uint8_t> _rawData;

	int32_t _width = 0;
	int32_t _height = 0;
	int32_t _bpp = 32;
	DXGI_FORMAT _format = DXGI_FORMAT_R8G8B8A8_UINT;
	bool _bMipmapped = false;
	bool _bReadOnly = false;
	bool _bIsRenderTarget = false;

	ID3D11Texture2D* _fTexture = nullptr;
	ID3D11SamplerState* _fSampler = nullptr;
	ID3D11ShaderResourceView* _fSRV = nullptr;
	ID3D11RenderTargetView* _fRTV = nullptr;

	virtual void Shutdown();
	virtual void GenerateTextureResourceDesc(D3D11_TEXTURE2D_DESC& desc, bool bIsRenderTarget);
	virtual void GenerateSamplerDesc(D3D11_SAMPLER_DESC& desc);
	virtual void GenerateSRVDesc(D3D11_SHADER_RESOURCE_VIEW_DESC& desc, D3D11_TEXTURE2D_DESC& infoDesc);
	virtual void GenerateRTVDesc(D3D11_RENDER_TARGET_VIEW_DESC& desc);

public:
	Texture();
	Texture(const std::string& fileName, bool bMakeReadOnly = false);
	inline virtual ~Texture() { Shutdown(); }

	inline Buffer<uint8_t>& GetRawData() { return _rawData; }

	virtual void InitResources(bool bMakeReadOnly = false, bool bIsRenderTarget = false);

	void Set(int32_t slot);
	void SetAsRenderTarget(int32_t slot);

	inline bool IsReadOnly() { return _bReadOnly; }

	inline void MakeReadOnly()
	{
		if (IsReadOnly())
			return;

		_bReadOnly = true;
		if(_rawData.IsAllocated())
			_rawData.Destroy();
	}

	inline void SetWidth(int32_t width)
	{
		if (IsReadOnly())
			return;

		_width = width;
	}

	inline void SetHeight(int32_t height)
	{
		if (IsReadOnly())
			return;

		_height = height;
	}

	inline void SetBPP(int32_t bpp)
	{
		if (IsReadOnly())
			return;

		_bpp = bpp;
	}

	inline void SetFormat(DXGI_FORMAT format)
	{
		if (IsReadOnly())
			return;

		_format = format;
	}

	inline void SetMipmapped(bool mipmapped)
	{
		if (IsReadOnly())
			return;

		_bMipmapped = mipmapped;
	}

	inline int32_t GetDataSizeBytes() const { return _height != 0 ? _width * _height * _bpp / 8 : _width * _width * _bpp / 8; }
	inline int32_t GetWidth() const { return _width; }
	inline int32_t GetHeight() const { return _height; }
	inline int32_t GetBPP() const { return _bpp; }
	inline DXGI_FORMAT GetFormat() const { return _format; }
	inline bool GetMipmapped() const { return _bMipmapped; }

	inline ID3D11Texture2D* GetResource() const { return _fTexture; }
	inline ID3D11SamplerState* GetSampler() const { return _fSampler; }
	inline ID3D11ShaderResourceView* GetSRV() const { return _fSRV; }
	inline ID3D11RenderTargetView* GetRTV() const { return _fRTV; }

	void CreateMipmaps();

	inline void AllocateRawDataToTextureSize()
	{
		int32_t size(_width * _height * _bpp / 8);
		_rawData.Allocate(size);
		_rawData.Resize(size);
	}


	static void ClearTextureSlot(int32_t slot);
};


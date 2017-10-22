#pragma once
#include "Texture.h"

class RWTexture :
	public Texture
{
protected:

	ID3D11UnorderedAccessView* _fUAV = nullptr;
	ID3D11Texture2D* _fAccessResource = nullptr;

	virtual void Shutdown() override;
	virtual void GenerateTextureResourceDesc(D3D11_TEXTURE2D_DESC& desc, bool bIsRenderTarget) override;
	virtual void GenerateUAVDesc(D3D11_UNORDERED_ACCESS_VIEW_DESC& desc);

public:
	inline RWTexture() : Texture() { }
	inline RWTexture(const std::string& fileName, bool bMakeReadOnly = false) : Texture(fileName, bMakeReadOnly) { }
	inline virtual ~RWTexture() { Shutdown(); }

	virtual void InitResources(bool bMakeReadOnly = false, bool bIsRenderTarget = false);

	void SetCS(int32_t slot);

	void AcquireDeviceData();

	inline ID3D11UnorderedAccessView* GetUAV() const { return _fUAV; }
};


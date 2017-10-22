#pragma once

#include "stdafx.h"
#include "Singleton.h"

#include <d3d11.h>
#include <DirectXMath.h>

class Renderer : public Singleton<Renderer>
{
public:

#pragma region Enum

	enum class RenderMode
	{
		FORWARD,
		DEFERRED
	};

	enum class BlendMode
	{
		SOLID = 0,
		ALPHA
	};

#pragma endregion

	const float DEFAULT_CLEAR_COLOR[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const float DEFAULT_CLEAR_DEPTH = 1.0f;
	const uint8_t DEFAULT_CLEAR_STENCIL = 255;
	const uint32_t DEFAULT_SAMPLE_COUNT = 1;

protected:

#pragma region Protected

	RenderMode _renderMode;
	BlendMode _blendMode;
	uint32_t _sampleQuality = 1;

	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;

	IDXGISwapChain* _swapChain = nullptr;

	ID3D11BlendState* _arrayBlendStates[2];

	ID3D11RasterizerState* _rasterizerState = nullptr;
	ID3D11DepthStencilState* _depthStencilState = nullptr;
	ID3D11BlendState* _blendState = nullptr;

	ID3D11Texture2D* _tMainRenderTarget = nullptr;
	ID3D11RenderTargetView* _vMainRenderTarget = nullptr;
	ID3D11Texture2D* _tDepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* _vDepthStencilBuffer = nullptr;

#pragma endregion

#pragma region Functions Protected

#pragma endregion

public:

#pragma region Functions Public

	Renderer();
	~Renderer();

	void Initialize();
	void Run();
	void Shutdown();

	inline void SetBlendMode(BlendMode bm) 
	{
		if (_blendMode != bm) 
		{ 
			_deviceContext->OMSetBlendState(_arrayBlendStates[static_cast<uint8_t>(bm)], nullptr, 0xFFFFFFFF); 
			_blendMode = bm;
		}
	}
	inline void CopyRenderTargetToMain(ID3D11Texture2D* rt) { _deviceContext->CopyResource(_tMainRenderTarget, rt); }
	inline void SetMainBlendState() { _deviceContext->OMSetBlendState(_blendState, nullptr, 0xFFFFFFFF); }
	inline void SetMainRenderTarget() { _deviceContext->OMSetRenderTargets(1, &_vMainRenderTarget, _vDepthStencilBuffer); }

	inline ID3D11Device* GetDevice() const { return _device; }
	inline ID3D11DeviceContext* GetDeviceContext() const { return _deviceContext; }
	inline RenderMode GetRenderMode() const { return _renderMode; }
	inline BlendMode GetBlendMode() const { return _blendMode; }
	inline uint32_t GetSampleQuality() const { return _sampleQuality; }

#pragma endregion
};


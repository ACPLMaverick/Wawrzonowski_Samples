#include "stdafx.h"
#include "Renderer.h"
#include "System.h"
#include "Scenes\Scene.h"
#include "Camera.h"

Renderer::Renderer() : 
	_renderMode(RenderMode::DEFERRED),
	_blendMode(BlendMode::SOLID)
{
}


Renderer::~Renderer()
{
}

void Renderer::Initialize()
{
	// create device and device context
#ifdef _DEBUG
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &_device, nullptr, &_deviceContext);
#else
	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &_device, nullptr, &_deviceContext);
#endif

	// create swap chain
	IDXGIDevice* dxgiDevice = nullptr;
	_device->QueryInterface(__uuidof(IDXGIDevice), (reinterpret_cast<void**>(&dxgiDevice)));

	ASSERT(dxgiDevice != nullptr);

	IDXGIAdapter* dxgiAdapter = nullptr;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));

	ASSERT(dxgiAdapter != nullptr);

	IDXGIFactory* dxgiFactory = nullptr;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));

	ASSERT(dxgiFactory != nullptr);

	//_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_sampleQuality);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = System::GetInstance()->GetOptions()._windowWidth;
	swapChainDesc.BufferDesc.Height = System::GetInstance()->GetOptions()._windowHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = System::GetInstance()->GetOptions()._refreshRate;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = DEFAULT_SAMPLE_COUNT;
	swapChainDesc.SampleDesc.Quality = _sampleQuality - 1;
	swapChainDesc.OutputWindow = System::GetInstance()->GetHWND();
	swapChainDesc.Windowed = !System::GetInstance()->GetOptions()._bFullscreen;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	dxgiFactory->CreateSwapChain(_device, &swapChainDesc, &_swapChain);

	ASSERT(_swapChain != nullptr);

	dxgiFactory->Release();
	dxgiAdapter->Release();
	dxgiDevice->Release();

	// create render target view

	_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&_tMainRenderTarget));
	ASSERT(_tMainRenderTarget != nullptr);
	_device->CreateRenderTargetView(_tMainRenderTarget, nullptr, &_vMainRenderTarget);
	ASSERT(_vMainRenderTarget != nullptr);

	// create depth scencil state

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 255;
	depthStencilDesc.StencilWriteMask = 255;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	_device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState);
	ASSERT(_depthStencilState != nullptr);
	_deviceContext->OMSetDepthStencilState(_depthStencilState, 1);

	// create depth and stencil buffer view

	D3D11_TEXTURE2D_DESC depthStencilDescTex;
	depthStencilDescTex.Width = System::GetInstance()->GetOptions()._windowWidth;
	depthStencilDescTex.Height = System::GetInstance()->GetOptions()._windowHeight;
	depthStencilDescTex.MipLevels = 1;
	depthStencilDescTex.ArraySize = 1;
	depthStencilDescTex.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDescTex.SampleDesc.Count = DEFAULT_SAMPLE_COUNT;
	depthStencilDescTex.SampleDesc.Quality = _sampleQuality - 1;
	depthStencilDescTex.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDescTex.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDescTex.CPUAccessFlags = 0;
	depthStencilDescTex.MiscFlags = 0;

	_device->CreateTexture2D(&depthStencilDescTex, nullptr, &_tDepthStencilBuffer);
	ASSERT(_tDepthStencilBuffer != nullptr);
	_device->CreateDepthStencilView(_tDepthStencilBuffer, nullptr, &_vDepthStencilBuffer);
	ASSERT(_vDepthStencilBuffer != nullptr);

	// set pipeline's output merger state
	
	_deviceContext->OMSetRenderTargets(1, &_vMainRenderTarget, _vDepthStencilBuffer);

	// create multiple blend states

	D3D11_BLEND_DESC blendDesc;
	D3D11_RENDER_TARGET_BLEND_DESC rbDesc;

	rbDesc.BlendEnable = false;
	rbDesc.SrcBlend = D3D11_BLEND_ONE;
	rbDesc.DestBlend = D3D11_BLEND_ZERO;
	rbDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rbDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	rbDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	rbDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rbDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	for(int i = 0; i < 8; ++i)
		blendDesc.RenderTarget[i] = rbDesc;

	_device->CreateBlendState(&blendDesc, &_arrayBlendStates[0]);
	ASSERT(_arrayBlendStates[0] != nullptr);

	rbDesc.BlendEnable = true;
	rbDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rbDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	for (int i = 0; i < 8; ++i)
		blendDesc.RenderTarget[i] = rbDesc;

	_device->CreateBlendState(&blendDesc, &_arrayBlendStates[1]);
	ASSERT(_arrayBlendStates[1] != nullptr);

	_blendState = _arrayBlendStates[0];
	_deviceContext->OMSetBlendState(_blendState, nullptr, 0xFFFFFFFF);

	// create rasterizer state

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.AntialiasedLineEnable = true;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.MultisampleEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = true;

	_device->CreateRasterizerState(&rasterizerDesc, &_rasterizerState);
	ASSERT(_rasterizerState != nullptr);
	_deviceContext->RSSetState(_rasterizerState);

	// set viewport (default 100%)

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(System::GetInstance()->GetOptions()._windowWidth);
	viewport.Height = static_cast<float>(System::GetInstance()->GetOptions()._windowHeight);
	viewport.TopLeftX = viewport.TopLeftY = viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	_deviceContext->RSSetViewports(1, &viewport);

	// set other global things

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::Run()
{
	_deviceContext->ClearRenderTargetView(_vMainRenderTarget, DEFAULT_CLEAR_COLOR);
	_deviceContext->ClearDepthStencilView(_vDepthStencilBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
		DEFAULT_CLEAR_DEPTH, DEFAULT_CLEAR_STENCIL);

	if (System::GetInstance()->GetScene() != nullptr)
	{
		System::GetInstance()->GetScene()->Draw();

		if (_renderMode == RenderMode::DEFERRED)
		{
			System::GetInstance()->GetScene()->GetMainCamera()->GetGBuffer().Draw();
		}
	}
	
	_swapChain->Present(0, 0);
}

void Renderer::Shutdown()
{
	_vMainRenderTarget->Release();
	_vMainRenderTarget = nullptr;

	_tMainRenderTarget->Release();
	_tMainRenderTarget = nullptr;

	_vDepthStencilBuffer->Release();
	_vDepthStencilBuffer = nullptr;

	_tDepthStencilBuffer->Release();
	_tDepthStencilBuffer = nullptr;

	_blendState->Release();
	_blendState = nullptr;

	_depthStencilState->Release();
	_depthStencilState = nullptr;

	_rasterizerState->Release();
	_rasterizerState = nullptr;

	_swapChain->Release();
	_swapChain = nullptr;

	_deviceContext->Release();
	_deviceContext = nullptr;

	_device->Release();
	_device = nullptr;
}
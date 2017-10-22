#include "GraphicsDeviceDX11.h"

#include "../Window.h"
#include "../WindowWin32.h"
#include "../RenderingManager.h"
#include "utility/MMatrix.h"
#include "core/Engine.h"
#include "renderer/RenderTarget2D.h"

namespace morphEngine
{
	using namespace utility;

	namespace renderer
	{
		namespace device
		{
			GraphicsDeviceDX11::GraphicsDeviceDX11() :
				GraphicsDevice()
			{
				_rasterizerStates.Allocate(RASTERIZER_STATES_COUNT);
				_depthStencilStates.Allocate(DEPTHSTENCIL_STATES_COUNT);
				_blendStates.Allocate(BLEND_STATES_COUNT);
			}

			GraphicsDeviceDX11::~GraphicsDeviceDX11()
			{
			}

			void GraphicsDeviceDX11::Initialize()
			{
				// create device and device context
				if (_options._bDebugMode)
				{
					D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &_device, nullptr, &_deviceContext);
				}
				else
				{
					D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &_device, nullptr, &_deviceContext);
				}

				// create swap chain
				IDXGIDevice* dxgiDevice = nullptr;
				_device->QueryInterface(__uuidof(IDXGIDevice), (reinterpret_cast<void**>(&dxgiDevice)));

				ME_ASSERT_S(dxgiDevice != nullptr);

				IDXGIAdapter* dxgiAdapter = nullptr;
				dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));

				ME_ASSERT_S(dxgiAdapter != nullptr);

				IDXGIFactory* dxgiFactory = nullptr;
				dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));

				ME_ASSERT_S(dxgiFactory != nullptr);

				_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, reinterpret_cast<UINT*>(&_sampleQuality));

				if (_options._window == nullptr)
				{
					_options._window = core::Engine::GetInstance()->GetWindow();
				}

				if (_options._window != nullptr)
				{
					_swapChainBufferWidth = _options._window->GetWidth();
					_swapChainBufferHeight = _options._window->GetHeight();
				}
				else
				{
					_swapChainBufferWidth = 640;
					_swapChainBufferHeight = 480;
					ME_WARNING(false, "Initializing GraphicsDeviceDX11 without a window.");
				}



				DXGI_SWAP_CHAIN_DESC swapChainDesc;
				swapChainDesc.BufferDesc.Width = _swapChainBufferWidth;
				swapChainDesc.BufferDesc.Height = _swapChainBufferHeight;
				swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;						// this is TODO TEMPORARY AND SHIT
				swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
				swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				swapChainDesc.BufferCount = 1;												// this is TODO TEMPORARY AND SHIT
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.SampleDesc.Count = _options._multisampleCount;
				swapChainDesc.SampleDesc.Quality = _sampleQuality - 1;
				swapChainDesc.OutputWindow = _options._window != nullptr ? reinterpret_cast<WindowWin32*>(_options._window)->GetHWND() : (HWND)0;
				swapChainDesc.Windowed = _options._window != nullptr ? !_options._window->IsFullscreen() : false;
				swapChainDesc.SwapEffect = static_cast<DXGI_SWAP_EFFECT>(_options._swapEffect);
				swapChainDesc.Flags = 0;

				dxgiFactory->CreateSwapChain(_device, &swapChainDesc, &_swapChain);

				ME_ASSERT_S(_swapChain != nullptr);

				dxgiFactory->Release();
				dxgiAdapter->Release();
				dxgiDevice->Release();

				// create render target view for main render target
				ID3D11Texture2D* rtTex = nullptr;
				_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&rtTex));
				ME_ASSERT_S(rtTex != nullptr);
				RenderTargetResourceDX11* res = new RenderTargetResourceDX11();
				res->Initialize(_swapChainBufferWidth, _swapChainBufferHeight, 4, GraphicDataFormat::UNORM_R8G8B8A8, rtTex, false);
				_mainRenderTarget = CreateRenderTarget(_swapChainBufferWidth, _swapChainBufferHeight, res);
				_mainRenderTarget->Initialize();

				// set pipeline's output merger state

				_mainRenderTarget->SetAsOutput();

				// set viewport (default 100%)

				D3D11_VIEWPORT viewport;

				if (_options._viewport._width != 0.0f || _options._viewport._height != 0.0f || _options._window == nullptr)
				{
					viewport.Width = static_cast<MFloat32>(_options._viewport._width);
					viewport.Height = static_cast<MFloat32>(_options._viewport._height);
				}
				else
				{
					viewport.Width = static_cast<MFloat32>(_options._window->GetWidth());
					viewport.Height = static_cast<MFloat32>(_options._window->GetHeight());
				}

				viewport.TopLeftX = viewport.TopLeftY = viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;

				_currentViewport = _options._viewport;

				_deviceContext->RSSetViewports(1, &viewport);

				// setup state arrays

				SetupStates();

				// create depth stencil state for back buffer

				_currentDepthStencilState = static_cast<DepthStencilState>(0);
				_currentRasterizerState = static_cast<RasterizerState>(0);
				_currentBlendState = static_cast<BlendState>(0);

				_deviceContext->OMSetDepthStencilState(_depthStencilStates[0], 1);

				_deviceContext->RSSetState(_rasterizerStates[0]);
				_deviceContext->OMSetBlendState(_blendStates[0], MColor::Transparent, 0xFFFFFFFF);


				// set scissor rect

				_deviceContext->RSSetScissorRects(1, reinterpret_cast<D3D11_RECT*>(&_options._scissorRect));

				// set other global things

				_deviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(_options._primitiveTopology));
				_syncInterval = _options._bVsync ? 1 : 0;
			}

			void GraphicsDeviceDX11::Initialize(const Options & options)
			{
				_options = options;
				Initialize();
			}

			void GraphicsDeviceDX11::Shutdown()
			{
				DeleteRenderTarget(_mainRenderTarget);

				for (MSize i = 0; i < BLEND_STATES_COUNT; ++i)
				{
					_blendStates[i]->Release();
				}

				for (MSize i = 0; i < DEPTHSTENCIL_STATES_COUNT; ++i)
				{
					_depthStencilStates[i]->Release();
				}

				for (MSize i = 0; i < RASTERIZER_STATES_COUNT; ++i)
				{
					_rasterizerStates[i]->Release();
				}

				_blendState = nullptr;
				_depthStencilState = nullptr;
				_rasterizerState = nullptr;

				_swapChain->Release();
				_swapChain = nullptr;

				_deviceContext->Release();
				_deviceContext = nullptr;

				_device->Release();
				_device = nullptr;
			}

			void GraphicsDeviceDX11::Clear()
			{
				switch (_options._clearMode)
				{
				case ClearMode::CLEAR_COLOR:
				{
					_deviceContext->ClearRenderTargetView(reinterpret_cast<const RenderTargetResourceDX11*>(_currentRenderTarget->GetResource())->_vRenderTarget, _options._clearColor);
					break;
				}
				case ClearMode::CLEAR_COLOR_AND_DEPTH :
				{
					_deviceContext->ClearRenderTargetView(reinterpret_cast<const RenderTargetResourceDX11*>(_currentRenderTarget->GetResource())->_vRenderTarget, _options._clearColor);
					_deviceContext->ClearDepthStencilView(reinterpret_cast<const DepthStencilResourceDX11*>(_currentRenderTarget->GetDepthResource())->_vDepthStencilBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 255);
					break;
				}
				case ClearMode::CLEAR_COLOR_AND_DEPTH_NO_STENCIL : 
				{
					_deviceContext->ClearRenderTargetView(reinterpret_cast<const RenderTargetResourceDX11*>(_currentRenderTarget->GetResource())->_vRenderTarget, _options._clearColor);
					_deviceContext->ClearDepthStencilView(reinterpret_cast<const DepthStencilResourceDX11*>(_currentRenderTarget->GetDepthResource())->_vDepthStencilBuffer, D3D11_CLEAR_DEPTH, 1.0f, 255);
					break;
				}
				case ClearMode::CLEAR_DEPTH : 
				{
					_deviceContext->ClearDepthStencilView(reinterpret_cast<const DepthStencilResourceDX11*>(_currentRenderTarget->GetDepthResource())->_vDepthStencilBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 255);
					break;
				}
				case ClearMode::CLEAR_DEPTH_NO_STENCIL :
				{
					_deviceContext->ClearDepthStencilView(reinterpret_cast<const DepthStencilResourceDX11*>(_currentRenderTarget->GetDepthResource())->_vDepthStencilBuffer, D3D11_CLEAR_DEPTH, 1.0f, 255);
					break;
				}
				default:
				{
					break;
				}
				}
			}

			void GraphicsDeviceDX11::Draw(MUint32 indexCount, MUint32 startIndexLocation, MUint32 startVertexLocation) const
			{
				_deviceContext->DrawIndexed(indexCount, startIndexLocation, startVertexLocation);
			}

			void GraphicsDeviceDX11::Draw(MUint32 vertexCount) const
			{
				_deviceContext->Draw(vertexCount, 0);
			}

			void GraphicsDeviceDX11::Present() const
			{
				_swapChain->Present(_syncInterval, 0);
			}

			void GraphicsDeviceDX11::PushRenderTarget()
			{
				_renderTargets.Push(_currentRenderTarget);
			}

			void GraphicsDeviceDX11::PopRenderTarget()
			{
				if (_renderTargets.GetSize() != 0)
				{
					_currentRenderTarget = _renderTargets.Pop();
				}
				else
				{
					_currentRenderTarget = _mainRenderTarget;
				}
				_currentRenderTarget->SetAsOutput();
			}

			void GraphicsDeviceDX11::PushRasterizerState()
			{
				_stRasterizerStates.Push(_currentRasterizerState);
			}

			void GraphicsDeviceDX11::PopRasterizerState()
			{
				if (!_stRasterizerStates.IsEmpty())
					SetRasterizerState(_stRasterizerStates.Pop());
			}

			void GraphicsDeviceDX11::PushBlendState()
			{
				_stBlendStates.Push(_currentBlendState);
			}

			void GraphicsDeviceDX11::PopBlendState()
			{
				if (!_stBlendStates.IsEmpty())
					SetBlendState(_stBlendStates.Pop());
			}

			void GraphicsDeviceDX11::PushDepthStencilState()
			{
				_stDepthStencilStates.Push(_currentDepthStencilState);
			}

			void GraphicsDeviceDX11::PopDepthStencilState()
			{
				if (!_stDepthStencilStates.IsEmpty())
					SetDepthStencilState(_stDepthStencilStates.Pop());
			}

			void GraphicsDeviceDX11::PushViewport()
			{
				_stViewports.Push(_currentViewport);
			}

			void GraphicsDeviceDX11::PopViewport()
			{
				if (!_stViewports.IsEmpty())
					SetViewport(_stViewports.Pop());
			}

			void GraphicsDeviceDX11::CopyToBackBuffer(const Texture2DResource * tex)
			{
				if (tex != _mainRenderTarget->GetResource())
				{
					_deviceContext->CopyResource(reinterpret_cast<const RenderTargetResourceDX11*>(_mainRenderTarget->GetResource())->_tRenderTarget,
						reinterpret_cast<const Texture2DResourceDX11*>(tex)->_textureResource);
				}
			}

			void GraphicsDeviceDX11::ClearBoundRenderTargets(MUint32 count)
			{
				MFixedArray<ID3D11RenderTargetView*> nullRenderTargets(count);
				ZeroMemory(nullRenderTargets.GetDataPointer(), sizeof(ID3D11RenderTargetView*) * count);
				_deviceContext->OMSetRenderTargets(count, nullRenderTargets.GetDataPointer(), nullptr);
			}

			void GraphicsDeviceDX11::ClearBoundShaderResourcesVS(MUint32 startIndex, MUint32 count)
			{
				MFixedArray<ID3D11ShaderResourceView*> nullRenderTargets(count);
				ZeroMemory(nullRenderTargets.GetDataPointer(), sizeof(ID3D11ShaderResourceView*) * count);
				_deviceContext->VSSetShaderResources(startIndex, count, nullRenderTargets.GetDataPointer());
				_deviceContext->VSSetSamplers(startIndex, count, reinterpret_cast<ID3D11SamplerState* const*>(nullRenderTargets.GetDataPointer()));
			}

			void GraphicsDeviceDX11::ClearBoundShaderResourcesPS(MUint32 startIndex, MUint32 count)
			{
				MFixedArray<ID3D11ShaderResourceView*> nullRenderTargets(count);
				ZeroMemory(nullRenderTargets.GetDataPointer(), sizeof(ID3D11ShaderResourceView*) * count);
				_deviceContext->PSSetShaderResources(startIndex, count, nullRenderTargets.GetDataPointer());
				_deviceContext->PSSetSamplers(startIndex, count, reinterpret_cast<ID3D11SamplerState* const*>(nullRenderTargets.GetDataPointer()));
			}

			void GraphicsDeviceDX11::ClearBoundShaderResourcesGS(MUint32 startIndex, MUint32 count)
			{
				MFixedArray<ID3D11ShaderResourceView*> nullRenderTargets(count);
				ZeroMemory(nullRenderTargets.GetDataPointer(), sizeof(ID3D11ShaderResourceView*) * count);
				_deviceContext->GSSetShaderResources(startIndex, count, nullRenderTargets.GetDataPointer());
				_deviceContext->GSSetSamplers(startIndex, count, reinterpret_cast<ID3D11SamplerState* const*>(nullRenderTargets.GetDataPointer()));
			}

			void GraphicsDeviceDX11::ClearBoundShaderResourcesHS(MUint32 startIndex, MUint32 count)
			{
				MFixedArray<ID3D11ShaderResourceView*> nullRenderTargets(count);
				ZeroMemory(nullRenderTargets.GetDataPointer(), sizeof(ID3D11ShaderResourceView*) * count);
				_deviceContext->HSSetShaderResources(startIndex, count, nullRenderTargets.GetDataPointer());
				_deviceContext->HSSetSamplers(startIndex, count, reinterpret_cast<ID3D11SamplerState* const*>(nullRenderTargets.GetDataPointer()));
			}

			void GraphicsDeviceDX11::ClearBoundShaderResourcesDS(MUint32 startIndex, MUint32 count)
			{
				MFixedArray<ID3D11ShaderResourceView*> nullRenderTargets(count);
				ZeroMemory(nullRenderTargets.GetDataPointer(), sizeof(ID3D11ShaderResourceView*) * count);
				_deviceContext->DSSetShaderResources(startIndex, count, nullRenderTargets.GetDataPointer());
				_deviceContext->DSSetSamplers(startIndex, count, reinterpret_cast<ID3D11SamplerState* const*>(nullRenderTargets.GetDataPointer()));
			}

			void GraphicsDeviceDX11::ClearBoundShaderResourcesCS(MUint32 startIndex, MUint32 count)
			{
				MFixedArray<ID3D11ShaderResourceView*> nullRenderTargets(count);
				ZeroMemory(nullRenderTargets.GetDataPointer(), sizeof(ID3D11ShaderResourceView*) * count);
				_deviceContext->CSSetShaderResources(startIndex, count, nullRenderTargets.GetDataPointer());
				_deviceContext->CSSetSamplers(startIndex, count, reinterpret_cast<ID3D11SamplerState* const*>(nullRenderTargets.GetDataPointer()));
			}

			void GraphicsDeviceDX11::ClearBoundVertexIndexBuffers()
			{
				// nullify vertex and index buffers
				const MUint64 null = 0;
				_deviceContext->IASetVertexBuffers(0, 1, reinterpret_cast<ID3D11Buffer *const *>(&null), reinterpret_cast<const UINT *>(&null), reinterpret_cast<const UINT *>(&null));
				_deviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
			}

			void GraphicsDeviceDX11::ClearBoundShaderVS()
			{
				_deviceContext->VSSetShader(nullptr, nullptr, 0);
			}

			void GraphicsDeviceDX11::ClearBoundShaderPS()
			{
				_deviceContext->PSSetShader(nullptr, nullptr, 0);
			}

			void GraphicsDeviceDX11::ClearBoundShaderGS()
			{
				_deviceContext->GSSetShader(nullptr, nullptr, 0);
			}

			void GraphicsDeviceDX11::ClearBoundShaderHS()
			{
				_deviceContext->HSSetShader(nullptr, nullptr, 0);
			}

			void GraphicsDeviceDX11::ClearBoundShaderDS()
			{
				_deviceContext->DSSetShader(nullptr, nullptr, 0);
			}

			void GraphicsDeviceDX11::ClearBoundShaderCS()
			{
				_deviceContext->CSSetShader(nullptr, nullptr, 0);
			}

			void GraphicsDeviceDX11::GetVertexShader(VertexShaderResource & outResource) const
			{
				_deviceContext->VSGetShader(&reinterpret_cast<VertexShaderResourceDX11&>(outResource)._shaderResource, nullptr, 0);
			}

			void GraphicsDeviceDX11::GetPixelShader(PixelShaderResource & outResource) const
			{
				_deviceContext->PSGetShader(&reinterpret_cast<PixelShaderResourceDX11&>(outResource)._shaderResource, nullptr, 0);
			}

			void GraphicsDeviceDX11::GetHullShader(HullShaderResource & outResource) const
			{
				_deviceContext->HSGetShader(&reinterpret_cast<HullShaderResourceDX11&>(outResource)._shaderResource, nullptr, 0);
			}

			void GraphicsDeviceDX11::GetDomainShader(DomainShaderResource & outResource) const
			{
				_deviceContext->DSGetShader(&reinterpret_cast<DomainShaderResourceDX11&>(outResource)._shaderResource, nullptr, 0);
			}

			void GraphicsDeviceDX11::GetGeometryShader(GeometryShaderResource & outResource) const
			{
				_deviceContext->GSGetShader(&reinterpret_cast<GeometryShaderResourceDX11&>(outResource)._shaderResource, nullptr, 0);
			}

			void GraphicsDeviceDX11::GetComputeShader(ComputeShaderResource & outResource) const
			{
				_deviceContext->CSGetShader(&reinterpret_cast<ComputeShaderResourceDX11&>(outResource)._shaderResource, nullptr, 0);
			}

			/*
			void GraphicsDeviceDX11::GetTextureResources(utility::MArray<Texture2DResource>& outData) const
			{
				_deviceContext->VSGet
			}

			void GraphicsDeviceDX11::GetConstantBufferResources(utility::MArray<ConstantBufferResource>& outData) const
			{
			}

			void GraphicsDeviceDX11::GetVertexBufferResources(utility::MArray<VertexBufferResource>& outData) const
			{
			}

			void GraphicsDeviceDX11::GetIndexBufferResource(IndexBufferResource & outResource) const
			{
			}
			*/

			RenderTarget2D* GraphicsDeviceDX11::GetRenderTarget() const
			{
				return _currentRenderTarget;
			}

			void GraphicsDeviceDX11::SetRenderTarget(RenderTarget2D * rt)
			{
				_currentRenderTarget = rt;
			}

			RenderTarget2D* GraphicsDeviceDX11::GetMainRenderTarget() const
			{
				return _mainRenderTarget;
			}

			void GraphicsDeviceDX11::ResizeMainRenderTarget(MUint16 width, MUint16 height)
			{
				_swapChainBufferWidth = static_cast<MInt32>(width);
				_swapChainBufferHeight = static_cast<MInt32>(height);
				_mainRenderTarget->PrepareForResizingMainOnly();

				_swapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
				ID3D11Texture2D* rtTex = nullptr;
				_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&rtTex));
				ME_ASSERT_S(rtTex != nullptr);
				RenderTargetResourceDX11* res = new RenderTargetResourceDX11();
				res->Initialize(_swapChainBufferWidth, _swapChainBufferHeight, 4, GraphicDataFormat::UNORM_R8G8B8A8, rtTex, false);

				_mainRenderTarget->SetNewResourceAndRecreateDepth(res, _swapChainBufferWidth, _swapChainBufferHeight);

				if (_currentRenderTarget == _mainRenderTarget)
				{
					_mainRenderTarget->SetAsOutput();
				}

				D3D11_VIEWPORT viewport;

				viewport.Width = static_cast<MFloat32>(_swapChainBufferWidth);
				viewport.Height = static_cast<MFloat32>(_swapChainBufferHeight);
				viewport.TopLeftX = viewport.TopLeftY = viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;

				_deviceContext->RSSetViewports(1, &viewport);
			}

			void GraphicsDeviceDX11::SetRasterizerState(RasterizerState state)
			{
				_currentRasterizerState = state;
				_rasterizerState = _rasterizerStates[static_cast<MSize>(state)];
				_deviceContext->RSSetState(_rasterizerState);
			}

			void GraphicsDeviceDX11::SetDepthStencilState(DepthStencilState state)
			{
				_currentDepthStencilState = state;
				_depthStencilState = _depthStencilStates[static_cast<MSize>(state)];
				_deviceContext->OMSetDepthStencilState(_depthStencilState, 255);
			}

			void GraphicsDeviceDX11::SetBlendState(BlendState state)
			{
				_currentBlendState = state;
				_blendState = _blendStates[static_cast<MSize>(state)];
				_deviceContext->OMSetBlendState(_blendState, nullptr, 0xFFFFFFFF);
			}

			void GraphicsDeviceDX11::SetViewport(const Viewport & vp)
			{
				_currentViewport = vp;
				_options._viewport = vp;
				D3D11_VIEWPORT viewport;

				if ((_options._viewport._width != 0.0f || _options._viewport._height != 0.0f) || _options._window == nullptr)
				{
					viewport.Width = static_cast<MFloat32>(_options._viewport._width);
					viewport.Height = static_cast<MFloat32>(_options._viewport._height);
				}
				else
				{
					viewport.Width = static_cast<MFloat32>(_options._window->GetWidth());
					viewport.Height = static_cast<MFloat32>(_options._window->GetWidth());
				}

				viewport.TopLeftX = vp._topLeft.X;
				viewport.TopLeftY = vp._topLeft.Y;
				viewport.MinDepth = vp._minDepth;
				viewport.MaxDepth = vp._maxDepth;

				_deviceContext->RSSetViewports(1, &viewport);
			}

			void GraphicsDeviceDX11::SetScissorRect(const ScissorRect & rect)
			{
				_options._scissorRect = rect;
				_deviceContext->RSSetScissorRects(1, reinterpret_cast<D3D11_RECT*>(&_options._scissorRect));
			}

			void GraphicsDeviceDX11::SetBackBufferSize(MInt32 width, MInt32 height)
			{
				_swapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
			}

			void GraphicsDeviceDX11::SetSwapChain(MInt32 width, MInt32 height, MInt32 multisampleCount, SwapEffect swapEffect)
			{
				_options._swapEffect = swapEffect;
				_swapChainBufferWidth = width;
				_swapChainBufferHeight = height;
				_swapChain->Release();

				IDXGIDevice* dxgiDevice = nullptr;
				_device->QueryInterface(__uuidof(IDXGIDevice), (reinterpret_cast<void**>(&dxgiDevice)));

				ME_ASSERT_S(dxgiDevice != nullptr);

				IDXGIAdapter* dxgiAdapter = nullptr;
				dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));

				ME_ASSERT_S(dxgiAdapter != nullptr);

				IDXGIFactory* dxgiFactory = nullptr;
				dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));

				ME_ASSERT_S(dxgiFactory != nullptr);

				_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, reinterpret_cast<UINT*>(&_sampleQuality));

				DXGI_SWAP_CHAIN_DESC swapChainDesc;
				swapChainDesc.BufferDesc.Width = _swapChainBufferWidth;
				swapChainDesc.BufferDesc.Height = _swapChainBufferHeight;
				swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;						// this is TODO TEMPORARY AND SHIT
				swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
				swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
				swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
				swapChainDesc.BufferCount = 1;												// this is TODO TEMPORARY AND SHIT
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.SampleDesc.Count = _options._multisampleCount;
				swapChainDesc.SampleDesc.Quality = _sampleQuality - 1;
				swapChainDesc.OutputWindow = _options._window != nullptr ? reinterpret_cast<WindowWin32*>(_options._window)->GetHWND() : (HWND)0;
				swapChainDesc.Windowed = _options._window != nullptr ? !_options._window->IsFullscreen() : false;
				swapChainDesc.SwapEffect = static_cast<DXGI_SWAP_EFFECT>(_options._swapEffect);
				swapChainDesc.Flags = 0;

				dxgiFactory->CreateSwapChain(_device, &swapChainDesc, &_swapChain);

				ME_ASSERT_S(_swapChain != nullptr);

				dxgiFactory->Release();
				dxgiAdapter->Release();
				dxgiDevice->Release();
			}

			void GraphicsDeviceDX11::SetClearMode(ClearMode clearMode)
			{
				_options._clearMode = clearMode;
			}

			void GraphicsDeviceDX11::SetPrimitiveTopology(PrimitiveTopology pt)
			{
				_deviceContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(pt));
			}

			void GraphicsDeviceDX11::SetWindow(Window * window)
			{
				_options._window = window;
				_swapChainBufferWidth = window->GetWidth();
				_swapChainBufferHeight = window->GetHeight();
				SetSwapChain(_swapChainBufferWidth, _swapChainBufferHeight, _options._multisampleCount, _options._swapEffect);
			}

			void GraphicsDeviceDX11::SetFullscreen(bool fullscreen)
			{
				_swapChain->SetFullscreenState(fullscreen, nullptr);
			}

			void GraphicsDeviceDX11::SetVSyncEnabled(bool vsync)
			{
				_options._bVsync = vsync;
				_syncInterval = _options._bVsync ? 1 : 0;
			}

			inline D3D11_TEXTURE2D_DESC GraphicsDeviceDX11::CreateDefaultTextureDepthStencilDesc()
			{
				GraphicsDeviceDX11* gd = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice());
				D3D11_TEXTURE2D_DESC depthStencilDescTex;
				depthStencilDescTex.Width = gd->_swapChainBufferWidth;
				depthStencilDescTex.Height = gd->_swapChainBufferHeight;
				depthStencilDescTex.MipLevels = 1;
				depthStencilDescTex.ArraySize = 1;
				depthStencilDescTex.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				depthStencilDescTex.SampleDesc.Count = gd->_options._multisampleCount;
				depthStencilDescTex.SampleDesc.Quality = gd->_sampleQuality - 1;
				depthStencilDescTex.Usage = D3D11_USAGE_DEFAULT;
				depthStencilDescTex.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				depthStencilDescTex.CPUAccessFlags = 0;
				depthStencilDescTex.MiscFlags = 0;
				return depthStencilDescTex;
			}

			inline D3D11_SAMPLER_DESC GraphicsDeviceDX11::CreateDefaultSamplerDesc(TextureSamplerFormat samplerFormat)
			{
				D3D11_SAMPLER_DESC sDesc;
				D3D11_TEXTURE_ADDRESS_MODE taMode;

				if (samplerFormat == TextureSamplerFormat::ADDRESS_CLAMP_FILTER_LINEAR ||
					samplerFormat == TextureSamplerFormat::ADDRESS_WRAP_FILTER_LINEAR)
				{
					sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				}
				else if (samplerFormat == TextureSamplerFormat::ADDRESS_CLAMP_FILTER_ANISOTROPIC ||
					samplerFormat == TextureSamplerFormat::ADDRESS_WRAP_FILTER_ANISOTROPIC)
				{
					sDesc.Filter = D3D11_FILTER_ANISOTROPIC;
				}
				else
				{
					sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				}

				if (samplerFormat == TextureSamplerFormat::ADDRESS_CLAMP_FILTER_ANISOTROPIC ||
					samplerFormat == TextureSamplerFormat::ADDRESS_CLAMP_FILTER_LINEAR ||
					samplerFormat == TextureSamplerFormat::ADDRESS_CLAMP_FILTER_NEAREST)
				{
					taMode = D3D11_TEXTURE_ADDRESS_CLAMP;
				}
				else
				{
					taMode = D3D11_TEXTURE_ADDRESS_WRAP;
				}

				sDesc.AddressU = taMode;
				sDesc.AddressV = taMode;
				sDesc.AddressW = taMode;
				sDesc.MipLODBias = 0.0f;
				sDesc.MaxAnisotropy = 16;
				sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				sDesc.BorderColor[0] = 0;
				sDesc.BorderColor[1] = 0;
				sDesc.BorderColor[2] = 0;
				sDesc.BorderColor[3] = 0;
				sDesc.MinLOD = 0;
				sDesc.MaxLOD = D3D11_FLOAT32_MAX;
				return sDesc;
			}

			void GraphicsDeviceDX11::SetupStates()
			{
				// create rasterizer states

				// #1
				
				D3D11_RASTERIZER_DESC rasterizerDesc;
				rasterizerDesc.AntialiasedLineEnable = true;
				rasterizerDesc.CullMode = D3D11_CULL_BACK;
				rasterizerDesc.DepthBias = 0;
				rasterizerDesc.DepthBiasClamp = 0;
				rasterizerDesc.DepthClipEnable = true;
				rasterizerDesc.FillMode = D3D11_FILL_SOLID;
				rasterizerDesc.FrontCounterClockwise = true;
				rasterizerDesc.MultisampleEnable = _options._multisampleCount != 0;
				rasterizerDesc.ScissorEnable = false;
				rasterizerDesc.SlopeScaledDepthBias = true;

				_device->CreateRasterizerState(&rasterizerDesc, &_rasterizerStates[0]);
				ME_ASSERT_S(_rasterizerStates[0] != nullptr);

				// #2

				rasterizerDesc.ScissorEnable = true;
				_device->CreateRasterizerState(&rasterizerDesc, &_rasterizerStates[1]);
				ME_ASSERT_S(_rasterizerStates[1] != nullptr);

				// #3

				rasterizerDesc.DepthClipEnable = false;
				_device->CreateRasterizerState(&rasterizerDesc, &_rasterizerStates[2]);
				ME_ASSERT_S(_rasterizerStates[2] != nullptr);

				// #4

				rasterizerDesc.CullMode = D3D11_CULL_FRONT;
				rasterizerDesc.DepthClipEnable = true;
				rasterizerDesc.ScissorEnable = false;
				_device->CreateRasterizerState(&rasterizerDesc, &_rasterizerStates[3]);
				ME_ASSERT_S(_rasterizerStates[3] != nullptr);

				// #5

				rasterizerDesc.CullMode = D3D11_CULL_NONE;
				_device->CreateRasterizerState(&rasterizerDesc, &_rasterizerStates[4]);
				ME_ASSERT_S(_rasterizerStates[4] != nullptr);

				// #6

				rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
				rasterizerDesc.DepthClipEnable = false;
				_device->CreateRasterizerState(&rasterizerDesc, &_rasterizerStates[5]);
				ME_ASSERT_S(_rasterizerStates[5] != nullptr);


				// create depth stencil states

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

				_device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilStates[0]);
				ME_ASSERT_S(_depthStencilStates[0] != nullptr);

				depthStencilDesc.StencilEnable = true;
				_device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilStates[1]);
				ME_ASSERT_S(_depthStencilStates[1] != nullptr);

				depthStencilDesc.DepthEnable = false;
				_device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilStates[2]);
				ME_ASSERT_S(_depthStencilStates[2] != nullptr);

				depthStencilDesc.StencilEnable = false;
				_device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilStates[3]);
				ME_ASSERT_S(_depthStencilStates[3] != nullptr);

				// create blend states

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
				blendDesc.RenderTarget[0] = rbDesc;

				_device->CreateBlendState(&blendDesc, &_blendStates[0]);
				ME_ASSERT_S(_blendStates[0] != nullptr);		

				rbDesc.BlendEnable = true;
				rbDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				rbDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendDesc.RenderTarget[0] = rbDesc;

				_device->CreateBlendState(&blendDesc, &_blendStates[1]);
				ME_ASSERT_S(_blendStates[1] != nullptr);

				rbDesc.SrcBlend = D3D11_BLEND_ONE;
				rbDesc.DestBlend = D3D11_BLEND_ONE;
				rbDesc.BlendOp = D3D11_BLEND_OP_ADD;
				rbDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
				rbDesc.DestBlendAlpha = D3D11_BLEND_ONE;
				rbDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
				rbDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				blendDesc.RenderTarget[0] = rbDesc;

				_device->CreateBlendState(&blendDesc, &_blendStates[2]);
				ME_ASSERT_S(_blendStates[2] != nullptr);
			}

			void VertexShaderResourceDX11::Initialize(MUint8 * byteCodePtr, MSize byteCodeLength, MSize layoutElementCount,
				utility::MString * layoutNames, MUint8 * layoutIndices, GraphicDataFormat * formats)
			{
				ME_ASSERT(layoutElementCount <= 16, "GraphicsDevice: An error occured while creating vertex shader resource - invalid layoutElementCount.");

				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();
				device->CreateVertexShader(byteCodePtr, byteCodeLength, nullptr, &_shaderResource);
				ME_ASSERT(_shaderResource != nullptr, "GraphicsDevice: An error occured while creating vertex shader resource.");

				if (layoutElementCount > 0)
				{
					MFixedArray<D3D11_INPUT_ELEMENT_DESC> layouts(layoutElementCount);

					for (MUint32 i = 0; i < layoutElementCount; ++i)
					{
						layouts[i].SemanticName = layoutNames[i];
						layouts[i].SemanticIndex = layoutIndices[i];
						layouts[i].Format = static_cast<DXGI_FORMAT>(formats[i]);
						layouts[i].InputSlot = i;									// every layout element has its own slot.
						layouts[i].AlignedByteOffset = 0;
						layouts[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
						layouts[i].InstanceDataStepRate = 0;
					}

					device->CreateInputLayout(layouts.GetDataPointer(), static_cast<MUint32>(layoutElementCount), byteCodePtr, byteCodeLength, &_inputLayout);
					ME_ASSERT(_inputLayout != nullptr, "GraphicsDevice: An error occured while creating vertex shader input layout.");
				}
			}

			void VertexShaderResourceDX11::Shutdown()
			{
				ME_ASSERT(_shaderResource != nullptr,
					"GraphicsDevice: An error occured while destroying vertex shader resource. (probably destroyed already?)");
				if(_inputLayout != nullptr) _inputLayout->Release();
				_shaderResource->Release();
			}

			void VertexShaderResourceDX11::Set(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->IASetInputLayout(_inputLayout);
				deviceContext->VSSetShader(_shaderResource, nullptr, 0);
			}

			void PixelShaderResourceDX11::Initialize(MUint8 * byteCodePtr, MSize byteCodeLength)
			{
				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();
				device->CreatePixelShader(byteCodePtr, byteCodeLength, nullptr, &_shaderResource);
				ME_ASSERT(_shaderResource != nullptr, "GraphicsDevice: An error occured while creating pixel shader resource.");
			}

			void PixelShaderResourceDX11::Shutdown()
			{
				ME_ASSERT(_shaderResource != nullptr,
					"GraphicsDevice: An error occured while destroying pixel shader resource. (probably destroyed already?)");
				_shaderResource->Release();
			}

			void PixelShaderResourceDX11::Set(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->PSSetShader(_shaderResource, nullptr, 0);
			}

			void HullShaderResourceDX11::Initialize(MUint8 * byteCodePtr, MSize byteCodeLength)
			{
				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();
				device->CreateHullShader(byteCodePtr, byteCodeLength, nullptr, &_shaderResource);
				ME_ASSERT(_shaderResource != nullptr, "GraphicsDevice: An error occured while creating hull shader resource.");
			}

			void HullShaderResourceDX11::Shutdown()
			{
				ME_ASSERT(_shaderResource != nullptr,
					"GraphicsDevice: An error occured while destroying hull shader resource. (probably destroyed already?)");
				_shaderResource->Release();
			}

			void HullShaderResourceDX11::Set(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->HSSetShader(_shaderResource, nullptr, 0);
			}

			void DomainShaderResourceDX11::Initialize(MUint8 * byteCodePtr, MSize byteCodeLength)
			{
				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();
				device->CreateDomainShader(byteCodePtr, byteCodeLength, nullptr, &_shaderResource);
				ME_ASSERT(_shaderResource != nullptr, "GraphicsDevice: An error occured while creating domain shader resource.");
			}

			void DomainShaderResourceDX11::Shutdown()
			{
				ME_ASSERT(_shaderResource != nullptr,
					"GraphicsDevice: An error occured while destroying domain shader resource. (probably destroyed already?)");
				_shaderResource->Release();
			}

			void DomainShaderResourceDX11::Set(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->DSSetShader(_shaderResource, nullptr, 0);
			}

			void GeometryShaderResourceDX11::Initialize(MUint8 * byteCodePtr, MSize byteCodeLength)
			{
				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();
				device->CreateGeometryShader(byteCodePtr, byteCodeLength, nullptr, &_shaderResource);
				ME_ASSERT(_shaderResource != nullptr, "GraphicsDevice: An error occured while creating geometry shader resource.");
			}

			void GeometryShaderResourceDX11::Shutdown()
			{
				ME_ASSERT(_shaderResource != nullptr,
					"GraphicsDevice: An error occured while destroying geometry shader resource. (probably destroyed already?)");
				_shaderResource->Release();
			}

			void GeometryShaderResourceDX11::Set(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->GSSetShader(_shaderResource, nullptr, 0);
			}

			void ComputeShaderResourceDX11::Initialize(MUint8 * byteCodePtr, MSize byteCodeLength)
			{
				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();
				device->CreateComputeShader(byteCodePtr, byteCodeLength, nullptr, &_shaderResource);
				ME_ASSERT(_shaderResource != nullptr, "GraphicsDevice: An error occured while creating compute shader resource.");
			}

			void ComputeShaderResourceDX11::Shutdown()
			{
				ME_ASSERT(_shaderResource != nullptr,
					"GraphicsDevice: An error occured while destroying compute shader resource. (probably destroyed already?)");
				_shaderResource->Release();
			}

			void ComputeShaderResourceDX11::Set(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->CSSetShader(_shaderResource, nullptr, 0);
			}

			void Texture1DResourceDX11::Initialize(MUint8 * dataPtr, MUint32 width, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat)
			{
				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();

				D3D11_TEXTURE1D_DESC desc;
				desc.ArraySize = 1;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.Format = static_cast<DXGI_FORMAT>(format);
				desc.MipLevels = 1;
				desc.MiscFlags = 0;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.Width = width;

				if (dataPtr != nullptr)
				{
					D3D11_SUBRESOURCE_DATA sData;
					sData.pSysMem = dataPtr;
					sData.SysMemPitch = static_cast<MUint32>(width * static_cast<MSize>(pixelSize / 8));
					device->CreateTexture1D(&desc, &sData, &_textureResource);
				}
				else
				{
					device->CreateTexture1D(&desc, nullptr, &_textureResource);
				}

				ME_ASSERT(_textureResource != nullptr, "GraphicsDevice: An error occured while creating texture 1D resource.");

				D3D11_SAMPLER_DESC sDesc = GraphicsDeviceDX11::CreateDefaultSamplerDesc(samplerFormat);
				device->CreateSamplerState(&sDesc, &_samplerState);
				ME_ASSERT(_samplerState != nullptr, "GraphicsDevice: An error occured while creating sampler state for 1D texture.");

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
				srvDesc.Format = desc.Format;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
				srvDesc.Texture1D.MipLevels = 1;
				srvDesc.Texture1D.MostDetailedMip = 0;

				device->CreateShaderResourceView(_textureResource, &srvDesc, &_shaderResourceView);
				ME_ASSERT(_shaderResourceView != nullptr, "GraphicsDevice: An error occured while creating shader resource view for 1D texture.");
			}

			void Texture1DResourceDX11::Shutdown()
			{
				ME_ASSERT(_textureResource != nullptr && _samplerState && _shaderResourceView != nullptr,
					"GraphicsDevice: An error occured while destroying texture 1D resources. (probably destroyed already?)");
				_shaderResourceView->Release();
				_shaderResourceView = nullptr;
				_textureResource->Release();
				_textureResource = nullptr;
				_samplerState->Release();
				_samplerState = nullptr;
			}

			void Texture1DResourceDX11::Set(MInt32 slot) const
			{
				SetPS(slot);
			}

			void Texture1DResourceDX11::SetVS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->VSSetSamplers(slot, 1, &_samplerState);
				deviceContext->VSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void Texture1DResourceDX11::SetPS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->PSSetSamplers(slot, 1, &_samplerState);
				deviceContext->PSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void Texture1DResourceDX11::SetGS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->GSSetSamplers(slot, 1, &_samplerState);
				deviceContext->GSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void Texture1DResourceDX11::SetHS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->HSSetSamplers(slot, 1, &_samplerState);
				deviceContext->HSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void Texture1DResourceDX11::SetDS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->DSSetSamplers(slot, 1, &_samplerState);
				deviceContext->DSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void Texture1DResourceDX11::SetCS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->CSSetSamplers(slot, 1, &_samplerState);
				deviceContext->CSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void Texture2DResourceDX11::Initialize(MUint8 * dataPtr, MUint32 width, MUint32 height, MUint8 pixelSize, GraphicDataFormat format,
				TextureSamplerFormat samplerFormat, bool bCompressed, MUint8 mipmapCount)
			{
				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();

				MUint8 realMipmapCount = mipmapCount != 0 ? mipmapCount : 1;
				D3D11_TEXTURE2D_DESC desc;
				desc.ArraySize = 1;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (mipmapCount == 0 ? D3D11_BIND_RENDER_TARGET : 0);
				desc.CPUAccessFlags = 0;
				desc.Format = static_cast<DXGI_FORMAT>(format);
				desc.MipLevels = mipmapCount;
				desc.MiscFlags = mipmapCount == 0 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.Width = (width);
				desc.Height = (height);
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;

				ME_ASSERT_S(mipmapCount <= GraphicsDeviceDX11::MAX_MIPMAP_COUNT);

				D3D11_SUBRESOURCE_DATA sData[GraphicsDeviceDX11::MAX_MIPMAP_COUNT];
				MUint32 heightShift = (desc.Format > 69 && width == height) ? 2 : 0;
				MUint32 currentWidth = static_cast<MUint32>(width);
				MUint32 currentHeight = static_cast<MUint32>(height) >> heightShift;
				MUint8* currentData = dataPtr;
				for (int i = 0; i < GraphicsDeviceDX11::MAX_MIPMAP_COUNT && realMipmapCount; ++i)
				{
					sData[i].pSysMem = currentData;
					sData[i].SysMemPitch = currentWidth * (pixelSize / 8);
					sData[i].SysMemSlicePitch = (currentWidth * currentHeight) * (pixelSize / 8);

					if (i < realMipmapCount)
					{
						currentData += (currentWidth * currentHeight) * (pixelSize / 8);
						currentWidth = currentWidth >> 1;
						currentHeight = utility::MMath::Max(currentHeight >> 1, 1);
					}
				}
				

				device->CreateTexture2D(&desc, sData, &_textureResource);
				ME_ASSERT(_textureResource != nullptr, "GraphicsDevice: An error occured while creating texture 2D resource.");

				D3D11_SAMPLER_DESC sDesc = GraphicsDeviceDX11::CreateDefaultSamplerDesc(samplerFormat);
				device->CreateSamplerState(&sDesc, &_samplerState);
				ME_ASSERT(_samplerState != nullptr, "GraphicsDevice: An error occured while creating sampler state for 2D texture.");

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
				srvDesc.Format = desc.Format;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = -1;
				srvDesc.Texture2D.MostDetailedMip = 0;

				device->CreateShaderResourceView(_textureResource, &srvDesc, &_shaderResourceView);
				ME_ASSERT(_shaderResourceView != nullptr, "GraphicsDevice: An error occured while creating shader resource view for 2D texture.");

				if (mipmapCount == 0)
				{
					ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
					deviceContext->GenerateMips(_shaderResourceView);
				}
			}

			void Texture2DResourceDX11::Shutdown()
			{
				ME_ASSERT(_textureResource != nullptr && _samplerState != nullptr && _shaderResourceView != nullptr,
					"GraphicsDevice: An error occured while destroying texture 2D resources. (probably destroyed already?)");
				_shaderResourceView->Release();
				_shaderResourceView = nullptr;
				_textureResource->Release();
				_textureResource = nullptr;
				_samplerState->Release();
				_samplerState = nullptr;
			}

			void Texture2DResourceDX11::Set(MInt32 slot) const
			{
				SetPS(slot);
			}

			void Texture2DResourceDX11::SetVS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->VSSetSamplers(slot, 1, &_samplerState);
				deviceContext->VSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void Texture2DResourceDX11::SetPS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->PSSetSamplers(slot, 1, &_samplerState);
				deviceContext->PSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void Texture2DResourceDX11::SetGS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->GSSetSamplers(slot, 1, &_samplerState);
				deviceContext->GSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void Texture2DResourceDX11::SetHS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->HSSetSamplers(slot, 1, &_samplerState);
				deviceContext->HSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void Texture2DResourceDX11::SetDS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->DSSetSamplers(slot, 1, &_samplerState);
				deviceContext->DSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void Texture2DResourceDX11::SetCS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->DSSetSamplers(slot, 1, &_samplerState);
				deviceContext->DSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void TextureCubeResourceDX11::Initialize(MUint8 * dataPtrPlusX, MUint8 * dataPtrMinusX, MUint8 * dataPtrPlusY, 
				MUint8 * dataPtrMinusY, MUint8 * dataPtrPlusZ, MUint8 * dataPtrMinusZ, 
				MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat, bool compressed, MUint8 mipmapCount)
			{
				ME_ASSERT(texWidth == texHeight, "TextureCubeResource: dimensions are not square.");

				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				MUint8 realMipmapCount = mipmapCount != 0 ? mipmapCount : 1;
				D3D11_TEXTURE2D_DESC desc;
				desc.ArraySize = 6;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (mipmapCount == 0 ? D3D11_BIND_RENDER_TARGET : 0);
				desc.CPUAccessFlags = 0;
				desc.Format = static_cast<DXGI_FORMAT>(format);
				desc.MipLevels = mipmapCount;
				desc.MiscFlags = (mipmapCount == 0 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0) | D3D11_RESOURCE_MISC_TEXTURECUBE;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.Width = texWidth;
				desc.Height = texHeight;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				srvDesc.Format = desc.Format;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				srvDesc.TextureCube.MipLevels = desc.MipLevels;
				srvDesc.TextureCube.MostDetailedMip = 0;

				D3D11_SUBRESOURCE_DATA* sDatas = new D3D11_SUBRESOURCE_DATA[6 * realMipmapCount];

				MUint8* dataPtrs[6] = { dataPtrPlusX, dataPtrMinusY, dataPtrMinusX, dataPtrPlusZ, dataPtrPlusY, dataPtrMinusZ };
				for (MSize i = 0; i < 6; ++i)
				{
					MUint8* currentData = dataPtrs[i];
					MUint32 heightShift = desc.Format > 69 ? 2 : 0;		// not in cubemaps AFAIK // a jednak.
					MUint32 currentWidth = static_cast<MUint32>(texWidth);
					MUint32 currentHeight = static_cast<MUint32>(texHeight) >> heightShift;
					for (MSize j = 0; j < realMipmapCount; ++j)
					{
						MSize tabIndex = j + i * realMipmapCount;
						sDatas[tabIndex].pSysMem = currentData;
						sDatas[tabIndex].SysMemPitch = currentWidth * pixelSize / 8;
						sDatas[tabIndex].SysMemSlicePitch = (currentWidth * currentHeight) * pixelSize / 8;

						currentData += (currentWidth * currentHeight) * pixelSize / 8;
						currentWidth = currentWidth >> 1;
						currentHeight = utility::MMath::Max(currentHeight >> 1, 1);
					}
				}

				device->CreateTexture2D(&desc, sDatas, &_textureResource);
				ME_ASSERT(_textureResource != nullptr, "GraphicsDevice: An error occured while creating texture CUBE resource.");

				delete sDatas;

				D3D11_SAMPLER_DESC sDesc = GraphicsDeviceDX11::CreateDefaultSamplerDesc(samplerFormat);
				device->CreateSamplerState(&sDesc, &_samplerState);
				ME_ASSERT(_samplerState != nullptr, "GraphicsDevice: An error occured while creating sampler state for CUBE texture.");

				device->CreateShaderResourceView(_textureResource, &srvDesc, &_shaderResourceView);
				ME_ASSERT(_shaderResourceView != nullptr, "GraphicsDevice: An error occured while creating SRV for CUBE texture.");
				
				if (mipmapCount == 0)
				{
					deviceContext->GenerateMips(_shaderResourceView);
				}
			}

			void TextureCubeResourceDX11::Shutdown()
			{
				ME_ASSERT(_textureResource != nullptr && _samplerState && _shaderResourceView != nullptr,
					"GraphicsDevice: An error occured while destroying texture CUBE resources. (probably destroyed already?)");
				_shaderResourceView->Release();
				_shaderResourceView = nullptr;
				_textureResource->Release();
				_textureResource = nullptr;
				_samplerState->Release();
				_samplerState = nullptr;
			}

			void TextureCubeResourceDX11::Set(MInt32 slot) const
			{
				SetPS(slot);
			}

			void TextureCubeResourceDX11::SetVS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->VSSetSamplers(slot, 1, &_samplerState);
				deviceContext->VSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void TextureCubeResourceDX11::SetPS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->PSSetSamplers(slot, 1, &_samplerState);
				deviceContext->PSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void TextureCubeResourceDX11::SetGS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->GSSetSamplers(slot, 1, &_samplerState);
				deviceContext->GSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void TextureCubeResourceDX11::SetHS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->HSSetSamplers(slot, 1, &_samplerState);
				deviceContext->HSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void TextureCubeResourceDX11::SetDS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->DSSetSamplers(slot, 1, &_samplerState);
				deviceContext->DSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void TextureCubeResourceDX11::SetCS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->CSSetSamplers(slot, 1, &_samplerState);
				deviceContext->CSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void DepthStencilResourceDX11::Initialize(MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize, GraphicDataFormat format)
			{
				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();

				ME_ASSERT_S(format == GraphicDataFormat::UNORM_D24_UINT_S8 || format == GraphicDataFormat::FLOAT_R32);

				const DXGI_FORMAT texFormat = format == GraphicDataFormat::UNORM_D24_UINT_S8 ? DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
				const DXGI_FORMAT srvFormat = format == GraphicDataFormat::UNORM_D24_UINT_S8 ? DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
				const DXGI_FORMAT dpFormat = format == GraphicDataFormat::UNORM_D24_UINT_S8 ? DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;

				const D3D11_SRV_DIMENSION srvDimension = RenderingManager::GetInstance()->GetDevice()->GetCurrentOptions()._multisampleCount != 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
				const D3D11_DSV_DIMENSION dsvDimension = RenderingManager::GetInstance()->GetDevice()->GetCurrentOptions()._multisampleCount != 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;

				D3D11_TEXTURE2D_DESC textureDesc = GraphicsDeviceDX11::CreateDefaultTextureDepthStencilDesc();
				textureDesc.Width = texWidth;
				textureDesc.Height = texHeight;
				textureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
				textureDesc.Format = texFormat;

				device->CreateTexture2D(&textureDesc, nullptr, &_tDepthStencilBuffer);
				ME_ASSERT_S(_tDepthStencilBuffer != nullptr);

				D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
				ZeroMemory(&dsDesc, sizeof(dsDesc));
				dsDesc.Format = dpFormat;
				dsDesc.ViewDimension = dsvDimension;
				device->CreateDepthStencilView(_tDepthStencilBuffer, &dsDesc, &_vDepthStencilBuffer);
				ME_ASSERT_S(_vDepthStencilBuffer != nullptr);

				if (format != GraphicDataFormat::UNORM_D24_UINT_S8)
				{
					D3D11_SAMPLER_DESC sDesc = GraphicsDeviceDX11::CreateDefaultSamplerDesc(TextureSamplerFormat::ADDRESS_CLAMP_FILTER_LINEAR);
					device->CreateSamplerState(&sDesc, &_samplerState);
					ME_ASSERT(_samplerState != nullptr, "GraphicsDevice: An error occured while creating sampler state for 2D texture.");

					D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
					ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
					srvDesc.Format = srvFormat;
					srvDesc.ViewDimension = srvDimension;
					srvDesc.Texture2D.MipLevels = 1;
					srvDesc.Texture2D.MostDetailedMip = 0;

					device->CreateShaderResourceView(_tDepthStencilBuffer, &srvDesc, &_shaderResourceView);
					ME_ASSERT(_shaderResourceView != nullptr, "GraphicsDevice: An error occured while creating shader resource view for 2D texture.");
				}
			}

			void DepthStencilResourceDX11::Initialize(MUint8 * dataPtr, MUint32 width, MUint32 height, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat, bool bCompressed, MUint8 mipmapCount)
			{
				Initialize(width, height, pixelSize, format);
			}

			void DepthStencilResourceDX11::Shutdown()
			{
				ME_ASSERT(
					_tDepthStencilBuffer != nullptr && _vDepthStencilBuffer != nullptr,
					"GraphicsDevice: An error occured while destroying DepthBuffer resources. (probably destroyed already?)");

				_vDepthStencilBuffer->Release();
				_tDepthStencilBuffer->Release();
				_vDepthStencilBuffer = nullptr;
				_tDepthStencilBuffer = nullptr;

				if (_shaderResourceView != nullptr)
				{
					_shaderResourceView->Release();
					_shaderResourceView = nullptr;
				}


				if (_samplerState != nullptr)
				{
					_samplerState->Release();
					_samplerState = nullptr;
				}
					
			}

			void DepthStencilResourceDX11::Clear(float depthValue, MUint8 stencilValue)
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->ClearDepthStencilView(_vDepthStencilBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthValue, stencilValue);
			}

			void DepthStencilResourceDX11::Set(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->PSSetSamplers(slot, 1, &_samplerState);
				deviceContext->PSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void DepthStencilResourceDX11::SetVS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->VSSetSamplers(slot, 1, &_samplerState);
				deviceContext->VSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void DepthStencilResourceDX11::SetPS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->PSSetSamplers(slot, 1, &_samplerState);
				deviceContext->PSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void DepthStencilResourceDX11::SetGS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->GSSetSamplers(slot, 1, &_samplerState);
				deviceContext->GSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void DepthStencilResourceDX11::SetHS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->HSSetSamplers(slot, 1, &_samplerState);
				deviceContext->HSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void DepthStencilResourceDX11::SetDS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->DSSetSamplers(slot, 1, &_samplerState);
				deviceContext->DSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void DepthStencilResourceDX11::SetCS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->CSSetSamplers(slot, 1, &_samplerState);
				deviceContext->CSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void RenderTargetResourceDX11::Initialize(MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize, GraphicDataFormat format, bool srv)
			{
				if (!srv) _shaderResourceView = reinterpret_cast<ID3D11ShaderResourceView*>(0xCF);
				Initialize(nullptr, texWidth, texHeight, pixelSize, format, TextureSamplerFormat::ADDRESS_CLAMP_FILTER_LINEAR, false, 0);
			}

			void RenderTargetResourceDX11::Initialize(MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize, GraphicDataFormat format, ID3D11Texture2D * extText, bool srv)
			{
				_tRenderTarget = extText;
				Initialize(texWidth, texHeight, pixelSize, format, srv);
			}

			void RenderTargetResourceDX11::Initialize(MUint8* dataPtr, MUint32 width, MUint32 height, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat,
				bool bCompressed, MUint8 mipmapCount)
			{
				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();
				MUint8 realMipmapCount = mipmapCount != 0 ? mipmapCount : 1;
				D3D11_TEXTURE2D_DESC desc;
				desc.ArraySize = 1;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (mipmapCount == 0 ? D3D11_BIND_RENDER_TARGET : 0);
				desc.CPUAccessFlags = 0;
				desc.Format = static_cast<DXGI_FORMAT>(format);
				desc.MipLevels = realMipmapCount;
				desc.MiscFlags = mipmapCount == 0 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.Width = width;
				desc.Height = height;
				desc.SampleDesc.Count = RenderingManager::GetInstance()->GetDevice()->GetCurrentOptions()._multisampleCount;
				desc.SampleDesc.Quality = RenderingManager::GetInstance()->GetDevice()->GetMultisampleQuality() - 1;
				ME_ASSERT_S(mipmapCount <= GraphicsDeviceDX11::MAX_MIPMAP_COUNT);

				if (_tRenderTarget == nullptr)
				{
					if (dataPtr != nullptr)
					{
						D3D11_SUBRESOURCE_DATA sData[GraphicsDeviceDX11::MAX_MIPMAP_COUNT];
						MUint32 heightShift = desc.Format > 69 ? 2 : 0;
						MUint32 currentWidth = static_cast<MUint32>(width);
						MUint32 currentHeight = static_cast<MUint32>(height) >> heightShift;
						MUint8* currentData = dataPtr;
						for (int i = 0; i < GraphicsDeviceDX11::MAX_MIPMAP_COUNT; ++i)
						{
							sData[i].pSysMem = currentData;
							sData[i].SysMemPitch = currentWidth * pixelSize / 8;
							sData[i].SysMemSlicePitch = (currentWidth * currentHeight) * pixelSize / 8;

							if (i < realMipmapCount)
							{
								currentData += (currentWidth * currentHeight) * pixelSize / 8;
								currentWidth = currentWidth >> 1;
								currentHeight = utility::MMath::Max(currentHeight >> 1, 1);
							}
						}


						device->CreateTexture2D(&desc, sData, &_tRenderTarget);
					}
					else
					{
						device->CreateTexture2D(&desc, nullptr, &_tRenderTarget);
					}

					ME_ASSERT(_tRenderTarget != nullptr, "GraphicsDevice: An error occured while creating texture 2D resource.");
				}	

				D3D11_SAMPLER_DESC sDesc = GraphicsDeviceDX11::CreateDefaultSamplerDesc(samplerFormat);
				device->CreateSamplerState(&sDesc, &_samplerState);
				ME_ASSERT(_samplerState != nullptr, "GraphicsDevice: An error occured while creating sampler state for 2D texture.");

				if (_shaderResourceView == nullptr)
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
					ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
					srvDesc.Format = desc.Format;
					srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MipLevels = realMipmapCount;
					srvDesc.Texture2D.MostDetailedMip = 0;

					device->CreateShaderResourceView(_tRenderTarget, &srvDesc, &_shaderResourceView);
					ME_ASSERT(_shaderResourceView != nullptr, "GraphicsDevice: An error occured while creating shader resource view for 2D texture.");

					if (mipmapCount == 0)
					{
						ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
						deviceContext->GenerateMips(_shaderResourceView);
					}
				}
				else
				{
					_shaderResourceView = nullptr;
				}

				// /////////////////////////////////////////

				// render target related things

				device->CreateRenderTargetView(_tRenderTarget, nullptr, &_vRenderTarget);
				ME_ASSERT_S(_vRenderTarget != nullptr);

				// /////
			}

			void RenderTargetResourceDX11::Shutdown()
			{
				ME_ASSERT(_tRenderTarget != nullptr &&
					 _vRenderTarget != nullptr,
					"GraphicsDevice: An error occured while destroying RenderTarget resources. (probably destroyed already?)");

				_vRenderTarget->Release();
				_tRenderTarget->Release();
				_vRenderTarget = nullptr;
				_tRenderTarget = nullptr;

				if (_shaderResourceView != nullptr)
				{
					_shaderResourceView->Release();
					_shaderResourceView = nullptr;
				}

				if(_samplerState != nullptr)
				{
					_samplerState->Release();
					_samplerState = nullptr;
				}	
			}

			void RenderTargetResourceDX11::Set(MInt32 slot) const
			{
				SetPS(slot);
			}

			void RenderTargetResourceDX11::SetVS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->VSSetSamplers(slot, 1, &_samplerState);
				deviceContext->VSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void RenderTargetResourceDX11::SetPS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->PSSetSamplers(slot, 1, &_samplerState);
				deviceContext->PSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void RenderTargetResourceDX11::SetGS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->GSSetSamplers(slot, 1, &_samplerState);
				deviceContext->GSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void RenderTargetResourceDX11::SetHS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->HSSetSamplers(slot, 1, &_samplerState);
				deviceContext->HSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void RenderTargetResourceDX11::SetDS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->DSSetSamplers(slot, 1, &_samplerState);
				deviceContext->DSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void RenderTargetResourceDX11::SetCS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->CSSetSamplers(slot, 1, &_samplerState);
				deviceContext->CSSetShaderResources(slot, 1, &_shaderResourceView);
			}

			void RenderTargetResourceDX11::Bind(const DepthStencilResource* depthStencil)
			{
				GraphicsDeviceDX11* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice());
				ID3D11DepthStencilView* dv = depthStencil != nullptr ? reinterpret_cast<const DepthStencilResourceDX11*>(depthStencil)->_vDepthStencilBuffer : nullptr;
				device->GetDeviceContext()->OMSetRenderTargets(1, &_vRenderTarget, dv);
			}

			void RenderTargetResourceDX11::BindMultiple(const RenderTargetResource *const* otherRenderTargets, MSize otherRenderTargetsCount, 
				const DepthStencilResource * depthStencil)
			{
				MFixedArray<ID3D11RenderTargetView*> views(otherRenderTargetsCount + 1);
				ID3D11DepthStencilView* dv = depthStencil != nullptr ? reinterpret_cast<const DepthStencilResourceDX11*>(depthStencil)->_vDepthStencilBuffer : nullptr;
				views[0] = this->_vRenderTarget;
				for (MSize i = 0; i < otherRenderTargetsCount; ++i)
					views[i + 1] = reinterpret_cast<const RenderTargetResourceDX11*>(otherRenderTargets[i])->_vRenderTarget;

				GraphicsDeviceDX11* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice());
				device->GetDeviceContext()->OMSetRenderTargets((UINT)(otherRenderTargetsCount) + 1, views.GetDataPointer(), dv);
			}

			void RenderTargetResourceDX11::Clear(const utility::MColor & color)
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->ClearRenderTargetView(_vRenderTarget, reinterpret_cast<const float*>(&color));
			}

			void VertexBufferResourceDX11::Initialize(MFloat32 * dataPtr, MSize vertexSize, MSize vertexCount, BufferAccessMode accessMode)
			{
				_accessMode = accessMode;

				UINT accessFlags = 0;
				if (_accessMode == BufferAccessMode::READ)
					accessFlags = D3D11_CPU_ACCESS_READ;
				else if (_accessMode == BufferAccessMode::WRITE)
					accessFlags = D3D11_CPU_ACCESS_WRITE;
				else if (_accessMode == BufferAccessMode::READ_WRITE)
					accessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

				_stride = static_cast<MUint32>(vertexSize);

				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();

				D3D11_BUFFER_DESC desc;
				D3D11_SUBRESOURCE_DATA sData;
				ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
				ZeroMemory(&sData, sizeof(D3D11_SUBRESOURCE_DATA));

				desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				desc.ByteWidth = static_cast<MUint32>(vertexSize * vertexCount);
				desc.CPUAccessFlags = accessFlags;
				desc.MiscFlags = 0;
				desc.StructureByteStride = 0;
				desc.Usage = accessFlags == 0 ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;

				sData.pSysMem = dataPtr;

				device->CreateBuffer(&desc, dataPtr != nullptr ? &sData : nullptr, &_bufferResource);
				ME_ASSERT(_bufferResource != nullptr, "GraphicsDevice: An error occured when creating vertex buffer.");
			}

			void VertexBufferResourceDX11::Shutdown()
			{
				ME_ASSERT(_bufferResource != nullptr, "GraphicsDevice: An error occured when destroying vertex buffer. May it be already destroyed?");
				_bufferResource->Release();
			}

			void VertexBufferResourceDX11::Set(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->IASetVertexBuffers(slot, 1, &_bufferResource, &_stride, &_offset);
			}

			void VertexBufferResourceDX11::Map(void ** outDataStructure)
			{
				ME_ASSERT(_resToMap.pData == nullptr, "GraphicsDevice: Calling Map on already mapped vertex buffer.");
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->Map(_bufferResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &_resToMap);
				*outDataStructure = _resToMap.pData;
			}

			void VertexBufferResourceDX11::Unmap()
			{
				ME_ASSERT(_resToMap.pData != nullptr, "GraphicsDevice: Calling Unmap on already unmapped vertex buffer.");
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->Unmap(_bufferResource, 0);
#ifdef _DEBUG
				ZeroMemory(&_resToMap, sizeof(D3D11_SUBRESOURCE_DATA));
#endif // _DEBUG
			}

			void VertexBufferResourceDX11::Update(void * data, MSize dataSizeBytes)
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->UpdateSubresource(_bufferResource, 0, nullptr, data, static_cast<MUint32>(dataSizeBytes), 0);
			}

			void IndexBufferResourceDX11::Initialize(MUint16 * dataPtr, MSize indexCount, BufferAccessMode accessMode)
			{
				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();

				_accessMode = accessMode;

				UINT accessFlags = 0;
				if (_accessMode == BufferAccessMode::READ)
					accessFlags = D3D11_CPU_ACCESS_READ;
				else if (_accessMode == BufferAccessMode::WRITE)
					accessFlags = D3D11_CPU_ACCESS_WRITE;
				else if (_accessMode == BufferAccessMode::READ_WRITE)
					accessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

				D3D11_BUFFER_DESC desc;
				D3D11_SUBRESOURCE_DATA sData;
				ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
				ZeroMemory(&sData, sizeof(D3D11_SUBRESOURCE_DATA));

				desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				desc.ByteWidth = static_cast<MUint32>(sizeof(MUint16) * indexCount);
				desc.CPUAccessFlags = accessFlags;
				desc.MiscFlags = 0;
				desc.StructureByteStride = 0;
				desc.Usage = accessFlags == 0 ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;

				sData.pSysMem = dataPtr;

				device->CreateBuffer(&desc, dataPtr != nullptr ? &sData : nullptr, &_bufferResource);
				ME_ASSERT(_bufferResource != nullptr, "GraphicsDevice: An error occured when creating index buffer.");
			}

			void IndexBufferResourceDX11::Shutdown()
			{
				ME_ASSERT(_bufferResource != nullptr, "GraphicsDevice: An error occured when destroying index buffer. May it be already destroyed?");
				_bufferResource->Release();
			}

			void IndexBufferResourceDX11::Set(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->IASetIndexBuffer(_bufferResource, DXGI_FORMAT_R16_UINT, 0);
			}

			void IndexBufferResourceDX11::Map(void ** outDataStructure)
			{
				ME_ASSERT(_resToMap.pData == nullptr, "GraphicsDevice: Calling Map on already mapped index buffer.");
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->Map(_bufferResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &_resToMap);
				*outDataStructure = _resToMap.pData;
			}

			void IndexBufferResourceDX11::Unmap()
			{
				ME_ASSERT(_resToMap.pData != nullptr, "GraphicsDevice: Calling Unmap on already unmapped index buffer.");
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->Unmap(_bufferResource, 0);
#ifdef _DEBUG
				ZeroMemory(&_resToMap, sizeof(D3D11_SUBRESOURCE_DATA));
#endif // _DEBUG
			}

			void ConstantBufferResourceDX11::Initialize(void * initialData, MSize byteSize, BufferAccessMode accessMode)
			{
				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();

				_accessMode = accessMode;

				UINT accessFlags = 0;
				if (_accessMode == BufferAccessMode::READ)
					accessFlags = D3D11_CPU_ACCESS_READ;
				else if (_accessMode == BufferAccessMode::WRITE)
					accessFlags = D3D11_CPU_ACCESS_WRITE;
				else if (_accessMode == BufferAccessMode::READ_WRITE)
					accessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

				D3D11_BUFFER_DESC bDesc;
				bDesc.Usage = D3D11_USAGE_DYNAMIC;
				bDesc.ByteWidth = static_cast<MUint32>(byteSize);
				bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				bDesc.CPUAccessFlags = accessFlags;
				bDesc.MiscFlags = 0;
				bDesc.StructureByteStride = 0;

				D3D11_SUBRESOURCE_DATA sData;
				sData.pSysMem = initialData;
				sData.SysMemPitch = 0;
				sData.SysMemSlicePitch = 0;

				D3D11_SUBRESOURCE_DATA* sDataPtr = initialData != nullptr ? &sData : nullptr;

				device->CreateBuffer(&bDesc, sDataPtr, &_bufferResource);
				ME_ASSERT(_bufferResource != nullptr, "GraphicsDevice: An error occured when creating constant buffer.");
			}

			void ConstantBufferResourceDX11::Shutdown()
			{
				ME_ASSERT(_bufferResource != nullptr, "GraphicsDevice: An error occured when destroying constant buffer. May it be already destroyed?");
				_bufferResource->Release();
			}

			void ConstantBufferResourceDX11::Set(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->VSSetConstantBuffers(slot, 1, &_bufferResource);
				deviceContext->PSSetConstantBuffers(slot, 1, &_bufferResource);
			}

			void ConstantBufferResourceDX11::SetVS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->VSSetConstantBuffers(slot, 1, &_bufferResource);
			}

			void ConstantBufferResourceDX11::SetPS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->PSSetConstantBuffers(slot, 1, &_bufferResource);
			}

			void ConstantBufferResourceDX11::SetGS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->GSSetConstantBuffers(slot, 1, &_bufferResource);
			}

			void ConstantBufferResourceDX11::SetHS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->HSSetConstantBuffers(slot, 1, &_bufferResource);
			}

			void ConstantBufferResourceDX11::SetDS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->DSSetConstantBuffers(slot, 1, &_bufferResource);
			}

			void ConstantBufferResourceDX11::SetCS(MInt32 slot) const
			{
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->CSSetConstantBuffers(slot, 1, &_bufferResource);
			}

			void ConstantBufferResourceDX11::Map(void ** outDataStructure)
			{
				ME_ASSERT(_resToMap.pData == nullptr, "GraphicsDevice: Calling Map on already mapped constant buffer.");
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->Map(_bufferResource, 0, D3D11_MAP_WRITE_DISCARD, 0, &_resToMap);
				*outDataStructure = _resToMap.pData;
			}

			void ConstantBufferResourceDX11::Unmap()
			{
				ME_ASSERT(_resToMap.pData != nullptr, "GraphicsDevice: Calling Unmap on already unmapped constant buffer.");
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->Unmap(_bufferResource, 0);
#ifdef _DEBUG
				ZeroMemory(&_resToMap, sizeof(D3D11_SUBRESOURCE_DATA));
#endif // _DEBUG
			}

			void ConstantBufferResourceDX11::ApplyMatrix(void * destination, const utility::MMatrix * mat)
			{
				(*reinterpret_cast<MMatrix*>(destination)) = mat->Transposed();
			}

			void StagingTexture2DResourceDX11::Initialize(MUint32 originX, MUint32 originY, MUint32 rectWidth, MUint32 rectHeight, GraphicDataFormat format)
			{
				StagingTexture2DResource::Initialize(originX, originY, rectWidth, rectHeight, format);

				ID3D11Device* device = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDevice();

				D3D11_TEXTURE2D_DESC desc;
				desc.ArraySize = 1;
				desc.BindFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				desc.Format = static_cast<DXGI_FORMAT>(format);
				desc.MipLevels = 1;
				desc.MiscFlags = 0;
				desc.Usage = D3D11_USAGE_STAGING;
				desc.Width = rectWidth;
				desc.Height = rectHeight;
				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;

				device->CreateTexture2D(&desc, nullptr, &_textureResource);
				ME_ASSERT_S(_textureResource != nullptr);
			}

			void StagingTexture2DResourceDX11::Shutdown()
			{
				if (_textureResource != nullptr)
				{
					_textureResource->Release();
				}
			}

			void StagingTexture2DResourceDX11::CopyTextureData(const Texture2DResource * resource)
			{
				D3D11_BOX srcBox;
				srcBox.left = OriginX;
				srcBox.right = OriginX + RectWidth;
				srcBox.top = OriginY;
				srcBox.bottom = OriginY + RectHeight;
				srcBox.front = 0;
				srcBox.back = 1;

				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();

				deviceContext->CopySubresourceRegion(_textureResource, 0, 0, 0, 0,
					reinterpret_cast<const Texture2DResourceDX11*>(resource)->_textureResource,
					0, &srcBox);
			}

			void StagingTexture2DResourceDX11::UpdateRectSize()
			{
				Shutdown();
				Initialize(OriginX, OriginY, RectWidth, RectHeight, Format);
			}

			void StagingTexture2DResourceDX11::Map(void ** outDataStructure)
			{
				ME_ASSERT(_resToMap.pData == nullptr, "GraphicsDevice: Calling Map on already mapped staging texture.");
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->Map(_textureResource, 0, D3D11_MAP_READ, 0, &_resToMap);
				*outDataStructure = _resToMap.pData;
			}

			void StagingTexture2DResourceDX11::Unmap()
			{
				ME_ASSERT(_resToMap.pData != nullptr, "GraphicsDevice: Calling Unmap on already unmapped staging texture.");
				ID3D11DeviceContext* deviceContext = reinterpret_cast<GraphicsDeviceDX11*>(RenderingManager::GetInstance()->GetDevice())->GetDeviceContext();
				deviceContext->Unmap(_textureResource, 0);
#ifdef _DEBUG
				ZeroMemory(&_resToMap, sizeof(D3D11_SUBRESOURCE_DATA));
#endif // _DEBUG
			}

}
	}
}
#pragma once
#include "renderer/device/GraphicsDevice.h"

#include "utility/MStack.h"
#include "utility/MFixedArray.h"

#include <d3d11.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace morphEngine
{
	namespace renderer
	{
		namespace device
		{

#pragma region ResourceTypes

			// here vertex input layout also is stored.
			struct VertexShaderResourceDX11 : public VertexShaderResource
			{
				ID3D11VertexShader* _shaderResource = nullptr;
				ID3D11InputLayout* _inputLayout = nullptr;

				virtual void Initialize(
					MUint8* byteCodePtr, MSize byteCodeLength,
					MSize layoutElementCount,
					utility::MString* layoutNames,
					MUint8* layoutIndices,
					GraphicDataFormat* formats) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
			};

			struct PixelShaderResourceDX11 : public PixelShaderResource
			{
				ID3D11PixelShader* _shaderResource = nullptr;

				virtual void Initialize(MUint8* byteCodePtr, MSize byteCodeLength) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
			};

			struct HullShaderResourceDX11 : public HullShaderResource
			{
				ID3D11HullShader* _shaderResource = nullptr;

				virtual void Initialize(MUint8* byteCodePtr, MSize byteCodeLength) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
			};

			struct DomainShaderResourceDX11 : public DomainShaderResource
			{
				ID3D11DomainShader* _shaderResource = nullptr;

				virtual void Initialize(MUint8* byteCodePtr, MSize byteCodeLength) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
			};

			struct GeometryShaderResourceDX11 : public GeometryShaderResource
			{
				ID3D11GeometryShader* _shaderResource = nullptr;

				virtual void Initialize(MUint8* byteCodePtr, MSize byteCodeLength) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
			};

			struct ComputeShaderResourceDX11 : public ComputeShaderResource
			{
				ID3D11ComputeShader* _shaderResource = nullptr;

				virtual void Initialize(MUint8* byteCodePtr, MSize byteCodeLength) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
			};

			struct Texture1DResourceDX11 : public Texture1DResource
			{
				ID3D11Texture1D* _textureResource = nullptr;
				ID3D11ShaderResourceView* _shaderResourceView = nullptr;
				ID3D11SamplerState* _samplerState = nullptr;

				virtual void Initialize(MUint8* dataPtr, MUint32 dataSize, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
				virtual void SetVS(MInt32 slot = 0) const override;
				virtual void SetPS(MInt32 slot = 0) const override;
				virtual void SetGS(MInt32 slot = 0) const override;
				virtual void SetHS(MInt32 slot = 0) const override;
				virtual void SetDS(MInt32 slot = 0) const override;
				virtual void SetCS(MInt32 slot = 0) const override;
			};

			struct Texture2DResourceDX11 : public Texture2DResource
			{
				ID3D11Texture2D* _textureResource = nullptr;
				ID3D11ShaderResourceView* _shaderResourceView = nullptr;
				ID3D11SamplerState* _samplerState = nullptr;

				// TODO: Compression, Sample quality params
				virtual void Initialize(MUint8* dataPtr, MUint32 width, MUint32 height, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat,
					bool bCompressed, MUint8 mipmapCount) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
				virtual void SetVS(MInt32 slot = 0) const override;
				virtual void SetPS(MInt32 slot = 0) const override;
				virtual void SetGS(MInt32 slot = 0) const override;
				virtual void SetHS(MInt32 slot = 0) const override;
				virtual void SetDS(MInt32 slot = 0) const override;
				virtual void SetCS(MInt32 slot = 0) const override;
			};

			struct StagingTexture2DResourceDX11 : public StagingTexture2DResource
			{
				ID3D11Texture2D* _textureResource = nullptr;
				D3D11_MAPPED_SUBRESOURCE _resToMap;

				virtual void Initialize(MUint32 originX, MUint32 originY, MUint32 rectWidth, MUint32 rectHeight,
					GraphicDataFormat format) override;

				virtual void Shutdown() override;

				virtual void CopyTextureData(const Texture2DResource* resource) override;
				virtual void UpdateRectSize() override;

				virtual void Map(void** outDataStructure) override;
				virtual void Unmap() override;
			};

			struct TextureCubeResourceDX11 : public TextureCubeResource
			{
				ID3D11Texture2D* _textureResource = nullptr;
				ID3D11ShaderResourceView* _shaderResourceView = nullptr;
				ID3D11SamplerState* _samplerState = nullptr;

				virtual void Initialize(
					MUint8* dataPtrPlusX,
					MUint8* dataPtrPlusY,
					MUint8* dataPtrPlusZ,
					MUint8* dataPtrMinusX,
					MUint8* dataPtrMinusY,
					MUint8* dataPtrMinusZ,
					MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat, bool compressed, MUint8 mipmapCount) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
				virtual void SetVS(MInt32 slot = 0) const override;
				virtual void SetPS(MInt32 slot = 0) const override;
				virtual void SetGS(MInt32 slot = 0) const override;
				virtual void SetHS(MInt32 slot = 0) const override;
				virtual void SetDS(MInt32 slot = 0) const override;
				virtual void SetCS(MInt32 slot = 0) const override;
			};

			struct DepthStencilResourceDX11 : public DepthStencilResource
			{
				ID3D11Texture2D* _tDepthStencilBuffer = nullptr;
				ID3D11DepthStencilView* _vDepthStencilBuffer = nullptr;

				ID3D11SamplerState* _samplerState = nullptr;
				ID3D11ShaderResourceView* _shaderResourceView = nullptr;

				virtual void Initialize(MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize = 4, GraphicDataFormat format = GraphicDataFormat::UNORM_D24_UINT_S8);
				virtual void Initialize(MUint8* dataPtr, MUint32 width, MUint32 height, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat,
					bool bCompressed, MUint8 mipmapCount) override;
				virtual void Shutdown() override;
				virtual void Clear(float depthValue = 1.0f, MUint8 stencilValue = 0xFF);
				virtual void Set(MInt32 slot = 0) const override;
				virtual void SetVS(MInt32 slot = 0) const override;
				virtual void SetPS(MInt32 slot = 0) const override;
				virtual void SetGS(MInt32 slot = 0) const override;
				virtual void SetHS(MInt32 slot = 0) const override;
				virtual void SetDS(MInt32 slot = 0) const override;
				virtual void SetCS(MInt32 slot = 0) const override;
			};

			struct RenderTargetResourceDX11 : public RenderTargetResource
			{
				ID3D11Texture2D* _tRenderTarget = nullptr;
				ID3D11RenderTargetView* _vRenderTarget = nullptr;

				ID3D11SamplerState* _samplerState = nullptr;
				ID3D11ShaderResourceView* _shaderResourceView = nullptr;

				virtual void Initialize(MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize, GraphicDataFormat format, bool srv = true) override;
				virtual void Initialize(MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize, GraphicDataFormat format, ID3D11Texture2D* extText, bool srv = true);
				virtual void Initialize(MUint8* dataPtr, MUint32 width, MUint32 height, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat,
					bool bCompressed, MUint8 mipmapCount) override;
				virtual void Shutdown() override;
				virtual void Bind(const DepthStencilResource* depthStencil) override;
				virtual void BindMultiple(const RenderTargetResource*const* otherRenderTargets, MSize otherRenderTargetsCount, const DepthStencilResource* depthStencil);
				virtual void Clear(const utility::MColor& color = utility::MColor(0.0f, 0.0f, 0.0f, 0.0f));
				virtual void Set(MInt32 slot = 0) const override;
				virtual void SetVS(MInt32 slot = 0) const override;
				virtual void SetPS(MInt32 slot = 0) const override;
				virtual void SetGS(MInt32 slot = 0) const override;
				virtual void SetHS(MInt32 slot = 0) const override;
				virtual void SetDS(MInt32 slot = 0) const override;
				virtual void SetCS(MInt32 slot = 0) const override;
			};

			struct VertexBufferResourceDX11 : public VertexBufferResource
			{
				D3D11_MAPPED_SUBRESOURCE _resToMap;
				ID3D11Buffer* _bufferResource = nullptr;
				MUint32 _stride = 0;
				MUint32 _offset = 0;

				virtual void Initialize(MFloat32* dataPtr, MSize vertexSize, MSize vertexCount, BufferAccessMode accessMode = BufferAccessMode::NONE) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
				virtual void Map(void** outDataStructure) override;
				virtual void Unmap() override;
				virtual void Update(void* data, MSize dataSizeBytes) override;
			};

			struct IndexBufferResourceDX11 : public IndexBufferResource
			{
				D3D11_MAPPED_SUBRESOURCE _resToMap;
				ID3D11Buffer* _bufferResource = nullptr;

				virtual void Initialize(MUint16* dataPtr, MSize indexCount, BufferAccessMode accessMode = BufferAccessMode::NONE) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
				virtual void Map(void** outDataStructure) override;
				virtual void Unmap() override;
			};

			// Please use ApplyMatrix method to assign matrices to this buffer.
			struct ConstantBufferResourceDX11 : public ConstantBufferResource
			{
				D3D11_MAPPED_SUBRESOURCE _resToMap;
				ID3D11Buffer* _bufferResource = nullptr;

				virtual void Initialize(void* initialData, MSize byteSize, BufferAccessMode accessMode = BufferAccessMode::WRITE) override;
				virtual void Shutdown() override;
				virtual void Set(MInt32 slot = 0) const override;
				virtual void SetVS(MInt32 slot = 0) const override;
				virtual void SetPS(MInt32 slot = 0) const override;
				virtual void SetGS(MInt32 slot = 0) const override;
				virtual void SetHS(MInt32 slot = 0) const override;
				virtual void SetDS(MInt32 slot = 0) const override;
				virtual void SetCS(MInt32 slot = 0) const override;
				virtual void Map(void** outDataStructure) override;
				virtual void Unmap() override;

				virtual void ApplyMatrix(void* destination, const utility::MMatrix* mat) override;
			};

#pragma endregion


			/// <summary>
			/// An implementation of GraphicsDevice using DX11 API.
			/// </summary>
			class GraphicsDeviceDX11 :
				public GraphicsDevice
			{
				friend class GraphicsDevice;
				friend struct RenderTargetResourceDX11;
				friend class RenderTarget2D;
			protected:

#pragma region Protected

				utility::MStack<RenderTarget2D*> _renderTargets;
				utility::MStack<RasterizerState> _stRasterizerStates;
				utility::MStack<BlendState> _stBlendStates;
				utility::MStack<DepthStencilState> _stDepthStencilStates;
				utility::MStack<Viewport> _stViewports;

				utility::MFixedArray<ID3D11RasterizerState*> _rasterizerStates;
				utility::MFixedArray<ID3D11DepthStencilState*> _depthStencilStates;
				utility::MFixedArray<ID3D11BlendState*> _blendStates;

				RenderTarget2D* _mainRenderTarget;
				RenderTarget2D* _currentRenderTarget;
				RasterizerState _currentRasterizerState;
				BlendState _currentBlendState;
				DepthStencilState _currentDepthStencilState;
				Viewport _currentViewport;

				ID3D11Device* _device = nullptr;
				ID3D11DeviceContext* _deviceContext = nullptr;

				IDXGISwapChain* _swapChain = nullptr;

				ID3D11RasterizerState* _rasterizerState = nullptr;
				ID3D11DepthStencilState* _depthStencilState = nullptr;
				ID3D11BlendState* _blendState = nullptr;

				MInt32 _swapChainBufferWidth = 0;
				MInt32 _swapChainBufferHeight = 0;
				MUint32 _syncInterval = 0;
				MUint8 _sampleQuality = 1;

#pragma endregion

#pragma region Functions Protected

				GraphicsDeviceDX11();
				virtual ~GraphicsDeviceDX11();

				inline void SetupStates();

#pragma endregion

			public:

#pragma region Static Const public

				static const MUint32 MAX_MIPMAP_COUNT = 14;

#pragma endregion

#pragma region Functions Public

#pragma region DX11_Specific

				inline ID3D11Device* GetDevice() const { return _device; }
				inline ID3D11DeviceContext* GetDeviceContext() const { return _deviceContext; }
				inline IDXGISwapChain* GetSwapChain() const { return _swapChain; }
				inline ID3D11RasterizerState* GetRasterizerStateDx() const { return _rasterizerState; }
				inline ID3D11DepthStencilState* GetDepthStencilStateDx() const { return _depthStencilState; }
				inline ID3D11BlendState* GetBlendStateDx() const { return _blendState; }
#pragma endregion

#pragma region General

				ME_DEPRECATED_MSG("TODO: Setting correct refresh rate for used monitor.")
				virtual void Initialize() override;
				virtual void Initialize(const Options& options) override;
				virtual void Shutdown() override;
				virtual void Clear() override;
				virtual void Draw(MUint32 indexCount, MUint32 startIndexLocation, MUint32 startVertexLocation) const override;
				virtual void Draw(MUint32 vertexCount) const override;
				virtual void Present() const override;
				virtual void PushRenderTarget() override;
				virtual void PopRenderTarget() override;
				virtual void PushRasterizerState() override;
				virtual void PopRasterizerState() override;
				virtual void PushBlendState() override;
				virtual void PopBlendState() override;
				virtual void PushDepthStencilState() override;
				virtual void PopDepthStencilState() override;
				virtual void PushViewport() override;
				virtual void PopViewport() override;
				virtual void CopyToBackBuffer(const Texture2DResource* tex) override;
				virtual void ClearBoundRenderTargets(MUint32 count) override;
				virtual void ClearBoundShaderResourcesVS(MUint32 startIndex, MUint32 count) override;
				virtual void ClearBoundShaderResourcesPS(MUint32 startIndex, MUint32 count) override;
				virtual void ClearBoundShaderResourcesGS(MUint32 startIndex, MUint32 count) override;
				virtual void ClearBoundShaderResourcesHS(MUint32 startIndex, MUint32 count) override;
				virtual void ClearBoundShaderResourcesDS(MUint32 startIndex, MUint32 count) override;
				virtual void ClearBoundShaderResourcesCS(MUint32 startIndex, MUint32 count) override;
				virtual void ClearBoundVertexIndexBuffers() override;
				virtual void ClearBoundShaderVS() override;
				virtual void ClearBoundShaderPS() override;
				virtual void ClearBoundShaderGS() override;
				virtual void ClearBoundShaderHS() override;
				virtual void ClearBoundShaderDS() override;
				virtual void ClearBoundShaderCS() override;

#pragma endregion

#pragma region GetResources

				virtual void GetVertexShader(VertexShaderResource& outResource) const override;
				virtual void GetPixelShader(PixelShaderResource& outResource) const override;
				virtual void GetHullShader(HullShaderResource& outResource) const override;
				virtual void GetDomainShader(DomainShaderResource& outResource) const override;
				virtual void GetGeometryShader(GeometryShaderResource& outResource) const override;
				virtual void GetComputeShader(ComputeShaderResource& outResource) const override;

				//virtual void GetTextureResources(utility::MArray<Texture1DResource>& outData) const override;
				//virtual void GetTextureResources(utility::MArray<Texture2DResource>& outData) const override;
				//virtual void GetTextureResources(utility::MArray<Texture3DResource>& outData) const override;
				//virtual void GetConstantBufferResources(utility::MArray<ConstantBufferResource>& outData) const override;
				//virtual void GetVertexBufferResources(utility::MArray<VertexBufferResource>& outData) const override;
				//virtual void GetIndexBufferResource(IndexBufferResource& outResource) const override;

				virtual RenderTarget2D* GetRenderTarget() const override;
				virtual RasterizerState GetRasterizerState() const override { return _currentRasterizerState; }
				virtual BlendState GetBlendState() const override { return _currentBlendState; }
				virtual DepthStencilState GetDepthStencilState() const override { return _currentDepthStencilState; }
				virtual Viewport GetViewport() const override { return _currentViewport; }
				
				virtual void SetRenderTarget(RenderTarget2D* rt) override;
				virtual RenderTarget2D* GetMainRenderTarget() const override;
				virtual void ResizeMainRenderTarget(MUint16 width, MUint16 height) override;

				virtual inline MUint8 GetMultisampleQuality() const { return _sampleQuality; }

#pragma endregion

#pragma region AccessorsOptions

				virtual void SetRasterizerState(RasterizerState state) override;
				virtual void SetDepthStencilState(DepthStencilState state) override;
				virtual void SetBlendState(BlendState state) override;
				virtual void SetViewport(const Viewport& viewport) override;
				virtual void SetScissorRect(const ScissorRect& rect) override;
				virtual void SetBackBufferSize(MInt32 width, MInt32 height) override;
				virtual void SetSwapChain(MInt32 width, MInt32 height, MInt32 multisampleCount, SwapEffect swapEffect) override;
				virtual void SetClearMode(ClearMode clearMode) override;
				virtual void SetPrimitiveTopology(PrimitiveTopology pt) override;
				virtual void SetWindow(Window* window) override;
				virtual void SetFullscreen(bool fullscreen) override;
				virtual void SetVSyncEnabled(bool vsync) override;

#pragma endregion

#pragma region Static

				static inline D3D11_TEXTURE2D_DESC CreateDefaultTextureDepthStencilDesc();
				static inline D3D11_SAMPLER_DESC CreateDefaultSamplerDesc(TextureSamplerFormat samplerFormat);

#pragma endregion

#pragma region Factory

				virtual inline VertexShaderResource* CreateVertexShaderInstance() override { return new VertexShaderResourceDX11(); }
				virtual inline PixelShaderResource* CreatePixelShaderInstance() override { return new PixelShaderResourceDX11(); }
				virtual inline GeometryShaderResource* CreateGeometryShaderInstance() override { return new GeometryShaderResourceDX11(); }
				virtual inline HullShaderResource* CreateHullShaderInstance() override { return new HullShaderResourceDX11(); }
				virtual inline DomainShaderResource* CreateDomainShaderInstance() override { return new DomainShaderResourceDX11(); }
				virtual inline ComputeShaderResource* CreateComputeShaderInstance() override { return new ComputeShaderResourceDX11(); }

				virtual inline Texture1DResource* CreateTexture1DInstance() override { return new Texture1DResourceDX11(); }
				virtual inline Texture2DResource* CreateTexture2DInstance() override { return new Texture2DResourceDX11(); }
				virtual inline StagingTexture2DResource* CreateStagingTexture2DInstance() override { return new StagingTexture2DResourceDX11(); }
				virtual inline TextureCubeResource* CreateTextureCubeInstance() override { return new TextureCubeResourceDX11(); }
				virtual inline RenderTargetResource* CreateRenderTargetInstance() override { return new RenderTargetResourceDX11(); }
				virtual inline DepthStencilResource* CreateDepthStencilInstance() override { return new DepthStencilResourceDX11(); }

				virtual inline VertexBufferResource* CreateVertexBufferInstance() override { return new VertexBufferResourceDX11(); }
				virtual inline IndexBufferResource* CreateIndexBufferInstance() override { return new IndexBufferResourceDX11(); }
				virtual inline ConstantBufferResource* CreateConstantBufferInstance() override { return new ConstantBufferResourceDX11(); }

#pragma endregion

#pragma endregion

};
		}
	}
}
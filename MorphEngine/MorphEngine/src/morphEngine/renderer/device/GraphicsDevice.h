#pragma once

#include "core/GlobalDefines.h"
#include "utility/MVector.h"
#include "utility/MArray.h"
#include "utility/MColor.h"

namespace morphEngine
{
	namespace utility
	{
		class MMatrix;
	}

	namespace renderer
	{
		class Window;
		class RenderTarget2D;

		namespace device
		{

#pragma region Formats and Modes

			enum class GraphicDataFormat : MUint8
			{
				FLOAT_R32G32B32A32 = 2,
				FLOAT_R32G32B32 = 6,
				FLOAT_R32G32 = 16,
				FLOAT_R32 = 41,
				FLOAT_R16G16B16A16 = 10,
				FLOAT_R16G16 = 34,
				FLOAT_R16 = 54,
				SNORM_R16G16B16A16 = 13,
				SNORM_R8G8B8A8 = 31,
				UNORM_R8G8B8A8 = 28,
				UNORM_R8G8 = 49,
				UNORM_R8 = 61,
				UNORM_D24_UINT_S8 = 45,
				UINT_R8G8B8A8 = 30,
				UINT_R8G8 = 50,
				UINT_R8 = 62,
				UINT_R32G32 = 17,
				SINT_R32G32B32A32 = 4
			};

			enum class TextureSamplerFormat : MUint8
			{
				ADDRESS_WRAP_FILTER_LINEAR = 0,
				ADDRESS_CLAMP_FILTER_LINEAR,
				ADDRESS_WRAP_FILTER_NEAREST,
				ADDRESS_CLAMP_FILTER_NEAREST,
				ADDRESS_WRAP_FILTER_ANISOTROPIC,
				ADDRESS_CLAMP_FILTER_ANISOTROPIC
			};

			enum class BufferAccessMode : MUint8
			{
				NONE,
				READ,
				WRITE,
				READ_WRITE
			};

#pragma endregion

#pragma region ResourceTypes

			struct DeviceResource
			{
				virtual void Shutdown() = 0;
				virtual void Set(MInt32 slot = 0) const = 0;
			};

			// here vertex input layout also is stored.
			struct VertexShaderResource : public DeviceResource
			{
				// All pointers must contain arrays of size layoutElementCount
				// Layout must contain at least one element and no more than 16.
				virtual void Initialize(
					MUint8* byteCodePtr, MSize byteCodeLength,
					MSize layoutElementCount,
					utility::MString* layoutNames,
					MUint8* layoutIndices,
					GraphicDataFormat* formats) = 0;
			};

			struct PixelShaderResource : public DeviceResource
			{
				virtual void Initialize(MUint8* byteCodePtr, MSize byteCodeLength) = 0;
			};

			struct HullShaderResource : public DeviceResource
			{
				virtual void Initialize(MUint8* byteCodePtr, MSize byteCodeLength) = 0;
			};

			struct DomainShaderResource : public DeviceResource
			{
				virtual void Initialize(MUint8* byteCodePtr, MSize byteCodeLength) = 0;
			};

			struct GeometryShaderResource : public DeviceResource
			{
				virtual void Initialize(MUint8* byteCodePtr, MSize byteCodeLength) = 0;
			};

			struct ComputeShaderResource : public DeviceResource
			{
				virtual void Initialize(MUint8* byteCodePtr, MSize byteCodeLength) = 0;
			};

			struct Texture1DResource : public DeviceResource
			{
				virtual void Initialize(MUint8* dataPtr, MUint32 width, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat) = 0;

				// Sets this texture for current Pixel Shader.
				virtual void Set(MInt32 slot = 0) const = 0;
				virtual void SetVS(MInt32 slot = 0) const = 0;
				virtual void SetPS(MInt32 slot = 0) const = 0;
				virtual void SetGS(MInt32 slot = 0) const = 0;
				virtual void SetHS(MInt32 slot = 0) const = 0;
				virtual void SetDS(MInt32 slot = 0) const = 0;
				virtual void SetCS(MInt32 slot = 0) const = 0;
			};

			struct Texture2DResource : public DeviceResource
			{
				virtual void Initialize(MUint8* dataPtr, MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat,
					bool compressed, MUint8 mipmapCount) = 0;

				// Sets this texture for current Pixel Shader.
				virtual void Set(MInt32 slot = 0) const = 0;
				virtual void SetVS(MInt32 slot = 0) const = 0;
				virtual void SetPS(MInt32 slot = 0) const = 0;
				virtual void SetGS(MInt32 slot = 0) const = 0;
				virtual void SetHS(MInt32 slot = 0) const = 0;
				virtual void SetDS(MInt32 slot = 0) const = 0;
				virtual void SetCS(MInt32 slot = 0) const = 0;
			};

			// Main purpose of this resource is to copy any rectangle from any Texture2D to CPU memory
			struct StagingTexture2DResource : public DeviceResource
			{
				MUint32 OriginX;
				MUint32 OriginY; 
				MUint32 RectWidth; 
				MUint32 RectHeight;
				GraphicDataFormat Format;
				virtual void Initialize(MUint32 originX, MUint32 originY, MUint32 rectWidth, MUint32 rectHeight,
					GraphicDataFormat format) 
				{
					OriginX = originX; OriginY = originY; RectWidth = rectWidth; RectHeight = rectHeight; Format = format;
				}

				virtual void CopyTextureData(const Texture2DResource* resource) = 0;
				virtual void UpdateRectSize() = 0;

				virtual void Map(void** outDataStructure) = 0;
				virtual void Unmap() = 0;

				// Sets this texture for current Pixel Shader.
				virtual void Set(MInt32 slot = 0) const override { }
			};

			struct TextureCubeResource : public DeviceResource
			{
				virtual void Initialize(
					MUint8* dataPtrPlusX, 
					MUint8* dataPtrPlusY,
					MUint8* dataPtrPlusZ,
					MUint8* dataPtrMinusX,
					MUint8* dataPtrMinusY,
					MUint8* dataPtrMinusZ,
					MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize, GraphicDataFormat format, TextureSamplerFormat samplerFormat, bool compressed, MUint8 mipmapCount) = 0;

				// Sets this texture for current Pixel Shader.
				virtual void Set(MInt32 slot = 0) const = 0;
				virtual void SetVS(MInt32 slot = 0) const = 0;
				virtual void SetPS(MInt32 slot = 0) const = 0;
				virtual void SetGS(MInt32 slot = 0) const = 0;
				virtual void SetHS(MInt32 slot = 0) const = 0;
				virtual void SetDS(MInt32 slot = 0) const = 0;
				virtual void SetCS(MInt32 slot = 0) const = 0;
			};

			struct DepthStencilResource : public Texture2DResource
			{
				virtual void Initialize(MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize = 4, GraphicDataFormat format = GraphicDataFormat::UNORM_D24_UINT_S8) = 0;
				virtual void Clear(float depthValue = 1.0f, MUint8 stencilValue = 0xFF) = 0;
			};

			struct RenderTargetResource : public Texture2DResource
			{
				virtual void Initialize(MUint32 texWidth, MUint32 texHeight, MUint8 pixelSize = 4, GraphicDataFormat format = GraphicDataFormat::UNORM_R8G8B8A8, bool srv = true) = 0;
				// Binds this render target to a device context. From now on the device will render into it.
				// You usually may want to call PushRenderTarget before this and PopRenderTarget sometime after.
				// DepthStencilResource can be nullptr
				virtual void Bind(const DepthStencilResource* depthStencil) = 0;
				// DepthStencilResource can be nullptr
				virtual void BindMultiple(const RenderTargetResource*const* otherRenderTargets, MSize otherRenderTargetsCount, const DepthStencilResource* depthStencil) = 0;
				virtual void Clear(const utility::MColor& color = utility::MColor(0.0f, 0.0f, 0.0f, 0.0f)) = 0;
			};

			struct VertexBufferResource : public DeviceResource
			{
				BufferAccessMode _accessMode = BufferAccessMode::NONE;

				virtual void Initialize(MFloat32* dataPtr, MSize vertexSize, MSize vertexCount, BufferAccessMode accessMode = BufferAccessMode::NONE) = 0;
				// OutDataStructure is a ref pointer to a data structure associated with this constant buffer.
				// A pointer to this buffer contents in GPU memory will be assigned, making it editable from CPU side.
				// Remember to call Unmap after editing this buffer.
				virtual void Map(void** outDataStructure) = 0;
				virtual void Unmap() = 0;
				virtual void Update(void* data, MSize dataSizeBytes) = 0;
			};

			struct IndexBufferResource : public DeviceResource
			{
				BufferAccessMode _accessMode = BufferAccessMode::NONE;

				virtual void Initialize(MUint16* dataPtr, MSize indexCount, BufferAccessMode accessMode = BufferAccessMode::NONE) = 0;
				// OutDataStructure is a ref pointer to a data structure associated with this constant buffer.
				// A pointer to this buffer contents in GPU memory will be assigned, making it editable from CPU side.
				// Remember to call Unmap after editing this buffer.
				virtual void Map(void** outDataStructure) = 0;
				virtual void Unmap() = 0;
			};

			// Please use ApplyMatrix method to assign matrices to this buffer.
			struct ConstantBufferResource : public DeviceResource
			{
				BufferAccessMode _accessMode = BufferAccessMode::WRITE;

				virtual void Initialize(void* initialData, MSize byteSize, BufferAccessMode accessMode = BufferAccessMode::WRITE) = 0;
				// OutDataStructure is a ref pointer to a data structure associated with this constant buffer.
				// A pointer to this buffer contents in GPU memory will be assigned, making it editable from CPU side.
				// Remember to call Unmap after editing this buffer.
				virtual void Map(void** outDataStructure) = 0;
				virtual void Unmap() = 0;
				// Sets this constant buffer in a default way - for current vertex and pixel shaders.
				virtual void Set(MInt32 slot = 0) const = 0;
				virtual void SetVS(MInt32 slot = 0) const = 0;
				virtual void SetPS(MInt32 slot = 0) const = 0;
				virtual void SetGS(MInt32 slot = 0) const = 0;
				virtual void SetHS(MInt32 slot = 0) const = 0;
				virtual void SetDS(MInt32 slot = 0) const = 0;
				virtual void SetCS(MInt32 slot = 0) const = 0;

				// Please use this function to assign a matrix to the constant buffer, as different implementations may vary in this matter.
				// Destination is a pointer to a place in constant buffer data structure where matrix shall be stored.
				virtual void ApplyMatrix(void* destination, const utility::MMatrix* mat) = 0;
			};

#pragma endregion

			/// <summary>
			/// An interface for currently used graphics device.
			/// </summary>
			class GraphicsDevice
			{
			public:

#pragma region Classes Public

				enum class Type : uint8_t
				{
					DX11,
					OGL,
					GLES,
					VULKAN,
					UNKNOWN
				};

#pragma region Options Related

				enum class SwapEffect : uint8_t
				{
					DISCARD = 0,
					SEQUENTIAL = 1,
					FLIP_SEQUENTIAL = 3
				};

				enum class CullMode : uint8_t
				{
					NONE = 1,
					FRONT = 2,
					BACK = 3
				};

				enum class FillMode : uint8_t
				{
					SOLID = 3,
					WIREFRAME = 2
				};

				enum class PrimitiveTopology : uint8_t
				{
					POINTS = 1,
					LINES = 2,
					TRIANGLE_LIST = 4,
					TRIANGLE_STRIP = 5
				};

				enum class ClearMode : uint8_t
				{
					NO_CLEAR,
					CLEAR_COLOR,
					CLEAR_DEPTH,
					CLEAR_COLOR_AND_DEPTH,
					CLEAR_DEPTH_NO_STENCIL,
					CLEAR_COLOR_AND_DEPTH_NO_STENCIL
				};

				enum class RasterizerState : uint8_t
				{
					CULL_BACK_FILL_SOLID_DEPTH_ON_SCISSOR_OFF = 0,
					CULL_BACK_FILL_SOLID_DEPTH_ON_SCISSOR_ON,
					CULL_BACK_FILL_SOLID_DEPTH_OFF_SCISSOR_ON,
					CULL_FRONT_FILL_SOLID_DEPTH_ON_SCISSOR_OFF,
					CULL_NONE_FILL_SOLID_DEPTH_ON_SCISSOR_OFF,
					CULL_NONE_FILL_WIRE_DEPTH_OFF_SCISSOR_OFF
				};

				enum class DepthStencilState : uint8_t
				{
					DEPTH_ENABLED_STENCIL_DISABLED = 0,
					DEPTH_ENABLED_STENCIL_ENABLED,
					DEPTH_DISABLED_STENCIL_ENABLED,
					DEPTH_DISABLED_STENCIL_DISABLED
				};

				enum class BlendState : uint8_t
				{
					SOLID = 0,
					ALPHA,
					ADDITIVE
				};

				struct Viewport
				{
					MFloat32 _width = 0.0f;
					MFloat32 _height = 0.0f;
					utility::MVector2 _topLeft = utility::MVector2(0.0f, 0.0f);
					MFloat32 _minDepth = 0.0f;
					MFloat32 _maxDepth = 1.0f;

					bool operator==(const Viewport& o) const { return _width == o._width && _height == o._height && _topLeft == o._topLeft && _minDepth == o._minDepth && _maxDepth == o._maxDepth; }
					bool operator!=(const Viewport& o) const { return !(operator==(o)); }
				};

				struct ScissorRect
				{
					MInt64 _left = 0;
					MInt64 _top = 0;
					MInt64 _right = 0;
					MInt64 _bottom = 0;
				};

				// If window pointer is set to null, one will be automatically obtained from Engine singleton.
				struct Options
				{
					utility::MColor _clearColor;
					Viewport _viewport;
					ScissorRect _scissorRect;
					RasterizerState _rasterizerState = static_cast<RasterizerState>(0);
					DepthStencilState _depthStencilState = static_cast<DepthStencilState>(0);
					BlendState _blendState = static_cast<BlendState>(0);
					SwapEffect _swapEffect = SwapEffect::DISCARD;
					ClearMode _clearMode = ClearMode::CLEAR_COLOR_AND_DEPTH;
					PrimitiveTopology _primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
					MInt32 _multisampleCount = 1;
					Window* _window = nullptr;
					bool _bVsync = false;
#ifdef _DEBUG
					bool _bDebugMode = true;
#else
					bool _bDebugMode = false;
#endif // _DEBUG
				};

#pragma endregion

#pragma endregion

			protected:

#pragma region Structs Protected

				struct RenderTargetDepthBufferPair
				{
					RenderTargetResource* _rt;
					DepthStencilResource* _ds;

					RenderTargetDepthBufferPair(RenderTargetResource* rt = nullptr, DepthStencilResource* ds = nullptr) :
						_rt(rt),
						_ds(ds)
					{
					}

					RenderTargetDepthBufferPair(const RenderTargetDepthBufferPair& other) :
						_rt(other._rt),
						_ds(other._ds)
					{
					}

					inline bool operator==(const RenderTargetDepthBufferPair& other)
					{
						return _rt == other._rt && _ds == other._ds;
					}

					inline bool operator!=(const RenderTargetDepthBufferPair& other)
					{
						return !(operator==(other));
					}

					inline RenderTargetDepthBufferPair& operator=(const RenderTargetDepthBufferPair& other)
					{
						_rt = other._rt;
						_ds = other._ds;
						return *this;
					}
				};

#pragma endregion

#pragma region Protected

				static const MInt32 RASTERIZER_STATES_COUNT = 6;
				static const MInt32 DEPTHSTENCIL_STATES_COUNT = 4;
				static const MInt32 BLEND_STATES_COUNT = 3;

				Options _options;
				Type _type = Type::UNKNOWN;

#pragma endregion

#pragma region Functions Protected

				GraphicsDevice();
				virtual ~GraphicsDevice();

				RenderTarget2D* CreateRenderTarget(MInt32 width, MInt32 height, RenderTargetResource* extRes);
				void DeleteRenderTarget(RenderTarget2D* toDel);

#pragma endregion
			public:

#pragma region Functions Public

#pragma region General

				virtual void Initialize() = 0;
				virtual void Initialize(const Options& options) = 0;
				virtual void Shutdown() = 0;
				// Clears current render taget, according to currently set ClearMode and ClearColor.
				virtual void Clear() = 0;
				// Draws bound vertex buffers using bound index buffer, with all currently bound shaders, buffers and resources, to current render target.
				virtual void Draw(MUint32 indexCount, MUint32 startIndexLocation, MUint32 startVertexLocation) const = 0;
				// Draws bound vertex buffers without index buffer, with all currently bound shaders, buffers and resources, to current render target.
				virtual void Draw(MUint32 vertexCount) const = 0;
				// Presents current render target to the window. This function usually crowns frame rendering process.
				// In most cases you need to make sure that main render target is set as the current one before calling this.
				virtual void Present() const = 0;
				// Pushes current render target to the stack, so it can be retrieved later, when drawing to a newly set one is finished.
				virtual void PushRenderTarget() = 0;
				// Pops a render target from the stack. If the stack is empty, main render target is set as the current one.
				virtual void PopRenderTarget() = 0;

				virtual void PushRasterizerState() = 0;
				virtual void PopRasterizerState() = 0;
				virtual void PushBlendState() = 0; 
				virtual void PopBlendState() = 0;
				virtual void PushDepthStencilState() = 0;
				virtual void PopDepthStencilState() = 0;
				virtual void PushViewport() = 0;
				virtual void PopViewport() = 0;

				// Copies given texture resource data to back buffer texture data
				virtual void CopyToBackBuffer(const Texture2DResource* tex) = 0;

				// A shortcut for clearing bound render target states
				virtual void ClearBoundRenderTargets(MUint32 count) = 0;
				// A shortcut for clearing bound shader resources
				virtual void ClearBoundShaderResourcesVS(MUint32 startIndex, MUint32 count) = 0;
				// A shortcut for clearing bound shader resources
				virtual void ClearBoundShaderResourcesPS(MUint32 startIndex, MUint32 count) = 0;
				// A shortcut for clearing bound shader resources
				virtual void ClearBoundShaderResourcesHS(MUint32 startIndex, MUint32 count) = 0;
				// A shortcut for clearing bound shader resources
				virtual void ClearBoundShaderResourcesDS(MUint32 startIndex, MUint32 count) = 0;
				// A shortcut for clearing bound shader resources
				virtual void ClearBoundShaderResourcesGS(MUint32 startIndex, MUint32 count) = 0;
				// A shortcut for clearing bound shader resources
				virtual void ClearBoundShaderResourcesCS(MUint32 startIndex, MUint32 count) = 0;
				// A shortcut for clearing bound vertex and index buffers
				virtual void ClearBoundVertexIndexBuffers() = 0;

				virtual void ClearBoundShaderVS() = 0;
				virtual void ClearBoundShaderPS() = 0;
				virtual void ClearBoundShaderGS() = 0;
				virtual void ClearBoundShaderHS() = 0;
				virtual void ClearBoundShaderDS() = 0;
				virtual void ClearBoundShaderCS() = 0;


#pragma endregion

#pragma region GetSetResources

				// Gets shader currently bound to graphics device context.
				virtual void GetVertexShader(VertexShaderResource& outResource) const = 0;
				// Gets shader currently bound to graphics device context.
				virtual void GetPixelShader(PixelShaderResource& outResource) const = 0;
				// Gets shader currently bound to graphics device context.
				virtual void GetHullShader(HullShaderResource& outResource) const = 0;
				// Gets shader currently bound to graphics device context.
				virtual void GetDomainShader(DomainShaderResource& outResource) const = 0;
				// Gets shader currently bound to graphics device context.
				virtual void GetGeometryShader(GeometryShaderResource& outResource) const = 0;
				// Gets shader currently bound to graphics device context.
				virtual void GetComputeShader(ComputeShaderResource& outResource) const = 0;

				//virtual void GetTextureResources(utility::MArray<Texture1DResource>& outData) const = 0;
				//virtual void GetTextureResources(utility::MArray<Texture2DResource>& outData) const = 0;
				//virtual void GetTextureResources(utility::MArray<Texture3DResource>& outData) const = 0;
				//virtual void GetConstantBufferResources(utility::MArray<ConstantBufferResource>& outData) const = 0;
				//virtual void GetVertexBufferResources(utility::MArray<VertexBufferResource>& outData) const = 0;
				//virtual void GetIndexBufferResource(IndexBufferResource& outResource) const = 0;

				// Gets current render target which device renders to.
				virtual RenderTarget2D* GetRenderTarget() const = 0;
				virtual RasterizerState GetRasterizerState() const = 0;
				virtual BlendState GetBlendState() const = 0;
				virtual DepthStencilState GetDepthStencilState() const = 0;
				virtual Viewport GetViewport() const = 0;

				// Gets main render target of graphics device.
				virtual RenderTarget2D* GetMainRenderTarget() const = 0;
				// This function only sets 'currentrendertarget' field without binding it to the output. You generally shouldn't call it manually.
				virtual void SetRenderTarget(RenderTarget2D* rt) = 0;
				// Used for resizing main render target only
				virtual void ResizeMainRenderTarget(MUint16 width, MUint16 height) = 0;
				virtual MUint8 GetMultisampleQuality() const = 0;

#pragma endregion

#pragma region AccessorsOptions

				const Options& GetCurrentOptions() { return _options; }

				virtual void SetRasterizerState(RasterizerState state) = 0;
				virtual void SetDepthStencilState(DepthStencilState state) = 0;
				virtual void SetBlendState(BlendState state) = 0;

				// Sets viewport parameters. See viewport struct definition for details.
				virtual void SetViewport(const Viewport& viewport) = 0;
				virtual void SetScissorRect(const ScissorRect& rect) = 0;

				// Resize back buffer render target texture and depthstencil map.
				// Does not cause swap chain to be recreated, so it can safely be used in runtime, i.e. when window resizes.
				virtual void SetBackBufferSize(MInt32 width, MInt32 height) = 0;

				// Creates new swap chain for this device. Should not be used in runtime, as this causes current
				// swap chain to be destroyed and creates new one.
				virtual void SetSwapChain(MInt32 width, MInt32 height, MInt32 multisampleCount, SwapEffect swapEffect) = 0;
				virtual void SetClearMode(ClearMode clearMode) = 0;
				virtual void SetPrimitiveTopology(PrimitiveTopology pt) = 0;

				// Changes window the graphics device renders to. Should not be used in runtime, as it causes swap chain to be recreated.
				virtual void SetWindow(Window* window) = 0;
				virtual void SetFullscreen(bool fullscreen) = 0;
				virtual void SetVSyncEnabled(bool vsync) = 0;

#pragma endregion

#pragma region Factory

				virtual inline VertexShaderResource* CreateVertexShaderInstance() = 0;
				virtual inline PixelShaderResource* CreatePixelShaderInstance() = 0;
				virtual inline GeometryShaderResource* CreateGeometryShaderInstance() = 0;
				virtual inline HullShaderResource* CreateHullShaderInstance() = 0;
				virtual inline DomainShaderResource* CreateDomainShaderInstance() = 0;
				virtual inline ComputeShaderResource* CreateComputeShaderInstance() = 0;

				virtual inline Texture1DResource* CreateTexture1DInstance() = 0;
				virtual inline Texture2DResource* CreateTexture2DInstance() = 0;
				virtual inline StagingTexture2DResource* CreateStagingTexture2DInstance() = 0;
				virtual inline TextureCubeResource* CreateTextureCubeInstance() = 0;
				virtual inline RenderTargetResource* CreateRenderTargetInstance() = 0;
				virtual inline DepthStencilResource* CreateDepthStencilInstance() = 0;

				virtual inline VertexBufferResource* CreateVertexBufferInstance() = 0;
				virtual inline IndexBufferResource* CreateIndexBufferInstance() = 0;
				virtual inline ConstantBufferResource* CreateConstantBufferInstance() = 0;


				ME_DEPRECATED_MSG("TODO: Implement settings and choose device based on it.")
				static GraphicsDevice* CreateInstance();
				static void DestroyInstance(GraphicsDevice* dev);

#pragma endregion

#pragma endregion

			};
		}

	}
}
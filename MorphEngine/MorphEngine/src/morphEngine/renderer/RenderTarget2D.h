#pragma once

#include "assetLibrary/MTexture2D.h"
#include "renderer/device/GraphicsDevice.h"
#include "renderer/RenderingManager.h"

namespace morphEngine
{
	namespace renderer
	{

		class RenderTarget2D :
			public assetLibrary::MTexture2D
		{
			friend class RenderingManager;
			friend class device::GraphicsDevice;
		protected:

			device::DepthStencilResource* _dsRes = nullptr;
			device::GraphicDataFormat _dsFormat;
			bool _bResizesWithWindow = false;

			virtual inline void InitializeFromRawData() override;
			virtual inline void Shutdown() override;

			RenderTarget2D(MUint16 width, MUint16 height, device::GraphicDataFormat format, bool depthBufferEnabled = true, 
				device::GraphicDataFormat depthBufferFormat = device::GraphicDataFormat::UNORM_D24_UINT_S8);
			RenderTarget2D(MUint16 width, MUint16 height, device::GraphicDataFormat format, device::RenderTargetResource* extRes, bool depthBufferEnabled = true,
				device::GraphicDataFormat depthBufferFormat = device::GraphicDataFormat::UNORM_D24_UINT_S8);
			inline RenderTarget2D(assetLibrary::MAssetType type, const utility::MFixedArray<MUint8>& data) : assetLibrary::MTexture2D(type, data) { }

			virtual ~RenderTarget2D();
		public:

			inline RenderTarget2D(const RenderTarget2D& other) : assetLibrary::MTexture2D(other) { }

			// ACHTUNG! On resizing RenderTarget, all current data is lost.
			void Resize(MUint16 newWidth, MUint16 newHeight);
			inline void PrepareForResizingMainOnly() { _res->Shutdown(); delete _res; _dsRes->Shutdown(); delete _dsRes; }
			void SetNewResourceAndRecreateDepth(device::RenderTargetResource* res, MUint32 nWidth, MUint32 nHeight);

			inline void ClearNoDepth(const utility::MColor& color = utility::MColor::Black)
			{
				reinterpret_cast<device::RenderTargetResource*>(_res)->Clear(color);
			}

			inline void ClearWithDepth(const utility::MColor& color = utility::MColor::Black, float depthValue = 1.0f, MUint8 stencilValue = 0xFF)
			{
				reinterpret_cast<device::RenderTargetResource*>(_res)->Clear(color);
				_dsRes->Clear(depthValue, stencilValue);
			}

			inline void SetAsInputWithDepth(const MUint8 slotRt, const MUint8 slotDepth) const
			{
				reinterpret_cast<device::RenderTargetResource*>(_res)->SetPS(slotRt);
				_dsRes->SetPS(slotDepth);
			}

			inline void SetAsOutput() const 
			{ 
				reinterpret_cast<device::RenderTargetResource*>(_res)->Bind(_dsRes); 
				RenderingManager::GetInstance()->GetDevice()->SetRenderTarget(const_cast<RenderTarget2D*>(this));
			}

			inline void SetAsOutputWithExternalDepthBuffer(const device::DepthStencilResource* depth)
			{
				reinterpret_cast<device::RenderTargetResource*>(_res)->Bind(depth);
				RenderingManager::GetInstance()->GetDevice()->SetRenderTarget(const_cast<RenderTarget2D*>(this));
			}

			static inline void SetMultipleAsOutput(RenderTarget2D** renderTargets, const MSize renderTargetCount, const MSize depthBufferIndex = 0)
			{
				ME_ASSERT_S(renderTargetCount > 1 && renderTargets != nullptr);
				utility::MFixedArray<device::RenderTargetResource*> resources(renderTargetCount - 1);
				for (MSize i = 1; i < renderTargetCount; ++i)
				{
					device::RenderTargetResource* rt = reinterpret_cast<device::RenderTargetResource*>(renderTargets[i]->_res);
					resources[i - 1] = rt;
				}
				RenderingManager::GetInstance()->GetDevice()->SetRenderTarget(renderTargets[0]);
				const device::RenderTargetResource*const* ptrs = resources.GetDataPointer();
				reinterpret_cast<device::RenderTargetResource*>(renderTargets[0]->_res)->BindMultiple(ptrs, renderTargetCount - 1,
					renderTargets[depthBufferIndex]->_dsRes);
			}

			inline bool IsDepthBufferEnabled() { return _dsRes != nullptr; }

			inline const device::DepthStencilResource* GetDepthResource() const { return _dsRes; }
			inline bool GetResizesWithWindow() const { return _bResizesWithWindow; }
			inline void SetResizesWithWindow(bool res) { _bResizesWithWindow = res; }
		};

		struct RenderTarget2DPair
		{
			RenderTarget2D* First;
			RenderTarget2D* Second;

			RenderTarget2DPair(RenderTarget2D* first = nullptr, RenderTarget2D* second = nullptr) :
				First(first),
				Second(second)
			{

			}

			RenderTarget2DPair(const RenderTarget2DPair& copy) :
				First(copy.First),
				Second(copy.Second)
			{

			}

			inline RenderTarget2D* operator->()
			{
				return First;
			}

			inline RenderTarget2D& operator*()
			{
				return *First;
			}

			inline operator RenderTarget2D*()
			{
				return First;
			}

			inline void Flip()
			{
				RenderTarget2D* tmp = First;
				First = Second;
				Second = tmp;
			}
		};
	}
}
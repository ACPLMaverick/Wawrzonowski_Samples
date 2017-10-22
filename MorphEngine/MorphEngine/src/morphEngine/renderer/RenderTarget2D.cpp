#include "RenderTarget2D.h"

using namespace morphEngine::assetLibrary;
using namespace morphEngine::renderer::device;
namespace morphEngine
{
	namespace renderer
	{
		RenderTarget2D::RenderTarget2D(MUint16 width, MUint16 height, device::GraphicDataFormat format, bool depthBufferEnabled,
			device::GraphicDataFormat depthBufferFormat) :
			MTexture2D()
		{
			_width = width;
			_height = height;
			_dataFormat = format;
			_bpp = 32;
			_bCompressed = false;
			_bReadOnly = true;
			_mipmapCount = 1;
			if (depthBufferEnabled)
			{
				_dsRes = reinterpret_cast<DepthStencilResource*>(depthBufferFormat);
				_dsFormat = depthBufferFormat;
			}
		}

		RenderTarget2D::RenderTarget2D(MUint16 width, MUint16 height, device::GraphicDataFormat format, device::RenderTargetResource * extRes, bool depthBufferEnabled, device::GraphicDataFormat depthBufferFormat) :
			RenderTarget2D(width, height, format, depthBufferEnabled, depthBufferFormat)
		{
			_res = extRes;
		}

		RenderTarget2D::~RenderTarget2D()
		{
			Shutdown();
		}

		void RenderTarget2D::Resize(MUint16 newWidth, MUint16 newHeight)
		{
			if (this == RenderingManager::GetInstance()->GetDevice()->GetMainRenderTarget())
			{
				RenderingManager::GetInstance()->GetDevice()->ResizeMainRenderTarget(newWidth, newHeight);
			}
			else
			{
				_width = newWidth;
				_height = newHeight;

				reinterpret_cast<RenderTargetResource*>(_res)->Shutdown();
				reinterpret_cast<RenderTargetResource*>(_res)->Initialize(_width, _height, _bpp, _dataFormat);

				if (_dsRes != nullptr)
				{
					_dsRes->Shutdown();
					_dsRes->Initialize(_width, _height, _bpp, _dsFormat);
				}

				if (RenderingManager::GetInstance()->GetDevice()->GetRenderTarget() == this)
				{
					SetAsOutput();
				}
			}
		}

		void RenderTarget2D::SetNewResourceAndRecreateDepth(device::RenderTargetResource * res, MUint32 nWidth, MUint32 nHeight)
		{
			_width = nWidth;
			_height = nHeight;
			_res = res;
			_dsRes = RenderingManager::GetInstance()->GetDevice()->CreateDepthStencilInstance();
			_dsRes->Initialize(_width, _height, _bpp, _dsFormat);
		}

		inline void RenderTarget2D::InitializeFromRawData()
		{
			if (_res == nullptr)
			{
				_res = RenderingManager::GetInstance()->GetDevice()->CreateRenderTargetInstance();
				if (_rawData.GetSize() != 0)
				{
					_res->Initialize(_rawData.GetDataPointer(), _width, _height, _bpp, _dataFormat, _samplerFormat, false, 1);
				}
				else
				{
					reinterpret_cast<RenderTargetResource*>(_res)->Initialize(_width, _height, _bpp, _dataFormat);
				}
			}

			if (_dsRes != nullptr)
			{
				_dsRes = RenderingManager::GetInstance()->GetDevice()->CreateDepthStencilInstance();
				_dsRes->Initialize(_width, _height, _bpp, _dsFormat);
			}

			if (_bReadOnly)
			{
				_rawData.Deallocate();
			}

			if (_dsRes != nullptr)
				ClearWithDepth(utility::MColor::Black);
			else
				ClearNoDepth(utility::MColor::Black);
		}

		inline void RenderTarget2D::Shutdown()
		{
			if (_dsRes != nullptr)
			{
				_dsRes->Shutdown();
				delete _dsRes;
				_dsRes = nullptr;
			}
		}
	}
}
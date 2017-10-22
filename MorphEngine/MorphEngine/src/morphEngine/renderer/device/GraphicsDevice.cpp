#include "GraphicsDevice.h"
#include "core/GlobalDefines.h"
#include "renderer/RenderTarget2D.h"

#include "GraphicsDeviceDX11.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace device
		{
			GraphicsDevice::GraphicsDevice()
			{
			}


			GraphicsDevice::~GraphicsDevice()
			{
			}

			RenderTarget2D * GraphicsDevice::CreateRenderTarget(MInt32 width, MInt32 height, RenderTargetResource* extRes)
			{
				return new RenderTarget2D(static_cast<MUint16>(width), static_cast<MUint16>(height), GraphicDataFormat::UNORM_R8G8B8A8, extRes, true, GraphicDataFormat::UNORM_D24_UINT_S8);
			}

			void GraphicsDevice::DeleteRenderTarget(RenderTarget2D * toDel)
			{
				delete toDel;
			}

			GraphicsDevice * GraphicsDevice::CreateInstance()
			{
#ifdef PLATFORM_WINDOWS

				return new GraphicsDeviceDX11();

#else

				return nullptr;

#endif // PLATFORM_WINDOWS
			}
			void GraphicsDevice::DestroyInstance(GraphicsDevice * dev)
			{
				delete dev;
			}
		}
	}
}
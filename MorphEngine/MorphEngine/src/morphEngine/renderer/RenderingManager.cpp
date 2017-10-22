#include "RenderingManager.h"
#include "device/GraphicsDevice.h"
#include "core/Engine.h"
#include "tests/rendererTests.h"
#include "resourceManagement/ResourceManager.h"
#include "RenderTarget2D.h"
#include "core/MessageSystem.h"
#include "gom/SceneManager.h"

namespace morphEngine
{
	namespace renderer
	{

		RenderingManager::RenderingManager()
		{
		}


		RenderingManager::~RenderingManager()
		{
		}

		void RenderingManager::Initialize()
		{
			_device = device::GraphicsDevice::CreateInstance();
			_device->Initialize();
			core::MessageSystem::OnWindowResize += new core::Event<void, MInt32, MInt32>::ClassRawDelegate<RenderingManager>(this, &RenderingManager::OnWindowResize);
		}

		void RenderingManager::Shutdown()
		{
			for (auto it = _renderTargets.GetBegin(); it.IsValid(); ++it)
			{
				delete (*it);
			}

			_device->Shutdown();
			device::GraphicsDevice::DestroyInstance(_device);
		}

		void RenderingManager::Draw()
		{
			_device->Clear();

			if (_rtResizeData.IsValid())
			{
				ResizeRenderTargets();
				_rtResizeData.Clear();
			}

			gom::SceneManager::GetInstance()->Draw();

			_device->Present();

#if _DEBUG
			// update statistics if debug
			if (_stats.IsValid())
			{
				_stats->_data.Fps = core::Engine::GetInstance()->GetGlobalTime().GetFPS();
				_stats->_data.Ms = core::Engine::GetInstance()->GetGlobalTime().GetDeltaTime();
			}
#endif
		}

		void RenderingManager::DrawEditor()
		{
			_device->Clear();

			if (_rtResizeData.IsValid())
			{
				ResizeRenderTargets();
				_rtResizeData.Clear();
			}

			gom::SceneManager::GetInstance()->DrawEditor();

			_device->Present();
		}

		RenderTarget2D * RenderingManager::CreateRenderTarget(MUint16 width, MUint16 height, device::GraphicDataFormat format, bool depthBufferEnabled, device::GraphicDataFormat depthBufferFormat)
		{
			RenderTarget2D* rt = new RenderTarget2D(width, height, format, depthBufferEnabled, depthBufferFormat);
			_renderTargets.Add(rt);
			return rt;
		}

		void RenderingManager::DestroyRenderTarget(RenderTarget2D * rt)
		{
			delete rt;
			_renderTargets.Remove(rt);
		}

		void RenderingManager::RegisterRenderStatisticsComponent(memoryManagement::Handle<debugging::Statistics> stats)
		{
			ME_WARNING(!_stats.IsValid(), "Registering statistics component while one is already registered. This shouldn't happen.");
			_stats = stats;
		}

		void RenderingManager::AddSetGlobalCall()
		{
			if (_stats.IsValid())
			{
				++_stats->_data.SetGlobalCount;
			}
		}

		void RenderingManager::AddSetMaterialCall()
		{
			if (_stats.IsValid())
			{
				++_stats->_data.SetMaterialCount;
			}
		}

		void RenderingManager::AddDrawCall(MSize vertices, MSize indices)
		{
			if (_stats.IsValid())
			{
				++_stats->_data.DrawCallCount;
				_stats->_data.VertexCount += vertices;
				_stats->_data.TriangleCount += indices / 3;
			}
		}

		void RenderingManager::OnWindowResize(MInt32 nWidth, MInt32 nHeight)
		{
			_rtResizeData = RenderTargetResizeData(static_cast<MUint16>(nWidth), static_cast<MUint16>(nHeight));
		}

		void RenderingManager::ResizeRenderTargets()
		{
			_device->ResizeMainRenderTarget(static_cast<MUint16>(_rtResizeData.NewWidth), static_cast<MUint16>(_rtResizeData.NewHeight));
			for (auto it = _renderTargets.GetBegin(); it.IsValid(); ++it)
			{
				if ((*it)->GetResizesWithWindow()) (*it)->Resize(_rtResizeData.NewWidth, _rtResizeData.NewHeight);
			}
		}

	}
}
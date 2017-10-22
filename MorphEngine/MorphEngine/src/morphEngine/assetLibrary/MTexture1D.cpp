#include "MTexture1D.h"
#include "renderer/RenderingManager.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		MTexture1D::~MTexture1D()
		{
			Shutdown();
		}

		void MTexture1D::InitializeFromRawData()
		{
			_res = renderer::RenderingManager::GetInstance()->GetDevice()->CreateTexture1DInstance();
			_res->Initialize(_rawData.GetDataPointer(), _width, _bpp, _dataFormat, _samplerFormat);
		}

		void MTexture1D::Shutdown()
		{
			MTexture::Shutdown();

			_res->Shutdown();
			delete _res;
			_res = nullptr;
		}
	}
}
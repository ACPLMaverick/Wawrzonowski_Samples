#include "MTextureCube.h"
#include "renderer/RenderingManager.h"

namespace morphEngine
{
	using namespace utility;

	namespace assetLibrary
	{

		MTextureCube::~MTextureCube()
		{
			Shutdown();
		}

		void MTextureCube::InitializeFromRawData()
		{
			_res = renderer::RenderingManager::GetInstance()->GetDevice()->CreateTextureCubeInstance();
			_res->Initialize(_rawDataPlusX.GetDataPointer(), _rawDataPlusY.GetDataPointer(), _rawDataPlusZ.GetDataPointer(), _rawDataMinusX.GetDataPointer(),
				_rawDataMinusY.GetDataPointer(), _rawDataMinusZ.GetDataPointer(), _width, _height, _bpp, _dataFormat, _samplerFormat, _bCompressed, _mipmapCount);
		}

		void MTextureCube::Shutdown()
		{
			MTexture::Shutdown();

			_res->Shutdown();
			delete _res;
			_res = nullptr;
		}

		bool MTextureCube::LoadMAsset(resourceManagement::fileSystem::File & file)
		{
			if (!MTexture::LoadMAsset(file))
				return false;

			SplitRawDataIntoFaces();

			return true;
		}

		bool MTextureCube::LoadDDS(resourceManagement::fileSystem::File & file)
		{
			if (!MTexture::LoadDDS(file))
				return false;

			SplitRawDataIntoFaces();

			return true;
		}

		bool MTextureCube::SaveMAsset(resourceManagement::fileSystem::File & file) const
		{
			if (!MTexture::SaveMAsset(file))
				return false;


			return true;
		}

		inline void MTextureCube::SplitRawDataIntoFaces()
		{
			MSize totalSize = _rawData.GetSize();
			MSize onePartSize = totalSize / 6;
			MSize partBorders[6]{ 0, onePartSize, 2 * onePartSize, 3 * onePartSize,
			4 * onePartSize, 5 * onePartSize };
			MFixedArray<MUint8>* arrayPtrs[6]{ &_rawDataPlusX, &_rawDataPlusY, &_rawDataPlusZ, &_rawDataMinusX, &_rawDataMinusY, &_rawDataMinusZ };

			for (MSize i = 0; i < 6; ++i)
			{
				MSize dataStart = partBorders[i];
				MFixedArray<MUint8>* arr = arrayPtrs[i];
				
				arr->Allocate(onePartSize);
				memcpy(arr->GetDataPointer(), _rawData.GetDataPointer() + dataStart, onePartSize);
			}
		}
	}
}
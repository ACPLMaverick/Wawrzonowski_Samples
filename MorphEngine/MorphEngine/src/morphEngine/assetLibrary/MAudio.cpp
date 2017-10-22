#include "MAudio.h"

#include "core\Engine.h"

using namespace morphEngine::resourceManagement::fileSystem;
using namespace morphEngine::utility;
using namespace morphEngine::audio;

namespace morphEngine
{
	namespace assetLibrary
	{
		bool MAudio::LoadWAV(File& file)
		{
			file.Read(_data);
			return true;
		}

		bool MAudio::LoadMP3(File& file)
		{
			ME_WARNING(false, "Loading MP3 files is not supported at this moment");
			return false;
		}

		bool MAudio::LoadOGG(File& file)
		{
			ME_WARNING(false, "Loading OGG files is not supported at this moment");
			return false;
		}

		bool MAudio::LoadMAudio(File& file)
		{
			MFixedArray<MUint8> readData;
			file.Read(readData);

			if (!CheckType(readData.GetDataPointer()))
				return false;

			_data.Allocate(readData.GetSize() - sizeof(MAssetType));
			memcpy(_data.GetDataPointer(), readData.GetDataPointer() + sizeof(MAssetType), _data.GetSize());
			return true;
		}

		bool MAudio::SaveMAudio(File& file) const
		{
			file.Write((MUint8*)&_type, sizeof(MAssetType));
			file.WriteAppend(_data);
			return true;
		}

		void MAudio::InitFMODSound()
		{
			FMOD::System* fmod = AudioManager::GetInstance()->GetFMODSystem();
			if(fmod == nullptr)
			{
				return;
			}

			if(_fmodSound != nullptr)
			{
				return;
			}

			FMOD_CREATESOUNDEXINFO info = {0};
			info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
			info.length = (unsigned int)_data.GetSize();
			FMOD_RESULT result = fmod->createSound((const char*)_data.GetDataPointer(), FMOD_OPENMEMORY_POINT, &info, &_fmodSound);
			FMOD_ERROR_CHECK(result);
		}

		void MAudio::Shutdown()
		{
			_data.Deallocate();
		}

		bool MAudio::LoadFromData(const MFixedArray<MUint8>& data)
		{
			_data.Allocate(data.GetSize());
			memcpy(_data.GetDataPointer(), data.GetDataPointer(), data.GetSize());
			return true;
		}

		bool MAudio::LoadFromData(const MArray<MUint8>& data)
		{
			_data.Allocate(data.GetSize());
			memcpy(_data.GetDataPointer(), data.GetDataPointer(), data.GetSize());
			return true;
		}
	}
}
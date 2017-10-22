#pragma once

#include "audio\AudioManager.h"
#include "MAsset.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MAudio : public MAsset
		{
		protected:
			utility::MFixedArray<MUint8> _data;
			FMOD::Sound* _fmodSound;

		protected:
			bool LoadWAV(resourceManagement::fileSystem::File& file);
			bool LoadMP3(resourceManagement::fileSystem::File& file);
			bool LoadOGG(resourceManagement::fileSystem::File& file);
			bool LoadMAudio(resourceManagement::fileSystem::File& file);

			bool SaveMAudio(resourceManagement::fileSystem::File& file) const;

			void InitFMODSound();

			virtual void Shutdown() override;

		public:
			inline MAudio() : MAsset() 
			{
				_fmodSound = nullptr;
				_type = MAssetType::AUDIO;
			}

			inline MAudio(const MAudio& other) : MAsset(other)
			{
				_type = MAssetType::AUDIO;
				_fmodSound = nullptr;
				_data = other._data;
			}

			inline virtual ~MAudio() { Shutdown(); }

			inline MUint8* GetData() const
			{
				return _data.GetDataPointer();
			}

			inline FMOD::Sound* GetSound() const
			{
				return _fmodSound;
			}

			virtual bool LoadFromData(const utility::MFixedArray<MUint8>& data);
			virtual bool LoadFromData(const utility::MArray<MUint8>& data);

			inline virtual bool LoadFromFile(resourceManagement::fileSystem::File& file)
			{
				if(!file.IsOpened())
				{
					return false;
				}
				SetPath(file.GetPath());

				utility::MString ext;
				GetExtension(file, ext);

				if(ext == "wav")
				{
					return LoadWAV(file);
				}
				else if(ext == "mp3")
				{
					return LoadMP3(file);
				}
				else if(ext == "ogg")
				{
					return LoadOGG(file);
				}
				else if(ext == "masset")
				{
					return LoadMAudio(file);
				}

				return false;
			}

			inline virtual bool SaveToFile(resourceManagement::fileSystem::File& file) const
			{
				if(!file.IsOpened())
				{
					return false;
				}

				return SaveMAudio(file);
			}

			inline virtual void Initialize()
			{
				InitFMODSound();
			}

			inline operator MAsset*()
			{
				return reinterpret_cast<MAsset*>(this);
			}
		};
	}
}
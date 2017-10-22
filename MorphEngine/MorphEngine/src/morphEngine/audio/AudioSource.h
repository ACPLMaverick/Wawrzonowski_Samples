#pragma once

#include "assetLibrary\MAudio.h"
#include "gom\GameObjectComponent.h"

namespace morphEngine
{
	namespace audio
	{
		DECLARE_MENUM(AudioSourceMode, SOURCE_2D, SOURCE_3D);

		class AudioSource : public gom::GameObjectComponent
		{
			ME_TYPE

		protected:
			assetLibrary::MAudio* _clip;
			AudioSourceMode _sourceMode;
			MInt32 _priority;
			MFloat32 _volume;
			MFloat32 _pitch;
			MFloat32 _minDistance;
			MFloat32 _maxDistance;
			bool _bIsLooping;

			FMOD::Channel* _channel;
			MFixedArray<FMOD_VECTOR> _roloffCurve;

		private:
			void RegisterProperties();
			void Update3DSettings();

		public:
			AudioSource(const gom::ObjectInitializer& objectInitializer);
			AudioSource(const AudioSource& otherSource, bool bDeepCopy = true);
			inline virtual ~AudioSource() { }

			void Play();
			void Play(assetLibrary::MAudio* clip);
			void Stop();
			void Pause();
			void Resume();
			bool IsPlaying();
			bool IsPaused();

			virtual void Update();
			virtual void Shutdown();

			inline AudioSourceMode GetSourceMode() const { return _sourceMode; }
			inline MInt32 GetPriority() const { return _priority; }
			inline MFloat32 GetVolume() const { return _volume; }
			inline MFloat32 GetPitch() const { return _pitch; }
			inline MFloat32 GetMinDistance() const { return _minDistance; }
			inline MFloat32 GetMaxDistance() const { return _maxDistance; }
			inline MFloat32 GetClipDuration() const
			{
				if(_clip == nullptr || _clip->GetSound() == nullptr)
				{
					return 0.0f;
				}

				MUint32 length;
				_clip->GetSound()->getLength(&length, FMOD_TIMEUNIT_MS);
				return (MFloat32)length * 0.001f;
			}

			inline bool IsLooping() const { return _bIsLooping; }

			void SetSourceMode(AudioSourceMode newMode);
			void SetPriority(MInt32 newPriority);
			void SetVolume(MFloat32 newVolume);
			void SetPitch(MFloat32 newPitch);
			void SetMinDistance(MFloat32 newMinDistance);
			void SetMaxDistance(MFloat32 newMaxDistance);
			void SetIsLooping(bool bNewIsLooping);
		};
	}
}
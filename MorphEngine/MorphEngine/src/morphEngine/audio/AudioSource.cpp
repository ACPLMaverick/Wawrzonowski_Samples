#include "AudioSource.h"

#include "gom\Transform.h"
#include "core\Engine.h"
#include "core\ITime.h"

using namespace morphEngine::gom;
using namespace morphEngine::core;

namespace morphEngine
{
	namespace audio
	{
		DEFINE_MENUM(AudioSourceMode);

		void AudioSource::RegisterProperties()
		{
			RegisterProperty("Clip", &_clip);
			RegisterProperty("Loop", &_bIsLooping);
			RegisterProperty("Volume", &_volume);
			RegisterProperty("Pitch", &_pitch);
			RegisterProperty("Priority", &_priority);
			RegisterProperty("Source mode", &_sourceMode);
			RegisterProperty("Min distance", &_minDistance);
			RegisterProperty("Max distance", &_maxDistance);
		}

		void AudioSource::Update3DSettings()
		{
			if(!_owner.IsValid() || _channel == nullptr)
			{
				return;
			}

			Handle<Transform> transform = _owner->GetTransform();
			FMOD_VECTOR oldPos;
			_channel->get3DAttributes(&oldPos, 0);
			FMOD_VECTOR newPos;
			memcpy(&newPos, &(transform->GetPosition()), sizeof(FMOD_VECTOR));
			MFloat32 delta = Engine::GetInstance()->GetGlobalTime().GetDeltaTime();
			if(MMath::Approximately(delta, 0.0f))
			{
				return;
			}
			FMOD_VECTOR vel;
			vel.x = (newPos.x - oldPos.x) / delta;
			vel.y = (newPos.y - oldPos.y) / delta;
			vel.z = (newPos.z - oldPos.z) / delta;
			_channel->set3DAttributes(&newPos, &vel);
		}

		AudioSource::AudioSource(const gom::ObjectInitializer& objectInitializer) : GameObjectComponent(objectInitializer), _clip(nullptr), _sourceMode(AudioSourceMode::SOURCE_2D), _priority(128), _volume(1.0f), _pitch(1.0f), _bIsLooping(false), _channel(nullptr), _minDistance(0.5f), _maxDistance(10.0f)
		{
			RegisterProperties();
			_roloffCurve.Allocate(3);
		}

		AudioSource::AudioSource(const AudioSource& otherSource, bool bDeepCopy) : GameObjectComponent(otherSource, bDeepCopy), _clip(otherSource._clip), _sourceMode(otherSource._sourceMode), _priority(otherSource._priority), _volume(otherSource._volume), _pitch(otherSource._pitch), _bIsLooping(otherSource._bIsLooping), _channel(nullptr), _roloffCurve(otherSource._roloffCurve), _minDistance(otherSource._minDistance), _maxDistance(otherSource._maxDistance)
		{
			RegisterProperties();
		}
		
		void AudioSource::Play()
		{
			if(_clip == nullptr)
			{
				return;
			}

			FMOD::System* fmod = AudioManager::GetInstance()->GetFMODSystem();
			if(fmod == nullptr)
			{
				return;
			}

			bool bWasChannelNull = _channel == nullptr;
			fmod->playSound(_clip->GetSound(), nullptr, false, &_channel);
			if(bWasChannelNull && _channel != nullptr)
			{
				_channel->setMode(_sourceMode == AudioSourceMode::SOURCE_2D ? FMOD_2D : FMOD_3D);
				_channel->setPriority(_priority);
				_channel->setVolume(_volume);
				_channel->setPitch(_pitch);
				_channel->setMode(_bIsLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
				_channel->setMode(FMOD_3D_CUSTOMROLLOFF);
				_roloffCurve[0].x = 0.0f;
				_roloffCurve[0].y = 1.0f;
				_roloffCurve[0].z = 0.0f;
				_roloffCurve[1].x = _minDistance;
				_roloffCurve[1].y = 1.0f;
				_roloffCurve[1].z = 0.0f;
				_roloffCurve[2].x = _maxDistance;
				_roloffCurve[2].y = 0.0f;
				_roloffCurve[2].z = 0.0f;
				_channel->set3DMinMaxDistance(_minDistance, _maxDistance);
				_channel->set3DCustomRolloff(_roloffCurve.GetDataPointer(), 3);
			}
		}

		void AudioSource::Play(assetLibrary::MAudio* clip)
		{
			if(clip == nullptr)
			{
				if(IsPlaying())
				{
					Stop();
				}
				_clip = nullptr;
				return;
			}

			_clip = clip;
			Play();
		}
		
		void AudioSource::Stop()
		{
			if(_channel == nullptr)
			{
				return;
			}

			_channel->stop();
			
		}

		void AudioSource::Pause()
		{
			if(_channel == nullptr)
			{
				return;
			}

			_channel->setPaused(true);
		}

		void AudioSource::Resume()
		{
			if(_channel == nullptr)
			{
				return;
			}

			_channel->setPaused(false);
		}

		bool AudioSource::IsPlaying()
		{
			if(_channel == nullptr)
			{
				return false;
			}

			bool bIsPlaying;
			_channel->isPlaying(&bIsPlaying);
			return bIsPlaying;
		}

		bool AudioSource::IsPaused()
		{
			if(_channel == nullptr)
			{
				return false;
			}

			bool bIsPaused;
			_channel->getPaused(&bIsPaused);
			return bIsPaused;
		}

		void AudioSource::Update()
		{
			if(_sourceMode == AudioSourceMode::SOURCE_3D)
			{
				Update3DSettings();
			}
		}

		void AudioSource::Shutdown()
		{
			GameObjectComponent::Shutdown();
			Stop();
			_clip = nullptr;
		}

		void AudioSource::SetSourceMode(AudioSourceMode newMode)
		{
			_sourceMode = newMode;
			if(_channel != nullptr)
			{
				_channel->setMode(_sourceMode == AudioSourceMode::SOURCE_2D ? FMOD_2D : FMOD_3D);
			}
		}

		void AudioSource::SetPriority(MInt32 newPriority)
		{
			_priority = MMath::Clamp(newPriority, 0, 256);
			if(_channel != nullptr)
			{
				_channel->setPriority(_priority);
			}
		}

		void AudioSource::SetVolume(MFloat32 newVolume)
		{
			_volume = newVolume;
			if(_channel != nullptr)
			{
				_channel->setVolume(_volume);
			}
		}

		void AudioSource::SetPitch(MFloat32 newPitch)
		{
			_pitch = MMath::Clamp(newPitch, 0.0f, 2.0f);
			if(_channel != nullptr)
			{
				_channel->setPitch(_pitch);
			}
		}

		void AudioSource::SetMinDistance(MFloat32 newMinDistance)
		{
			_minDistance = MMath::Clamp(newMinDistance, 0.0f, _maxDistance);
			if(_channel != nullptr)
			{
				_roloffCurve[1].x = _minDistance;
				_channel->set3DMinMaxDistance(_minDistance, _maxDistance);
				_channel->set3DCustomRolloff(_roloffCurve.GetDataPointer(), 3);
			}
		}

		void AudioSource::SetMaxDistance(MFloat32 newMaxDistance)
		{
			_maxDistance = MMath::Clamp(newMaxDistance, _minDistance, FLT_MAX);
			if(_channel != nullptr)
			{
				_roloffCurve[2].x = _maxDistance;
				_channel->set3DMinMaxDistance(_minDistance, _maxDistance);
				_channel->set3DCustomRolloff(_roloffCurve.GetDataPointer(), 3);
			}
		}

		void AudioSource::SetIsLooping(bool bNewIsLooping)
		{
			_bIsLooping = bNewIsLooping;
			if(_channel != nullptr)
			{
				_channel->setMode(_bIsLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
			}
		}
	}
}



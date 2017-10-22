#include "AudioManager.h"

#include "core\Engine.h"
#include "core\GlobalDefines.h"
#include "gom\Transform.h"

#include <Windows.h>

using namespace morphEngine::core;
using namespace morphEngine::gom;

namespace morphEngine
{
	namespace audio
	{
		void AudioManager::UpdateListener()
		{
			if(!_listener.IsValid() || !_listener->GetOwner().IsValid())
			{
				return;
			}
			Handle<Transform> listenerTransform = _listener->GetOwner()->GetTransform();
			if(!listenerTransform.IsValid())
			{
				return;
			}

			MFloat32 delta = Engine::GetInstance()->GetGlobalTime().GetDeltaTime();
			if(MMath::Approximately(delta, 0.0f))
			{
				return;
			}

			MVector3 listenerPosition = listenerTransform->GetPosition();
			MVector3 listenerForward = listenerTransform->GetDirection();
			MVector3 listenerUp = MQuaternion::FromEuler(listenerTransform->GetRotation()) * MVector3::Up;

			FMOD_VECTOR forward;
			memcpy(&forward, &listenerForward, sizeof(FMOD_VECTOR));
			FMOD_VECTOR up;
			memcpy(&up, &listenerUp, sizeof(FMOD_VECTOR));
			FMOD_VECTOR oldPos;
			_fmod->get3DListenerAttributes(0, &oldPos, 0, 0, 0);
			FMOD_VECTOR newPos;
			memcpy(&newPos, &listenerPosition, sizeof(FMOD_VECTOR));

			FMOD_VECTOR vel;
			vel.x = (newPos.x - oldPos.x) / delta;
			vel.y = (newPos.y - oldPos.y) / delta;
			vel.z = (newPos.z - oldPos.z) / delta;

			_fmod->set3DListenerAttributes(0, &newPos, &vel, &forward, &up);
			_defaultChannelGroup->setVolume(_listener->GetVolume());
		}

		bool AudioManager::Initialize()
		{
#ifdef PLATFORM_WINDOWS
			CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
#endif
			FMOD_RESULT result = FMOD::System_Create(&_fmod);
			FMOD_ERROR_CHECK_RETURN(result);

			result = _fmod->init(512, FMOD_INIT_NORMAL, 0);
			FMOD_ERROR_CHECK_RETURN(result);

			_fmod->set3DSettings(1.0f, 1.0f, 1.0f);
			_fmod->set3DNumListeners(1);

			_fmod->getMasterChannelGroup(&_defaultChannelGroup);
			_defaultChannelGroup->setVolume(1.0f);

			return true;
		}

		void AudioManager::Shutdown()
		{
			if(_fmod != nullptr)
			{
				_fmod->release();
				_fmod = nullptr;
			}

#ifdef PLATFORM_WINDOWS
			CoUninitialize();
#endif
		}

		void AudioManager::Update()
		{
			if(_fmod != nullptr)
			{
				UpdateListener();
				_fmod->update();
			}
		}
	}
}
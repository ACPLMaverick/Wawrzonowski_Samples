#pragma once

#include "debugging\Debug.h"
#include "core\Singleton.h"
#include "AudioListener.h"

#include "FMOD\fmod.hpp"
#if _DEBUG
#include "FMOD\fmod_errors.h"
#endif

#if _DEBUG
#pragma comment(lib, "fmodL64_vc.lib")
#else
#pragma comment(lib, "fmod64_vc.lib")
#endif

#if _DEBUG
#define FMOD_ERROR_CHECK(fmodResult) \
if(fmodResult != FMOD_OK) \
{ \
	morphEngine::debugging::Debug::Log(morphEngine::debugging::ELogType::ERR, morphEngine::utility::MString("FMOD ERROR: ") + FMOD_ErrorString(fmodResult)); \
	return; \
}

#define FMOD_ERROR_CHECK_RETURN(fmodResult) \
if(fmodResult != FMOD_OK) \
{ \
	morphEngine::debugging::Debug::Log(morphEngine::debugging::ELogType::ERR, morphEngine::utility::MString("FMOD ERROR: ") + FMOD_ErrorString(fmodResult)); \
	return false; \
}
#else
#define FMOD_ERROR_CHECK(fmodResult)
#define FMOD_ERROR_CHECK_RETURN(fmodResult)
#endif

namespace morphEngine
{
	namespace audio
	{
		/// <summary>
		/// Singleton.
		/// </summary>
		class AudioManager :
			public core::Singleton<AudioManager>
		{
		protected:
			FMOD::System* _fmod;
			FMOD::ChannelGroup* _defaultChannelGroup;
			Handle<AudioListener> _listener;

		protected:
			void UpdateListener();

		public:
			inline AudioManager() : _fmod(nullptr), _listener(true) { }
			inline virtual ~AudioManager() { }

			bool Initialize();
			void Shutdown();

			void Update();

			inline FMOD::System* GetFMODSystem() const
			{
				return _fmod;
			}

			inline void RegisterListener(Handle<AudioListener> listener)
			{
				ME_WARNING_RETURN_STATEMENT(!_listener.IsValid(), "There can be only one registered listener!");
				ME_WARNING_RETURN_STATEMENT(listener.IsValid(), "Listener to register should be valid!");

				_listener = listener;
			}

			inline void DeregisterListener(Handle<AudioListener> listener)
			{
				ME_WARNING_RETURN_STATEMENT(listener.IsValid(), "Only valid listener can be deregistered");
				ME_WARNING_RETURN_STATEMENT(_listener == listener, "Trying to deregister not registered listener");

				_listener.Invalidate();
			}
		};

	}
}


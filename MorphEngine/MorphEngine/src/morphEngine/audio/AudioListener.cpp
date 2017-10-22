#include "AudioListener.h"
#include "AudioManager.h"

using namespace morphEngine::gom;

namespace morphEngine
{
	namespace audio
	{

		void AudioListener::Initialize()
		{
			GameObjectComponent::Initialize();
			AudioManager::GetInstance()->RegisterListener(_this);
		}

		void AudioListener::Shutdown()
		{
			AudioManager::GetInstance()->DeregisterListener(_this);
			GameObjectComponent::Shutdown();
		}
	}
}
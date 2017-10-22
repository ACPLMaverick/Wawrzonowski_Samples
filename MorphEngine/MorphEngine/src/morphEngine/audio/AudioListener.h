#pragma once

#include "gom\GameObjectComponent.h"

namespace morphEngine
{
	namespace audio
	{
		class AudioListener : public gom::GameObjectComponent
		{
			ME_TYPE

		protected:
			MFloat32 _volume;

		public:
			inline AudioListener(const gom::ObjectInitializer& objectInitializer) : gom::GameObjectComponent(objectInitializer), _volume(1.0f) { }
			inline AudioListener(const AudioListener& other, bool bDeepCopy = true) : gom::GameObjectComponent(other, bDeepCopy), _volume(other._volume) { }
			inline virtual ~AudioListener() { }

			virtual void Initialize();
			virtual void Shutdown();

			inline MFloat32 GetVolume() const { return _volume; }
			inline void SetVolume(MFloat32 newVolume) { _volume = newVolume; }
		};
	}
}
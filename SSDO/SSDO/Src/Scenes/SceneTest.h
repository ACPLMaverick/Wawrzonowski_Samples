#pragma once

#include "Scene.h"

namespace Scenes
{
	class SceneTest :
		public Scene
	{
	protected:

		virtual void SetupScene() override;

	public:
		SceneTest();
		~SceneTest();
	};
}


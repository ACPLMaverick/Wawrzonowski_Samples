#pragma once
#include "Scene.h"
class SceneSphere :
	public Scene
{
protected:

#pragma region Functions Protected

	virtual void InitializeScene() override;

#pragma endregion

public:
	SceneSphere();
	~SceneSphere();
};


#pragma once
#include "Scene.h"
class SceneCornel :
	public Scene
{
protected:

#pragma region Functions Protected

	virtual void InitializeScene() override;

#pragma endregion

public:
	SceneCornel();
	~SceneCornel();
};


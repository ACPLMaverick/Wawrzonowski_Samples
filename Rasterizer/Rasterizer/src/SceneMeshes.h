#pragma once
#include "Scene.h"

class SceneMeshes :
	public Scene
{
protected:

#pragma region Functions Protected

	virtual void InitializeScene() override;

#pragma endregion

public:
	SceneMeshes();
	~SceneMeshes();
};


#pragma once
#include "stdafx.h"
#include "Buffer.h"

class Scene;

class IRenderer
{
protected:

#pragma region Protected

	Buffer<Color32> _bufferColor;
	Buffer<float> _bufferDepth;

#pragma endregion

#pragma region Functions Protected

	IRenderer(SystemSettings* settings);

#pragma endregion

public:
	~IRenderer();

#pragma region Functions Public

	virtual void Draw(Scene* scene) = 0;
	
	virtual Buffer<Color32>* GetColorBuffer() { return &_bufferColor; }

#pragma endregion
};


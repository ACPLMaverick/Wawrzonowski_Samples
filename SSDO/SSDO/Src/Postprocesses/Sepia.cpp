#include "stdafx.h"
#include "Sepia.h"
#include "Shader.h"
#include "System.h"
#include "Scenes\Scene.h"
using namespace Scenes;

namespace Postprocesses
{
	Sepia::Sepia()
	{
		_shaders.push_back(System::GetInstance()->GetScene()->LoadShader(std::wstring(L"PPSepia")));
	}

	Sepia::~Sepia()
	{
	}

	void Sepia::Update()
	{
	}

}
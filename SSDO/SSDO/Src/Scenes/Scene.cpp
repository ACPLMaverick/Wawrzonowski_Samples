#include "stdafx.h"
#include "Scene.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "Object.h"
#include "Text.h"
#include "Controller.h"
#include "Renderer.h"
#include "Lights/LightAmbient.h"
#include "Lights/LightDirectional.h"
#include "Lights/LightPoint.h"
#include "Postprocesses/Postprocess.h"
using namespace Lights;
using namespace Postprocesses;

namespace Scenes
{
	Scene::Scene() : 
		_controller(this)
	{

	}


	Scene::~Scene()
	{
	}

	void Scene::Initialize()
	{
		_textShader = LoadShader(L"TextShader");
		_shaders.emplace(L"TextShader", _textShader);

		SetupScene();
		_controller.Initialize();
	}

	void Scene::Shutdown()
	{
		_controller.Shutdown();

		if (_mainCamera != nullptr)
		{
			delete _mainCamera;
		}

		for (auto it = _objects.begin(); it != _objects.end(); ++it)
		{
			delete *it;
		}

		for (auto it = _lightsDirectional.begin(); it != _lightsDirectional.end(); ++it)
		{
			delete *it;
		}

		for (auto it = _lightsPoint.begin(); it != _lightsPoint.end(); ++it)
		{
			delete *it;
		}

		for (auto it = _postprocesses.begin(); it != _postprocesses.end(); ++it)
		{
			delete *it;
		}

		for (auto it = _texts.begin(); it != _texts.end(); ++it)
		{
			delete *it;
		}

		_objects.clear();
		_lightsPoint.clear();
		_lightsDirectional.clear();
		_postprocesses.clear();
		_texts.clear();

		if (_lightAmbient != nullptr) delete _lightAmbient;

		for (auto it = _shaders.begin(); it != _shaders.end(); ++it)
		{
			delete (*it).second;
		}

		for (auto it = _computeShaders.begin(); it != _computeShaders.end(); ++it)
		{
			delete (*it).second;
		}

		for (auto it = _materials.begin(); it != _materials.end(); ++it)
		{
			delete (*it).second;
		}

		for (auto it = _meshes.begin(); it != _meshes.end(); ++it)
		{
			delete (*it).second;
		}

		for (auto it = _fonts.begin(); it != _fonts.end(); ++it)
		{
			delete (*it).second;
		}

		_materials.clear();
		_meshes.clear();
		_shaders.clear();
		_computeShaders.clear();
		_fonts.clear();
	}

	void Scene::Update()
	{
		_mainCamera->Update();

		for (auto it = _materials.begin(); it != _materials.end(); ++it)
		{
			(*it).second->Update();
		}

		for (auto it = _objects.begin(); it != _objects.end(); ++it)
		{
			(*it)->Update();
		}

		_controller.Update();
	}

	void Scene::Draw()
	{
		_mainCamera->Draw(*this);
	}

	void Scene::SetLightAmbient(Lights::LightAmbient * obj)
	{
		if (obj == nullptr)
			return;

		if (_lightAmbient != nullptr)
		{
			delete _lightAmbient;
		}

		_lightAmbient = obj;
	}

}
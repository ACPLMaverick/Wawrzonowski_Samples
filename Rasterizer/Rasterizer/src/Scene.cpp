#include "stdafx.h"
#include "Scene.h"
#include "Primitive.h"
#include "Camera.h"
#include "Material.h"
#include "Texture.h"

#ifndef RENDERER_MAV

#include "light/LightAmbient.h"
#include "light/LightDirectional.h"
#include "light/LightSpot.h"

#endif

Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::Initialize(uint32_t uID, std::string * name)
{
	_name = *name;
	_uID = uID;

	Texture* blankDiff = new Texture(Color32(0xFFFFFFFF));
	Texture* blankNrm = new Texture(Color32((uint8_t)255, (uint8_t)127, (uint8_t)127, (uint8_t)255));
	_defaultMaterial = new Material
	(
		blankDiff,
		blankNrm
	);

	InitializeScene();
}

void Scene::Shutdown()
{
	for (std::vector<Primitive*>::iterator it = _primitives.begin(); it != _primitives.end(); ++it)
	{
		delete *it;
	}

	for (std::vector<Camera*>::iterator it = _cameras.begin(); it != _cameras.end(); ++it)
	{
		delete *it;
	}

	for (std::vector<Material*>::iterator it = _materials.begin(); it != _materials.end(); ++it)
	{
		delete *it;
	}
	delete _defaultMaterial;

#ifndef RENDERER_MAV

	delete _lightAmbient;
	for (std::vector<light::LightDirectional*>::iterator it = _lightsDirectional.begin(); it != _lightsDirectional.end(); ++it)
	{
		delete *it;
	}
	for (std::vector<light::LightSpot*>::iterator it = _lightsSpot.begin(); it != _lightsSpot.end(); ++it)
	{
		delete *it;
	}

#endif // !RENDERER_MAV

	_primitives.clear();
	_cameras.clear();
	_lightsDirectional.clear();
	_lightsSpot.clear();
}

void Scene::Update()
{
	if (_flagToAddPrimitive)
	{
		_flagToAddPrimitive = false;
		for (std::vector<Primitive*>::iterator it = _primitivesToAdd.begin(); it != _primitivesToAdd.end(); ++it)
		{
			_primitives.push_back(*it);
		}
		_primitivesToAdd.clear();
	}
	if (_flagToRemovePrimitive)
	{
		_flagToRemovePrimitive = false;
		for (std::vector<std::vector<Primitive*>::iterator>::iterator it = _primitivesToRemove.begin(); it != _primitivesToRemove.end(); ++it)
		{
			_primitives.erase(*it);
		}
		_primitivesToRemove.clear();
	}

	for (std::vector<Primitive*>::iterator it = _primitives.begin(); it != _primitives.end(); ++it)
	{
		(*it)->Update();
	}

	for (std::vector<Camera*>::iterator it = _cameras.begin(); it != _cameras.end(); ++it)
	{
		(*it)->Update();
	}
}

void Scene::Draw()
{
	for (std::vector<Primitive*>::iterator it = _primitives.begin(); it != _primitives.end(); ++it)
	{
		(*it)->Draw();
	}
}

Camera * const Scene::GetCamera(uint32_t uid)
{
	for (std::vector<Camera*>::iterator it = _cameras.begin(); it != _cameras.end(); ++it)
	{
		if ((*it)->GetUID() == uid)
		{
			return *it;
		}
	}

	return nullptr;
}

Camera * const Scene::GetCamera(std::string * name)
{
	for (std::vector<Camera*>::iterator it = _cameras.begin(); it != _cameras.end(); ++it)
	{
		if (*(*it)->GetName() == *name)
		{
			return *it;
		}
	}

	return nullptr;
}

void Scene::AddPrimitive(Primitive * const Primitive)
{
	_primitivesToAdd.push_back(Primitive);
	_flagToAddPrimitive = true;
}

void Scene::AddCamera(Camera * const camera)
{
	_cameras.push_back(camera);
}

void Scene::AddLightAmbient(light::LightAmbient * const la)
{
	_lightAmbient = la;
}

void Scene::AddLightDirectional(light::LightDirectional * const ld)
{
	_lightsDirectional.push_back(ld);
}

void Scene::AddLightSpot(light::LightSpot * const ls)
{
	_lightsSpot.push_back(ls);
}

void Scene::AddLightPoint(light::LightPoint * const lp)
{
	_lightsPoint.push_back(lp);
}

Camera * const Scene::RemoveCamera(uint32_t uid)
{
	for (std::vector<Camera*>::iterator it = _cameras.begin(); it != _cameras.end(); ++it)
	{
		if ((*it)->GetUID() == uid)
		{
			Camera* tmp = *it;
			_cameras.erase(it);
			return *it;
		}
	}

	return nullptr;
}

Camera * const Scene::RemoveCamera(std::string * name)
{
	for (std::vector<Camera*>::iterator it = _cameras.begin(); it != _cameras.end(); ++it)
	{
		if (*(*it)->GetName() == *name)
		{
			Camera* tmp = *it;
			_cameras.erase(it);
			return *it;
		}
	}

	return nullptr;
}

#include "stdafx.h"
#include "Controller.h"
#include "System.h"
#include "Scenes/Scene.h"
#include "Input.h"
#include "Camera.h"
#include "Timer.h"
#include "Postprocesses/SimpleSSAO.h"
#include "Postprocesses/SSDOBase.h"
#include "Postprocesses/SSDOImproved.h"
#include "Postprocesses/SSDOImprovedB.h"
#include "Postprocesses/Sepia.h"
#include "Profiler.h"
#include "Lights/LightDirectional.h"

using namespace Scenes;
using namespace Postprocesses;

Controller::Controller(Scene* scene) : 
	_scene(scene), 
	_cameraSpeed(1.0f), 
	_cameraBoost(30.0f), 
	_cameraRotateSpeedX(1.0f), 
	_cameraRotateSpeedY(1.0f),
	_bRotateLight(true)
{
}

Controller::~Controller()
{
}

void Controller::Initialize()
{
	_profiler = new Profiler(_scene);
	_profiler->Initialize();

	_postprocessCounter = 0;
	for (auto it = _scene->_postprocesses.begin(); it != _scene->_postprocesses.end(); ++it)
	{
		// enable only first postprocess in scene
		if (it != _scene->_postprocesses.begin())
		{
			(*it)->SetEnabled(false);
		}
		else
		{
			(*it)->SetEnabled(true);
		}

		// assign names to postprocesses in profiler
		if (dynamic_cast<SimpleSSAO*>(*it) != nullptr)
		{
			_profiler->RegisterPostprocessName("SSAO");
		}
		else if (dynamic_cast<SSDOBase*>(*it) != nullptr)
		{
			_profiler->RegisterPostprocessName("SSDO - A");
		}
		else if (dynamic_cast<SSDOImproved*>(*it) != nullptr)
		{
			_profiler->RegisterPostprocessName("SSDO - B");
		}
		else if (dynamic_cast<SSDOImprovedB*>(*it) != nullptr)
		{
			_profiler->RegisterPostprocessName("SSDO - C");
		}
		else if (dynamic_cast<Sepia*>(*it) != nullptr)
		{
			_profiler->RegisterPostprocessName("Sepia");
		}
		else
		{
			_profiler->RegisterPostprocessName("Unknown");
		}
	}
	_profiler->SwitchPostprocessName(_postprocessCounter);
	SwitchDirectionalLight();
}

void Controller::Update()
{
	_profiler->Update();

	// Base controls
	if (Input::GetInstance()->GetKey(VK_ESCAPE))
	{
		System::GetInstance()->Exit();
	}

	// Camera controls
	Camera* cam = _scene->_mainCamera;

	if (cam != nullptr)
	{
		XMFLOAT3A pos = cam->GetPosition();
		XMFLOAT3A tgt = cam->GetTarget();
		XMFLOAT3A up = cam->GetUp();
		XMFLOAT3A dir = cam->GetDirection();

		XMVECTOR vPos = XMLoadFloat3A(&pos), vTgt = XMLoadFloat3A(&tgt), vUp = XMLoadFloat3A(&up), vDir = XMLoadFloat3A(&dir);
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));

		if (Input::GetInstance()->GetKey(VK_RBUTTON))
		{
			XMFLOAT2A mouseMovement = Input::GetInstance()->GetMouseRelativePosition();
			mouseMovement.x *= _cameraRotateSpeedX * Timer::GetInstance()->GetDeltaTime();
			mouseMovement.y *= _cameraRotateSpeedY * Timer::GetInstance()->GetDeltaTime();

			XMVECTOR rotYaw = XMQuaternionRotationAxis(vUp, mouseMovement.x);
			XMVECTOR rotPitch = XMQuaternionRotationAxis(vRight, mouseMovement.y);

			vDir = XMVector3Rotate(XMVector3Rotate(vDir, rotYaw), rotPitch);
			cam->SetDirection(vDir);
		}

		float currentBoost = 1.0f;
		if (Input::GetInstance()->GetKey(VK_SHIFT))
		{
			currentBoost = _cameraBoost;
		}

		bool bPositionChanged = false;

		if (Input::GetInstance()->GetKey('Q'))	// up
		{
			vPos = vPos + vUp * currentBoost * _cameraSpeed * Timer::GetInstance()->GetDeltaTime();
			bPositionChanged = true;
		}
		if (Input::GetInstance()->GetKey('E')) // down
		{
			vPos = vPos - vUp * currentBoost * _cameraSpeed * Timer::GetInstance()->GetDeltaTime();
			bPositionChanged = true;
		}
		if (Input::GetInstance()->GetKey('A')) // left
		{
			vPos = vPos - vRight * currentBoost * _cameraSpeed * Timer::GetInstance()->GetDeltaTime();
			bPositionChanged = true;
		}
		if (Input::GetInstance()->GetKey('D')) // right
		{
			vPos = vPos + vRight * currentBoost * _cameraSpeed * Timer::GetInstance()->GetDeltaTime();
			bPositionChanged = true;
		}
		if (Input::GetInstance()->GetKey('W')) // forwards
		{
			vPos = vPos + vDir * currentBoost * _cameraSpeed * Timer::GetInstance()->GetDeltaTime();
			bPositionChanged = true;
		}
		if (Input::GetInstance()->GetKey('S')) // backwards
		{
			vPos = vPos - vDir * currentBoost * _cameraSpeed * Timer::GetInstance()->GetDeltaTime();
			bPositionChanged = true;
		}

		if (bPositionChanged)
		{
			cam->SetPosition(vPos);
			cam->SetDirection(vDir);
		}
	}

	// Component controls
	if (_scene->_postprocesses.size() > 0)
	{
		// switch postprocess to next, disable all if reached max count
		if (Input::GetInstance()->GetKeyDown('V'))	
		{
			++_postprocessCounter;
			if (_postprocessCounter >= _scene->_postprocesses.size())
			{
				_scene->_postprocesses[_postprocessCounter - 1]->ToggleEnabled();
				// overflow
				_postprocessCounter = -1;
			}
			else
			{
				if(_postprocessCounter != 0)
					_scene->_postprocesses[_postprocessCounter - 1]->ToggleEnabled();

				_scene->_postprocesses[_postprocessCounter]->ToggleEnabled();
			}
			_profiler->SwitchPostprocessName(_postprocessCounter);
			SwitchDirectionalLight();
		}
	}

	// Light rotation
	if (_bRotateLight && _scene->GetLightsDirectional().size() > 0)
	{
		PerformOneLightRotation();
	}

	if (Input::GetInstance()->GetKeyDown('P'))
	{
		_bRotateLight = !_bRotateLight;
	}
	if (Input::GetInstance()->GetKey('O') && !_bRotateLight)
	{
		PerformOneLightRotation();
	}
}

void Controller::Shutdown()
{
	_profiler->Shutdown();
	delete _profiler;
}

inline void Controller::SwitchDirectionalLight()
{
	return;	// !!!!!!

	if (_scene->GetLightsDirectional().size() <= 0)
	{
		return;
	}

	if (_postprocessCounter == 0)
	{
		_scene->GetLightsDirectional()[0]->SetEnabled(false);
	}
	else
	{
		_scene->GetLightsDirectional()[0]->SetEnabled(true);
	}
}

inline void Controller::PerformOneLightRotation()
{
	Lights::LightDirectional* lDir(_scene->GetLightsDirectional()[0]);
	const float rotationSpeed(15.0f);
	XMFLOAT3A rotationTemp(XMFLOAT3A(1.0f, 0.0f, 0.0f));
	const XMVECTOR rotationAxeVec(XMLoadFloat3A(&rotationTemp));

	const float lightRotationAngle = rotationSpeed * Timer::GetInstance()->GetDeltaTime();

	XMVECTOR direction(-XMLoadFloat3A(&lDir->GetDirection()));
	direction = XMVector3Rotate(direction, XMQuaternionRotationAxis(rotationAxeVec, XMConvertToRadians(lightRotationAngle)));
	XMStoreFloat3A(&rotationTemp, direction);
	lDir->SetDirection(rotationTemp);
}

#include "stdafx.h"
#include "Camera.h"
#include "System.h"
#include "Scenes\Scene.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Object.h"
#include "Postprocesses/Postprocess.h"
#include "Text.h"
#include "Lights/LightAmbient.h"
#include "Lights/LightDirectional.h"
#include "Lights/LightPoint.h"

using namespace Scenes;

Camera::Camera(const XMFLOAT3A & position, const XMFLOAT3A & target, const XMFLOAT3A & up, float fov, float nearPlane, float farPlane) :
	_position(position),
	_target(target),
	_up(up),
	_fov(fov),
	_near(nearPlane),
	_far(farPlane),
	_gBuffer(*this)
{
	UpdateView();
	UpdateProjection();
	MergeMatrices();
}

Camera::~Camera()
{
}

void Camera::Update()
{
	bool mergeMatrices = false;

	if (_bNeedUpdateView)
	{
		UpdateView();
		_bNeedUpdateView = false;
		mergeMatrices = true;
	}

	if (_bNeedUpdateProj)
	{
		UpdateProjection();
		_bNeedUpdateProj = false;
		mergeMatrices = true;
	}

	if (mergeMatrices)
	{
		MergeMatrices();
	}
}

void Camera::Draw(Scene& scene) const
{
	if (Renderer::GetInstance()->GetRenderMode() == Renderer::RenderMode::FORWARD)
	{
		Renderer::GetInstance()->SetBlendMode(Renderer::BlendMode::SOLID);

		for (auto it = scene._objects.begin(); it != scene._objects.end(); ++it)
		{
			(*it)->Draw(*this);
		}

		Renderer::GetInstance()->SetBlendMode(Renderer::BlendMode::ALPHA);

		scene.GetTextShader()->Set();

		for (auto it = scene._texts.begin(); it != scene._texts.end(); ++it)
		{
			(*it)->Draw();
		}
	}
	else
	{
		Renderer::GetInstance()->SetBlendMode(Renderer::BlendMode::SOLID);

		GBuffer& gBuffer = const_cast<GBuffer&>(_gBuffer);
		// draw objects
		gBuffer.SetDrawMeshes();
		for (auto it = scene._objects.begin(); it != scene._objects.end(); ++it)
		{
			(*it)->Draw(*this);
		}

		// enlighten objects
		gBuffer.SetDrawLights();

		if (scene._lightAmbient != nullptr && scene._lightAmbient->GetEnabled())
		{
			gBuffer.SetDrawLightAmbient();
			gBuffer.DrawLightAmbient(*scene._lightAmbient);
		}

		if (scene._lightsDirectional.size() > 0)
		{
			gBuffer.SetDrawLightDirectional();

			for (auto it = scene._lightsDirectional.begin(); it != scene._lightsDirectional.end(); ++it)
			{
				if((*it)->GetEnabled())
					gBuffer.DrawLightDirectional(**it);
			}
		}

		if (scene._lightsPoint.size() > 0)
		{
			gBuffer.SetDrawLightPoint();

			for (auto it = scene._lightsPoint.begin(); it != scene._lightsPoint.end(); ++it)
			{
				if ((*it)->GetEnabled())
					gBuffer.DrawLightPoint(**it);
			}
		}

		// apply postprocesses

		scene.GetController()->GetProfiler()->UpdatePostprocessBegin();

		gBuffer.SetDrawPostprocesses();
		for (auto it = scene._postprocesses.begin(); it != scene._postprocesses.end(); ++it)
		{
			if ((*it)->GetEnabled())
			{
				gBuffer.DrawPostprocess(**it);
			}
		}

		scene.GetController()->GetProfiler()->UpdatePostprocessEnd();

		gBuffer.SetDrawTexts();

		Renderer::GetInstance()->SetBlendMode(Renderer::BlendMode::ALPHA);

		scene.GetTextShader()->Set();

		for (auto it = scene._texts.begin(); it != scene._texts.end(); ++it)
		{
			(*it)->Draw();
		}

		Renderer::GetInstance()->SetBlendMode(Renderer::BlendMode::SOLID);

		gBuffer.EndFrame();
	}
}

void Camera::SetDirection(const XMFLOAT3A & direction)
{
	XMVECTOR pos = XMLoadFloat3(&_position);
	XMVECTOR dir = XMLoadFloat3(&_target) - pos;

	XMVECTOR len = XMVector3Length(dir);
	for (size_t i = 0; i < 4; ++i)
	{
		len.m128_f32[i] = max(len.m128_f32[i], 0.1f);
	}
	XMVECTOR nTgt = pos + XMVector3Normalize(XMLoadFloat3(&direction)) * len;

	XMStoreFloat3(&_target, nTgt);

	_bNeedUpdateView = true;
}

void Camera::SetDirection(const XMVECTOR & direction)
{
	XMVECTOR pos = XMLoadFloat3(&_position);
	XMVECTOR dir = XMLoadFloat3(&_target) - pos;

	XMVECTOR nTgt = pos + XMVector3Normalize(direction);

	XMStoreFloat3(&_target, nTgt);

	_bNeedUpdateView = true;
}

inline void Camera::UpdateView()
{
	XMVECTOR pos = XMLoadFloat3(&_position);
	XMVECTOR tgt = XMLoadFloat3(&_target);
	XMVECTOR up = XMVector3Normalize(XMLoadFloat3(&_up));
	XMVECTOR dir = XMVector3Normalize(tgt - pos);
	XMVECTOR right = XMVector3Cross(up, dir);

	XMStoreFloat3(&_direction, dir);
	XMStoreFloat3(&_up, up);
	XMStoreFloat3(&_right, right);

	XMMATRIX mat = XMMatrixLookAtLH(pos, tgt, up);
	XMStoreFloat4x4(&_matView, mat);
	XMStoreFloat4x4(&_matViewInverse, XMMatrixInverse(&XMMatrixDeterminant(mat), mat));
}

inline void Camera::UpdateProjection()
{
	XMMATRIX matProj = XMMatrixPerspectiveFovLH(_fov,
		System::GetInstance()->GetOptions()._aspectRatio, 
		_near, 
		_far);
	XMStoreFloat4x4(&_matProj, matProj);
	XMStoreFloat4x4(&_matProjInverse, XMMatrixInverse(&XMMatrixDeterminant(matProj), matProj));
}

inline void Camera::MergeMatrices()
{
	XMMATRIX v = XMLoadFloat4x4(&_matView);
	XMMATRIX p = XMLoadFloat4x4(&_matProj);
	XMMATRIX vp = v * p;
	XMStoreFloat4x4(&_matViewProj, vp);
}

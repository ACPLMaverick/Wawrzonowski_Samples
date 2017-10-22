#include "MeshMav.h"
#include "RendererMav.h"
#include "GraphicsDevice.h"
#include "../System.h"
#include "../Camera.h"
#include "../Scene.h"
#include "../Timer.h"

namespace rendererMav
{
	MeshMav::MeshMav() :
		Mesh()
	{
		_material = System::GetInstance()->GetCurrentScene()->GetDefaultMaterial();
	}

	MeshMav::MeshMav(const math::Float3 * pos, const math::Float3 * rot, const math::Float3 * scl, const std::string * fPath) :
		Mesh(pos, rot, scl, fPath)
	{
		_material = System::GetInstance()->GetCurrentScene()->GetDefaultMaterial();
	}


	MeshMav::~MeshMav()
	{
	}

	RayHit MeshMav::CalcIntersect(Ray & ray)
	{
		return RayHit();
	}

	void MeshMav::Update()
	{
		math::Float3 trans = *_transform.GetRotation();
		trans = trans + _rotationVector * (float)Timer::GetInstance()->GetDeltaTime();
		_transform.SetRotation(&trans);
	}

	void MeshMav::Draw()
	{
		GraphicsDevice* gd = ((RendererMav*)System::GetInstance()->GetRenderer())->GetGraphicsDevice();
		gd->SetVertexBuffer(_positionArray.data());
		gd->SetUVBuffer(_uvArray.data());
		gd->SetNormalBuffer(_normalArray.data());	// temporarily setting normal buffer as color buffer

		Camera* cam = System::GetInstance()->GetCurrentScene()->GetCurrentCamera();
		math::Matrix4x4 wvp = *_transform.GetWorldMatrix() * *cam->GetViewProjMatrix();
		math::Matrix4x4 wv = *_transform.GetWorldMatrix() * *cam->GetViewMatrix();
		gd->SetWorldViewProjMatrix(&wvp);

		gd->SetWorldMatrix(_transform.GetWorldMatrix());
		gd->SetWorldInverseTransposeMatrix(_transform.GetWorldInverseTransposeMatrix());

		gd->SetCameraPosition(cam->GetPosition());
		gd->SetMaterial(_material);

		gd->DrawIndexed(_triangleCount, _indexArray.data());
	}

}
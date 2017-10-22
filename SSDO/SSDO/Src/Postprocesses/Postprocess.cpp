#include "stdafx.h"
#include "Postprocess.h"
#include "Shader.h"
#include "Camera.h"

namespace Postprocesses
{
	Postprocess::Postprocess()
	{
	}

	Postprocess::~Postprocess()
	{
	}

	void Postprocess::SetPass(GBuffer& gBuffer, const Camera& camera, int32_t passIndex) const
	{
		_shaders[passIndex]->Set();
		Shader::LightCommonDataPS* cd = reinterpret_cast<Shader::LightCommonDataPS*>(_shaders[passIndex]->MapPsBuffer(0));
		XMMATRIX projInv = XMLoadFloat4x4A(&camera.GetMatProjInverse());
		XMStoreFloat4x4A(&cd->gProjInverse, XMMatrixTranspose(projInv));
		_shaders[passIndex]->UnmapPsBuffer(0);
	}

	void Postprocess::AfterPass(GBuffer & gBuffer, const Camera & camera, int32_t passIndex) const
	{
	}

}
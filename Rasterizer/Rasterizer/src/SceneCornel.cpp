#include "SceneCornel.h"
#include "Camera.h"
#include "Triangle.h"
#include "SpecificObjectFactory.h"
#include "System.h"
#include "Mesh.h"
#include "light/LightAmbient.h"
#include "light/LightDirectional.h"
#include "light/LightSpot.h"

#include <string>

SceneCornel::SceneCornel()
{
}


SceneCornel::~SceneCornel()
{
}

void SceneCornel::InitializeScene()
{
	// camera

	_cameras.push_back(new Camera(
		&math::Float3(0.0f, 8.0f, -20.0f),
		&math::Float3(0.0f, 0.0f, 0.0f),
		&math::Float3(0.0f, 1.0f, 0.0f),
		50.0f,
		(float)System::GetInstance()->GetSystemSettings()->GetDisplayWidth() /
		(float)System::GetInstance()->GetSystemSettings()->GetDisplayHeight()
	));

	// materials

	Texture* diffWhite = new Texture(Color32(0xFFFFFFFF));
	Texture* nrm = new Texture(Color32(0xFF7F7FFF));

	Material* matDiffWhite =  new Material
	(
		diffWhite,
		nrm,
		Color32(0xFFFFFFFF),
		Color32(0xFFFFFFFF),
		Color32(0xFFFFFFFF),
		60.0f
	);
	_materials.push_back(matDiffWhite);

	Material* matDiffRed = new Material
	(
		diffWhite,
		nrm,
		Color32(0xFFFFFFFF),
		Color32(0xFFFF0000),
		Color32(0xFFFFFFFF),
		60.0f
	);
	_materials.push_back(matDiffRed);

	Material* matDiffBlue = new Material
	(
		diffWhite,
		nrm,
		Color32(0xFFFFFFFF),
		Color32(0xFF0000FF),
		Color32(0xFFFFFFFF),
		60.0f
	);
	_materials.push_back(matDiffBlue);

	Material* matRefl = new Material
	(
		diffWhite,
		nrm,
		Color32(0xFFFFFFFF),
		Color32(0xFFFFFFFF),
		Color32(0xFFFFFFFF),
		60.0f,
		1.0f
	);
	_materials.push_back(matRefl);

	Material* matRefr = new Material
	(
		diffWhite,
		nrm,
		Color32(0xFFFFFFFF),
		Color32(0xFFFFFFFF),
		Color32(0xFFFFFFFF),
		60.0f,
		0.0f,
		1.0f,
		1.1f
	);
	_materials.push_back(matRefr);

	// meshes

	math::Float3 cPos(1.0f, 2.0f, -10.0f);
	math::Float3 cPos2(-1.0f, 2.0f, -10.0f);
	math::Float3 cRot(0.0f, 0.0f, 0.0f);
	math::Float3 cScl(0.5f, 0.5f, 0.5f);
	std::string cPath = "sphere";
	float offset = 1.0f;
	int row = 10;
	for (int i = 0, w = 0; i < 50; ++i)
	{
		if (i != 0 && i % row == 0)
		{
			++w;
		}

		cPos.z = -10.0f + (i % row) * offset;
		cPos.x = -(1.0f + w * offset);

		cPos2.z = -10.0f + (i % row) * offset;
		cPos2.x = 1.0f + w * offset;

		Mesh* sRefl = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
		sRefl->SetMaterialPtr(matRefl);
		_primitives.push_back(sRefl);
		Mesh* sRefr = SpecificObjectFactory::GetMesh(&cPos2, &cRot, &cScl, &cPath);
		sRefr->SetMaterialPtr(matRefr);
		_primitives.push_back(sRefr);
	}


	cPath = "cube";
	cPos = math::Float3(0.0f, 10.0f, 20.0f);
	cScl = math::Float3(10.0f, 10.0f, 10.0f);
	Mesh* c1 = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
	c1->SetMaterialPtr(matDiffWhite);
	_primitives.push_back(c1);

	cPos = math::Float3(-20.0f, 10.0f, 0.0f);
	Mesh* c2 = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
	c2->SetMaterialPtr(matDiffRed);
	_primitives.push_back(c2);

	cPos = math::Float3(20.0f, 10.0f, 0.0f);
	Mesh* c3 = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
	c3->SetMaterialPtr(matDiffBlue);
	_primitives.push_back(c3);

	cPath = "floor";
	cPos = math::Float3(0.0f, 0.0f, 9.0f);
	cScl = math::Float3(5.0f, 5.0f, 5.0f);
	Mesh* fl = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
	fl->SetMaterialPtr(matDiffWhite);
	_primitives.push_back(fl);

	// lights

	Color32 col(1.0f, 0.05f, 0.02f, 0.1f);
	_lightAmbient = SpecificObjectFactory::GetLightAmbient(&col);

	col = Color32(1.0f, 0.2f, 0.2f, 0.2f);
	math::Float3 dirDir(-1.0f, -1.0f, 1.0f);
	//_lightsDirectional.push_back(SpecificObjectFactory::GetLightDirectional(&col, &dirDir));

	dirDir = math::Float3(0.0f, -1.0f, 0.0f);
	col = Color32(1.0f, 1.0f, 1.0f, 1.0f);
	cPos = math::Float3(0.0f, 10.0f, 0.0f);
	_lightsSpot.push_back(SpecificObjectFactory::GetLightSpot(
		&col,
		&dirDir,
		&cPos,
		0.0f,
		0.005f,
		0.005f,
		0.0f,
		0.0f,
		0.6f
	));
}
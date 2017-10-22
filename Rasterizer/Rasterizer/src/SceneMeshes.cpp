#include "SceneMeshes.h"
#include "Camera.h"
#include "Triangle.h"
#include "SpecificObjectFactory.h"
#include "System.h"
#include "Mesh.h"
#include "light/LightAmbient.h"
#include "light/LightDirectional.h"
#include "light/LightSpot.h"

#ifdef RENDERER_MAV

#include "rendererMav/MeshMav.h"

#endif // RENDERER_MAV


SceneMeshes::SceneMeshes()
{
}


SceneMeshes::~SceneMeshes()
{
}

void SceneMeshes::InitializeScene()
{
	// camera

	_cameras.push_back(new Camera(
		&math::Float3(3.0f, 3.0f, -10.0f),
		&math::Float3(0.0f, 0.0f, 0.0f),
		&math::Float3(0.0f, 1.0f, 0.0f),
		50.0f,
		(float)System::GetInstance()->GetSystemSettings()->GetDisplayWidth() /
		(float)System::GetInstance()->GetSystemSettings()->GetDisplayHeight()
	));

	// materials

	std::string cPath = "sunflowers";
	Texture* mat1Diff = new Texture(&cPath, Texture::SampleMode::SPHERICAL);
	cPath = "canteen_normals";
	Texture* mat1Nrm = new Texture(&cPath);
	Material* mat1 = new Material
	(
		mat1Diff,
		mat1Nrm,
		Color32(0xFFFFFFFF),
		Color32(0xFFFFFFFF),
		Color32(0xFFFFEEEE),
		60.0f
	);
	_materials.push_back(mat1);

	cPath = "janusz";
	Texture* mat2Diff = new Texture(&cPath, Texture::SampleMode::UV, Texture::WrapMode::CLAMP);
	Texture* mat2Nrm = new Texture(Color32((uint8_t)255, 127, 127, 255));
	Material* mat2 = new Material
	(
		mat2Diff,
		mat2Nrm,
		Color32(0xFFFFFFFF),
		Color32(0xFFFFFFFF),
		Color32(0xFFFFFFFF),
		30.0f
	);
	_materials.push_back(mat2);

	cPath = "floor";
	Texture* mat3Diff = new Texture(&cPath);
	Texture* mat3Nrm = new Texture(Color32((uint8_t)255, 127, 127, 255));
	Material* mat3 = new Material
	(
		mat3Diff,
		mat3Nrm,
		Color32(0xFFFFFFFF),
		Color32(0xFFFFFFFF),
		Color32(0xFFFFFFFF),
		60.0f
	);
	_materials.push_back(mat3);

	// meshes

	math::Float3 cPos(0.0f, 1.0f, 0.0f);
	math::Float3 cRot(0.0f, 90.0f, 0.0f);
	math::Float3 cScl(2.0f, 2.0f, 2.0f);
	//cPath = "monkey";
	cPath = "sphere";
	Mesh* m1 = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
	m1->SetMaterialPtr(mat1);
	_primitives.push_back(m1);

	cPath = "cube";
	cPos = math::Float3(-8.0f, 0.5f, 4.0f);
	Mesh* m2 = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
	m2->SetMaterialPtr(mat2);
	_primitives.push_back(m2);

	cPath = "floor";
	cPos = math::Float3(0.0f, -1.0f, 9.0f);
	cScl = math::Float3(4.0f, 4.0f, 4.0f);
	Mesh* m3 = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
	m3->SetMaterialPtr(mat3);
	_primitives.push_back(m3);

	cPath = "cubeStretchedUVs";
	cPos = math::Float3(4.0f, 0.0f, -3.0f);
	cScl = math::Float3(1.0f, 1.0f, 1.0f);
	Mesh* m4 = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
	m4->SetMaterialPtr(mat2);
	_primitives.push_back(m4);

	cPos = math::Float3(-1.0f, 0.0f, -4.5f);
	cScl = math::Float3(1.0f, 0.5f, 0.5f);
	cRot = math::Float3(0.0f, 30.0f, 0.0f);
	Mesh* m5 = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
	m5->SetMaterialPtr(mat3);
	_primitives.push_back(m5);

	cPos = math::Float3(3.2f, 2.0f, 4.45f);
	cScl = math::Float3(1.0f, 1.0f, 1.0f);
	cRot = math::Float3(0.0f, 50.0f, 0.0f);
	Mesh* m6 = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
	m6->SetMaterialPtr(mat2);
	_primitives.push_back(m6);

	cPos = math::Float3(3.0f, 0.0f, 4.0f);
	cRot = math::Float3(0.0f, 0.0f, 0.0f);
	Mesh* m7 = SpecificObjectFactory::GetMesh(&cPos, &cRot, &cScl, &cPath);
	m7->SetMaterialPtr(mat2);
	_primitives.push_back(m7);

	// lights

	Color32 col(1.0f, 0.05f, 0.02f, 0.1f);
	_lightAmbient = SpecificObjectFactory::GetLightAmbient(&col);

	col = Color32(1.0f, 0.4f, 0.5f, 0.4f);
	math::Float3 dirDir(-1.0f, -1.0f, 1.0f);
	_lightsDirectional.push_back(SpecificObjectFactory::GetLightDirectional(&col, &dirDir));

	dirDir = math::Float3(0.5f, -0.75f, 0.0f);
	col = Color32(1.0f, 1.0f, 1.0f, 1.0f);
	cPos = math::Float3(2.0f, 3.0f, -2.0f);
	_lightsSpot.push_back(SpecificObjectFactory::GetLightSpot(
		&col,
		&dirDir,
		&cPos,
		0.0f,
		0.05f,
		0.05f,
		0.0f,
		0.0f,
		0.5f
	));

#ifdef RENDERER_MAV

	cRot = math::Float3(0.0f, 20.0f, 0.0f);
	((rendererMav::MeshMav*)m1)->SetRotationVector(&cRot);
	cRot = math::Float3(0.0f, 12.0f, 0.0f);
	((rendererMav::MeshMav*)m2)->SetRotationVector(&cRot);

#endif // RENDERER_MAV
}
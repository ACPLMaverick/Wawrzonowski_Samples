#include "Material.h"



Material::Material
(
	Texture* const diffuse,
	Texture* const normal,
	Color32 amb,
	Color32 diff,
	Color32 spec,
	float gloss,
	float reflectivity,
	float refractivity,
	float refrCoeff
) :
	_colorAmbient(amb),
	_colorDiffuse(diff),
	_colorSpecular(spec),
	_glossiness(gloss),
	_reflectivity(Clamp(reflectivity, 0.0f, 1.0f)),
	_refractivity(Clamp(refractivity, 0.0f, 1.0f)),
	_refractionCoeff(refrCoeff),
	_mapDiffuse(diffuse),
	_mapNormal(normal)
{
}


Material::~Material()
{
	if (_mapDiffuse != nullptr)
	{
		delete _mapDiffuse;
	}
	if (_mapNormal != nullptr)
	{
		delete _mapNormal;
	}
}

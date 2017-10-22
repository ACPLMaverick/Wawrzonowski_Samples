#pragma once
#include "LightAmbient.h"
#include "../Float3.h"

namespace light
{
	class LightPoint : public LightAmbient
	{
	protected:

#pragma region Protected

		math::Float3 _position;
		float _attenuationConstant;
		float _attenuationLinear;
		float _attenuationQuadratic;

#pragma endregion

	public:

#pragma region Functions Public

		LightPoint();
		LightPoint(const Color32* col, const math::Float3* position, float attConstant, float attLinear, float attQuadratic);
		~LightPoint();

#pragma region Accessors

		const math::Float3* GetPosition() { return &_position; }
		float GetAttenuationConstant() { return _attenuationConstant; }
		float GetAttenuationLinear() { return _attenuationLinear; }
		float GetAttenuationQuadratic() { return _attenuationQuadratic; }

#pragma endregion

#pragma endregion
	};

}
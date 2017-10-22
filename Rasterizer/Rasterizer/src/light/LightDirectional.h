#pragma once

#include "LightAmbient.h"
#include "../Float3.h"

namespace light
{
	class LightDirectional : public LightAmbient
	{
	protected:

#pragma region Protected

		math::Float3 _direction;

#pragma endregion

	public:

#pragma region FunctionsPublic

		LightDirectional();
		LightDirectional(const Color32* col, const math::Float3* direction);
		~LightDirectional();

#pragma region Accessors

		const math::Float3* GetDirection() const { return &_direction; }
		void SetDirection(const math::Float3* dir) { _direction = *dir; }

#pragma endregion

#pragma endregion

	};
}
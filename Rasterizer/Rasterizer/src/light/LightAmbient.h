#pragma once

#include "../Color32.h"

namespace light
{
	class LightAmbient
	{
	protected:

#pragma region Protected

		Color32 _color;

#pragma endregion

	public:
#pragma region Functions Public

		LightAmbient();
		LightAmbient(const Color32* color);
		~LightAmbient();

#pragma region Accessors

		const Color32* GetColor() const { return &_color; }
		void SetColor(const Color32* col) { _color = *col; }

#pragma endregion

#pragma endregion
	};
}
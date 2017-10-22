#pragma once

#include "renderer/Light.h"
#include "utility/MColor.h"

namespace morphEngine
{
	namespace renderer
	{
		class LightAmbient :
			public Light
		{
			ME_TYPE
		protected:

#pragma region Protected

			utility::MColor _color = utility::MColor::Black;

#pragma endregion

#pragma region Functions Protected

			LightAmbient(const gom::ObjectInitializer& initializer);
			LightAmbient(const LightAmbient& copy, bool bDeepCopy = true);

			inline LightAmbient& operator=(const LightAmbient& copy)
			{
				Light::operator=(copy);
				_color = copy._color;
				return *this;
			}

			virtual void RegisterProperties() override;

#pragma endregion
		public:
#pragma region Functions Public

			virtual ~LightAmbient();

#pragma region Accessors

			inline utility::MColor GetColor() const { return _color; }
			inline void SetColor(const utility::MColor& color) { _color = color; }

#pragma endregion

#pragma endregion
		};

	}
}
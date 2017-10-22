#pragma once

#include "renderer/Light.h"
#include "utility/MColor.h"
#include "utility/MVector.h"

namespace morphEngine
{
	namespace renderer
	{
		class LightDirectional :
			public Light
		{
			ME_TYPE
		protected:

#pragma region Protected

			utility::MColor _color = utility::MColor::White;

#pragma endregion

#pragma region Functions Protected

			LightDirectional(const gom::ObjectInitializer& initializer);
			LightDirectional(const LightDirectional& copy, bool bDeepCopy = true);

			inline LightDirectional& operator=(const LightDirectional& copy)
			{
				Light::operator=(copy);
				_color = copy._color;
				return *this;
			}

			virtual void RegisterProperties() override;

#pragma endregion
		public:

#pragma region Functions Public

			~LightDirectional();

#pragma region Accessors
			
			inline utility::MColor GetColor() const { return _color; }
			inline utility::MVector3 GetDirection() const { return _owner->GetTransform()->GetDirection(); }

			inline void SetColor(const utility::MColor& color) { _color = color; }

#pragma endregion

#pragma endregion
		};

	}
}
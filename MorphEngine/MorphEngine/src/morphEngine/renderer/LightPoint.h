#pragma once

#include "renderer/Light.h"
#include "../utility/MColor.h"

namespace morphEngine
{
	namespace renderer
	{
		class LightPoint :
			public Light
		{
			ME_TYPE
		protected:

#pragma region Protected

			utility::MColor _color = utility::MColor::White;
			MFloat32 _range = 10.0f;

#pragma endregion

#pragma region Functions Protected

			LightPoint(const gom::ObjectInitializer& initializer);
			LightPoint(const LightPoint& copy, bool bDeepCopy = true);

			inline LightPoint& operator=(const LightPoint& copy)
			{
				Light::operator=(copy);
				_color = copy._color;
				_range = copy._range;
				return *this;
			}

			virtual void RegisterProperties() override;
			virtual void UpdateBounds() override;

#pragma endregion
		public:

#pragma region Functions Public

			virtual ~LightPoint();

#pragma region Accessors

			inline utility::MColor GetColor() const { return _color; }
			inline MFloat32 GetRange() const { return _range; }
			inline utility::MVector3 GetPosition() const { return _owner->GetTransform()->GetPosition(); }

			inline void SetColor(const utility::MColor& color) { _color = color; }
			inline void SetRange(MFloat32 range) { _range = range; }

#pragma endregion

#pragma endregion
		};

	}
}
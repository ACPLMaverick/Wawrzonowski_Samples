#pragma once

#include "renderer/Light.h"
#include "../utility/MVector.h"
#include "../utility/MColor.h"
#include "../utility/MMath.h"

namespace morphEngine
{
	namespace renderer
	{
		class LightSpot :
			public Light
		{
			ME_TYPE
		protected:

#pragma region Protected

			utility::MColor _color = utility::MColor::White;
			MFloat32 _range = 10.0f;
			MFloat32 _coneAngle = MMath::PIOver4;
			MFloat32 _smooth = 0.5f;

#pragma endregion

#pragma region Functions Protected

			LightSpot(const gom::ObjectInitializer& initializer);
			LightSpot(const LightSpot& copy, bool bDeepCopy = true);

			inline LightSpot& operator=(const LightSpot& copy)
			{
				Light::operator=(copy);
				_color = copy._color;
				_range = copy._range;
				_smooth = copy._smooth;
			}
			
			virtual void RegisterProperties() override;
			virtual void UpdateBounds() override;

#pragma endregion

		public:

#pragma region Functions Public

			virtual ~LightSpot();

#pragma region Accessors

			inline utility::MColor GetColor() const { return _color; }
			inline utility::MVector3 GetPosition() const { return _owner->GetTransform()->GetPosition(); }
			inline utility::MVector3 GetDirection() const { return _owner->GetTransform()->GetDirection(); }
			inline MFloat32 GetRange() const { return _range; }
			inline MFloat32 GetConeAngle() const { return MMath::Rad2Deg * _coneAngle; }
			inline MFloat32 GetSmooth() const { return _smooth; }

			inline void SetColor(const utility::MColor& color) { _color = color; }
			inline void SetRange(MFloat32 range) { _range = range; }
			inline void SetConeAngle(MFloat32 coneAngle) { _coneAngle = MMath::Deg2Rad * coneAngle; }
			inline void SetSmooth(MFloat32 smooth) { _smooth = smooth; }

#pragma endregion

#pragma endregion
		};

	}
}
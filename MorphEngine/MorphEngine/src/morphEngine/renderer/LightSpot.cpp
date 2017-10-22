#include "LightSpot.h"


namespace morphEngine
{
	namespace renderer
	{

		LightSpot::LightSpot(const gom::ObjectInitializer& initializer) :
			Light(initializer)
		{
			RegisterProperties();
		}

		LightSpot::LightSpot(const LightSpot & copy, bool bDeepCopy) :
			Light(copy, bDeepCopy),
			_color(copy._color),
			_range(copy._range),
			_coneAngle(copy._coneAngle),
			_smooth(copy._smooth)
		{
			RegisterProperties();
		}


		void LightSpot::RegisterProperties()
		{
			RegisterProperty("Color", &_color);
			RegisterProperty("Range", &_range);
			RegisterProperty("ConeAngle", &_coneAngle);
			RegisterProperty("Smooth", &_smooth);
		}

		void LightSpot::UpdateBounds()
		{
			MFloat32 r = _range * 0.1f;
			MVector3 halfSize(r, r, r);
			MVector3 offset(0.0f, 0.0f, r);
			_bounds.MinLocal = -halfSize + offset;
			_bounds.MaxLocal = halfSize + offset;
			UpdateBoundsWorld(_owner->GetTransform());
		}

		LightSpot::~LightSpot()
		{
		}

	}
}
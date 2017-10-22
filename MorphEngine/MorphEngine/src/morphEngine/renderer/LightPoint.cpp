#include "LightPoint.h"


namespace morphEngine
{
	namespace renderer
	{
		LightPoint::LightPoint(const gom::ObjectInitializer& initializer) :
			Light(initializer)
		{
			RegisterProperties();
		}

		LightPoint::LightPoint(const LightPoint & copy, bool bDeepCopy) :
			Light(copy, bDeepCopy),
			_color(copy._color),
			_range(copy._range)
		{
			RegisterProperties();
		}


		void LightPoint::RegisterProperties()
		{
			RegisterProperty("Color", &_color);
			RegisterProperty("Range", &_range);
		}

		void LightPoint::UpdateBounds()
		{
			MFloat32 r = _range * 0.5f * MMath::Sqrt(2.0f) * 3.0f;
			MVector3 halfSize(r, r, r);
			_bounds.MinLocal = -halfSize;
			_bounds.MaxLocal = halfSize;
			UpdateBoundsWorld(_owner->GetTransform());
		}

		LightPoint::~LightPoint()
		{
		}

	}
}
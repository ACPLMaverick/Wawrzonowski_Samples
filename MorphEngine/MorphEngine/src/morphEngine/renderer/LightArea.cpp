#include "LightArea.h"


namespace morphEngine
{
	namespace renderer
	{
		LightArea::LightArea(const gom::ObjectInitializer& initializer) :
			Light(initializer)
		{
			RegisterProperties();
		}

		LightArea::LightArea(const LightArea & copy, bool bDeepCopy) :
			Light(copy, bDeepCopy),
			_color(copy._color),
			_size(copy._size),
			_range(copy._range),
			_smooth(copy._smooth)
		{
			RegisterProperties();
		}


		void LightArea::RegisterProperties()
		{
			RegisterProperty("Color", &_color);
			RegisterProperty("Size", &_size);
			RegisterProperty("Range", &_range);
			RegisterProperty("Smooth", &_smooth);
		}

		void LightArea::UpdateBounds()
		{
			MFloat32 s(1.5f);
			MVector3 halfSize(_size.X * s, _size.Y * s, _range * s);
			MVector3 offset(0.0f, 0.0f, halfSize.Z);
			_bounds.MinLocal = -halfSize + offset;
			_bounds.MaxLocal = halfSize + offset;
			UpdateBoundsWorld(_owner->GetTransform());
		}

		LightArea::~LightArea()
		{
		}

	}
}
#pragma once

#include "renderer\Light.h"
#include "utility/MColor.h"
#include "utility/MVector.h"

namespace morphEngine
{
	namespace renderer
	{
		class LightArea :
			public Light
		{
			ME_TYPE
		protected:

#pragma region Protected

			utility::MColor _color = utility::MColor::White;
			utility::MVector2 _size = MVector2(10.0f, 10.0f);
			MFloat32 _range = 10.0f;
			MFloat32 _smooth = 0.5f;

#pragma endregion

#pragma region Functions Protected

			LightArea(const gom::ObjectInitializer& initializer);
			LightArea(const LightArea& copy, bool bDeepCopy = true);

			inline LightArea& operator=(const LightArea& copy)
			{
				Light::operator=(copy);
				_color = copy._color;
				_size = copy._size;
				_range = copy._range;
				_smooth = copy._smooth;

				return *this;
			}

			virtual void RegisterProperties() override;
			virtual void UpdateBounds() override;

#pragma endregion
		public:

#pragma region Functions Public

			virtual ~LightArea();

#pragma region Accessors

			inline utility::MColor GetColor() const { return _color; }
			inline utility::MVector3 GetPosition() const { return _owner->GetTransform()->GetPosition(); }
			inline utility::MVector3 GetDirection() const { return _owner->GetTransform()->GetDirection(); }
			inline utility::MVector3 GetRight() const { return _owner->GetTransform()->GetRight(); }
			inline utility::MVector2 GetSize() const { return _size; }
			inline MFloat32 GetRange() const { return _range; }
			inline MFloat32 GetSmooth() const { return _smooth; }

			inline void SetColor(const utility::MColor& color) { _color = color; }
			inline void SetSize(const utility::MVector2 size) { _size = size; }
			inline void SetRange(MFloat32 range) { _range = range; }
			inline void SetSmooth(MFloat32 smooth) { _smooth = smooth; }

#pragma endregion

#pragma endregion
		};

	}
}
#pragma once

#include "../Triangle.h"
#include "../SpecificObjectFactory.h"

namespace rendererMav
{
	/// <summary>
	/// Winding direction - clockwise
	/// Rasterization rule - top-left are rasterized with addition
	/// Right-handed coordinate system
	/// Row-major matrices
	/// </summary>
	class TriangleMav : public Triangle
	{
		friend class SpecificObjectFactory;
	protected:
#pragma region Functions Protected

		TriangleMav(math::Float3& x, math::Float3& y, math::Float3& z,
			math::Float2& ux, math::Float2& uy, math::Float2& uz,
			math::Float3& cx = math::Float3(1.0f, 1.0f, 1.0f), math::Float3& cy = math::Float3(1.0f, 1.0f, 1.0f), math::Float3& cz = math::Float3(1.0f, 1.0f, 1.0f),
			Color32& col = Color32());

#pragma endregion

	public:

#pragma region Functions Public

		virtual ~TriangleMav();

		virtual void Update() override;
		virtual void Draw();

#pragma endregion
	};
}
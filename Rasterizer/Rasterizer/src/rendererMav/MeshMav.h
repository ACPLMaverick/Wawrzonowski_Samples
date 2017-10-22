#pragma once

#include "../Mesh.h"

namespace rendererMav
{
	class MeshMav :
		public Mesh
	{
	protected:

#pragma region Protected

		math::Float3 _rotationVector;

#pragma endregion

	public:

#pragma region Functions Public

		MeshMav();
		MeshMav
		(
			const math::Float3* pos,
			const math::Float3* rot,
			const math::Float3* scl,
			const std::string* fPath
		);
		~MeshMav();

		RayHit CalcIntersect(Ray& ray) override;
		virtual void Update();
		virtual void Draw();

#pragma region Accessors

		const math::Float3* GetRotationVector() const { return &_rotationVector; }
		void SetRotationVector(const math::Float3* rv) { _rotationVector = *rv; }

#pragma endregion

#pragma endregion
	};

}
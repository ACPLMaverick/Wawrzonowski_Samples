#pragma once

#include "../Mesh.h"
#include "TriangleFGK.h"

namespace rendererFGK
{
	class MeshFGK :
		public Mesh
	{
	protected:

#pragma region Protected

		math::Float3 _bbMin;
		math::Float3 _bbMax;

		math::Float3 _bbMinMul;
		math::Float3 _bbMaxMul;

		std::vector<TriangleFGK> _triangles;

#pragma endregion

	public:

#pragma region Functions Public

		MeshFGK();
		MeshFGK
		(
			const math::Float3* pos,
			const math::Float3* rot,
			const math::Float3* scl,
			const std::string* fPath
		);
		virtual ~MeshFGK();

		virtual RayHit CalcIntersect(Ray& ray) override;

		/// <summary>
		/// Updates world matrix.
		/// </summary>
		virtual void Update() override;

		/// <summary>
		/// Does nothing.
		/// </summary>
		virtual void Draw() override;

#pragma endregion

	};
}
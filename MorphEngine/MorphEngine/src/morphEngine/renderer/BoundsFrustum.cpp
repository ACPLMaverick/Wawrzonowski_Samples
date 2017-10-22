#include "BoundsFrustum.h"
#include "BoundsBox.h"
#include "BoundsSphere.h"
#include "meshes/MeshBox.h"
#include "resourceManagement/ResourceManager.h"
#include "shaders/ShaderBounds.h"
#include "gom/Camera.h"

#define COMPLEX_CHECK

namespace morphEngine
{
	using namespace resourceManagement;

	namespace renderer
	{
		using namespace renderer;

		BoundsFrustum::~BoundsFrustum()
		{
			if (_mesh != nullptr)
			{
				ResourceManager::GetInstance()->DestroyMeshInstance(_mesh);
				_mesh = nullptr;
			}
		}

		void BoundsFrustum::Draw(memoryManagement::Handle<gom::Camera> camera) const
		{
			if (_mesh == nullptr)
			{
				const_cast<BoundsFrustum*>(this)->_mesh = reinterpret_cast<meshes::MeshBox*>(ResourceManager::GetInstance()->CreateMeshInstance("MeshBox"));
				_mesh->SetSmooth(true);

				const_cast<BoundsFrustum*>(this)->UpdateMeshVertices();

				_mesh->Initialize();
			}

			_shader->SetLocal(camera->GetMatViewProj(), *_mesh);

			RenderingManager::GetInstance()->GetDevice()->Draw(static_cast<MUint32>(_mesh->GetIndexCount()), 0, 0);
		}

		bool BoundsFrustum::IntersectsWith(const BoundsSphere & other) const
		{
			ME_ASSERT(false, "Not implemented.");
			return false;
		}

		bool BoundsFrustum::IntersectsWith(const BoundsBox & other) const
		{
			// http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm

			MVector3 min = other.GetMinWorld();
			MVector3 max = other.GetMaxWorld();

			for (MSize i = 0; i < 6; ++i)
			{
				MSize out = 0;
				out += (Planes[i].GetIsPointBehind(min) ? 0 : 1);
				out += (Planes[i].GetIsPointBehind(MVector3(max.X, min.Y, min.Z)) ? 0 : 1);
				out += (Planes[i].GetIsPointBehind(MVector3(min.X, max.Y, min.Z)) ? 0 : 1);
				out += (Planes[i].GetIsPointBehind(MVector3(max.X, max.Y, min.Z)) ? 0 : 1);
				out += (Planes[i].GetIsPointBehind(MVector3(min.X, min.Y, max.Z)) ? 0 : 1);
				out += (Planes[i].GetIsPointBehind(MVector3(max.X, min.Y, max.Z)) ? 0 : 1);
				out += (Planes[i].GetIsPointBehind(MVector3(min.X, max.Y, max.Z)) ? 0 : 1);
				out += (Planes[i].GetIsPointBehind(max) ? 0 : 1);
				if (out == 8) return false;	// box is completely on external side of this plane, so it cannot be inside frustum
			}

#ifdef COMPLEX_CHECK

			// perform complex check 
			// inversed situation - test if each corner of frustum lies outside of a plane defined by AABB's side
			MSize out;

			out = 0;
			for (MSize i = 0; i < 8; ++i)
			{
				out += (_pointsWorld[i].X > max.X) ? 1 : 0;
			}
			if (out == 8)
				return false;

			out = 0;
			for (MSize i = 0; i < 8; ++i)
			{
				out += (_pointsWorld[i].X < min.X) ? 1 : 0;
			}
			if (out == 8)
				return false;

			out = 0;
			for (MSize i = 0; i < 8; ++i)
			{
				out += (_pointsWorld[i].Y > max.Y) ? 1 : 0;
			}
			if (out == 8)
				return false;

			out = 0;
			for (MSize i = 0; i < 8; ++i)
			{
				out += (_pointsWorld[i].Y < min.Y) ? 1 : 0;
			}
			if (out == 8)
				return false;

			out = 0;
			for (MSize i = 0; i < 8; ++i)
			{
				out += (_pointsWorld[i].Z > max.Z) ? 1 : 0;
			}
			if (out == 8)
				return false;

			out = 0;
			for (MSize i = 0; i < 8; ++i)
			{
				out += (_pointsWorld[i].Z < min.Z) ? 1 : 0;
			}
			if (out == 8)
				return false;

#endif
			return true;
		}

		inline void BoundsFrustum::UpdateMeshVertices()
		{
			// ForwardTopLeft, ForwardBottomLeft, ForwardBottomRight, ForwardTopRight, RearTopLeft, RearBottomLeft, RearBottomRight, RearTopRight
			/*_mesh->GetVertexPositionsBuffer()[0] = _pointsWorld;
			_mesh->GetVertexPositionsBuffer()[1] = (MVector3(_maxWorld.X, _minWorld.Y, _minWorld.Z));
			_mesh->GetVertexPositionsBuffer()[2] = (MVector3(_maxWorld.X, _maxWorld.Y, _minWorld.Z));
			_mesh->GetVertexPositionsBuffer()[3] = (MVector3(_minWorld.X, _maxWorld.Y, _minWorld.Z));
			_mesh->GetVertexPositionsBuffer()[4] = (MVector3(_minWorld.X, _minWorld.Y, _maxWorld.Z));
			_mesh->GetVertexPositionsBuffer()[5] = (MVector3(_maxWorld.X, _minWorld.Y, _maxWorld.Z));
			_mesh->GetVertexPositionsBuffer()[6] = _maxWorld;
			_mesh->GetVertexPositionsBuffer()[7] = (MVector3(_minWorld.X, _maxWorld.Y, _maxWorld.Z));*/

			_mesh->GetVertexPositionsBuffer()[0] = _pointsWorld[5];
			_mesh->GetVertexPositionsBuffer()[1] = _pointsWorld[6];
			_mesh->GetVertexPositionsBuffer()[2] = _pointsWorld[7];
			_mesh->GetVertexPositionsBuffer()[3] = _pointsWorld[4];
			_mesh->GetVertexPositionsBuffer()[4] = _pointsWorld[1];
			_mesh->GetVertexPositionsBuffer()[5] = _pointsWorld[2];
			_mesh->GetVertexPositionsBuffer()[6] = _pointsWorld[3];
			_mesh->GetVertexPositionsBuffer()[7] = _pointsWorld[0];

			_mesh->UpdateVerticesResource();
		}
	}
}
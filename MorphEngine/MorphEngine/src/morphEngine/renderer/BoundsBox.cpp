#include "BoundsBox.h"
#include "BoundsSphere.h"
#include "BoundsFrustum.h"
#include "renderer/meshes/MeshBox.h"
#include "resourceManagement/ResourceManager.h"
#include "RenderingManager.h"
#include "shaders/ShaderBounds.h"
#include "gom/Camera.h"

namespace morphEngine
{
	using namespace resourceManagement;
	namespace renderer
	{
		void BoundsBox::Shutdown()
		{
			if (_mesh != nullptr)
			{
				ResourceManager::GetInstance()->DestroyMeshInstance(_mesh);
				_mesh = nullptr;
			}
		}

		void BoundsBox::Draw(memoryManagement::Handle<gom::Camera> camera) const
		{
			if (_mesh == nullptr)
			{
				const_cast<BoundsBox*>(this)->_mesh = reinterpret_cast<meshes::MeshBox*>(ResourceManager::GetInstance()->CreateMeshInstance("MeshBox"));
				_mesh->SetSmooth(true);
				
				const_cast<BoundsBox*>(this)->UpdateMeshVertices();

				_mesh->Initialize();
			}

			_shader->SetLocal(camera->GetMatViewProj(), *_mesh);

			RenderingManager::GetInstance()->GetDevice()->Draw(static_cast<MUint32>(_mesh->GetIndexCount()), 0, 0);
		}

		void BoundsBox::UpdateToWorld(memoryManagement::Handle<gom::Transform> transform)
		{
			CreateLocalCorners();

			_minWorld = MVector3(FLT_MAX, FLT_MAX, FLT_MAX);
			_maxWorld = MVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			for (MSize i = 0; i < 8; ++i)
			{
				_corners[i] = transform->GetMatWorld() * MVector4(_corners[i] * ExternalScale, 1.0f);
				_minWorld = MVector3::Min(_minWorld, _corners[i]);
				_maxWorld = MVector3::Max(_maxWorld, _corners[i]);
			}


			_center = (_minWorld + _maxWorld) * 0.5f;
			_extents = MVector3::Abs((_maxWorld - _minWorld) * 0.5f);

			if (_mesh != nullptr)
			{
				UpdateMeshVertices();
			}
		}

		void BoundsBox::UpdateToWorldIdentity()
		{
			CreateLocalCorners();

			_minWorld = MinLocal * ExternalScale;
			_maxWorld = MaxLocal * ExternalScale;

			_center = (_minWorld + _maxWorld) * 0.5f;
			_extents = MVector3::Abs((_maxWorld - _minWorld) * 0.5f);

			if (_mesh != nullptr)
			{
				UpdateMeshVertices();
			}
		}


		bool BoundsBox::IntersectsWith(const BoundsSphere & other) const
		{
			return GetSquaredDistToClosestPoint(other.GetCenterWorld()) <= other.GetRadiusWorld() * other.GetRadiusWorld();
		}

		bool BoundsBox::IntersectsWith(const BoundsFrustum & other) const
		{
			return other.IntersectsWith(*this);
		}

		bool BoundsBox::IsInside(const BoundsSphere & other) const
		{
			// TODO: Take all vertices into consideration
			MFloat32 rSquared = other.GetRadiusWorld() * other.GetRadiusWorld();
			return (_minWorld, other.GetCenterWorld()).LengthSquared() < rSquared && (_maxWorld, other.GetCenterWorld()).LengthSquared() < rSquared;
		}

		bool BoundsBox::IsInside(const BoundsFrustum & other) const
		{
			return false;
		}


		inline void BoundsBox::CreateLocalCorners()
		{
			_corners[0] = MinLocal;
			_corners[1] = (MVector3(MaxLocal.X, MinLocal.Y, MinLocal.Z));
			_corners[2] = (MVector3(MaxLocal.X, MaxLocal.Y, MinLocal.Z));
			_corners[3] = (MVector3(MinLocal.X, MaxLocal.Y, MinLocal.Z));
			_corners[4] = (MVector3(MinLocal.X, MinLocal.Y, MaxLocal.Z));
			_corners[5] = (MVector3(MaxLocal.X, MinLocal.Y, MaxLocal.Z));
			_corners[6] = MaxLocal;
			_corners[7] = (MVector3(MinLocal.X, MaxLocal.Y, MaxLocal.Z));
		}

		inline void BoundsBox::UpdateMeshVertices()
		{
			// Update mesh vertices according to minWorld and maxWorld
			_mesh->GetVertexPositionsBuffer()[0] = _minWorld;
			_mesh->GetVertexPositionsBuffer()[1] = (MVector3(_maxWorld.X, _minWorld.Y, _minWorld.Z));
			_mesh->GetVertexPositionsBuffer()[2] = (MVector3(_maxWorld.X, _maxWorld.Y, _minWorld.Z));
			_mesh->GetVertexPositionsBuffer()[3] = (MVector3(_minWorld.X, _maxWorld.Y, _minWorld.Z));
			_mesh->GetVertexPositionsBuffer()[4] = (MVector3(_minWorld.X, _minWorld.Y, _maxWorld.Z));
			_mesh->GetVertexPositionsBuffer()[5] = (MVector3(_maxWorld.X, _minWorld.Y, _maxWorld.Z));
			_mesh->GetVertexPositionsBuffer()[6] = _maxWorld;
			_mesh->GetVertexPositionsBuffer()[7] = (MVector3(_minWorld.X, _maxWorld.Y, _maxWorld.Z));

			_mesh->UpdateVerticesResource();
		}
	}
}
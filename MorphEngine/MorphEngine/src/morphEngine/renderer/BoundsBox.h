#pragma once
#include "Bounds.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace meshes
		{
			class MeshBox;
		}

		class BoundsBox : public Bounds
		{

		public:

#pragma region Public

			utility::MVector3 MinLocal = utility::MVector3(-1.0f, -1.0f, -1.0f);
			utility::MVector3 MaxLocal = utility::MVector3(1.0f, 1.0f, 1.0f);

#pragma endregion

		protected:

#pragma region Protected

			MVector3 _corners[8]{ utility::MVector3::Zero };

			utility::MVector3 _minWorld = MinLocal;
			utility::MVector3 _maxWorld = MaxLocal;

			utility::MVector3 _center = utility::MVector3::Zero;
			// I.e. box half-dimensions in each axis.
			utility::MVector3 _extents = utility::MVector3::One;

			meshes::MeshBox* _mesh = nullptr;

#pragma endregion

#pragma region Functions Protected

			inline void CreateLocalCorners();
			inline void UpdateMeshVertices();

#pragma endregion

		public:

#pragma region Functions Public

			inline BoundsBox()
			{
			}

			inline BoundsBox(const BoundsBox& c) : Bounds(c),
				MinLocal(c.MinLocal),
				MaxLocal(c.MaxLocal),
				_minWorld(c._minWorld),
				_maxWorld(c._maxWorld),
				_center(c._center),
				_extents(c._extents),
				_mesh(c._mesh)
			{ }

			inline virtual ~BoundsBox() 
			{ 
			}

			virtual void Shutdown() override;

			virtual void Draw(memoryManagement::Handle<gom::Camera> camera) const override;

			virtual void UpdateToWorld(memoryManagement::Handle<gom::Transform> transform) override;
			virtual void UpdateToWorldIdentity() override;

			inline virtual bool IntersectsWith(const BoundsBox& other) const override
			{
				if (MMath::Abs(_center.X - other._center.X) > (_extents.X + other._extents.X)) return false;
				if (MMath::Abs(_center.Y - other._center.Y) > (_extents.Y + other._extents.Y)) return false;
				if (MMath::Abs(_center.Z - other._center.Z) > (_extents.Z + other._extents.Z)) return false;

				// We have an intersection.
				return true;
			}

			virtual bool IntersectsWith(const BoundsSphere& other) const override;
			virtual bool IntersectsWith(const BoundsFrustum& other) const override;

			inline virtual bool IsInside(const BoundsBox& other) const override
			{
				return _minWorld.X > other._minWorld.X && _minWorld.Y > other._minWorld.Y && _minWorld.Z > other._minWorld.Z &&
					_maxWorld.X < other._maxWorld.X && _maxWorld.Y < other._maxWorld.Y && _maxWorld.Z < other._maxWorld.Z;
			}

			virtual bool IsInside(const BoundsSphere& other) const override;
			virtual bool IsInside(const BoundsFrustum& other) const override;

			const utility::MVector3& GetMinWorld() const { return _minWorld; }
			const utility::MVector3& GetMaxWorld() const { return _maxWorld; }
			const utility::MVector3& GetCenterWorld() const { return _center; }
			// I.e. box half-dimensions in each axis.
			const utility::MVector3& GetExtentsWorld() const { return _extents; }

			inline MFloat32 GetSquaredDistToClosestPoint(const utility::MVector3& point) const
			{
				MFloat32 ret = 0.0f;

				for (MInt32 i = 0; i < 3; ++i)
				{
					if (point[i] < _minWorld[i])
					{
						MFloat32 val = (_minWorld[i] - point[i]);
						ret += val * val;
					}

					if (point[i] > _maxWorld[i])
					{
						MFloat32 val = (point[i] - _maxWorld[i]);
						ret += val * val;
					}
				}

				return ret;
			}

			inline MVector3 GetClosestPoint(const utility::MVector3& point) const
			{
				MVector3 ret;

				for (MInt32 i = 0; i < 3; ++i)
				{
					if (point[i] > _maxWorld[i])
					{
						ret[i] = _maxWorld[i];
					}
					else if (point[i] < _minWorld[i])
					{
						ret[i] = _minWorld[i];
					}
					else
					{
						ret[i] = point[i];
					}
				}

				return ret;
			}

#pragma endregion


		};
	}
}
#pragma once
#include "Bounds.h"

namespace morphEngine
{
	namespace gom
	{
		class Camera;
	}

	namespace renderer
	{
		namespace meshes
		{
			class MeshBox;
		}

		class BoundsFrustum : public Bounds
		{
		protected:

#pragma region Structs Protected

			struct Plane
			{
				utility::MVector3 CenterLocal = utility::MVector3::Zero;
				utility::MVector3 NormalLocal = utility::MVector3::Forward;
				utility::MVector3 CenterWorld = CenterLocal;
				utility::MVector3 NormalWorld = NormalLocal;

				inline Plane() { }
				inline Plane(const Plane& c) :
					CenterLocal(c.CenterLocal),
					NormalLocal(c.NormalLocal),
					CenterWorld(c.CenterWorld),
					NormalWorld(c.NormalWorld)
				{
				}
				inline ~Plane() { }

				inline void UpdateToWorld(memoryManagement::Handle<gom::Transform> transform, MFloat32 externalScale)
				{
					CenterWorld = transform->GetMatWorld() * MVector4(CenterLocal * externalScale, 1.0f);
					NormalWorld = transform->GetMatWorldInverseTranspose() * MVector4(NormalLocal, 0.0f);
				}


				inline bool GetIsPointBehind(const utility::MVector3 point) const
				{
					MVector3 dir = (point - CenterWorld).Normalized();
					MFloat32 dot = MVector3::Dot(dir, NormalWorld);
					return (dot < 0.0f);
				}
			};

#pragma endregion


		public:

#pragma region Public

			// Top, Bottom, Left, Right, Front, Back
			Plane Planes[6];
			// ForwardTopLeft, ForwardBottomLeft, ForwardBottomRight, ForwardTopRight, RearTopLeft, RearBottomLeft, RearBottomRight, RearTopRight
			utility::MVector3 PointsLocal[8];

#pragma endregion

		protected:

#pragma region Protected

			// ForwardTopLeft, ForwardBottomLeft, ForwardBottomRight, ForwardTopRight, BottomTopLeft, BottomBottomLeft, BottomBottomRight, BottomTopRight
			utility::MVector3 _pointsWorld[8];

			meshes::MeshBox* _mesh = nullptr;

#pragma endregion

#pragma region Functions Protected

			void UpdateMeshVertices();

#pragma endregion

		public:

#pragma region Functions Public

			inline BoundsFrustum()
			{
			}

			inline BoundsFrustum(const BoundsFrustum& c) : Bounds(c)
			{
				for (MSize i = 0; i < 6; ++i)
				{
					Planes[i] = c.Planes[i];
				}
				for (MSize i = 0; i < 8; ++i)
				{
					PointsLocal[i] = c.PointsLocal[i];
					_pointsWorld[i] = c._pointsWorld[i];
				}
			}

			virtual ~BoundsFrustum();

			virtual void Draw(memoryManagement::Handle<gom::Camera> camera) const override;

			inline virtual void UpdateToWorld(memoryManagement::Handle<gom::Transform> transform) override
			{
				for(MSize i = 0; i < 6; ++i)
					Planes[i].UpdateToWorld(transform, ExternalScale);

				for (MSize i = 0; i < 8; ++i)
				{
					_pointsWorld[i] = transform->GetMatWorld() * MVector4(PointsLocal[i], 1.0f);
				}

				if (_mesh != nullptr)
				{
					UpdateMeshVertices();
				}
			}
			inline virtual void UpdateToWorldIdentity() override
			{
				for (MSize i = 0; i < 6; ++i)
				{
					Planes[i].CenterWorld = Planes[i].CenterLocal;
					Planes[i].NormalWorld = Planes[i].NormalLocal;
				}

				for (MSize i = 0; i < 8; ++i)
				{
					_pointsWorld[i] = PointsLocal[i];
				}

				if (_mesh != nullptr)
				{
					UpdateMeshVertices();
				}
			}

			inline virtual bool IntersectsWith(const BoundsFrustum& other) const override
			{
				ME_ASSERT(false, "Not implemented.");
				return false;
			}

			virtual bool IntersectsWith(const BoundsSphere& other) const override;

			virtual bool IntersectsWith(const BoundsBox& other) const override;

			virtual bool IsInside(const BoundsBox& other) const override { return false; }
			virtual bool IsInside(const BoundsSphere& other) const override { return false; }
			virtual bool IsInside(const BoundsFrustum& other) const override { return false; }

			const utility::MVector3* GetPointsWorld() const { return _pointsWorld; }

#pragma endregion


		};
	}
}
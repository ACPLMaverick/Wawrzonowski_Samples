#pragma once
#include "core/GlobalDefines.h"
#include "gom/Transform.h"

namespace morphEngine
{
	namespace gom
	{
		class Camera;
	}
	namespace renderer
	{
		class BoundsBox;
		class BoundsSphere;
		class BoundsFrustum;

		namespace shaders
		{
			class ShaderBounds;
		}

		class Bounds
		{
		protected:

			shaders::ShaderBounds* _shader = nullptr;

			void LoadShader();

		public:

			// This is used for arbitrarily enlarging bounds, when updating to world space.
			// Value of one means no effect.
			MFloat32 ExternalScale = 1.0f;

			inline Bounds()
			{
			}

			inline Bounds(const Bounds& copy) : _shader(copy._shader) { }

			inline virtual ~Bounds()
			{
			}

			void SetShader() const;
			void UnsetShader() const;

			virtual void Initialize() { LoadShader(); }
			virtual void Shutdown() { }

			inline virtual void Draw(memoryManagement::Handle<gom::Camera> camera) const = 0;
			inline virtual void UpdateToWorld(memoryManagement::Handle<gom::Transform> transform) = 0;
			inline virtual void UpdateToWorldIdentity() = 0;

			virtual bool IntersectsWith(const BoundsBox& other) const = 0;
			virtual bool IntersectsWith(const BoundsSphere& other) const = 0;
			virtual bool IntersectsWith(const BoundsFrustum& other) const = 0;

			virtual bool IsInside(const BoundsBox& other) const = 0;
			virtual bool IsInside(const BoundsSphere& other) const = 0;
			virtual bool IsInside(const BoundsFrustum& other) const = 0;
		};

		class BoundsInvalid : public Bounds
		{
			inline virtual void Draw(memoryManagement::Handle<gom::Camera> camera) const { }
			inline virtual void UpdateToWorld(memoryManagement::Handle<gom::Transform> transform) { }
			inline virtual void UpdateToWorldIdentity() { }

			virtual bool IntersectsWith(const BoundsBox& other) const override { return false; }
			virtual bool IntersectsWith(const BoundsSphere& other) const override { return false; }
			virtual bool IntersectsWith(const BoundsFrustum& other) const override { return false; }

			virtual bool IsInside(const BoundsBox& other) const override { return false; }
			virtual bool IsInside(const BoundsSphere& other) const override { return false; }
			virtual bool IsInside(const BoundsFrustum& other) const override { return false; }
		};
	}
}
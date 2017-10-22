#pragma once

#include "assetLibrary/MMesh.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace meshes
		{
			class MeshPlane :
				public assetLibrary::MMesh
			{
			protected:

				inline void InitBuffers();

			public:
				MeshPlane();
				~MeshPlane();

				// initializes this mesh to a simple quad plane.
				virtual void Initialize() override;

				utility::MVector2 GetScale() const;
				utility::MVector2 GetTiling() const;

				void SetScaleAndTiling(const utility::MVector2& scale, const utility::MVector2& tiling);
			};
		}
	}
}
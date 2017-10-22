#pragma once

#include "assetLibrary/MMesh.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace meshes
		{
			class MeshBox :
				public assetLibrary::MMesh
			{
			protected:

				inline void InitSmooth();
				inline void InitNonSmooth();

			public:
				MeshBox();
				virtual ~MeshBox();

				virtual void Initialize() override;

				void SetSmooth(bool bSmooth);
				void FlipTrianglesAndNormals();
			};

		}
	}
}
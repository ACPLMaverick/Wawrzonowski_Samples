#pragma once

#include "assetLibrary/MMesh.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace meshes
		{
			class MeshTerrain :
				public assetLibrary::MMesh
			{
			public:
				MeshTerrain();
				~MeshTerrain();
			};

		}
	}
}
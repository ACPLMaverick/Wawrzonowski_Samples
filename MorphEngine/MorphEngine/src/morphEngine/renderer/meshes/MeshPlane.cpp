#include "MeshPlane.h"


namespace morphEngine
{
	using namespace utility;

	namespace renderer
	{
		namespace meshes
		{
			MeshPlane::MeshPlane()
			{
				_header.Type = assetLibrary::MeshType::STATIC;
				_bIsInternal = true;
			}


			MeshPlane::~MeshPlane()
			{
			}

			void MeshPlane::Initialize()
			{
				// setup all buffers and indices

				InitBuffers();

				// initialize as usual
				MMesh::Initialize();
			}

			utility::MVector2 MeshPlane::GetScale() const
			{
				return utility::MVector2(_vertices[2].X, _vertices[2].Z) * 2.0f;
			}

			utility::MVector2 MeshPlane::GetTiling() const
			{
				return utility::MVector2(_uvs[2].X, _uvs[2].Y);
			}

			void MeshPlane::SetScaleAndTiling(const utility::MVector2& scale, const utility::MVector2& tiling)
			{
				InitBuffers();
				for (auto it = _vertices.GetIterator(); it.IsValid(); ++it)
				{
					(*it).X *= scale.X;
					(*it).Y *= scale.Y;
				}

				for (auto it = _uvs.GetIterator(); it.IsValid(); ++it)
				{
					(*it).X *= tiling.X;
					(*it).Y *= tiling.Y;
				}
				_resVertices->Update(_vertices.GetDataPointer(), _vertices.GetSize() * sizeof(utility::MVector3));
				_resUvs->Update(_uvs.GetDataPointer(), _uvs.GetSize() * sizeof(utility::MVector2));
			}

			inline void MeshPlane::InitBuffers()
			{
				_vertices.Clear();
				_normals.Clear();
				_uvs.Clear();
				_indices.Clear();
				_submeshes.Clear();

				_header.VerticesCount = 4;
				_header.IndicesCount = 6;

				_vertices.Add(MVector3(-0.5f, -0.5f, 0.0f));
				_vertices.Add(MVector3(0.5f, -0.5f, 0.0f));
				_vertices.Add(MVector3(0.5f, 0.5f, 0.0f));
				_vertices.Add(MVector3(-0.5f, 0.5f, 0.0f));

				_normals.Add(MVector3(0.0f, 0.0f, -1.0f));
				_normals.Add(MVector3(0.0f, 0.0f, -1.0f));
				_normals.Add(MVector3(0.0f, 0.0f, -1.0f));
				_normals.Add(MVector3(0.0f, 0.0f, -1.0f));

				_uvs.Add(MVector2(0.0f, 0.0f));
				_uvs.Add(MVector2(1.0f, 0.0f));
				_uvs.Add(MVector2(1.0f, 1.0f));
				_uvs.Add(MVector2(0.0f, 1.0f));

				_indices.Add(0);
				_indices.Add(1);
				_indices.Add(3);
				_indices.Add(1);
				_indices.Add(2);
				_indices.Add(3);

				_submeshes.Add(0);
			}
		}
	}
}
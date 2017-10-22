#include "MeshBox.h"


namespace morphEngine
{
	using namespace utility;

	namespace renderer
	{
		namespace meshes
		{
			MeshBox::MeshBox()
			{
				_header.Type = assetLibrary::MeshType::STATIC;
				_bIsInternal = true;
			}

			MeshBox::~MeshBox()
			{
			}

			void MeshBox::Initialize()
			{
				InitSmooth();

				MMesh::Initialize();
			}

			void MeshBox::SetSmooth(bool bSmooth)
			{
				// check if same value
				if ((bSmooth && (_header.VerticesCount == 8)) || (!bSmooth && (_header.VerticesCount == 24)))
					return;

				// check if initialized
				if (_resVertices == nullptr)
					return;

				if (bSmooth)
				{
					InitSmooth();
				}
				else
				{
					InitNonSmooth();
				}

				_resVertices->Shutdown();
				_resNormals->Shutdown();
				_resUvs->Shutdown();
				_resIndices->Shutdown();

				_resVertices->Initialize(reinterpret_cast<MFloat32*>(_vertices.GetDataPointer()), sizeof(MVector3), _header.VerticesCount, _resVertices->_accessMode);
				_resNormals->Initialize(reinterpret_cast<MFloat32*>(_normals.GetDataPointer()), sizeof(MVector3), _header.VerticesCount, _resNormals->_accessMode);
				_resUvs->Initialize(reinterpret_cast<MFloat32*>(_uvs.GetDataPointer()), sizeof(MVector2), _header.VerticesCount, _resUvs->_accessMode);
				_resIndices->Initialize(_indices.GetDataPointer(), _header.IndicesCount, _resIndices->_accessMode);
			}

			void MeshBox::FlipTrianglesAndNormals()
			{
				for (MSize i = 0; i < _indices.GetSize(); i += 3)
				{
					MUint16 tmp = _indices[i];
					_indices[i] = _indices[i + 2];
					_indices[i + 2] = tmp;
				}

				for (MSize i = 0; i < _normals.GetSize(); i += 3)
				{
					_normals[i] = -_normals[i];
				}

				UpdateNormalsResource();
				UpdateIndicesResource();
			}

			inline void MeshBox::InitSmooth()
			{
				_vertices.Clear();
				_normals.Clear();
				_uvs.Clear();
				_indices.Clear();
				_submeshes.Clear();

				_header.VerticesCount = 8;
				_header.IndicesCount = 36;
				_header.SubmeshesCount = 1;

				MFloat32 s = 0.5f;

				// back
				_vertices.Add(MVector3(-s, -s, -s));
				_vertices.Add(MVector3(s, -s, -s));
				_vertices.Add(MVector3(s, s, -s));
				_vertices.Add(MVector3(-s, s, -s));
				_vertices.Add(MVector3(-s, -s, s));
				_vertices.Add(MVector3(s, -s, s));
				_vertices.Add(MVector3(s, s, s));
				_vertices.Add(MVector3(-s, s, s));
				_uvs.Add(MVector2(0.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));

				for (MSize i = 0; i < _header.VerticesCount; ++i)
				{
					_normals.Add(_vertices[i].Normalized());
				}

				// back
				_indices.Add(3);
				_indices.Add(0);
				_indices.Add(1);
				_indices.Add(3);
				_indices.Add(1);
				_indices.Add(2);

				// front

				_indices.Add(4);
				_indices.Add(7);
				_indices.Add(5);
				_indices.Add(5);
				_indices.Add(7);
				_indices.Add(6);

				// left
				_indices.Add(3);
				_indices.Add(4);
				_indices.Add(0);
				_indices.Add(3);
				_indices.Add(7);
				_indices.Add(4);

				// right
				_indices.Add(2);
				_indices.Add(1);
				_indices.Add(5);
				_indices.Add(2);
				_indices.Add(5);
				_indices.Add(6);

				// bottom
				_indices.Add(4);
				_indices.Add(1);
				_indices.Add(0);
				_indices.Add(4);
				_indices.Add(5);
				_indices.Add(1);

				// top
				_indices.Add(7);
				_indices.Add(3);
				_indices.Add(2);
				_indices.Add(7);
				_indices.Add(2);
				_indices.Add(6);

				_submeshes.Add(0);
			}

			inline void MeshBox::InitNonSmooth()
			{
				_vertices.Clear();
				_normals.Clear();
				_uvs.Clear();
				_indices.Clear();
				_submeshes.Clear();

				_header.VerticesCount = 24;
				_header.IndicesCount = 36;
				_header.SubmeshesCount = 1;

				MFloat32 s = 0.5f;
				MUint16 io = 0;

				// back
				_vertices.Add(MVector3(-s, -s, -s));
				_vertices.Add(MVector3(s, -s, -s));
				_vertices.Add(MVector3(s, s, -s));
				_vertices.Add(MVector3(-s, s, -s));
				_normals.Add(-MVector3::Forward);
				_normals.Add(-MVector3::Forward);
				_normals.Add(-MVector3::Forward);
				_normals.Add(-MVector3::Forward);
				_uvs.Add(MVector2(0.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));

				// front
				_vertices.Add(MVector3(-s, -s, s));
				_vertices.Add(MVector3(-s, s, s));
				_vertices.Add(MVector3(s, s, s));
				_vertices.Add(MVector3(s, -s, s));
				_normals.Add(MVector3::Forward);
				_normals.Add(MVector3::Forward);
				_normals.Add(MVector3::Forward);
				_normals.Add(MVector3::Forward);
				_uvs.Add(MVector2(0.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));

				// left
				_vertices.Add(MVector3(-s, -s, -s));
				_vertices.Add(MVector3(-s, s, -s));
				_vertices.Add(MVector3(-s, s, s));
				_vertices.Add(MVector3(-s, -s, s));
				_normals.Add(-MVector3::Right);
				_normals.Add(-MVector3::Right);
				_normals.Add(-MVector3::Right);
				_normals.Add(-MVector3::Right);
				_uvs.Add(MVector2(0.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));

				// right
				_vertices.Add(MVector3(s, -s, -s));
				_vertices.Add(MVector3(s, -s, s));
				_vertices.Add(MVector3(s, s, s));
				_vertices.Add(MVector3(s, s, -s));
				_normals.Add(MVector3::Right);
				_normals.Add(MVector3::Right);
				_normals.Add(MVector3::Right);
				_normals.Add(MVector3::Right);
				_uvs.Add(MVector2(0.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));

				// bottom
				_vertices.Add(MVector3(s, -s, -s));
				_vertices.Add(MVector3(-s, -s, -s));
				_vertices.Add(MVector3(-s, -s, s));
				_vertices.Add(MVector3(s, -s, s));
				_normals.Add(-MVector3::Up);
				_normals.Add(-MVector3::Up);
				_normals.Add(-MVector3::Up);
				_normals.Add(-MVector3::Up);
				_uvs.Add(MVector2(0.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));

				// top
				_vertices.Add(MVector3(-s, s, -s));
				_vertices.Add(MVector3(s, s, -s));
				_vertices.Add(MVector3(s, s, s));
				_vertices.Add(MVector3(-s, s, s));
				_normals.Add(MVector3::Up);
				_normals.Add(MVector3::Up);
				_normals.Add(MVector3::Up);
				_normals.Add(MVector3::Up);
				_uvs.Add(MVector2(0.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 1.0f));
				_uvs.Add(MVector2(1.0f, 0.0f));
				_uvs.Add(MVector2(0.0f, 0.0f));

				for (; io < _header.IndicesCount; io += 4)
				{
					_indices.Add(io + 0);
					_indices.Add(io + 1);
					_indices.Add(io + 3);
					_indices.Add(io + 1);
					_indices.Add(io + 2);
					_indices.Add(io + 3);
				}

				_submeshes.Add(0);
			}
		}
	}
}
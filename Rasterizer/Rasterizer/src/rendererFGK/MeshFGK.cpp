#include "MeshFGK.h"

namespace rendererFGK
{
	MeshFGK::MeshFGK() :
		Mesh()
	{
	}

	MeshFGK::MeshFGK(const math::Float3 * pos, const math::Float3 * rot, const math::Float3 * scl, const std::string * fPath) :
		Mesh(pos, rot, scl, fPath),
		_bbMax(FLT_MIN, FLT_MIN, FLT_MIN),
		_bbMin(FLT_MAX, FLT_MAX, FLT_MAX),
		_bbMinMul(_bbMin),
		_bbMaxMul(_bbMax)
	{
#ifdef RENDERER_FGK_BOUNDINGBOX

		// calculate bounding box.
		for (std::vector<math::Float3>::iterator it = _positionArray.begin(); it != _positionArray.end(); ++it)
		{
			if ((*it).x < _bbMin.x)
			{
				_bbMin.x = (*it).x;
			}
			if ((*it).y < _bbMin.y)
			{
				_bbMin.y = (*it).y;
			}
			if ((*it).z < _bbMin.z)
			{
				_bbMin.z = (*it).z;
			}
			if ((*it).x > _bbMax.x)
			{
				_bbMax.x = (*it).x;
			}
			if ((*it).y > _bbMax.y)
			{
				_bbMax.y = (*it).y;
			}
			if ((*it).z > _bbMax.z)
			{
				_bbMax.z = (*it).z;
			}
		}
#endif
		// generate triangles
		for (std::vector<math::UShort3>::iterator it = _indexArray.begin(); it != _indexArray.end(); (it += 3))
		{
			// get data by indices
			math::Float3 positions[3] =
			{
				_positionArray[(*it).p],
				_positionArray[(*(it + 1)).p],
				_positionArray[(*(it + 2)).p]
			};

			math::Float3 normals[3] =
			{
				_normalArray[(*it).n],
				_normalArray[(*(it + 1)).n],
				_normalArray[(*(it + 2)).n]
			};

			// create triangle and check its intersection
			TriangleFGK triangle
			(
				positions[0], positions[1], positions[2],
				_uvArray[(*it).t], _uvArray[(*(it + 1)).t], _uvArray[(*(it + 2)).t],
				normals[0], normals[1], normals[2]
			);

			_triangles.push_back(triangle);
		}
	}


	MeshFGK::~MeshFGK()
	{
	}

	RayHit MeshFGK::CalcIntersect(Ray & ray)
	{
		RayHit rayHit(false, math::Float3(FLT_MAX, FLT_MAX, FLT_MAX));

#ifdef RENDERER_FGK_BOUNDINGBOX
		// bounding box check
		math::Float3 dirFrac
		(
			1.0f / ray.GetDirection().x,
			1.0f / ray.GetDirection().y,
			1.0f / ray.GetDirection().z
		);

		math::Float3 tMinVec
		(
			(_bbMinMul.x - ray.GetOrigin().x) * dirFrac.x,
			(_bbMinMul.y - ray.GetOrigin().y) * dirFrac.y,
			(_bbMinMul.z - ray.GetOrigin().z) * dirFrac.z
		);
		math::Float3 tMaxVec
		(
			(_bbMaxMul.x - ray.GetOrigin().x) * dirFrac.x,
			(_bbMaxMul.y - ray.GetOrigin().y) * dirFrac.y,
			(_bbMaxMul.z - ray.GetOrigin().z) * dirFrac.z
		);

		float tMin = max(max(min(tMinVec.x, tMaxVec.x), min(tMinVec.y, tMaxVec.y)), min(tMinVec.z, tMaxVec.z));
		float tMax = min(min(max(tMinVec.x, tMaxVec.x), max(tMinVec.y, tMaxVec.y)), max(tMinVec.z, tMaxVec.z));

		if (tMax >= 0.0f && (tMin <= tMax))
		{
#endif
			// per-triangle check
			for (std::vector<TriangleFGK>::iterator it = _triangles.begin(); it != _triangles.end(); ++it)
			{
				RayHit hit = (*it).CalcIntersect(ray);
				if (hit.hit)
				{
					math::Float3 diffRet = ray.GetOrigin() - rayHit.point;
					math::Float3 diffM = ray.GetOrigin() - hit.point;
					if (math::Float3::LengthSquared(diffM) < math::Float3::LengthSquared(diffRet))
					{
						rayHit = hit;
					}
				}
			}
#ifdef RENDERER_FGK_BOUNDINGBOX
		}
		else
		{
			return RayHit();
		}
#endif
		return rayHit;
	}

	void MeshFGK::Update()
	{
		math::Float3 bbVerts[8];

		// from TL clockwise, first lower, then upper

		bbVerts[0] = math::Float3(_bbMin.x, _bbMin.y, _bbMax.z);
		bbVerts[1] = math::Float3(_bbMax.x, _bbMin.y, _bbMax.z);
		bbVerts[2] = math::Float3(_bbMax.x, _bbMin.y, _bbMin.z);
		bbVerts[3] = _bbMin;
		bbVerts[4] = math::Float3(_bbMin.x, _bbMax.y, _bbMax.z);
		bbVerts[5] = _bbMax;
		bbVerts[6] = math::Float3(_bbMax.x, _bbMax.y, _bbMin.z);
		bbVerts[7] = math::Float3(_bbMin.x, _bbMax.y, _bbMin.z);

		math::Matrix4x4 wm = *_transform.GetWorldMatrix();
		for (size_t i = 0; i < 8; ++i)
		{
			math::Float4 janusz(bbVerts[i]);
			bbVerts[i] = (wm * janusz);
		}

		_bbMaxMul = math::Float3(FLT_MIN, FLT_MIN, FLT_MIN);
		_bbMinMul = math::Float3(FLT_MAX, FLT_MAX, FLT_MAX);

		for (size_t i = 0; i < 8; ++i)
		{
			if (bbVerts[i].x < _bbMinMul.x)
			{
				_bbMinMul.x = bbVerts[i].x;
			}
			if (bbVerts[i].y < _bbMinMul.y)
			{
				_bbMinMul.y = bbVerts[i].y;
			}
			if (bbVerts[i].z < _bbMinMul.z)
			{
				_bbMinMul.z = bbVerts[i].z;
			}
			if (bbVerts[i].x > _bbMaxMul.x)
			{
				_bbMaxMul.x = bbVerts[i].x;
			}
			if (bbVerts[i].y > _bbMaxMul.y)
			{
				_bbMaxMul.y = bbVerts[i].y;
			}
			if (bbVerts[i].z > _bbMaxMul.z)
			{
				_bbMaxMul.z = bbVerts[i].z;
			}
		}

		/*
		// code from
		// http://www.gamedev.net/topic/349370-transform-aabb-from-local-to-world-space-for-frustum-culling/
		//

		_bbMinMul = _bbMin;
		_bbMaxMul = _bbMax;
		float av, bv;
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				av = _transform.GetWorldMatrix()->tabf[i].tab[j] * _bbMin[j];
				bv = _transform.GetWorldMatrix()->tabf[i].tab[j] * _bbMax[j];

				if (av < bv)
				{
					_bbMinMul.tab[i] += av;
					_bbMaxMul.tab[i] += bv;
				}
				else
				{
					_bbMinMul.tab[i] += bv;
					_bbMaxMul.tab[i] += av;
				}
			}
		}
		*/
		std::vector<math::UShort3>::iterator itIndex = _indexArray.begin();
		for (std::vector<TriangleFGK>::iterator it = _triangles.begin(); it != _triangles.end(); ++it, itIndex += 3)
		{
			(*it).SetPosition
			(
				*_transform.GetWorldMatrix() * math::Float4(_positionArray[(*(itIndex)).p].x, _positionArray[(*(itIndex)).p].y, _positionArray[(*(itIndex)).p].z, 1.0f),
				*_transform.GetWorldMatrix() * math::Float4(_positionArray[(*(itIndex + 1)).p].x, _positionArray[(*(itIndex + 1)).p].y, _positionArray[(*(itIndex + 1)).p].z, 1.0f),
				*_transform.GetWorldMatrix() * math::Float4(_positionArray[(*(itIndex + 2)).p].x, _positionArray[(*(itIndex + 2)).p].y, _positionArray[(*(itIndex + 2)).p].z, 1.0f)
			);
			(*it).SetNormals
			(
				*_transform.GetWorldInverseTransposeMatrix() * math::Float4(_normalArray[(*(itIndex)).n].x, _normalArray[(*(itIndex)).n].y, _normalArray[(*(itIndex)).n].z, 0.0f),
				*_transform.GetWorldInverseTransposeMatrix() * math::Float4(_normalArray[(*(itIndex + 1)).n].x, _normalArray[(*(itIndex + 1)).n].y, _normalArray[(*(itIndex + 1)).n].z, 0.0f),
				*_transform.GetWorldInverseTransposeMatrix() * math::Float4(_normalArray[(*(itIndex + 2)).n].x, _normalArray[(*(itIndex + 2)).n].y, _normalArray[(*(itIndex + 2)).n].z, 0.0f)
			);
		}
	}

	void MeshFGK::Draw()
	{
	}
}
#pragma once

#include "Primitive.h"
#include "Float3.h"
#include "Float2.h"
#include "UShort3.h"

#include <vector>

#define MESH_PATH "./meshes/"
#define MESH_EXTENSION ".obj"

class Mesh :
	public Primitive
{
protected:

#pragma region Protected

	std::vector<math::Float3> _positionArray;
	std::vector<math::Float3> _colorArray;
	std::vector<math::Float2> _uvArray;
	std::vector<math::Float3> _normalArray;
	std::vector<math::UShort3> _indexArray;

	uint16_t _vertexCount = 0;
	uint16_t _uvCount = 0;
	uint16_t _normalCount = 0;
	uint16_t _triangleCount = 0;

#pragma endregion

#pragma region Fumctions Protected

	inline void LoadFromFile(const std::string* fileName);
	inline void SplitString(const std::string* str, const std::string delim, std::vector<std::string>* vec);

#pragma endregion

public:

#pragma region Functions Public

	Mesh();
	Mesh
	(
		const math::Float3* pos,
		const math::Float3* rot,
		const math::Float3* scl,
		const std::string* fPath
	);
	~Mesh();

	virtual void Update() = 0;
	virtual void Draw() = 0;

#pragma endregion
};


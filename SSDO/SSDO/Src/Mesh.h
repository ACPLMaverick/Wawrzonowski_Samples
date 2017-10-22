#pragma once

#include "stdafx.h"
#include "Buffer.h"

#include <vector>

class Camera;
class Material;
class Font;

class Mesh
{
	friend class Material;
protected:

#pragma region Structs Protected

	struct Triangle
	{
		union
		{
			struct
			{
				int16_t _x;
				int16_t _y;
				int16_t _z;
			};
			int16_t tab[3];
		};

		Triangle()
		{
			ZeroMemory(&tab, 3 * sizeof(int16_t));
		}

		Triangle(int16_t x, int16_t y, int16_t z) :
			_x(x),
			_y(y),
			_z(z)
		{
		}

		int16_t operator[](int64_t index) const
		{
			ASSERT(index >= 0 && index < 3);
			return tab[index];
		}

		int16_t& operator[](int64_t index)
		{
			ASSERT(index >= 0 && index < 3);
			return tab[index];
		}
	};

	struct LineData
	{
		int32_t StartIndex;
		float Width;

		LineData() : StartIndex(0), Width(0) { }
		LineData(int32_t startIndex, float w) : StartIndex(startIndex), Width(w) { }

		bool operator==(const LineData& o) const { return StartIndex == o.StartIndex && Width == o.Width; }
		bool operator!=(const LineData& o) const { return !operator==(o); }
	};

#pragma endregion

#pragma region Protected

	Buffer<XMFLOAT3A> _vPositions;
	Buffer<XMFLOAT3A> _vNormals;
	Buffer<XMFLOAT2A> _vUvs;
	Buffer<Triangle> _indices;

	ID3D11Buffer* _fPositions = nullptr;
	ID3D11Buffer* _fNormals = nullptr;
	ID3D11Buffer* _fUvs = nullptr;
	ID3D11Buffer* _fIndices = nullptr;

	bool _bufferReinitFlag = false;
	bool _bReadOnly = true;
	uint16_t _lastTextSize = 0;

#pragma endregion

#pragma region Functions Protected

	inline bool Float3Equal(const XMFLOAT3A& lhs, const XMFLOAT3A& rhs) const;
	inline bool Float2Equal(const XMFLOAT2A& lhs, const XMFLOAT2A& rhs) const;


#pragma endregion

public:

#pragma region Public Struct

	struct VertexDescSimple
	{
		XMFLOAT3A Position;
		XMFLOAT3A Normal;
		XMFLOAT2A Uv;
	};

#pragma endregion

#pragma region Functions Public

	// Creates dummy mesh without initializing buffers
	Mesh(bool bReadOnly);
	// Loads mesh from file
	Mesh(const std::wstring& filePath, bool bReadOnly);
	~Mesh();

	void DrawBuffers() const;

	inline void SetDataSize(size_t size, size_t indexSize) 
	{ 
		if (size != _vPositions.GetSize())
		{
			_vPositions.Resize(size); _vNormals.Resize(size); _vUvs.Resize(size);
			_bufferReinitFlag = true;
		}

		if (indexSize != _indices.GetSize())
		{
			_indices.Resize(indexSize);
			_bufferReinitFlag = true;
		}
	}
	inline size_t GetDataSize() { return _vPositions.GetSize(); }
	inline size_t GetDataIndexSize() { return _indices.GetSize(); }
	inline XMFLOAT3A* GetPositionsData() { return _vPositions.GetDataPtr(); }
	inline XMFLOAT3A* GetNormalsData() { return _vNormals.GetDataPtr(); }
	inline XMFLOAT2A* GetUvsData() { return _vUvs.GetDataPtr(); }
	inline Triangle* GetIndexData() { return _indices.GetDataPtr(); }
	inline bool GetReadOnly() { return _bReadOnly; }

	void ReinitBuffers();

	// Creates new mesh suitable for rendering a given text with a given font. Avoids reinitializing buffers if possible.
	void UpdateDataFromText(const std::string& text, const Font& font);

	static Mesh* CreateResource(const std::wstring& name) { return new Mesh(name, true); }

#pragma endregion
};


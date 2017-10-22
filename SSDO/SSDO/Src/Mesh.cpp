#include "stdafx.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Font.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include <tiny_obj_loader/tiny_obj_loader.h>


Mesh::Mesh(bool bReadOnly) : 
	_bReadOnly(bReadOnly)
{
	/*
	_vPositions.Resize(8);
	_vNormals.Resize(8);
	_vUvs.Resize(8);
	_indices.Resize(12);

	float s = 0.5f;
	_vPositions[0] = XMFLOAT3A(-s, -s, -s);
	_vPositions[1] = XMFLOAT3A(s, -s, -s);
	_vPositions[2] = XMFLOAT3A(-s, s, -s);
	_vPositions[3] = XMFLOAT3A(s, s, -s);
	_vPositions[4] = XMFLOAT3A(-s, -s, s);
	_vPositions[5] = XMFLOAT3A(s, -s, s);
	_vPositions[6] = XMFLOAT3A(-s, s, s);
	_vPositions[7] = XMFLOAT3A(s, s, s);

	for (int i = 0; i < 8; ++i)
	{
		_vUvs[i] = XMFLOAT2A(0.0f, 0.0f);
	}

	_indices[0] = Triangle(0, 1, 2);
	_indices[1] = Triangle(1, 3, 2);
	_indices[2] = Triangle(4, 5, 6);
	_indices[3] = Triangle(5, 7, 6);
	_indices[4] = Triangle(6, 2, 3);
	_indices[5] = Triangle(6, 3, 7);
	_indices[6] = Triangle(4, 0, 1);
	_indices[7] = Triangle(4, 1, 5);
	_indices[8] = Triangle(3, 1, 5);
	_indices[9] = Triangle(3, 5, 7);
	_indices[10] = Triangle(2, 0, 4);
	_indices[11] = Triangle(2, 4, 6);

	for (int i = 0; i < 12; ++i)
	{
		Triangle tr = _indices[i];
		for (int j = 0; j < 3; ++j)
		{
			XMFLOAT3A me = _vPositions[tr[j]];
			XMFLOAT3A neighbour1 = _vPositions[tr[(j + 1) % 3]];
			XMFLOAT3A neighbour2 = _vPositions[tr[(j + 2) % 3]];

			XMVECTOR nrm = XMLoadFloat3(&_vNormals[tr[j]]);
			XMVECTOR n1 = XMVector3Normalize(XMLoadFloat3(&neighbour1) - XMLoadFloat3(&me));
			XMVECTOR n2 = XMVector3Normalize(XMLoadFloat3(&neighbour2) - XMLoadFloat3(&me));
			XMVECTOR cr = XMVector3Normalize(XMVector3Cross(n1, n2));
			cr = cr + nrm;
			XMStoreFloat3(&_vNormals[tr[j]], cr);
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		XMVECTOR nrm = XMLoadFloat3(&_vNormals[i]);
		nrm = XMVector3Normalize(nrm);
		XMStoreFloat3(&_vNormals[i], nrm);
	}
	ReinitBuffers();
	*/
}

Mesh::Mesh(const std::wstring & filePath, bool bReadOnly)
{
	_bReadOnly = bReadOnly;

	std::string path = std::string(filePath.begin(), filePath.end());
	path = "./Resources/Meshes/" + path + ".obj";

	Buffer<uint16_t> indices;
	indices.Allocate(16);
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, nullptr, nullptr, path.c_str());
	ASSERT(ret);

	size_t vCount = attrib.vertices.size() / 3;
	_vPositions.Resize(vCount);
	_vNormals.Resize(vCount);
	_vUvs.Resize(vCount);

	for (auto it = shapes.begin(); it != shapes.end(); ++it)
	{
		for (auto jt = (*it).mesh.indices.begin(); jt != (*it).mesh.indices.end(); ++jt)
		{
			uint16_t index = (*jt).vertex_index;
			XMFLOAT3A normal = XMFLOAT3A(attrib.normals[(*jt).normal_index * 3], attrib.normals[(*jt).normal_index * 3 + 1], attrib.normals[(*jt).normal_index * 3 + 2]);
			XMFLOAT2A uv = XMFLOAT2A(attrib.texcoords[(*jt).texcoord_index * 2], attrib.texcoords[(*jt).texcoord_index * 2 + 1]);
			XMFLOAT3A position = XMFLOAT3A(attrib.vertices[(*jt).vertex_index * 3], attrib.vertices[(*jt).vertex_index * 3 + 1], attrib.vertices[(*jt).vertex_index * 3 + 2]);

			XMFLOAT3A cPosition = _vPositions[index];
			XMFLOAT3A cNormal = _vNormals[index];
			XMFLOAT2A cUv = _vUvs[index];

			// check if normal or uv are already assigned to this vertex
			if (Float3Equal(cPosition, position) &&
				Float3Equal(cNormal, normal) &&
				Float2Equal(cUv, uv))
			{
				// already has assigned normal and uv and does not need to be altered.
				indices.Add(index);
			}
			else
			{
				if (Float3Equal(cPosition, position))	// if already written to this position
				{
					// already has assigned normal and uv but they're different, so we need to create a new vertex with a new id
					uint16_t newIndex = static_cast<uint16_t>(_vPositions.GetSize());
					_vPositions.Add(position);
					_vNormals.Add(normal);
					_vUvs.Add(uv);
					indices.Add(newIndex);
				}
				else
				{
					// has not assigned any normal or uv yet
					indices.Add(index);
					_vPositions[index] = position;
					_vNormals[index] = normal;
					_vUvs[index] = uv;
				}
			}
		}
	}

	_indices.Allocate(indices.GetSize() / 3);
	for (auto it = indices.GetIterator(); it.IsValid(); it += 3)
	{
		_indices.Add(Triangle(*(it + 2), *(it + 1), *(it)));
	}

	for (auto it = _vNormals.GetIterator(); it.IsValid(); ++it)
	{
		XMVECTOR nv = XMLoadFloat3(&(*it));
		nv = XMVector3Normalize(nv);
		XMStoreFloat3(&(*it), nv);
	}

	ReinitBuffers();
}

Mesh::~Mesh()
{
	_fPositions->Release();
	_fPositions = nullptr;

	_fNormals->Release();
	_fNormals = nullptr;

	_fUvs->Release();
	_fUvs = nullptr;

	_fIndices->Release();
	_fIndices = nullptr;
}

void Mesh::DrawBuffers() const
{
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();

	uint32_t stridePos = sizeof(_vPositions[0]);
	uint32_t strideUvs = sizeof(_vUvs[0]);
	uint32_t strideNrms = sizeof(_vNormals[0]);
	uint32_t offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &_fPositions, &stridePos, &offset);	// this can be simplified
	deviceContext->IASetVertexBuffers(1, 1, &_fUvs, &strideUvs, &offset);
	deviceContext->IASetVertexBuffers(2, 1, &_fNormals, &strideNrms, &offset);
	deviceContext->IASetIndexBuffer(_fIndices, DXGI_FORMAT_R16_UINT, 0);

	deviceContext->DrawIndexed(static_cast<uint32_t>(_indices.GetSize() * 3), 0, 0);
}

void Mesh::UpdateDataFromText(const std::string & text, const Font& font)
{/*
	_vPositions.Resize(4);
	_vNormals.Resize(4);
	_vUvs.Resize(4);
	_indices.Resize(2);

	_vPositions[0] = XMFLOAT3A(-0.5f, -0.5f, 0.0f);
	_vPositions[1] = XMFLOAT3A(-0.5f, 0.5f, 0.0f);
	_vPositions[2] = XMFLOAT3A(0.5f, 0.5f, 0.0f);
	_vPositions[3] = XMFLOAT3A(0.5f, -0.5f, 0.0f);

	ZEROM(_vNormals.GetDataPtr(), _vNormals.GetSizeBytes());

	_vUvs[0] = XMFLOAT2A(0.0f, 0.0f);
	_vUvs[1] = XMFLOAT2A(0.0f, 1.0f);
	_vUvs[2] = XMFLOAT2A(1.0f, 1.0f);
	_vUvs[3] = XMFLOAT2A(1.0f, 0.0f);

	_indices[0] = Triangle(3, 1, 0);
	_indices[1] = Triangle(3, 2, 1);

	ReinitBuffers();
*/
	
	uint16_t textSize = static_cast<uint16_t>(text.length());
	bool bSameLength = textSize == _lastTextSize;
	_lastTextSize = textSize;

	int32_t totalVertices = static_cast<int32_t>(text.length()) * 4;
	int32_t totalIndices = static_cast<int32_t>(text.length()) * 6;

	if (!bSameLength)
	{
		_vPositions.Resize(totalVertices);
		_vUvs.Resize(totalVertices);
		_indices.Resize(totalIndices / 3);
	}

	float totalWidth = 0.0f;
	float totalHeight = 0.0f;
	float newLineWidth = 0.0f;
	float newLineHeight = 0.0f;
	const float baseCharSize = static_cast<float>(font.GetCharacterSizePixels());
	const float fontTexturePitch = static_cast<float>(font.GetWidth());
	const float fontTexturePitchRec = 1.0f / fontTexturePitch;
	const float sizeMultiplier = 4.0f;

	std::vector<LineData> newLines(4);
	int32_t currentLineIndex = 0;

	// calculate face positions and uvs from font and its alignment data

	for (int32_t i = 0, iv = 0, ii = 0; i < text.length(); ++i, iv += 4, ii += 2)
	{
		char c = text[i];

		if (c == '\n' || i == text.length() - 1)	// to always add the last line, even if there's no tailing \n
		{
			// add current (not new) line data to newLines
			newLines.push_back(LineData(currentLineIndex, newLineWidth));
			currentLineIndex = (i + 1) * 4;	// first index of next line, multiplied by 4 because 4 vertices per letter

			if (c == '\n')
			{
				newLineHeight -= (baseCharSize * fontTexturePitchRec);
				newLineWidth = 0.0f;
				continue;
			}
		}

		Font::CharAlignment alignment = font.GetAlignment(c);
		//MFont::CharAlignment alignment(0.0f, 0.0f, 0.05f, 0.05f);

		XMFLOAT3A positionBase(newLineWidth, newLineHeight + alignment.BaselineCorrection, 0.0f);
		XMVECTOR positionBaseV(XMLoadFloat3A(&positionBase));
		XMStoreFloat3(&_vPositions[iv], (positionBaseV * sizeMultiplier));
		XMStoreFloat3(&_vPositions[iv + 1], ((positionBaseV + XMLoadFloat3A(&XMFLOAT3A(alignment.Width, 0.0f, 0.0f))) * sizeMultiplier));
		XMStoreFloat3(&_vPositions[iv + 2], ((positionBaseV + XMLoadFloat3A(&XMFLOAT3A(alignment.Width, alignment.Height, 0.0f))) * sizeMultiplier));
		XMStoreFloat3(&_vPositions[iv + 3], ((positionBaseV + XMLoadFloat3A(&XMFLOAT3A(0.0f, alignment.Height, 0.0f))) * sizeMultiplier));

		_vUvs[iv] = (XMFLOAT2A(alignment.TopX, 1.0f - (alignment.LeftY + alignment.Height)));
		_vUvs[iv + 1] = (XMFLOAT2A(alignment.TopX + alignment.Width, 1.0f - (alignment.LeftY + alignment.Height)));
		_vUvs[iv + 2] = (XMFLOAT2A(alignment.TopX + alignment.Width, 1.0f - alignment.LeftY));
		_vUvs[iv + 3] = (XMFLOAT2A(alignment.TopX, 1.0f - alignment.LeftY));

		_indices[ii][0] = (static_cast<uint16_t>(iv));
		_indices[ii][1] = (static_cast<uint16_t>(iv) + 1);
		_indices[ii][2] = (static_cast<uint16_t>(iv) + 3);
		_indices[ii + 1][0] = (static_cast<uint16_t>(iv) + 1);
		_indices[ii + 1][1] = (static_cast<uint16_t>(iv) + 2);
		_indices[ii + 1][2] = (static_cast<uint16_t>(iv) + 3);

		newLineWidth += alignment.Width;

		totalWidth = max(totalWidth, newLineWidth);
		totalHeight = min(totalHeight, newLineHeight);
	}
	totalHeight = abs(totalHeight);
	if (totalHeight == 0.0f)	// if text consists of no endlines
		totalHeight = (baseCharSize * fontTexturePitchRec);

	// Shift each line, first to center it, and then - according to alignment mode
	for (int32_t i = 0; i < newLines.size(); ++i)
	{
		LineData& data = newLines[i];

		float xOffset = -(data.Width * 0.5f);

		XMFLOAT3A shiftTotal(xOffset * sizeMultiplier, totalHeight * 0.5f * sizeMultiplier, 0.0f);
		XMVECTOR shiftTotalVec(XMLoadFloat3A(&shiftTotal));

		int32_t destIndex = (i == static_cast<int32_t>(newLines.size()) - 1) ? static_cast<int32_t>(_vPositions.GetSize()) : newLines[i + 1].StartIndex;
		for (int32_t j = data.StartIndex; j < destIndex; ++j)
		{
			XMVECTOR posVec = XMLoadFloat3A(&_vPositions[j]);
			posVec += shiftTotalVec;
			XMStoreFloat3A(&_vPositions[j], posVec);

			// fit text onto 1x1 plane, to make its size consistent with GUITransfom's size later
			//XMFLOAT2A planeScale((totalHeight / totalWidth) * 2.0f, 1.0f);
			//_vPositions[j] *= planeScale * sizeMultiplier;
		}
	}

	//_unscaledSize = XMFLOAT2A(totalWidth * 0.5f, totalHeight) * sizeMultiplier;

	_vNormals.Resize(_vPositions.GetSize());
	ZEROM(_vNormals.GetDataPtr(), _vNormals.GetSizeBytes());

	_bufferReinitFlag = !bSameLength;
	ReinitBuffers();
	
}

inline bool Mesh::Float3Equal(const XMFLOAT3A & lhs, const XMFLOAT3A & rhs) const
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

inline bool Mesh::Float2Equal(const XMFLOAT2A & lhs, const XMFLOAT2A & rhs) const
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline void Mesh::ReinitBuffers()
{
	ID3D11Device* device = Renderer::GetInstance()->GetDevice();
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();

	// destroy existing buffers if necessary
	if (_fPositions != nullptr)
	{
		if (_bufferReinitFlag)
		{
			_fPositions->Release();
			_fPositions = nullptr;

			_fNormals->Release();
			_fNormals = nullptr;

			_fUvs->Release();
			_fUvs = nullptr;

			_fIndices->Release();
			_fIndices = nullptr;
		}
		else
		{
			// update existing buffers
			D3D11_MAPPED_SUBRESOURCE sr;

			deviceContext->Map(_fPositions, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
			ASSERT(sr.pData != nullptr);
			memcpy(sr.pData, _vPositions.GetDataPtr(), _vPositions.GetSizeBytes());
			deviceContext->Unmap(_fPositions, 0);
			ZERO_ON_DEBUG(sr);

			deviceContext->Map(_fNormals, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
			ASSERT(sr.pData != nullptr);
			memcpy(sr.pData, _vNormals.GetDataPtr(), _vNormals.GetSizeBytes());
			deviceContext->Unmap(_fNormals, 0);
			ZERO_ON_DEBUG(sr);

			deviceContext->Map(_fUvs, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
			ASSERT(sr.pData != nullptr);
			memcpy(sr.pData, _vUvs.GetDataPtr(), _vUvs.GetSizeBytes());
			deviceContext->Unmap(_fUvs, 0);
			ZERO_ON_DEBUG(sr);

			deviceContext->Map(_fIndices, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
			ASSERT(sr.pData != nullptr);
			memcpy(sr.pData, _indices.GetDataPtr(), _indices.GetSizeBytes());
			deviceContext->Unmap(_fIndices, 0);
			ZERO_ON_DEBUG(sr);

			return;
		}
	}

	// create new buffers

	D3D11_BUFFER_DESC desc[4];
	D3D11_SUBRESOURCE_DATA sData[4];
	ID3D11Buffer** bufferPtrs[4] = { &_fPositions, &_fUvs, &_fNormals, &_fIndices };

	desc[0].BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc[0].ByteWidth = static_cast<uint32_t>(sizeof(XMFLOAT3A) * _vPositions.GetSize());
	desc[0].CPUAccessFlags = _bReadOnly ? 0 : D3D11_CPU_ACCESS_WRITE;
	desc[0].MiscFlags = 0;
	desc[0].StructureByteStride = 0;
	desc[0].Usage = _bReadOnly ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC;

	desc[1] = desc[2] = desc[3] = desc[0];
	desc[1].ByteWidth = static_cast<uint32_t>(sizeof(XMFLOAT2A) * _vUvs.GetSize());
	desc[3].BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc[3].ByteWidth = static_cast<uint32_t>(sizeof(int16_t) * _indices.GetSize() * 3);

	ZeroMemory(sData, sizeof(D3D11_SUBRESOURCE_DATA) * 4);
	sData[0].pSysMem = &_vPositions[0];
	sData[1].pSysMem = &_vUvs[0];
	sData[2].pSysMem = &_vNormals[0];
	sData[3].pSysMem = &_indices[0];

	for (int i = 0; i < 4; ++i)
	{
		device->CreateBuffer(desc + i, sData + i, bufferPtrs[i]);
		ASSERT(*bufferPtrs[i] != nullptr);
	}

	_bufferReinitFlag = false;
}
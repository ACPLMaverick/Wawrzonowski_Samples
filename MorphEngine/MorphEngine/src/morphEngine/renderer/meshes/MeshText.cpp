#include "MeshText.h"
#include "assetLibrary/MFont.h"
#include "renderer/RenderingManager.h"
#include "resourceManagement/ResourceManager.h"

namespace morphEngine
{
	using namespace assetLibrary;
	using namespace utility;
	using namespace resourceManagement;

	namespace renderer
	{
		namespace meshes
		{
			void MeshText::Initialize()
			{
				_font = ResourceManager::GetInstance()->GetFont(ResourceManager::GetInstance()->DEFAULT_FONT_PATH);

				_resVertices = RenderingManager::GetInstance()->GetDevice()->CreateVertexBufferInstance();
				_resNormals = nullptr;
				_resUvs = RenderingManager::GetInstance()->GetDevice()->CreateVertexBufferInstance();
				_resIndices = RenderingManager::GetInstance()->GetDevice()->CreateIndexBufferInstance();

				_resVertices->Initialize(nullptr, sizeof(utility::MVector3), _text.Length() * 4, device::BufferAccessMode::WRITE);
				_resUvs->Initialize(nullptr, sizeof(utility::MVector2), _text.Length() * 4, device::BufferAccessMode::WRITE);
				_resIndices->Initialize(nullptr, _text.Length() * 6, device::BufferAccessMode::WRITE);

				_submeshes.Add(0);

				CalculateVertices(false);
			}

			void MeshText::CalculateVertices(bool bSameLength)
			{
				ME_ASSERT(_font != nullptr, "Font not set for MeshText!");

				MSize totalVertices = _text.Length() * 4;
				MSize totalIndices = _text.Length() * 6;

				if (!bSameLength)
				{
					_vertices.Resize(totalVertices);
					_uvs.Resize(totalVertices);
					_indices.Resize(totalIndices);
				}

				MFloat32 totalWidth = 0.0f;
				MFloat32 totalHeight = 0.0f;
				MFloat32 newLineWidth = 0.0f;
				MFloat32 newLineHeight = 0.0f;
				const MFloat32 baseCharSize = static_cast<MFloat32>(_font->GetCharacterSizePixels());
				const MFloat32 fontTexturePitch = static_cast<MFloat32>(_font->GetWidth());
				const MFloat32 fontTexturePitchRec = 1.0f / fontTexturePitch;
				const MFloat32 sizeMultiplier = 4.0f;

				MArray<LineData> newLines(4);
				MSize currentLineIndex = 0;

				// calculate face positions and uvs from font and its alignment data

				for (MSize i = 0, iv = 0, ii = 0; i < _text.Length(); ++i, iv += 4, ii += 6)
				{
					char c = _text[i];

					if (c == '\n' || i == _text.Length() - 1)	// to always add the last line, even if there's no tailing \n
					{
						// add current (not new) line data to newLines
						newLines.Add(LineData(currentLineIndex, newLineWidth));
						currentLineIndex = (i + 1) * 4;	// first index of next line, multiplied by 4 because 4 vertices per letter

						if (c == '\n')
						{
							newLineHeight -= (baseCharSize * fontTexturePitchRec);
							newLineWidth = 0.0f;
							continue;
						}
					}

					MFont::CharAlignment alignment = _font->GetAlignment(c);
					//MFont::CharAlignment alignment(0.0f, 0.0f, 0.05f, 0.05f);

					MVector3 positionBase(newLineWidth, newLineHeight + alignment.BaselineCorrection, 0.0f);
					_vertices[iv] = (positionBase * sizeMultiplier);
					_vertices[iv + 1] = ((positionBase + MVector3(alignment.Width, 0.0f, 0.0f)) * sizeMultiplier);
					_vertices[iv + 2] = ((positionBase + MVector3(alignment.Width, alignment.Height, 0.0f)) * sizeMultiplier);
					_vertices[iv + 3] = ((positionBase + MVector3(0.0f, alignment.Height, 0.0f)) * sizeMultiplier);

					_uvs[iv] = (MVector2(alignment.TopX, 1.0f - (alignment.LeftY + alignment.Height)));
					_uvs[iv + 1] = (MVector2(alignment.TopX + alignment.Width, 1.0f - (alignment.LeftY + alignment.Height)));
					_uvs[iv + 2] = (MVector2(alignment.TopX + alignment.Width, 1.0f - alignment.LeftY));
					_uvs[iv + 3] = (MVector2(alignment.TopX, 1.0f - alignment.LeftY));

					_indices[ii] = (static_cast<MUint16>(iv));
					_indices[ii + 1] = (static_cast<MUint16>(iv) + 1);
					_indices[ii + 2] = (static_cast<MUint16>(iv) + 3);
					_indices[ii + 3] = (static_cast<MUint16>(iv) + 1);
					_indices[ii + 4] = (static_cast<MUint16>(iv) + 2);
					_indices[ii + 5] = (static_cast<MUint16>(iv) + 3);

					newLineWidth += alignment.Width;

					totalWidth = MMath::Max(totalWidth, newLineWidth);
					totalHeight = MMath::Min(totalHeight, newLineHeight);
				}
				totalHeight = MMath::Abs(totalHeight);
				if (totalHeight == 0.0f)	// if text consists of no endlines
					totalHeight = (baseCharSize * fontTexturePitchRec);

				// Shift each line, first to center it, and then - according to alignment mode
				for (MSize i = 0; i < newLines.GetSize(); ++i)
				{
					LineData& data = newLines[i];

					MFloat32 xOffset = -(data.Width * 0.5f);

					if (_alignment == Alignment::LEFT)
					{
						xOffset = -totalWidth * 0.5f;
					}
					else if (_alignment == Alignment::RIGHT)
					{
						xOffset += (totalWidth - data.Width) * 0.5f;
					}
					// else _alignment == Alignment::CENTER

					MVector3 shiftTotal(xOffset, totalHeight * 0.5f, 0.0f);
					shiftTotal *= sizeMultiplier;

					MSize destIndex = (i == newLines.GetSize() - 1) ? _vertices.GetSize() : newLines[i + 1].StartIndex;
					for (MSize j = data.StartIndex; j < destIndex; ++j)
					{
						_vertices[j] += shiftTotal;

						// fit text onto 1x1 plane, to make its size consistent with GUITransfom's size later
						MVector2 planeScale((totalHeight / totalWidth) * 2.0f, 1.0f);
						_vertices[j] *= planeScale * sizeMultiplier;
					}
				}

				_unscaledSize = MVector2(totalWidth * 0.5f, totalHeight) * sizeMultiplier;

				// Update or reinitialize GPU resources

				if (!bSameLength)
				{
					_resVertices->Shutdown();
					_resUvs->Shutdown();
					_resIndices->Shutdown();
					_resVertices->Initialize(reinterpret_cast<MFloat32*>(_vertices.GetDataPointer()), sizeof(utility::MVector3), _vertices.GetSize(), device::BufferAccessMode::WRITE);
					_resUvs->Initialize(reinterpret_cast<MFloat32*>(_uvs.GetDataPointer()), sizeof(utility::MVector2), _uvs.GetSize(), device::BufferAccessMode::WRITE);
					_resIndices->Initialize(reinterpret_cast<MUint16*>(_indices.GetDataPointer()), _indices.GetSize(), device::BufferAccessMode::WRITE);
				}
				else
				{
					MVector3* v3Ptr; MVector2* v2Ptr; MUint16* indPtr;
					
					_resVertices->Map(reinterpret_cast<void**>(&v3Ptr));
					memcpy(v3Ptr, _vertices.GetDataPointer(), _vertices.GetSize() * sizeof(MVector3));
					_resVertices->Unmap();

					_resUvs->Map(reinterpret_cast<void**>(&v2Ptr));
					memcpy(v2Ptr, _uvs.GetDataPointer(), _uvs.GetSize() * sizeof(MVector2));
					_resUvs->Unmap();

					_resIndices->Map(reinterpret_cast<void**>(&indPtr));
					memcpy(indPtr, _indices.GetDataPointer(), _indices.GetSize() * sizeof(MUint16));
					_resIndices->Unmap();
				}
			}
		}
	}
}
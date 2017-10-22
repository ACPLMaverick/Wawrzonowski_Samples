#include "MFont.h"
#include "resourceManagement/ResourceManager.h"
#include "debugging/Debug.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define Lib() ResourceManager::GetInstance()->GetFreeTypeLibrary()

namespace morphEngine
{
	using namespace utility;
	using namespace debugging;
	using namespace resourceManagement;

	namespace assetLibrary
	{

		bool MFont::LoadMAsset(resourceManagement::fileSystem::File & file)
		{
			MAssetType type;
			file.Read(reinterpret_cast<MUint8*>(&type), 0, sizeof(MAssetType));

			MSize offset = sizeof(MAssetType);

			if (!CheckType(type))
			{
				return false;
			}

			InternalHeader hdr;
			file.Read(reinterpret_cast<MUint8*>(&hdr), offset, sizeof(InternalHeader));

			_width = hdr._width;
			_height = hdr._height;
			_bpp = hdr._bpp;
			_dataFormat = hdr._dataFormat;
			_samplerFormat = hdr._samplerFormat;
			_mipmapCount = hdr._mipmapCount;
			_bReadOnly = hdr._bReadOnly;

			// here goes font internal header
			offset += sizeof(InternalHeader);
			file.Read(reinterpret_cast<MUint8*>(&_charSize), offset, sizeof(MInt32));
			
			offset += sizeof(MInt32);
			MSize charCount(0);
			file.Read(reinterpret_cast<MUint8*>(&charCount), offset, sizeof(MSize));

			offset += sizeof(MSize);
			MSize nameLength(0);
			file.Read(reinterpret_cast<MUint8*>(&nameLength), offset, sizeof(MSize));

			offset += sizeof(MSize);
			_name = "";
			MFixedArray<char> chars(nameLength);
			file.Read(reinterpret_cast<MUint8*>(chars.GetDataPointer()), offset, nameLength + sizeof(char));
			for (MSize i = 0; i < nameLength; ++i)
			{
				_name += chars[i];
			}

			offset += nameLength * sizeof(char);
			_alignmentTab.Allocate(charCount);
			file.Read(reinterpret_cast<MUint8*>(_alignmentTab.GetDataPointer()), offset, charCount * sizeof(CharAlignment));

			offset += charCount * sizeof(CharAlignment);

			// here goes char alignment data

			MSize siz = hdr._totalDataSize;
			_rawData.Allocate(siz);
			file.Read(_rawData.GetDataPointer(), offset, siz);

			return true;
		}

		bool MFont::LoadFont(resourceManagement::fileSystem::File & file)
		{
			MFixedArray<MUint8> bytes(file.GetSize());
			file.Read(bytes.GetDataPointer(), 0, file.GetSize());

			const MSize aTabSize = 256;
			_alignmentTab.Allocate(aTabSize);

			// R8_UNORM format
			const MSize rawDataSize = aTabSize * _charSize * _charSize * sizeof(MUint8);
			const MSize rawDataPitch = static_cast<MSize>(MMath::Sqrt(static_cast<MUint32>(rawDataSize)));
			const MFloat32 rawDataPitchRec = 1.0f / ((MFloat32)rawDataPitch);

			_rawData.Allocate(rawDataSize);
			ZeroMemory(_rawData.GetDataPointer(), rawDataSize);
			//for (MSize i = 0; i < rawDataSize; ++i)
			//	_rawData[i] = 0xFF;
			ZeroMemory(_alignmentTab.GetDataPointer(), aTabSize * sizeof(MVector4));

			FT_Face face;
			FT_Error error;
			error = FT_New_Memory_Face(Lib(), bytes.GetDataPointer(), static_cast<FT_Long>(bytes.GetSize()), 0, &face);
			if (error) { return false; }

			_name = face->family_name;
			_name += " ";
			_name += face->style_name;

			// set correct face size
			error = FT_Set_Pixel_Sizes(face, 0, _charSize);
			if (error) { return false; }

			// calculate min max variables
			MInt64 maxWidth = INT64_MIN, maxHeight = INT64_MIN, minLeft = INT64_MAX, minTop = INT64_MAX;
			for (MSize c = 0; c < aTabSize; ++c)
			{
				if (FT_Load_Char(face, static_cast<FT_ULong>(c), FT_LOAD_RENDER))
				{
					continue;
				}

				if (face->glyph->bitmap_left < minLeft)
				{
					minLeft = face->glyph->bitmap_left;
				}
				if (face->glyph->bitmap_top < minTop)
				{
					minTop = face->glyph->bitmap_top;
				}
				if (face->glyph->bitmap.width > maxWidth)
				{
					maxWidth = face->glyph->bitmap.width;
				}
				if (face->glyph->bitmap.rows > maxHeight)
				{
					maxHeight = face->glyph->bitmap.rows;
				}
			}


			MInt64 advX = 0, advY = 0;
			MInt64 pxOffset = 3;
			// load glyphs to raw data (R8_UNORM format)
			for (MSize c = 0; c < aTabSize; ++c)
			{
				if (FT_Load_Char(face, static_cast<FT_ULong>(c), FT_LOAD_RENDER))
				{
					_alignmentTab[c] = CharAlignment();
					continue;
				}

				FT_GlyphSlot g = face->glyph;

				MInt64 baseX = advX + g->bitmap_left - minLeft;
				MInt64 baseY = advY + (_charSize - (g->bitmap_top + minTop));
				MInt64 lastX = (baseX + g->bitmap.width);
				MInt64 lastY = (baseY + g->bitmap.rows);
				ME_ASSERT_S(baseX >= 0 && baseY >= 0 && baseY < static_cast<MInt64>(rawDataPitch));

				if (lastX >= static_cast<MInt64>(rawDataPitch) || baseX >= static_cast<MInt64>(rawDataPitch))
				{
					advX = MMath::Clamp(static_cast<MInt32>(advX - rawDataPitch), 0, static_cast<MInt32>(rawDataPitch));
					advY += _charSize;

					baseX = advX + g->bitmap_left - minLeft;
					baseY = advY + (_charSize - (g->bitmap_top + minTop));
					lastX = (baseX + g->bitmap.width);
					lastY = (baseY + g->bitmap.rows);
				}

				// copy from bitmap buffer to raw data at correct position
				for (MInt64 i = baseY, ib = 0; i < lastY; ++i, ++ib)
				{
					for (MInt64 j = baseX, jb = 0; j < lastX; ++j, ++jb)
					{
						// convert j and i to 1D index
						MInt64 indexRawData = j + (rawDataPitch - i) * rawDataPitch;
						MInt64 indexBitmap = jb + ib * g->bitmap.width;
						ME_ASSERT_S(indexRawData < static_cast<MInt64>(_rawData.GetSize()) && indexBitmap < g->bitmap.width * g->bitmap.rows);

						_rawData[indexRawData] = g->bitmap.buffer[indexBitmap];
					}
				}

				// load character alignment for this char

				MInt64 currentAdvX = (g->advance.x >> 6);
				MInt64 currentAdvY = (g->advance.y >> 6);

				_alignmentTab[c] = CharAlignment
				(
					(MFloat32)baseX * rawDataPitchRec,
					(MFloat32)baseY * rawDataPitchRec,
					(MFloat32)(MMath::Max((MInt32)currentAdvX, (MInt32)g->bitmap.width)) * rawDataPitchRec,
					(MFloat32)(g->bitmap.rows) * rawDataPitchRec,
					(MFloat32)(g->bitmap_top - static_cast<FT_Int>(g->bitmap.rows)) * rawDataPitchRec
				);

				advX += currentAdvX + pxOffset;
				advY += currentAdvY;

				if (advX >= static_cast<MInt64>(rawDataPitch))
				{
					advX -= rawDataPitch;
					advY += _charSize;
				}
			}

			// set texture parameters
			_width = _height = static_cast<MUint16>(rawDataPitch);
			_mipmapCount = 0;
			_bpp = 8;
			_dataFormat = renderer::device::GraphicDataFormat::UNORM_R8;
			_samplerFormat = renderer::device::TextureSamplerFormat::ADDRESS_CLAMP_FILTER_LINEAR;

			return true;
		}

		bool MFont::SaveMAsset(resourceManagement::fileSystem::File & file) const
		{
			file.WriteAppend(reinterpret_cast<const MUint8*>(&_type), sizeof(MAssetType));

			InternalHeader hdr;
			hdr._width = _width;
			hdr._height = _height;
			hdr._bpp = _bpp;
			hdr._dataFormat = _dataFormat;
			hdr._samplerFormat = _samplerFormat;
			hdr._mipmapCount = _mipmapCount;
			hdr._bReadOnly = _bReadOnly;
			hdr._totalDataSize = _rawData.GetSize();
			file.WriteAppend(reinterpret_cast<const MUint8*>(&hdr), sizeof(InternalHeader));

			// here goes font data
			// char size, char count, name length, name, alignment data (size = charSize * sizeof(CharAlignment)
			file.WriteAppend(reinterpret_cast<const MUint8*>(&_charSize), sizeof(MInt32));
			MSize charCount = _alignmentTab.GetSize();
			file.WriteAppend(reinterpret_cast<const MUint8*>(&charCount), sizeof(MSize));
			MSize nameLength = _name.Length();
			file.WriteAppend(reinterpret_cast<const MUint8*>(&nameLength), sizeof(MSize));
			file.WriteAppend(reinterpret_cast<const MUint8*>(_name.Data()), _name.Length() * sizeof(char));
			file.WriteAppend(reinterpret_cast<const MUint8*>(_alignmentTab.GetDataPointer()), charCount * sizeof(CharAlignment));

			// here goes texture data

			file.WriteAppend(_rawData.GetDataPointer(), _rawData.GetSize());

			return true;
		}

		void MFont::Shutdown()
		{
			_alignmentTab.Deallocate();
			utility::MString _name = "";
			MInt32 _charSize = 0;
		}
	}
}
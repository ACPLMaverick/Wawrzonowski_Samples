#include "stdafx.h"
#include "Font.h"
#include "System.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define Lib() System::GetInstance()->GetFreeTypeLibrary()

void Font::LoadFont(std::wstring fileName)
{
	std::string sFileName(fileName.begin(), fileName.end());
	sFileName = "./Resources/Fonts/" + sFileName + ".ttf";
	const size_t aTabSize = 256;
	_alignmentTab.Allocate(aTabSize);
	_alignmentTab.Resize(aTabSize);

	// R8_UNORM format
	const size_t rawDataSize = aTabSize * _charSize * _charSize * sizeof(uint8_t);
	const size_t rawDataPitch = static_cast<size_t>(sqrt(static_cast<uint32_t>(rawDataSize)));
	const float rawDataPitchRec = 1.0f / ((float)rawDataPitch);

	_texture.GetRawData().Allocate(rawDataSize);
	_texture.GetRawData().Resize(rawDataSize);
	ZEROM(_texture.GetRawData().GetDataPtr(), rawDataSize);
	//for (size_t i = 0; i < rawDataSize; ++i)
	//	_texture.GetRawData()[i] = 0xFF;
	ZEROM(_alignmentTab.GetDataPtr(), aTabSize * sizeof(XMFLOAT4A));

	FT_Face face;
	FT_Error error;
	error = FT_New_Face(Lib(), sFileName.c_str(), 0, &face);
	ASSERT(!error);

	_name = face->family_name;
	_name += " ";
	_name += face->style_name;

	// set correct face size
	error = FT_Set_Pixel_Sizes(face, 0, _charSize);
	ASSERT(!error);

	// calculate min max variables
	int64_t maxWidth = INT64_MIN, maxHeight = INT64_MIN, minLeft = INT64_MAX, minTop = INT64_MAX;
	for (size_t c = 0; c < aTabSize; ++c)
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


	int64_t advX = 0, advY = 0;
	int64_t pxOffset = 16;
	// load glyphs to raw data (R8_UNORM format)
	for (size_t c = 0; c < aTabSize; ++c)
	{
		if (FT_Load_Char(face, static_cast<FT_ULong>(c), FT_LOAD_RENDER))
		{
			_alignmentTab[c] = CharAlignment();
			continue;
		}

		FT_GlyphSlot g = face->glyph;

		int64_t baseX = advX + g->bitmap_left - minLeft;
		int64_t baseY = advY + (_charSize - (g->bitmap_top + minTop));
		int64_t lastX = (baseX + g->bitmap.width);
		int64_t lastY = (baseY + g->bitmap.rows);
		ASSERT(baseX >= 0 && baseY >= 0 && baseY < static_cast<int64_t>(rawDataPitch));

		if (lastX >= static_cast<int64_t>(rawDataPitch) || baseX >= static_cast<int64_t>(rawDataPitch))
		{
			advX = CLAMP(static_cast<int32_t>(advX - rawDataPitch), 0, static_cast<int32_t>(rawDataPitch));
			advY += _charSize;

			baseX = advX + g->bitmap_left - minLeft;
			baseY = advY + (_charSize - (g->bitmap_top + minTop));
			lastX = (baseX + g->bitmap.width);
			lastY = (baseY + g->bitmap.rows);
		}

		// copy from bitmap buffer to raw data at correct position
		for (int64_t i = baseY, ib = 0; i < lastY; ++i, ++ib)
		{
			for (int64_t j = baseX, jb = 0; j < lastX; ++j, ++jb)
			{
				// convert j and i to 1D index
				int64_t indexRawData = j + (rawDataPitch - i) * rawDataPitch;
				int64_t indexBitmap = jb + ib * g->bitmap.width;
				ASSERT(indexRawData < static_cast<int64_t>(_texture.GetRawData().GetSize()) && indexBitmap < g->bitmap.width * g->bitmap.rows);

				_texture.GetRawData()[indexRawData] = g->bitmap.buffer[indexBitmap];
			}
		}

		// load character alignment for this char

		int64_t currentAdvX = (g->advance.x >> 6);
		int64_t currentAdvY = (g->advance.y >> 6);

		_alignmentTab[c] = CharAlignment
		(
			(float)baseX * rawDataPitchRec,
			(float)baseY * rawDataPitchRec,
			(float)(max((int32_t)currentAdvX, (int32_t)g->bitmap.width)) * rawDataPitchRec,
			(float)(g->bitmap.rows) * rawDataPitchRec,
			(float)(g->bitmap_top - static_cast<FT_Int>(g->bitmap.rows)) * rawDataPitchRec
		);

		advX += currentAdvX + pxOffset;
		advY += currentAdvY;

		if (advX >= static_cast<int64_t>(rawDataPitch))
		{
			advX -= rawDataPitch;
			advY += _charSize;
		}
	}

	// Create texture resource
	_texture.SetWidth(static_cast<int32_t>(rawDataPitch));
	_texture.SetMipmapped(true);
	_texture.SetBPP(8);
	_texture.SetFormat(DXGI_FORMAT::DXGI_FORMAT_R8_UNORM);

	_texture.InitResources(true);
}

void Font::Shutdown()
{
}
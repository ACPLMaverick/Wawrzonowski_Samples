#pragma once

#include "GlobalDefines.h"
#include "Buffer.h"
#include "Texture.h"

class Font
{
public:

	struct CharAlignment
	{
		float TopX = 0.0f;
		float LeftY = 0.0f;
		float Width = 0.0f;
		float Height = 0.0f;
		float BaselineCorrection = 0.0f;

		CharAlignment() { }
		CharAlignment(float tx, float ty, float w, float h, float bc) :
			TopX(tx),
			LeftY(ty),
			Width(w),
			Height(h),
			BaselineCorrection(bc)
		{

		}
	};

protected:

	static const int32_t BASE_CHAR_SIZE = 128;

	Texture _texture;

	// X, Y - top left corner, W - width, Z - height
	Buffer<CharAlignment> _alignmentTab;
	std::string _name;
	int32_t _charSize = BASE_CHAR_SIZE;

	void LoadFont(std::wstring fileName);

	void Shutdown();

public:

	inline Font(const std::wstring& fileName) { LoadFont(fileName); }

	inline ~Font() { Shutdown(); }

	inline void Set(int32_t slot) { _texture.Set(slot); }

	inline CharAlignment GetAlignment(char c) const
	{
		if (c > _alignmentTab.GetSize())
			c = 0;
		return _alignmentTab[static_cast<int32_t>(c)];
	}
	inline int32_t GetCharacterSizePixels() const { return BASE_CHAR_SIZE; }
	inline int32_t GetWidth() const { return _texture.GetWidth(); }

	static Font* CreateResource(const std::wstring& fileName) { return new Font(fileName); }
};
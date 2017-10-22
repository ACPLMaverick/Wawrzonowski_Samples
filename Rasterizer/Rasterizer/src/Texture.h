#pragma once

#include "stdafx.h"
#include "Color32.h"
#include "Float2.h"

#define TEXTURE_PATH "./textures/"
#define TEXTURE_EXTENSION ".tga"

class Texture
{
public:

#pragma region Enums Public

	enum SampleMode
	{
		UV,
		SPHERICAL
	};

	enum WrapMode
	{
		WRAP,
		CLAMP
	};

	enum FilterMode
	{
		NEAREST,
		LINEAR
	};

#pragma endregion

protected:

#pragma region Protected

	Color32* _data = nullptr;
	SampleMode _sm = SampleMode::UV;
	WrapMode _wm = WrapMode::WRAP;
	FilterMode _fm = FilterMode::LINEAR;
	uint16_t _width = 0;

#pragma endregion

#pragma region Functions Protected

	inline void LoadFromFile(const std::string* name);

#pragma endregion

public:

#pragma region Functions Public

	Texture();
	Texture(Color32 col, SampleMode sm = SampleMode::UV, WrapMode wm = WrapMode::WRAP, FilterMode fm = FilterMode::LINEAR);
	Texture(const std::string* name, SampleMode sm = SampleMode::UV, WrapMode wm = WrapMode::WRAP, FilterMode fm = FilterMode::LINEAR);
	~Texture();

	Color32 GetColor(const math::Float2& uv, const math::Float3& modelPos) const;

#pragma region Accessors

	uint16_t GetWidth() const { return _width; }

#pragma endregion

#pragma endregion
};


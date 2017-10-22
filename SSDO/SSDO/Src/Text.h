#pragma once

#include "GlobalDefines.h"
#include "Mesh.h"
#include "Shader.h"

class Font;

class Text
{
protected:

	Shader::TextConstantBuffer _bufferData;
	Mesh _mesh;
	std::string _text;
	Font* _font = nullptr;

	inline void InitCommon();

public:
	Text(const std::string& text = "Dummy text", bool bReadOnly = true, Font* font = nullptr,
		const XMFLOAT2A& position = XMFLOAT2A(0.0f, 0.0f), const XMFLOAT2A& scale = XMFLOAT2A(1.0f, 1.0f));
	~Text();

	// Assuming correct shader is already set.
	void Draw();

	void SetText(const std::string& text);
	void SetFont(Font* font);
	inline void SetColor(const XMFLOAT4A& color) { _bufferData.Color = color; }
	inline void SetPosition(const XMFLOAT2A& position) { *reinterpret_cast<XMFLOAT2A*>(&_bufferData.PositionScale) = position; }
	inline void SetScale(const XMFLOAT2A& scale) { *reinterpret_cast<XMFLOAT2A*>(&_bufferData.PositionScale.z) = scale; }
	inline void SetScale(float scale) { SetScale(XMFLOAT2A(scale, scale)); }
	inline const std::string& GetText() { return _text; }
	inline Font* GetFont() { return _font; }
	inline const XMFLOAT4A& GetColor() { return _bufferData.Color; }
	inline const XMFLOAT2A& GetPosition() { return *reinterpret_cast<XMFLOAT2A*>(&_bufferData.PositionScale); }
	inline const XMFLOAT2A& GetScale() { return *reinterpret_cast<XMFLOAT2A*>(&_bufferData.PositionScale.z); }

};


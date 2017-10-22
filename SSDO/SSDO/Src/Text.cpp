#include "stdafx.h"
#include "Text.h"
#include "System.h"
#include "Scenes\Scene.h"
#include "Renderer.h"

#include <functional>

Text::Text(const std::string & text, bool bReadOnly, Font * font, const XMFLOAT2A& position, const XMFLOAT2A& scale) :
	_text(text),
	_font(font),
	_mesh(bReadOnly)
{
	SetPosition(position);
	SetScale(scale);

	ASSERT(System::GetInstance()->GetScene() != nullptr);
	if(_font == nullptr)
		_font = System::GetInstance()->GetScene()->LoadFont(L"arial");
	InitCommon();
}

Text::~Text()
{
}

void Text::Draw()
{
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();

	_font->Set(0);

	Shader::TextConstantBuffer* dataPtr = reinterpret_cast<Shader::TextConstantBuffer*>(System::GetInstance()->GetScene()->GetTextShader()->MapVsBuffer(0));
	Shader::TextConstantBuffer copy = _bufferData;
	float xScale = System::GetInstance()->GetOptions()._aspectRatio;
	copy.PositionScale.z /= xScale;
	memcpy(dataPtr, &copy, sizeof(Shader::TextConstantBuffer));
	System::GetInstance()->GetScene()->GetTextShader()->UnmapVsBuffer(0);

	_mesh.DrawBuffers();
}

void Text::SetText(const std::string & text)
{
	if (!_mesh.GetReadOnly())
	{
		size_t current(std::hash<std::string>{}(_text));
		size_t other(std::hash<std::string>{}(text));

		if (current != other)
		{
			_text = text;
			_mesh.UpdateDataFromText(_text, *_font);
		}
	}
}

void Text::SetFont(Font * font)
{
	if (!_mesh.GetReadOnly())
	{
		if (_font != font)
		{
			_font = font;
			_mesh.UpdateDataFromText(_text, *_font);
		}
	}
}

inline void Text::InitCommon()
{
	_mesh.UpdateDataFromText(_text, *_font);
}

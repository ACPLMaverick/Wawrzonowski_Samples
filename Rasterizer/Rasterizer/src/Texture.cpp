#include "Texture.h"

#include <fstream>
#include <string>

Texture::Texture()
{
}

Texture::Texture(Color32 col, SampleMode sm, WrapMode wm, FilterMode fm)
{
	_sm = sm;
	_wm = wm;
	_fm = fm;
	_width = 1;
	_data = new Color32[1];
	_data[0] = col;
}

Texture::Texture(const std::string * name, SampleMode sm, WrapMode wm , FilterMode fm)
{
	_sm = sm;
	_wm = wm;
	_fm = fm;
	LoadFromFile(name);
}

Texture::~Texture()
{
	if (_data != nullptr)
	{
		delete[] _data;
	}
}

Color32 Texture::GetColor(const math::Float2& uv, const math::Float3& modelPos) const
{
	math::Float2 newUV(uv);
	newUV.v = 1.0f - newUV.v;

	if (_sm == SampleMode::SPHERICAL)
	{
		math::Float3 normModelPos(modelPos);
		math::Float3::Normalize(normModelPos);

		newUV.u = atan2(normModelPos.x, normModelPos.z) * M_1_PI * 0.5f;
		newUV.v = 1.0f - acos(normModelPos.y) * M_1_PI;
	}

	if (_wm == WrapMode::CLAMP)
	{
		newUV.u = Clamp(newUV.u, 0.0f, 1.0f);
		newUV.v = Clamp(newUV.v, 0.0f, 1.0f);
	}
	else if (_wm == WrapMode::WRAP)
	{
		math::Float2 floors(floorf(newUV.u), floorf(newUV.v));
		newUV = newUV - floors;
	}

	if (_fm == FilterMode::NEAREST)
	{
		newUV = math::Float2(roundf(newUV.u * (_width - 1.0f)), roundf(newUV.v * (_width - 1.0f)));
		return _data[(int32_t)newUV.y * (int32_t)_width + (int32_t)newUV.x];
	}
	else if (_fm == FilterMode::LINEAR)
	{
		newUV = math::Float2(newUV.u * (_width - 1.0f), newUV.v * (_width - 1.0f));
		math::Float2 floors(floorf(newUV.u), floorf(newUV.v));
		math::Float2 ratios(newUV.x - floors.x, newUV.y - floors.y);
		math::Float2 opposites(1.0f - ratios.x, 1.0f - ratios.y);
		int32_t iuf = (int32_t)(floors.u);
		int32_t ivf = (int32_t)(floors.v);

		return ((_data[ivf * _width + iuf] * opposites.x +
			_data[ivf * _width + iuf + 1] * ratios.x) * opposites.y +
			(_data[min(ivf + 1, _width - 1) * _width + iuf] * opposites.x +
				_data[min(ivf + 1, _width - 1) * _width + iuf + 1] * ratios.x) * ratios.y);
	}

	return Color32();
}

void Texture::LoadFromFile(const std::string * name)
{
	std::ifstream file(TEXTURE_PATH + *name + TEXTURE_EXTENSION, std::ios::in | std::ios::binary);
	uint8_t bpp;

	if (file.is_open())
	{
		// check if it is TGA file
		file.seekg(-18, std::ios_base::end);
		char sigBuffer[18];
		file.read(sigBuffer, 18);
		if (strcmp(sigBuffer, "TRUEVISION-XFILE."))
		{
#ifdef _DEBUG

			std::cout << "Not a TGA file: " << TEXTURE_PATH + *name + TEXTURE_EXTENSION << std::endl;

#endif // _DEBUG

			return;
		}

		// get file size and check if it's square
		uint16_t wh[2];
		file.seekg(12, std::ios_base::beg);
		file.read((char*)wh, 4);
		if (wh[0] != wh[1])
		{
#ifdef _DEBUG

			std::cout << "No square dimensions: " << TEXTURE_PATH + *name + TEXTURE_EXTENSION << std::endl;

#endif // _DEBUG

			return;
		}
		_width = wh[0];

		// check whether it is 24-bit or 32-bit file
		file.read((char*)&bpp, 1);

		// create data buffer and fill with BGRA bytes
		file.seekg(1, std::ios_base::cur);
		uint32_t siz = _width * _width;
		_data = new Color32[siz];

		if (bpp == 32)
		{
			file.read((char*)_data, siz * bpp);
		}
		else if (bpp == 24)
		{
			uint8_t tempBuffer[3];

			for (size_t i = 0; i < siz; ++i)
			{
				file.read((char*)tempBuffer, 3);
				_data[i] = Color32(255, tempBuffer[2], tempBuffer[1], tempBuffer[0]);
			}
		}

		return;
	}

#ifdef _DEBUG

	std::cout << "Unable to open file: " << TEXTURE_PATH + *name + TEXTURE_EXTENSION << std::endl;

#endif // _DEBUG
}
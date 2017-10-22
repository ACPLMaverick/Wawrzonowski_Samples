#pragma once

#include "stdafx.h"

template <class T>
class Buffer
{
protected:

	T* _data;

	uint16_t _width;
	uint16_t _height;

public:

#pragma region Functions Public

	Buffer<T>(uint16_t width, uint16_t height);
	~Buffer<T>();

	void SetPixel(uint16_t x, uint16_t y, T val);
	T GetPixel(uint16_t x, uint16_t y);
	T GetPixelScaled(uint16_t x, uint16_t y, uint16_t w, uint16_t h);

	void Fill(T val);

	uint16_t GetWidth() { return _width; }
	uint16_t GetHeight() { return _height; }

#pragma endregion
};

#pragma region Function Definitions


template <class T> Buffer<T>::Buffer(uint16_t width, uint16_t height) :
	_width(width),
	_height(height)
{
	_data = new T[width * height];
	ZeroMemory(_data, width * height * sizeof(T));
}

template <class T> Buffer<T>::~Buffer()
{
	delete[] _data;
}


#pragma endregion

template<class T>
inline void Buffer<T>::SetPixel(uint16_t x, uint16_t y, T val)
{
	if (x >= 0 && x <= _width && y >= 0 && y <= _height)
	{
		_data[y * (_width) + x] = val;
	}
}

template<class T>
inline T Buffer<T>::GetPixel(uint16_t x, uint16_t y)
{
	return _data[y * (_width) + x];
}

template<class T>
inline T Buffer<T>::GetPixelScaled(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	uint16_t coordX = (uint16_t)((float)x * (float)(_width) / (float)(w));
	uint16_t coordY = (uint16_t)((float)y * (float)(_height) / (float)(h));
	return GetPixel(coordX, coordY);
}

template<class T>
inline void Buffer<T>::Fill(T val)
{
	size_t t = _width * _height;
	for (size_t i = 0; i < t; ++i)
	{
		_data[i] = val;
	}
}

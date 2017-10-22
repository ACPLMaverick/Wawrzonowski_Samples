#pragma once

#include "stdafx.h"

template <class T>
class Buffer
{
public:

	template <class T>
	class Iterator
	{
		friend class Buffer<T>;
	protected:
		Buffer<T>& _buf;
		size_t _pos = 0;

		inline void VerifyPos() const
		{
			ASSERT_D(_pos <= _buf.GetSize(), L"Buffer iterator error: invalid position.");
		}

		template <typename T> Iterator(Buffer<T>& buf, size_t startPos) : 
			_buf(buf),
			_pos(startPos)
		{
			VerifyPos();
		}

		template <typename T> Iterator(const Iterator<T>& copy) :
			_buf(copy.buf),
			_pos(copy._pos)
		{
			VerifyPos();
		}

	public:

		inline bool IsValid() const { return _pos < _buf.GetSize(); }

		inline T& operator*()
		{
			return _buf[_pos];
		}

		inline T operator*() const
		{
			return _buf[_pos];
		}

		inline Iterator<T>& operator++()
		{
			operator+=(1);
			return *this;
		}

		inline Iterator<T>& operator--()
		{
			operator+=(-1);
			return *this;
		}

		inline Iterator<T>& operator+=(int64_t ind)
		{
			_pos += ind;
			VerifyPos();
			return *this;
		}

		inline Buffer<T>& operator-=(int64_t ind)
		{
			operator+=(-ind);
			return *this;
		}

		inline Iterator<T> operator+(int64_t ind) const
		{
			Iterator<T> copy(*this);
			copy += ind;
			return copy;
		}

		inline Iterator<T> operator-(int64_t ind) const
		{
			return operator+(-ind);
		}
	};

protected:

#pragma region Protected

	size_t _capacity = 0;
	size_t _size = 0;
	T* _data = nullptr;

	inline void Reallocate();
	inline void ShiftAt(size_t at, size_t amount);

#pragma endregion

public:

#pragma region Functions Public

	Buffer();
	~Buffer();

	void Allocate(size_t capacity);
	void Destroy();
	void Resize(size_t size);

	void Add(T& obj);
	void Remove(T& obj);
	void RemoveAt(size_t index);

	T operator[](size_t index) const;
	T& operator[](size_t index);

	inline T* GetDataPtr() { return _data; }
	inline size_t GetSize() const { return _size; }
	inline size_t GetSizeBytes() const { return GetSize() * sizeof(T); }
	inline bool IsAllocated() const { return _data != nullptr; }

	inline Iterator<T> GetIterator() { return Iterator<T>(*this, 0); }

#pragma endregion
};

#pragma region Definitions

template<class T>
inline Buffer<T>::Buffer()
{
}

template<class T>
inline Buffer<T>::~Buffer()
{
	if (_data != nullptr)
	{
		Destroy();
	}
}

template<class T>
inline void Buffer<T>::Allocate(size_t capacity)
{
	ASSERT(capacity != 0);
	if (_data != nullptr)
		Destroy();
	_size = 0;
	_capacity = capacity;
	_data = new T[_capacity];
	ZeroMemory(_data, _capacity * sizeof(T));
}

template<class T>
inline void Buffer<T>::Destroy()
{
	delete[] _data;
	_data = nullptr;
	_size = 0;
	_capacity = 0;
}

template<class T>
inline void Buffer<T>::Resize(size_t size)
{
	if (size > _capacity)
	{
		Allocate(size);
	}

	_size = size;
}

template<class T>
inline void Buffer<T>::Add(T & obj)
{
	ASSERT(_data != nullptr);

	++_size;
	if (_capacity <= _size)
	{
		Reallocate();
	}

	_data[_size - 1] = obj;
}

template<class T>
inline void Buffer<T>::Remove(T & obj)
{
	ASSERT(_data != nullptr);

	for (size_t i = 0; i < _size; ++i)
	{
		if (_data[i] == obj)
		{
			ShiftAt(i, -1);
			--_size;
			break;
		}
	}
}

template<class T>
inline void Buffer<T>::RemoveAt(size_t index)
{
	ASSERT(_data != nullptr && index < _size);

	ShiftAt(index, -1);
	--_size;
	break;
}

template<class T>
inline T Buffer<T>::operator[](size_t index) const
{
	ASSERT(_data != nullptr && index < _size);
	return _data[index];
}

template<class T>
inline T& Buffer<T>::operator[](size_t index)
{
	ASSERT(_data != nullptr && index < _size);
	return _data[index];
}

template<class T>
inline void Buffer<T>::Reallocate()
{
	_capacity *= 2;
	T* newData = new T[_capacity];
	memcpy(newData, _data, _size * sizeof(T));
	delete[] _data;
	_data = newData;
}

template<class T>
inline void Buffer<T>::ShiftAt(size_t at, size_t amount)
{
	memmove(_data + at, _data + at - amount, sizeof(T) * (_size - at - 1));
}


#pragma endregion
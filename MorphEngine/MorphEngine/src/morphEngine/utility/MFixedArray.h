#pragma once

#include "core/GlobalDefines.h"
#include "utility/MCollection.h"
#include "MArray.h"

namespace morphEngine
{
	namespace utility
	{
		template <class T> class MArray;
		/// <summary>
		/// This is simple fixed-length array, meant to be used in place of normal c++ new[] operator.
		/// It is designed to store simple types and pointers, so please do not use it for the storage of complex class objects.
		/// </summary>
		template <class T>
		class MFixedArray
		{
		protected:
			T* _data;
			MSize _dataSize;

			inline void CopyArray(const MFixedArray<T>& copy);
			inline void CopyNfArray(const MArray<T>& copy);

		public:
#pragma region Functions Public

			inline MFixedArray();
			MFixedArray(MSize dataSize);
			MFixedArray(MSize dataSize, const T& dummyObject);
			MFixedArray(const MFixedArray<T>& copy);
			//MFixedArray(const MArray<T>& copy);
			virtual ~MFixedArray();

			inline void Allocate(MSize size);
			inline void Allocate(MSize size, const T& dummyObject);
			inline void Deallocate();
			inline void FillWithZeros();

			MSize GetSize() const { return _dataSize; }
			T* GetDataPointer() const { return _data; }
			T operator[](MSize index) const { return _data[index]; }
			T& operator[](MSize index) { return _data[index]; }
			MFixedArray<T>& operator=(const MFixedArray<T>& copy);
			MFixedArray<T>& operator=(const MArray<T>& copy);
			bool operator==(const MFixedArray<T>& other) const;
			bool operator!=(const MFixedArray<T>& other) const;

#pragma endregion
		};

	}
}

#pragma region Function Definitions

namespace morphEngine
{
	namespace utility
	{

		template<class T>
		inline MFixedArray<T>::MFixedArray() : _data(0), _dataSize(0)
		{
		
		}

		template <class T> MFixedArray<T>::MFixedArray(MSize dataSize) : MFixedArray<T>()
		{
			Allocate(dataSize);
		}

		template <class T> MFixedArray<T>::MFixedArray(MSize dataSize, const T& dummyObject) : MFixedArray<T>()
		{
			Allocate(dataSize, dummyObject);
		}

		template<class T>
		inline MFixedArray<T>::MFixedArray(const MFixedArray<T>& copy) : _data(0), _dataSize(0)
		{
			CopyArray(copy);
		}

		//template<class T>
		//inline MFixedArray<T>::MFixedArray(const MArray<T>& copy)
		//{
		//	CopyNfArray(copy);
		//}

		template <class T> MFixedArray<T>::~MFixedArray()
		{
			Deallocate();
		}

		template <class T> inline void MFixedArray<T>::Allocate(MSize size)
		{
			ME_ASSERT(size != 0, "Why are you trying to allocate 0 bytes?");

			if (_data != nullptr)
			{
				MCollectionHelper::Deallocate(reinterpret_cast<void*>(_data));
			}

			_dataSize = size;
			_data = reinterpret_cast<T*>(MCollectionHelper::Allocate(_dataSize * sizeof(T), __alignof(T)));

			for(MSize i = 0; i < size; ++i)
			{
				T* d = reinterpret_cast<T*>(_data) + i;
				::new(d) T();
			}
		}

		template <class T> inline void MFixedArray<T>::Allocate(MSize size, const T& dummyObject)
		{
			ME_ASSERT(size != 0, "Why are you trying to allocate 0 bytes?");

			if(_data != nullptr)
			{
				MCollectionHelper::Deallocate(reinterpret_cast<void*>(_data));
			}

			_dataSize = size;
			_data = reinterpret_cast<T*>(MCollectionHelper::Allocate(_dataSize * sizeof(T), __alignof(T)));

			for(MSize i = 0; i < size; ++i)
			{
				T* d = reinterpret_cast<T*>(_data) + i;
				::new(d) T(dummyObject);
			}
		}

		template<class T>
		inline void MFixedArray<T>::Deallocate()
		{
			for(MSize i = 0; i < _dataSize; ++i)
			{
				T* d = reinterpret_cast<T*>(_data) + i;
				d->~T();
			}

			if (_data != nullptr)
			{
				MCollectionHelper::Deallocate(reinterpret_cast<void*>(_data));
			}
			_data = 0;
		}

		template<class T>
		inline void MFixedArray<T>::FillWithZeros()
		{
			ME_ASSERT(_data != nullptr, "Data not initialized.");
			ZeroMemory(_data, _dataSize * sizeof(T));
		}

		template<class T>
		inline MFixedArray<T>& MFixedArray<T>::operator=(const MFixedArray<T>& copy)
		{
			CopyArray(copy);
			return *this;
		}

		template<class T>
		inline MFixedArray<T>& MFixedArray<T>::operator=(const MArray<T>& copy)
		{
			CopyNfArray(copy);
			return *this;
		}

		template<class T>
		inline bool MFixedArray<T>::operator==(const MFixedArray<T>& other) const
		{
			if(_dataSize != other._dataSize)
			{
				return false;
			}

			for(MSize i = 0; i < _dataSize; ++i)
			{
				if(operator[](i) != other[i])
				{
					return false;
				}
			}

			return true;
		}

		template<class T>
		inline bool MFixedArray<T>::operator!=(const MFixedArray<T>& other) const
		{
			if(_dataSize == other._dataSize)
			{
				return false;
			}

			for(MSize i = 0; i < _dataSize; ++i)
			{
				if(operator[](i) == other[i])
				{
					return false;
				}
			}

			return true;
		}

		template<class T>
		inline void MFixedArray<T>::CopyArray(const MFixedArray<T>& copy)
		{
			if (copy._dataSize != _dataSize)
			{
				Allocate(copy._dataSize);
			}

			memcpy(_data, copy._data, copy._dataSize * sizeof(T));
		}

		template<class T>
		inline void MFixedArray<T>::CopyNfArray(const MArray<T>& copy)
		{
			if (copy.GetSize() != _dataSize)
			{
				Allocate(copy.GetSize());
			}

			memcpy(_data, copy.GetDataPointer(), copy.GetSize() * sizeof(T));
		}
	}
}

#pragma endregion
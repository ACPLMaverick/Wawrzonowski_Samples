#pragma once

#include "core/GlobalDefines.h"

namespace morphEngine
{
	namespace utility
	{
		namespace MCollectionHelper
		{
			extern void* Allocate(MSize size, MUint8 align);
			extern void Deallocate(void* ptr);
			extern void* AllocateL(MSize size, MUint8 align);
			extern void DeallocateL(void* ptr);
		}

		template <class T>
		class MCollection
		{
		public:

#pragma region Enums Public

			enum CollectionType
			{
				UNSPECIFIED,
				ARRAY,
				STACK,
				QUEUE,
				PRIORITY_QUEUE,
				PRIORITY_QUEUE_ASCENDING,
				LINKED_LIST,
				DICTIONARY
			};

			enum GrowthStrategy
			{
				GROW_MUL2_SIZE,
				GROW_HALF_INITIAL_SIZE,
				NO_GROW
			};

			enum ShrinkStrategy
			{
				SHRINK_HALF_AFTER_HALF_EMPTY,
				SHRINK_HALF_AFTER_THREE_QUARTERS_EMPTY,
				NO_SHRINK
			};

#pragma endregion

#pragma region Typedefs

			// Example:
			// Returns -1 when A < B, returns 0 when A == B, returns 1 when A > B
			// Void can be an additional parameter, usually a pointer to instance of caller
			typedef MInt8(*CompareFunctionPointer)(T *, T *, void*);

#pragma endregion


		protected:

#pragma region Protected

			void* _data = nullptr;
			MSize _size = 0;
			MSize _capacity = 0;
			CollectionType _type;
			GrowthStrategy _growthStrategy;
			ShrinkStrategy _shrinkStrategy;

#pragma endregion

#pragma region Functions Protected

			inline void AssignToMemory(const T& object, T* destination);
			inline void UnassignMemory(T* destination);

			inline void MemoryCopy(T* destination, T* source, MSize size);
			inline void MoveMemoryCopy(T* destination, T* source, MSize size);
			inline void ClearMemoryOne(void * from, T & obj);
			inline void ClearMemoryOne(void * from);
			void ClearMemory(void * from, MSize howMany);
			void ClearMemory(void * from, MSize howMany, T & obj);

			virtual void Reallocate(MSize newSize);
			virtual inline bool CheckIfGrow();
			virtual inline bool CheckIfShrink();
			virtual inline void Shrink();
			virtual inline void Grow();

			inline void Quicksort(MInt64 left, MInt64 right, CompareFunctionPointer compare, void* param);

			void ShiftOne(MSize from, MSize size, bool right);
			void Shift(MSize from, MSize size, MInt64 i);

			bool ValidateIndex(const MSize index) const;

#pragma endregion

		public:			

#pragma region Classes Public

			class MIteratorBase
			{
				friend class MCollection<T>;
			protected:

			public:
				MIteratorBase()
				{

				}

				MIteratorBase(const MIteratorBase& c)
				{

				}

				virtual ~MIteratorBase()
				{
				}

				virtual bool IsValid() const { return false; }
			};

#pragma endregion


#pragma region Functions Public

			inline MCollection(MSize beginCapacity,
				GrowthStrategy growthStrategy = GrowthStrategy::GROW_MUL2_SIZE,
				ShrinkStrategy shrinkStrategy = ShrinkStrategy::NO_SHRINK);
			inline MCollection(const MCollection& c);
			inline virtual ~MCollection();

			inline void Shutdown();

			inline virtual void Clear();

			inline CollectionType GetType() { return _type; }

			virtual inline bool IsEmpty() const = 0;
			virtual inline MSize GetSize() const = 0;
			virtual inline MSize GetCapacity() const = 0;

			// Example of CompareFunctionPointer:
			// Returns -1 when A < B, returns 0 when A == B, returns 1 when A > B
			virtual inline void Sort(CompareFunctionPointer compare = CompareAscending, void* param = nullptr);

			virtual inline void Fill(T& value);

			virtual inline bool Contains(const T& obj) const = 0;

			virtual inline MCollection<T>& operator=(const MCollection<T>& c);

#pragma endregion

		};

#pragma region Definitions

		template <class T> MCollection<T>::MCollection(MSize beginCapacity,
				GrowthStrategy growthStrategy,
				ShrinkStrategy shrinkStrategy) :
			_capacity(beginCapacity),
			_growthStrategy(growthStrategy),
			_shrinkStrategy(shrinkStrategy),
			_size(0),
			_data(nullptr)
		{
		}

		template <class T> MCollection<T>::MCollection(const MCollection& c) : 
			_size(c._size)
		{
			
		}

		template <class T> MCollection<T>::~MCollection()
		{
			Shutdown();
		}

		template<class T>
		inline void MCollection<T>::Shutdown()
		{
			Clear();

			if (_data != nullptr)
			{
				MCollectionHelper::Deallocate(_data);
				_data = nullptr;
			}
		}

		template<class T>
		inline void MCollection<T>::Clear()
		{
			if(_data != nullptr)
			{
				T* d = reinterpret_cast<T*>(_data);
				ClearMemory(d, _size);
				_size = 0;
			}
		}


		template<class T>
		inline void MCollection<T>::Sort(CompareFunctionPointer compare, void* param)
		{
			if (_size <= 1)
				return;

			Quicksort(0, static_cast<MInt64>(_size) - 1, compare, param);
		}

		template<class T>
		inline void MCollection<T>::Fill(T & value)
		{
			T* dataPtr = reinterpret_cast<T*>(_data);
			for (MSize i = 0; i < _size; ++i)
			{
				AssignToMemory(value, dataPtr + i);
			}
		}

		template<class T>
		inline MCollection<T>& MCollection<T>::operator=(const MCollection<T>& c)
		{
			void* temp = _data;
			
			if(temp != nullptr)
			{
				MCollectionHelper::Deallocate(temp);
			}
			_capacity = c._capacity;
			_size = c._size;

			_data = MCollectionHelper::Allocate(_capacity * sizeof(T), _alignof(T));

			MemoryCopy(reinterpret_cast<T*>(_data), reinterpret_cast<T*>(c._data), c._size);

			return *this;
		}

		template<class T>
		inline void MCollection<T>::AssignToMemory(const T& object, T* destination)
		{
			::new(destination) T(object);
		}

		template<class T>
		inline void MCollection<T>::UnassignMemory(T * destination)
		{
			destination->~T();
		}

		template<class T>
		inline void MCollection<T>::MemoryCopy(T * destination, T * source, MSize size)
		{
			for (MSize i = 0; i < size; ++i)
			{
				AssignToMemory(source[i], destination + i);
			}
		}

		template<class T>
		inline void MCollection<T>::MoveMemoryCopy(T * destination, T * source, MSize size)
		{
			// check whether we can blindly copy forward or use temporary buffer
			if (destination <= source)
			{
				// copy buffer to destination
				for (MSize i = 0; i < size; ++i)
				{
					AssignToMemory(source[i], destination + i);
				}
			}
			else
			{
				// we need to copy it backwards
				for (MSize i = size - 1; i >= 0; --i)
				{
					AssignToMemory(source[i], destination + i);
				}
				/*
				// copy to buffer
				T* tab = new T[size];
				for (size_t i = 0; i < size; ++i)
				{
					tab[i] = source[i];
				}

				// copy buffer to destination
				for (size_t i = 0; i < size; ++i)
				{
					destination[i] = tab[i];
				}
				delete tab;
				*/
			}
		}

		template<class T>
		inline void MCollection<T>::ClearMemoryOne(void * from, T & obj)
		{
			UnassignMemory(reinterpret_cast<T*>(from));
			ZeroMemory(from, sizeof(obj));
		}

		template<class T>
		inline void MCollection<T>::ClearMemoryOne(void* from)
		{
			UnassignMemory(reinterpret_cast<T*>(from));
			ZeroMemory(from, sizeof(T));
		}

		template<class T>
		inline void MCollection<T>::ClearMemory(void* from, MSize howMany)
		{
			//ZeroMemory(from, howMany * sizeof(T));
			
			for (MSize i = 0; i < howMany; ++i)
			{
				ClearMemoryOne(from);
				from = reinterpret_cast<void*>(reinterpret_cast<T*>(from) + 1);
			}
		}

		template<class T>
		inline void MCollection<T>::ClearMemory(void * from, MSize howMany, T & obj)
		{
			//ZeroMemory(from, howMany * sizeof(T));
			for (MSize i = 0; i < howMany; ++i)
			{
				ClearMemoryOne(from, obj);
				from = reinterpret_cast<void*>(reinterpret_cast<T*>(from) + 1);
			}
		}

		template<class T>
		void MCollection<T>::Reallocate(MSize newSize)
		{
			_capacity = newSize;
			MSize oldSize = _size;

			T* ptr = reinterpret_cast<T*>(MCollectionHelper::Allocate(_capacity * sizeof(T), _alignof(T)));
			ZeroMemory(ptr, newSize);

			MemoryCopy(ptr, reinterpret_cast<T*>(_data), oldSize);

			ClearMemory(_data, oldSize);
			MCollectionHelper::Deallocate(_data);
			_data = ptr;
		}

		template<class T>
		inline bool MCollection<T>::CheckIfGrow()
		{
			return (_size >= _capacity);
		}

		template<class T>
		inline bool MCollection<T>::CheckIfShrink()
		{
			switch (_shrinkStrategy)
			{
			case ShrinkStrategy::SHRINK_HALF_AFTER_HALF_EMPTY:

				if (_size < GetCapacity() / 2)
				{
					return true;
				}
				else return false;

			case ShrinkStrategy::SHRINK_HALF_AFTER_THREE_QUARTERS_EMPTY:

			if (_size < GetCapacity() / 4)
			{
				return true;
			}
			else return false;

				break;
			case ShrinkStrategy::NO_SHRINK:
				return false;
			default:
				return false;
			}
		}

		template<class T>
		void MCollection<T>::Shrink()
		{
			if (_shrinkStrategy == ShrinkStrategy::SHRINK_HALF_AFTER_HALF_EMPTY ||
				_shrinkStrategy == ShrinkStrategy::SHRINK_HALF_AFTER_THREE_QUARTERS_EMPTY)
			{
				Reallocate((GetCapacity()) / 2);
			}
			else return;
		}

		template<class T>
		void MCollection<T>::Grow()
		{
			switch (_growthStrategy)
			{
			case GrowthStrategy::GROW_MUL2_SIZE:
				Reallocate(2 * GetCapacity());
				break;
			case GrowthStrategy::GROW_HALF_INITIAL_SIZE:
				Reallocate(GetCapacity() + GetCapacity() / 2);
				break;
			}
		}

		template<class T>
		inline void MCollection<T>::Quicksort(MInt64 left, MInt64 right, CompareFunctionPointer compare, void* param)
		{
			MInt64 i = left, j = right;
			T* dataPtr = reinterpret_cast<T*>(_data);
			MUint8 tmpBuf[sizeof(T)];
			T* tmp = reinterpret_cast<T*>(tmpBuf);
			T* pivot = dataPtr + ((left + right) / 2);

			// partition
			while (i <= j)
			{
				while (compare(dataPtr + i, pivot, param) < 0 && i <= right)
				{
					++i;
				}
				while (compare(dataPtr + j, pivot, param) > 0 && j > left)
				{
					--j;
				}
				if (i <= j)
				{
					AssignToMemory(*(dataPtr + i), tmp);
					AssignToMemory(*(dataPtr + j), dataPtr + i);
					AssignToMemory(*tmp, dataPtr + j);
					++i;
					--j;
				}
			}

			// recursion
			if (left < j)
			{
				Quicksort(left, j, compare, param);
			}
			if (i < right)
			{
				Quicksort(i, right, compare, param);
			}
		}

		template<class T>
		inline void MCollection<T>::ShiftOne(MSize from, MSize size, bool right)
		{
			T* d = reinterpret_cast<T*>(_data);
			MInt64 dir = ((MInt64)right) * 2 - 1;
			MoveMemoryCopy(&d[from + dir], &d[from], (size - from));
			if (right)
			{
				ClearMemoryOne(&d[from]);
			}
			else
			{
				ClearMemoryOne(&d[size - 1]);
			}
		}

		template<class T>
		inline void MCollection<T>::Shift(MSize from, MSize size, MInt64 i)
		{
			T* d = reinterpret_cast<T*>(_data);
			T* startPtr = &d[from];
			T* dstPtr = &d[from + i];

			MoveMemoryCopy(dstPtr, startPtr, size - from);
			if (startPtr < dstPtr)
			{
				ClearMemory(startPtr, (i < 0 ? -i : i));
			}
			else
			{
				ClearMemory(d + size - (i < 0 ? -i : i), (i < 0 ? -i : i));
			}
		}

		template<class T>
		inline bool MCollection<T>::ValidateIndex(const MSize index) const
		{
			return index < _size;
		}

#pragma endregion

#pragma region CompareFunctionHelpers

		template <typename T> T& InitializeDefault(T* mem)
		{
			return *(::new(mem) T());
		}

		template <typename T> MInt8 CompareAscending(T* a, T* b, void* param)
		{
			if (*a > *b)
				return 1;
			else if (*a < *b)
				return -1;
			else return 0;
		}

		template <typename T> MInt8 CompareDescending(T* a, T* b, void* param)
		{
			if (*a > *b)
				return -1;
			else if (*a < *b)
				return 1;
			else return 0;
		}

#pragma endregion

	}
}
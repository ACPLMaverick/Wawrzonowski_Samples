#pragma once

#include "MCollection.h"

namespace morphEngine
{
	namespace utility
	{
		template <class T>
		class MArray : public MCollection<T>
		{
			//[vansten]
			//Doing this to give access to _dataPtr for MString
			//Needed for const char* operator overloading but didn't want to allocate and deallocate memory
			friend class MString;

		public:

#pragma region Classes Public

			class MIteratorArray : public MIteratorBase
			{
				friend class MArray<T>;
			protected:
				T* _dataPtr;
				MArray<T>& _array;

				MIteratorArray(MArray<T>& ref, T* data) :
					MIteratorBase(),
					_dataPtr(data),
					_array(ref)
				{

				}

			public:

				MIteratorArray(const MIteratorArray& c) :
					MIteratorBase(c),
					_dataPtr(c._dataPtr),
					_array(c._array)
				{

				}

				virtual ~MIteratorArray()
				{
				}

				// Reminder: Iterator of empty array is invalid.
				virtual bool IsValid() const override
				{
					MSize diff = _dataPtr - reinterpret_cast<T*>(_array._data);
					return diff < _array._size;
				}

				virtual bool operator==(const MIteratorArray& other) const
				{
					return _dataPtr == other._dataPtr;
				}

				virtual bool operator>=(const MIteratorArray& other) const
				{
					return _dataPtr >= other._dataPtr;
				}

				virtual bool operator<=(const MIteratorArray& other) const
				{
					return _dataPtr <= other._dataPtr;
				}

				virtual bool operator>(const MIteratorArray& other) const 
				{
					return _dataPtr > other._dataPtr;
				}

				virtual bool operator<(const MIteratorArray& other) const
				{
					return _dataPtr < other._dataPtr;
				}

				virtual const T& operator * () const
				{
					ME_ASSERT(IsValid(), "MArray: Used dereference operator on invalid iterator.");
					return *_dataPtr;
				}

				virtual T& operator * ()
				{
					ME_ASSERT(IsValid(), "MArray: Used dereference operator on invalid iterator.");
					return *_dataPtr;
				}

				virtual MIteratorArray operator+ (const MSize& rhs) const
				{
					MIteratorArray result(*this);
					result._dataPtr += rhs;
					return result;
				}

				virtual MIteratorArray operator- (const MSize& rhs) const
				{
					MIteratorArray result(*this);
					result._dataPtr -= rhs;
					return result;
				}

				virtual MIteratorArray& operator+= (const MSize& rhs)
				{
					_dataPtr += rhs;
					return *this;
				}

				virtual MIteratorArray& operator-= (const MSize& rhs)
				{
					_dataPtr -= rhs;
					return *this;
				}

				MIteratorArray& operator++()
				{
					++_dataPtr;
					return *this;
				}

				MIteratorArray& operator--()
				{
					--_dataPtr;
					return *this;
				}

				MIteratorArray operator++(MInt32)
				{
					MIteratorArray result(*this);
					++(*this);
					return result;
				}

				MIteratorArray operator--(MInt32)
				{
					MIteratorArray result(*this);
					--(*this);
					return result;
				}
			};

#pragma endregion

		protected:

#pragma region Static Const Protected

			static const MSize DEFAULT_SIZE = 16;

#pragma endregion

#pragma region Protected

#pragma endregion

#pragma region FunctionsProtected



#pragma endregion

		public:

#pragma region Functions Public

			MArray(MSize beginCapacity = MArray<T>::DEFAULT_SIZE,
				GrowthStrategy growthStrategy = GrowthStrategy::GROW_MUL2_SIZE,
				ShrinkStrategy shrinkStrategy = ShrinkStrategy::NO_SHRINK);
			MArray(const MArray<T>& copy);
			virtual ~MArray();

			virtual MIteratorArray GetIterator();
			virtual MIteratorArray GetBegin();
			virtual MIteratorArray GetEnd();

			virtual bool IsEmpty() const override;
			inline virtual T* GetDataPointer() const { return (T*)_data; }
			virtual MSize GetSize() const override;
			virtual MSize GetCapacity() const override;

			virtual bool Contains(const T& obj) const override;

			// Adds object, its creation (i.e. kind of ctor called) is specified by initFunc. Default one calls non-parametrized ctor.
			virtual T& AddCreate(T& (*initFunc)(T*) = InitializeDefault<T>);
			virtual void Add(const T& obj);
			virtual void Add(const T& obj, MSize position);
			virtual void Remove(const T& obj);
			virtual void RemoveAt(MSize position);
			virtual MSize IndexOf(const T& obj);
			virtual void Clear() override;
			void Resize(MSize newSize); 
			void ResizeWithReinit(MSize newSize);

			virtual T operator[](const MSize index) const;
			virtual T& operator[](const MSize index);

			virtual MArray<T>& operator=(const MArray<T>& c);
			
			virtual bool operator==(const MArray<T>& rhs) const;
			virtual bool operator!=(const MArray<T>& rhs) const;

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
		MArray<T>::MArray(MSize beginCapacity, GrowthStrategy growthStrategy, ShrinkStrategy shrinkStrategy) :
			MCollection<T>(beginCapacity, growthStrategy, shrinkStrategy)
		{
			_type = CollectionType::ARRAY;
			//_growthStrategy = growthStrategy;
			//_shrinkStrategy = shrinkStrategy;

			if (_capacity != 0)
			{
				_data = MCollectionHelper::Allocate(_capacity * sizeof(T), _alignof(T));

				T* d = reinterpret_cast<T*>(_data);
				ZeroMemory(d, _capacity * sizeof(T));
			}
		}

		template<class T>
		MArray<T>::MArray(const MArray<T>& copy) :
			MArray<T>::MArray(copy._capacity, copy._growthStrategy, copy._shrinkStrategy)
		{
			_size = copy._size;
			MemoryCopy(reinterpret_cast<T*>(_data), reinterpret_cast<T*>(copy._data), _size);
		}

		template <class T> MArray<T>::~MArray()
		{
		}

		template<class T>
		typename MArray<T>::MIteratorArray MArray<T>::GetIterator()
		{
			return MIteratorArray(*this, reinterpret_cast<T*>(_data));
		}

		template<class T>
		typename MArray<T>::MIteratorArray MArray<T>::GetBegin()
		{
			return MIteratorArray(*this, reinterpret_cast<T*>(_data));
		}

		template<class T>
		typename MArray<T>::MIteratorArray MArray<T>::GetEnd()
		{
			MSize size = static_cast<MSize>(max(static_cast<MInt64>(_size) - 1, 0));
			return MArray<T>::MIteratorArray(*this, reinterpret_cast<T*>(_data) + size);
		}

		template<class T>
		inline bool MArray<T>::IsEmpty() const
		{
			return _size == 0;
		}

		template<class T>
		MSize MArray<T>::GetSize() const 
		{
			return _size;
		}

		template<class T>
		MSize MArray<T>::GetCapacity() const
		{
			return _capacity;
		}

		template<class T>
		inline bool MArray<T>::Contains(const T& obj) const
		{
			T* ptr = reinterpret_cast<T*>(_data);
			for (MSize i = 0; i < _size; ++i)
			{
				if (ptr[i] == obj)
				{
					return true;
				}
			}
			return false;
		}

		template<class T>
		inline T & MArray<T>::AddCreate(T& (*initFunc)(T*))
		{
			if (_size == _capacity)
			{
				Grow();
			}

			T* d = reinterpret_cast<T*>(_data) + _size;
			++_size;
			return initFunc(d);
		}

		template<class T>
		void MArray<T>::Add(const T& obj)
		{
			if (_size == _capacity)
			{
				Grow();
			}

			T* d = reinterpret_cast<T*>(_data) + _size;
			AssignToMemory(obj, d);
			++_size;
		}

		template<class T>
		void MArray<T>::Add(const T& obj, MSize position)
		{
			if (CheckIfGrow())
			{
				Grow();
			}

			ShiftOne(position, _size, true);
			T* d = reinterpret_cast<T*>(_data) + _size;
			AssignToMemory(obj, d);
			++_size;
		}

		template<class T>
		void MArray<T>::Remove(const T& obj)
		{
			for (MSize i = 0; i < _size; ++i)
			{
				if (obj == reinterpret_cast<T*>(_data)[i])
				{
					RemoveAt(i);
				}
			}
		}

		template<class T>
		void MArray<T>::RemoveAt(MSize position)
		{
			if (position < _size - 1)
			{
				ShiftOne(position + 1, _size, false);
			}
			else
			{
				T* d = reinterpret_cast<T*>(_data);
				ClearMemoryOne(&d[position]);
			}

			if (CheckIfShrink())
			{
				Shrink();
			}
			--_size;
		}

		template<class T>
		inline MSize MArray<T>::IndexOf(const T & obj)
		{
			T* ptr = reinterpret_cast<T*>(_data);
			for (MSize i = 0; i < _size; ++i)
			{
				if (ptr[i] == obj)
				{
					return i;
				}
			}
			return UINT64_MAX;
		}

		template<class T>
		void MArray<T>::Clear()
		{
			MCollection<T>::Clear();
		}

		template<class T>
		void MArray<T>::Resize(MSize newSize)
		{
			//ME_ASSERT_S(newSize != 0);

			if (newSize < _size && _shrinkStrategy != ShrinkStrategy::NO_SHRINK)
			{
				Reallocate(newSize);
			}
			else if (newSize > _size)
			{
				if (newSize > _capacity)
				{
					Reallocate(newSize);
				}
			}

			_size = newSize;
		}

		template<class T>
		inline void MArray<T>::ResizeWithReinit(MSize newSize)
		{
			MSize oldSize = _size;

			Resize(newSize);

			for (MSize i = oldSize; i < _size; ++i)
			{
				::new(reinterpret_cast<T*>(_data) + i) T();
			}
		}

		template<class T>
		T MArray<T>::operator[](const MSize index) const
		{
#ifdef _DEBUG
			if (ValidateIndex(index))
			{
#endif
				return reinterpret_cast<T*>(_data)[index];
#ifdef _DEBUG
			}
			else
			{
				return reinterpret_cast<T*>(_data)[0];
			}
#endif // _DEBUG
		}

		template<class T>
		inline T & MArray<T>::operator[](const MSize index)
		{
#ifdef _DEBUG
			if (ValidateIndex(index))
			{
#endif
				return reinterpret_cast<T*>(_data)[index];
#ifdef _DEBUG
			}
			else
			{
				return reinterpret_cast<T*>(_data)[0];
			}
#endif // _DEBUG
		}

		template<class T>
		MArray<T>& MArray<T>::operator=(const MArray<T>& c)
		{
			MCollection<T>::operator=(c);
			return *this;
		}

		template<class T>
		inline bool MArray<T>::operator==(const MArray<T>& rhs) const
		{
			if (GetSize() != rhs.GetSize())
			{
				return false;
			}
			else
			{
				for (MSize i = 0; i < _size; ++i)
				{
					if (reinterpret_cast<T*>(_data)[i] != reinterpret_cast<T*>(rhs._data)[i])
					{
						return false;
					}
				}
				return true;
			}
		}

		template<class T>
		inline bool MArray<T>::operator!=(const MArray<T>& rhs) const
		{
			return false;
		}

	}
}

#pragma endregion

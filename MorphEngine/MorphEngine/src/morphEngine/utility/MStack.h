#pragma once

#include "utility/MCollection.h"

namespace morphEngine
{
	namespace utility
	{
		template <class T>
		class MStack :
			public MCollection<T>
		{
		protected:

#pragma region Static Const Protected

			static const MUint64 DEFAULT_MAX_SIZE = UINT64_MAX;
			static const MSize DEFAULT_SIZE = 16;

#pragma endregion

#pragma region Protected

			MUint64 _maxSize;
			T* _afterTop = nullptr;

#pragma endregion

#pragma region Functions Protected

			virtual void Reallocate(MSize newSize) override;

#pragma endregion

		public:

#pragma region Classes Public

			class MIteratorStack : public MIteratorBase
			{
				friend class MStack<T>;
			protected:
				T* _dataPtr;
				MStack<T> _stackRef;

				MIteratorStack(MStack<T>& stack, T* data) :
					MIteratorBase(),
					_dataPtr(data),
					_stackRef(stack)
				{

				}

				virtual bool IsValid() const override
				{
					MSize diff = _dataPtr - reinterpret_cast<T*>(_stackRef._data);
					return diff < _stackRef._size;
				}

			public:
				MIteratorStack(const MIteratorStack& c) :
					MIteratorBase(c),
					_dataPtr(c._dataPtr),
					_stackRef(c._stackRef)
				{

				}

				virtual ~MIteratorStack()
				{
				}

				virtual bool operator==(const MIteratorStack& other) const 
				{
					return _dataPtr == other._dataPtr;
				}

				virtual bool operator>=(const MIteratorStack& other) const
				{
					return _dataPtr >= other._dataPtr;
				}

				virtual bool operator<=(const MIteratorStack& other) const
				{
					return _dataPtr <= other._dataPtr;
				}

				virtual bool operator>(const MIteratorStack& other) const
				{
					return _dataPtr > other._dataPtr;
				}

				virtual bool operator<(const MIteratorStack& other) const
				{
					return _dataPtr < other._dataPtr;
				}

				virtual const T& operator * () const
				{
					ME_ASSERT(IsValid(), "MStack: Used dereference operator on invalid iterator.");
					return *_dataPtr;
				}

				virtual T& operator * ()
				{
					ME_ASSERT(IsValid(), "MStack: Used dereference operator on invalid iterator.");
					return *_dataPtr;
				}

				virtual MIteratorStack& operator+= (const MSize& rhs)
				{
					_dataPtr += rhs;
					return *this;
				}

				virtual MIteratorStack& operator-= (const MSize& rhs)
				{
					_dataPtr -= rhs;
					return *this;
				}

				virtual MIteratorStack operator+ (const MSize& rhs) const
				{
					MIteratorStack result(*this);
					result += rhs;
					return result;
				}

				virtual MIteratorStack operator- (const MSize& rhs) const
				{
					MIteratorStack result(*this);
					result -= rhs;
					return result;
				}

				MIteratorStack& operator++()
				{
					++_dataPtr;
					return *this;
				}

				MIteratorStack& operator--()
				{
					--_dataPtr;
					return *this;
				}

				MIteratorStack operator++(MInt32)
				{
					MIteratorStack result(*this);
					++(*this);
					return result;
				}

				MIteratorStack operator--(MInt32)
				{
					MIteratorStack result(*this);
					--(*this);
					return result;
				}
			};

#pragma endregion

#pragma region Functions Public

			MStack(MSize beginCapacity = MStack<T>::DEFAULT_SIZE, MSize maxCapacity = MStack<T>::DEFAULT_MAX_SIZE,
				GrowthStrategy growthStrategy = GrowthStrategy::GROW_MUL2_SIZE,
				ShrinkStrategy shrinkStrategy = ShrinkStrategy::NO_SHRINK);
			MStack(const MStack& c);
			virtual ~MStack();

			virtual void Push(T obj);
			virtual T Pop();
			virtual T Peek();
			virtual void Clear() override;

			virtual bool IsEmpty() const override;
			virtual MSize GetSize() const override;
			virtual MSize GetCapacity() const override;
			
			virtual bool Contains(const T& obj) const override;

			virtual MIteratorStack GetIterator();
			virtual MIteratorStack GetFirst();
			virtual MIteratorStack GetTop();

			virtual T operator[](const MSize index) const;
			virtual T& operator[](const MSize index);
			virtual MStack<T>& operator=(const MStack<T>& c);

#pragma endregion

		};

#pragma region Definitions

		template <class T> MStack<T>::MStack(MSize beginCapacity, MSize maxCapacity, 
			GrowthStrategy gr, ShrinkStrategy sh) :
			MCollection<T>(beginCapacity, gr, sh),
			_maxSize(maxCapacity)	
		{
			_type = CollectionType::STACK;
			_data = MCollectionHelper::Allocate(_capacity * sizeof(T), _alignof(T));

			ZeroMemory(_data, _capacity);

			_afterTop = reinterpret_cast<T*>(_data);
		}

		template<class T>
		MStack<T>::MStack(const MStack & c) :
			MStack(c._capacity, c._maxSize, c._growthStrategy, c._shrinkStrategy)
		{
			_size = c._size;
			MSize g = c._afterTop - reinterpret_cast<T*>(c._data);
			_afterTop = reinterpret_cast<T*>(_data) + g;
			MemoryCopy(reinterpret_cast<T*>(_data), reinterpret_cast<T*>(c._data), g);
		}


		template <class T> MStack<T>::~MStack()
		{
			Shutdown();
		}

		template<class T>
		void MStack<T>::Push(T obj)
		{
			MSize s = GetSize();
			if (s >= _capacity)
			{
				// checking max size here, taking into account default hard coded growth method
				if (2 * _capacity > _maxSize)
				{
					return;
				}
				Grow();
			}

			AssignToMemory(obj, _afterTop);
			++_afterTop;
			++_size;
		}

		template<class T>
		T MStack<T>::Pop()
		{
			// checking for empty stack is suppressed, user must take care of it himself
			// it is done so not to double ifs unnecessarily

			//if (!IsEmpty())
			//{
				T data = *(_afterTop - 1);

				if ((GetSize() + 1) < _capacity / 4)
				{
					Shrink();
				}

				ClearMemoryOne(_afterTop - 1);
				--_afterTop;
				--_size;

				return data;
			//}
			//else
			//{
			//	ME_ASSERT(false, "MStack: called Pop on empty stack");
			//	T copy;
			//	return copy;
			//}
		}

		template<class T>
		inline T MStack<T>::Peek()
		{
			return *(_afterTop - 1);
		}

		template<class T>
		void MStack<T>::Clear()
		{
			MCollection<T>::Clear();
			_afterTop = reinterpret_cast<T*>(_data);
		}

		template<class T>
		inline bool MStack<T>::IsEmpty() const
		{
			return GetSize() == 0;
		}

		template<class T>
		inline MSize MStack<T>::GetSize() const
		{
			return _afterTop - reinterpret_cast<T*>(_data);
		}

		template<class T>
		inline MSize MStack<T>::GetCapacity() const
		{
			return _capacity;
		}

		template<class T>
		inline bool MStack<T>::Contains(const T & obj) const
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
		typename MStack<T>::MIteratorStack MStack<T>::GetIterator()
		{
			T* d = reinterpret_cast<T*>(_data);
			return MIteratorStack(*this, (_afterTop - 1) < d ? d : (_afterTop - 1));
		}

		template<class T>
		typename MStack<T>::MIteratorStack MStack<T>::GetFirst()
		{
			return MIteratorStack(*this, reinterpret_cast<T*>(_data));
		}

		template<class T>
		typename MStack<T>::MIteratorStack MStack<T>::GetTop()
		{
			T* d = reinterpret_cast<T*>(_data);
			return MIteratorStack(*this, (_afterTop - 1) < d ? d : (_afterTop - 1));
		}

		template<class T>
		T MStack<T>::operator[](const MSize index) const
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
		inline T & MStack<T>::operator[](const MSize index)
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
		inline MStack<T>& MStack<T>::operator=(const MStack<T>& c)
		{
			MCollection<T>::operator=(c);
			MSize g = c._afterTop - reinterpret_cast<T*>(c._data);
			_afterTop = reinterpret_cast<T*>(_data) + g;
			return *this;
		}

		template<class T>
		void MStack<T>::Reallocate(MSize newSize)
		{
			MSize siz = GetSize();
			MCollection<T>::Reallocate(newSize);
			_afterTop = reinterpret_cast<T*>(_data) + siz;
		}

#pragma endregion

	}
}
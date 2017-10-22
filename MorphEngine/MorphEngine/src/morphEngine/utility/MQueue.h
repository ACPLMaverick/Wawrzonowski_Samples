#pragma once

#include "utility/MCollection.h"

namespace morphEngine
{
	namespace utility
	{
		template <class T>
		class MQueue :
			public MCollection<T>
		{
		protected:

#pragma region Static Const Protected

			static const MUint64 DEFAULT_MAX_SIZE = UINT64_MAX;
			static const MSize DEFAULT_SIZE = 16;

#pragma endregion

#pragma region Protected

			MUint64 _maxSize;
			T* _front;
			T* _rear;

#pragma endregion

#pragma region Functions Protected

			virtual MSize GetSizeWithFrontGap() const;
			virtual MSize GetFrontGap() const;

			virtual void ShiftBack();
			virtual void Reallocate(MSize newSize);

#pragma endregion

		public:

#pragma region Classes Public

			class MIteratorQueue : public MIteratorBase
			{
				friend class MQueue<T>;
			protected:
				T* _dataPtr;
				MQueue<T>& _array;

				MIteratorQueue(T* data, MQueue<T>& ref) :
					MIteratorBase(),
					_dataPtr(data),
					_array(ref)
				{

				}

				virtual bool IsValid() const override
				{
					MSize diff = _dataPtr - reinterpret_cast<T*>(_array._data);
					return diff < _array._size;
				}

			public:

				MIteratorQueue(const MIteratorQueue& c) :
					MIteratorBase(c),
					_dataPtr(c._dataPtr),
					_array(c._array)
				{

				}

				virtual ~MIteratorQueue()
				{
				}

				virtual bool operator==(const MIteratorQueue& other) const
				{
					return _dataPtr == other._dataPtr;
				}

				virtual bool operator>=(const MIteratorQueue& other) const
				{
					return _dataPtr >= other._dataPtr;
				}

				virtual bool operator<=(const MIteratorQueue& other) const
				{
					return _dataPtr <= other._dataPtr;
				}

				virtual bool operator>(const MIteratorQueue& other) const
				{
					return _dataPtr > other._dataPtr;
				}

				virtual bool operator<(const MIteratorQueue& other) const
				{
					return _dataPtr < other._dataPtr;
				}

				virtual const T& operator * () const
				{
					ME_ASSERT(IsValid(), "MQueue: Used dereference operator on invalid iterator.");
					return *_dataPtr;
				}

				virtual T& operator * ()
				{
					ME_ASSERT(IsValid(), "MQueue: Used dereference operator on invalid iterator.");
					return *_dataPtr;
				}

				virtual MIteratorQueue operator+ (const MSize& rhs) const
				{
					MIteratorQueue result(*this);
					result._dataPtr += rhs;
					return result;
				}

				virtual MIteratorQueue operator- (const MSize& rhs) const
				{
					MIteratorQueue result(*this);
					result._dataPtr -= rhs;
					return result;
				}

				virtual MIteratorQueue& operator+= (const MSize& rhs)
				{
					_dataPtr += rhs;
					return *this;
				}

				virtual MIteratorQueue& operator-= (const MSize& rhs)
				{
					_dataPtr -= rhs;
					return *this;
				}

				MIteratorQueue& operator++()
				{
					++_dataPtr;
					return *this;
				}

				MIteratorQueue& operator--()
				{
					--_dataPtr;
					return *this;
				}

				MIteratorQueue operator++(MInt32)
				{
					MIteratorQueue result(*this);
					++(*this);
					return result;
				}

				MIteratorQueue operator--(MInt32)
				{
					MIteratorQueue result(*this);
					--(*this);
					return result;
				}
			};

#pragma endregion

#pragma region Functions Public

			MQueue(MSize beginCapacity = MQueue<T>::DEFAULT_SIZE, MSize maxCapacity = MQueue<T>::DEFAULT_MAX_SIZE,
				GrowthStrategy growthStrategy = GrowthStrategy::GROW_MUL2_SIZE,
				ShrinkStrategy shrinkStrategy = ShrinkStrategy::NO_SHRINK);
			MQueue(const MQueue& c);
			virtual ~MQueue();

			virtual void Enqueue(const T& obj);
			virtual T Dequeue();
			virtual T Peek();
			virtual void Clear();

			virtual bool IsEmpty() const override;
			virtual MSize GetSize() const override;
			virtual MSize GetCapacity() const override;

			virtual bool Contains(const T& obj) const override;

			virtual MIteratorQueue GetIterator();
			virtual MIteratorQueue GetFront();
			virtual MIteratorQueue GetRear();

			virtual MQueue& operator=(const MQueue& c);

#pragma endregion

		};

#pragma region Definitions

		template <class T> MQueue<T>::MQueue(MSize beginCapacity, MSize maxCapacity,
			GrowthStrategy gr, ShrinkStrategy sh) :
			MCollection<T>(beginCapacity, gr, sh),
			_maxSize(maxCapacity)
		{
			_type = CollectionType::QUEUE;
			_data = MCollectionHelper::Allocate(_capacity * sizeof(T), _alignof(T));

			ZeroMemory(_data, _capacity);

			_front = _rear = reinterpret_cast<T*>(_data);
		}

		template<class T>
		MQueue<T>::MQueue(const MQueue & c) :
			MQueue(c._capacity, c._maxSize, c._growthStrategy, c._shrinkStrategy)
		{
			_size = c._size;
			_front = reinterpret_cast<T*>(_data) + c.GetFrontGap();
			_rear = _front + c.GetSize();
			MemoryCopy(_front, c._front, _rear - _front);
		}


		template <class T> MQueue<T>::~MQueue()
		{
			Shutdown();
		}

		template<class T>
		void MQueue<T>::Enqueue(const T& obj)
		{
			if (GetSizeWithFrontGap() >= _capacity)
			{
				if (_front != reinterpret_cast<T*>(_data))
				{
					ShiftBack();
				}
				else
				{
					Grow();
				}
			}
			AssignToMemory(obj, _rear);
			++_size;
			++_rear;
		}

		template<class T>
		T MQueue<T>::Dequeue()
		{
			// checking for empty queue is suppressed, user must take care of it himself
			// it is done so not to double ifs unnecessarily

			if (GetSizeWithFrontGap() < _capacity / 4)
			{
				if (GetFrontGap() >= _capacity / 2)
				{
					ShiftBack();
				}
				Shrink();
			}

			T data = *_front;
			ClearMemoryOne(_front);
			--_size;
			if (_size > _capacity)	// taki hak
				_size = 0;
			++_front;
			return data;
		}

		template<class T>
		T MQueue<T>::Peek()
		{
			return *_front;
		}

		template<class T>
		void MQueue<T>::Clear()
		{
			if (_data != nullptr)
			{
				T* d = reinterpret_cast<T*>(_front);
				ClearMemory(d, _size);
				_size = 0;
				_front = _rear = reinterpret_cast<T*>(_data);
			}
		}

		template<class T>
		bool MQueue<T>::IsEmpty() const
		{
			return GetSize() == 0;
		}

		template<class T>
		MSize MQueue<T>::GetSize() const
		{
			return _rear - _front;
		}

		template<class T>
		MSize MQueue<T>::GetCapacity() const
		{
			return _capacity;
		}

		template<class T>
		inline bool MQueue<T>::Contains(const T & obj) const
		{
			T* ptr = reinterpret_cast<T*>(_front);
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
		typename MQueue<T>::MIteratorQueue MQueue<T>::GetIterator()
		{
			return MIteratorQueue(_front, *this);
		}

		template<class T>
		typename MQueue<T>::MIteratorQueue MQueue<T>::GetFront()
		{
			return MIteratorQueue(_front, *this);
		}

		template<class T>
		typename MQueue<T>::MIteratorQueue MQueue<T>::GetRear()
		{
			T* d = reinterpret_cast<T*>(_data);
			return MIteratorQueue((_rear - 1) < d ? d : (_rear - 1), *this);
		}

		template<class T>
		inline MQueue<T>& MQueue<T>::operator=(const MQueue & c)
		{
			MCollection<T>::operator=(c);
			_front = reinterpret_cast<T*>(_data) + c.GetFrontGap();
			_rear = _front + c.GetSize();
			return *this;
		}

		template<class T>
		MSize MQueue<T>::GetSizeWithFrontGap() const
		{
			return _rear - reinterpret_cast<T*>(_data);
		}

		template<class T>
		MSize MQueue<T>::GetFrontGap() const
		{
			return _front - reinterpret_cast<T*>(_data);
		}

		template<class T>
		void MQueue<T>::ShiftBack()
		{
			MSize siz = GetSize();
			MSize gap = GetFrontGap();
			MoveMemoryCopy(reinterpret_cast<T*>(_data), _front, gap);
			_front = reinterpret_cast<T*>(_data);
			_rear = _front + siz;
		}

		template<class T>
		void MQueue<T>::Reallocate(MSize newSize)
		{
			_capacity = newSize;

			T* ptr = reinterpret_cast<T*>(MCollectionHelper::Allocate(_capacity * sizeof(T), _alignof(T)));
			ZeroMemory(ptr, newSize);

			MSize posFront = GetFrontGap();
			MSize posRear = GetSizeWithFrontGap();

			MSize siz = GetSize();
			// also, we automatically shift to start here
			MemoryCopy(ptr, _front, siz);

			MCollectionHelper::Deallocate(_data);
			_data = ptr;
			_front = ptr + posFront;
			_rear = ptr + posRear;
		}

#pragma endregion

}
}
#pragma once

#include "utility/MQueue.h"
#include "utility/MBinaryTree.h"

namespace morphEngine
{
	namespace utility
	{
		template <class T>
		class MPriorityQueue :
			public MQueue<T>,
			public MBinaryTree<T>
		{
		protected:

#pragma region Protected


#pragma endregion

#pragma region Functions Protected

			///
			///<summary>
			/// Returns higher valued item, which goes up the queue
			///</summary>
			inline virtual T* Compare(T* a1, T* a2);

			virtual void Reallocate(MSize newSize) override;

#pragma endregion

		public:

#pragma region Classes Public

			class MIteratorPriorityQueue : public MQueue<T>::MIteratorQueue
			{
				friend class MPriorityQueue<T>;
			protected:

				MIteratorPriorityQueue(T* data, MQueue<T>& ref) :
					MIteratorQueue(data, ref)
				{

				}

			public:

				MIteratorPriorityQueue(const MIteratorPriorityQueue& c) :
					MIteratorQueue(c)
				{

				}

				virtual ~MIteratorPriorityQueue()
				{
				}

			};

#pragma endregion

#pragma region Functions Public

			MPriorityQueue(MSize beginCapacity = MQueue<T>::DEFAULT_SIZE, 
				MSize maxCapacity = MQueue<T>::DEFAULT_MAX_SIZE,
				GrowthStrategy growthStrategy = GrowthStrategy::GROW_MUL2_SIZE,
				ShrinkStrategy shrinkStrategy = ShrinkStrategy::NO_SHRINK);
			MPriorityQueue(const MPriorityQueue& c);
			virtual ~MPriorityQueue();

			virtual void Enqueue(T obj);
			virtual T Dequeue();

			virtual MIteratorQueue GetIterator() override;
			virtual MIteratorQueue GetFront() override;
			virtual MIteratorQueue GetRear() override;

#pragma endregion
		};

#pragma region Definitions


		template <class T> MPriorityQueue<T>::MPriorityQueue(MSize beginCapacity,
			MSize maxCapacity, GrowthStrategy gr, ShrinkStrategy sh) :
			MQueue<T>(beginCapacity, maxCapacity, gr, sh)
		{
			_type = CollectionType::PRIORITY_QUEUE;
			_rear = _front = (reinterpret_cast<T*>(_data) + 1);
		}

		template<class T>
		MPriorityQueue<T>::MPriorityQueue(const MPriorityQueue & c) :
			MPriorityQueue<T>(c._capacity, c._maxCapacity, c._growthStrategy, c._shrinkStrategy),
			MQueue<T>(c)
		{
			_size = c._size;
			_rear = _front = (reinterpret_cast<T*>(_data) + 1);
		}


		template <class T> MPriorityQueue<T>::~MPriorityQueue()
		{
			
		}

		template<class T>
		void MPriorityQueue<T>::Enqueue(T obj)
		{
			if (GetSizeWithFrontGap() >= _capacity)
			{
				if (_front != (reinterpret_cast<T*>(_data) + 1))
				{
					ShiftBack();
				}
				else
				{
					Grow();
				}
			}

			T* ptr = _rear;
			// assuming it's a zero memory block and initializing it.
			// any other node is not a memory block
			AssignToMemory(obj, ptr);
			T* parent;
			while ((parent = GetParent(ptr, reinterpret_cast<T*>(_data), GetSize())) != reinterpret_cast<T*>(_data) && parent != nullptr)
			{
				if (Compare(ptr, parent) != ptr)
				{
					// parent is more valued than ptr, stop
					break;
				}
				else
				{
					// parent is less valued so we swap places with him
					Swap(ptr, parent);
					ptr = parent;
				}
			}

			// properly setting rear ptr
			++_rear;
			++_size;
		}

		template<class T>
		T MPriorityQueue<T>::Dequeue()
		{
			if (GetSizeWithFrontGap() < _capacity / 4)
			{
				if (GetFrontGap() >= _capacity / 2)
				{
					ShiftBack();
				}
				Shrink();
			}

			// move last element to the top and move it down
			// until heap condition satisfied
			T data = *_front;
			*_front = *(_rear - 1);
			--_rear;
			ClearMemoryOne(_rear);
			MSize size = GetSizeWithFrontGap();

			T* ptr = _front;
			while (ptr <= reinterpret_cast<T*>(_data) + size)
			{
				T* l = GetLeftChild(ptr, reinterpret_cast<T*>(_data), GetSize());
				T* r = GetRightChild(ptr, reinterpret_cast<T*>(_data), GetSize());
				// left child is more valued so should be higher than us
				if (l != reinterpret_cast<T*>(_data) && 
					l < reinterpret_cast<T*>(_data) + size &&
					Compare(ptr, l) == l)
				{
					Swap(ptr, l);
					ptr = l;
				}
				else if (r != reinterpret_cast<T*>(_data) && 
					r < reinterpret_cast<T*>(_data) + size &&
					Compare(ptr, r) == r)
				{
					Swap(ptr, r);
					ptr = r;
				}
				else
				{
					// we re at the end or every child is less valued
					break;
				}
			}
			--_size;
			
			return data;
		}

		template<class T>
		typename MQueue<T>::MIteratorQueue MPriorityQueue<T>::GetIterator()
		{
			return GetFront();
		}

		template<class T>
		typename MQueue<T>::MIteratorQueue MPriorityQueue<T>::GetFront()
		{
			return MIteratorPriorityQueue(_front, *this);
		}

		template<class T>
		typename MQueue<T>::MIteratorQueue MPriorityQueue<T>::GetRear()
		{
			return MIteratorPriorityQueue(GetParent(_rear, reinterpret_cast<T*>(_data), GetSize()), *this);
		}

		template<class T>
		inline T* MPriorityQueue<T>::Compare(T* a1, T* a2)
		{
			if (a1 == nullptr && a2 != nullptr)
			{
				return a2;
			}
			else if (a2 == nullptr && a1 != nullptr)
			{
				return a1;
			}
			else if(a1 != nullptr && a2 != nullptr)
			{
				return (*a1 > *a2 ? a1 : a2);
			}
			else return nullptr;
		}

		template<class T>
		inline void MPriorityQueue<T>::Reallocate(MSize newSize)
		{
			_capacity = newSize;

			T* ptr = reinterpret_cast<T*>(MCollectionHelper::Allocate(_capacity * sizeof(T), _alignof(T)));
			ClearMemory(ptr, newSize);

			MSize posFront = GetFrontGap();
			MSize posRear = GetSizeWithFrontGap();

			MSize siz = GetSize();
			// also, we automatically shift to start here
			MemoryCopy(ptr + 1, _front, siz);

			MCollectionHelper::Deallocate(_data);
			_data = ptr;
			_front = ptr + posFront;
			_rear = ptr + posRear;
		}

#pragma endregion

	}
}
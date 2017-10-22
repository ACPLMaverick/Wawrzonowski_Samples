#pragma once

#include "utility/MPriorityQueue.h"

namespace morphEngine
{
	namespace utility
	{
		template <class T>
		class MPriorityQueueAsc :
			public MPriorityQueue<T>
		{
		protected:

#pragma region Functions Protected

			inline virtual T* Compare(T* a1, T* a2) override;

#pragma endregion

		public:

#pragma region Functions Public

			MPriorityQueueAsc(MSize beginCapacity = MQueue<T>::DEFAULT_SIZE,
				MSize maxCapacity = MQueue<T>::DEFAULT_MAX_SIZE,
				GrowthStrategy growthStrategy = GrowthStrategy::GROW_MUL2_SIZE,
				ShrinkStrategy shrinkStrategy = ShrinkStrategy::NO_SHRINK);
			MPriorityQueueAsc(const MPriorityQueueAsc& c);
			virtual ~MPriorityQueueAsc();

#pragma endregion
		};

#pragma region Definitions

		template <class T>
		MPriorityQueueAsc<T>::MPriorityQueueAsc(MSize beginCapacity,
			MSize maxCapacity, GrowthStrategy gr, ShrinkStrategy sh) :
			MPriorityQueue<T>(beginCapacity, maxCapacity, gr, sh)
		{
			_type = CollectionType::PRIORITY_QUEUE_ASCENDING;
		}

		template<class T>
		inline MPriorityQueueAsc<T>::MPriorityQueueAsc(const MPriorityQueueAsc & c) :
			MPriorityQueue(c)
		{
			_size = c._size;
		}

		template <class T>
		MPriorityQueueAsc<T>::~MPriorityQueueAsc()
		{
		}

		template<class T>
		inline T * MPriorityQueueAsc<T>::Compare(T * a1, T * a2)
		{
			if (a1 == nullptr && a2 != nullptr)
			{
				return a2;
			}
			else if (a2 == nullptr && a1 != nullptr)
			{
				return a1;
			}
			else if (a1 != nullptr && a2 != nullptr)
			{
				return (*a1 < *a2 ? a1 : a2);
			}
			else return nullptr;
		}

#pragma endregion

	}
}
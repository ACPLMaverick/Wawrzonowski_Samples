#include "FixedPoolAllocator.h"

namespace morphEngine
{
	namespace memoryManagement
	{

		FixedPoolAllocator::FixedPoolAllocator(MSize size, MSize objectSize, MUint8 objectAlignment) :
			Allocator::Allocator(size)
		{
			ME_ASSERT(objectSize >= sizeof(void*) && objectSize <= size, "FixedPoolAllocator: wrong size given in constructor.");
			ME_ASSERT(objectAlignment != 0, "FixedPoolAllocator: ObjectAlignment can't be zero.");

			_objectSize = objectSize;
			_objectAlignment = objectAlignment;

			ClearInternal();
		}

		FixedPoolAllocator::FixedPoolAllocator(const FixedPoolAllocator & c) :
			FixedPoolAllocator(c._sizeTotal, c._objectSize, c._objectAlignment)
		{
			
		}


		FixedPoolAllocator::~FixedPoolAllocator()
		{
			_freeList = nullptr;
		}

		void * FixedPoolAllocator::AllocateInternal(MSize size, MUint8 alignment)
		{
			ME_ASSERT(size == _objectSize && alignment == _objectAlignment, 
				"FixedPoolAllocator::Allocate: Wrong size or alignment of object you want to allocate.");

			if (_freeList == nullptr)
			{
				// That shouldn't happen if this allocator is packed in AllocatorCollection
				return _freeList;
			}

			void* p = _freeList;
			_freeList = reinterpret_cast<void**>(*_freeList);

			_usedMemory += size;
			++_numAllocations;

			return p;
		}

		void FixedPoolAllocator::DeallocateInternal(void * p)
		{
			*(reinterpret_cast<void**>(p)) = _freeList;
			_freeList = reinterpret_cast<void**>(p);

			_usedMemory -= _objectSize;
			--_numAllocations;
		}

		void FixedPoolAllocator::ClearInternal()
		{
			MUint8 adj = AlignForwardAdjustment(_start, _objectAlignment);

			// Aligning properly whole list, so every block is aligned
			_freeList = reinterpret_cast<void**>(reinterpret_cast<MUptr>(_start) + static_cast<MUptr>(adj));

			MSize numObjects = (_sizeTotal - adj) / (_objectSize);

			void** p = _freeList;

			// Initialize free blocks list
			for (MSize i = 0; i < numObjects - 1; ++i)
			{
				*p = reinterpret_cast<void*>(reinterpret_cast<MUptr>(p) + static_cast<MUptr>(_objectSize));
				p = reinterpret_cast<void**>(*p);
			}

			_numAllocations = 0;
			_usedMemory = 0;
		}
	}
}
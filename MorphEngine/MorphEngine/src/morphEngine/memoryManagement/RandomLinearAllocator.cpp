#include "RandomLinearAllocator.h"


namespace morphEngine
{
	namespace memoryManagement
	{
		RandomLinearAllocator::RandomLinearAllocator(MSize size) :
			Allocator::Allocator(size)
		{
			_currentPtr = _start;
		}

		RandomLinearAllocator::RandomLinearAllocator(const RandomLinearAllocator & c) : 
			RandomLinearAllocator(c._sizeTotal)
		{
		}


		RandomLinearAllocator::~RandomLinearAllocator()
		{
		}

		void * RandomLinearAllocator::AllocateInternal(MSize size, MUint8 alignment)
		{
			MUint8 adj = AlignForwardAdjustment(_currentPtr, alignment);
			MSize totalSize = size + static_cast<MSize>(adj);

			if (GetFreeMemory() < totalSize)
			{
				return nullptr;
			}

			void* aligned = reinterpret_cast<void*>(reinterpret_cast<MSize>(_currentPtr) + adj);
			_currentPtr = reinterpret_cast<void*>(reinterpret_cast<MSize>(_currentPtr) + totalSize);

			_usedMemory += totalSize;
			++_numAllocations;

			return aligned;
		}

		void RandomLinearAllocator::DeallocateInternal(void * p)
		{
			ME_ASSERT(false, "RandomLinearAllocator: Cannot deallocate single memory address on linear allocator.");
		}

		void RandomLinearAllocator::ClearInternal()
		{
			_numAllocations = 0;
			_usedMemory = 0;
			_currentPtr = _start;
		}

	}
}
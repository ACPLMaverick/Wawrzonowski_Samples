#include "RandomFreeListAllocator.h"


namespace morphEngine
{
	namespace memoryManagement
	{
		RandomFreeListAllocator::RandomFreeListAllocator(MSize size) :
			Allocator(size)
		{
			ME_ASSERT(size > sizeof(FreeBlock), "RandomFreeListAllocator: Bad size given.");
			_freeBlocks = reinterpret_cast<FreeBlock*>(_start);
			_freeBlocks->size = size;
			_freeBlocks->next = nullptr;
		}

		RandomFreeListAllocator::RandomFreeListAllocator(const RandomFreeListAllocator & c) : 
			RandomFreeListAllocator(c._sizeTotal)
		{
		}


		RandomFreeListAllocator::~RandomFreeListAllocator()
		{
			_freeBlocks = nullptr;
		}

		MSize RandomFreeListAllocator::GetFreeMemory() const
		{
			return (reinterpret_cast<MUptr>(_start) + static_cast<MUptr>(_sizeTotal)) -
				(reinterpret_cast<MUptr>(_freeBlocks) + 
				static_cast<MUptr>(AlignForwardAdjustmentWithHeader(_freeBlocks, ME_BYTE_ALIGNMENT, sizeof(AllocationHeader))));
		}

		void * RandomFreeListAllocator::AllocateInternal(MSize size, MUint8 alignment)
		{
			ME_ASSERT(size != 0 && alignment != 0, "RandomFreeListAllocator::AllocateInternal: Bad size or bad aligmnent.");

			FreeBlock* prevFreeBlock = nullptr;
			// Set start address as start of the whole memory space.
			FreeBlock* freeBlock = _freeBlocks;

			// Keep searching free blocks until reaching an end.
			while (freeBlock != nullptr)
			{
				// Calculate adjustment needed to keep object correctly aligned.
				MUint8 adjustment = AlignForwardAdjustmentWithHeader(freeBlock, alignment, sizeof(AllocationHeader));
				MSize totalSize = size + adjustment;
				
				// If allocation doesn't fit in this FreeBlock, try the next
				if (freeBlock->size < totalSize)
				{
					prevFreeBlock = freeBlock;
					freeBlock = freeBlock->next;
					continue;
				}

				if (freeBlock->size - totalSize <= sizeof(AllocationHeader))
				{
					// Increase allocation size instead of creating a new FreeBlock

					totalSize = freeBlock->size;

					if (prevFreeBlock != nullptr)
					{
						prevFreeBlock->next = freeBlock->next;
					}
					else
					{
						_freeBlocks = freeBlock->next;
					}
				}
				else				
				{
					// Create new FreeBlock containing ALL remaining memory

					FreeBlock* nextBlock = reinterpret_cast<FreeBlock*>(reinterpret_cast<MUptr>(freeBlock) + totalSize);
					nextBlock->size = freeBlock->size - totalSize;
					nextBlock->next = freeBlock->next;

					if (prevFreeBlock != nullptr)
					{
						prevFreeBlock->next = nextBlock;
					}
					else
					{
						_freeBlocks = nextBlock;
					}
				}

				// Create header BEFORE allocated memory.
				MUptr alignedAddress = reinterpret_cast<MUptr>(freeBlock) + adjustment;
				AllocationHeader* header = reinterpret_cast<AllocationHeader*>(alignedAddress - sizeof(AllocationHeader));
				header->size = totalSize;
				header->adjustment = adjustment;

				_usedMemory += totalSize;
				++_numAllocations;

				ME_ASSERT(_usedMemory <= _sizeTotal, "Cos sie zjebalo");

				ME_ASSERT(AlignForwardAdjustment(reinterpret_cast<void*>(alignedAddress), alignment) == 0, "RandomFreeListAllocator::AllocateInternal: Bad memory alignment after allocation.");
				return reinterpret_cast<void*>(alignedAddress);
			}

			//ME_ASSERT(false, "AllocateInternal: Out of memory.");
			
			return nullptr;
		}

		void RandomFreeListAllocator::DeallocateInternal(void * p)
		{
			ME_ASSERT(p != nullptr, "RandomFreeListAllocator::DeallocateInternal: Null pointer to memory.");

			AllocationHeader* header = reinterpret_cast<AllocationHeader*>(reinterpret_cast<MUptr>(p) - sizeof(AllocationHeader));

			MUptr blockStart = reinterpret_cast<MUptr>(p) - header->adjustment;
			MSize blockSize = header->size;
			MUptr blockEnd = blockStart + blockSize;

			FreeBlock* prevFreeBlock = nullptr;
			FreeBlock* freeBlock = _freeBlocks;

			while (freeBlock != nullptr)
			{
				if (reinterpret_cast<MUptr>(freeBlock) >= blockEnd)
				{
					break;
				}

				prevFreeBlock = freeBlock;
				freeBlock = freeBlock->next;
			}

			if (prevFreeBlock == nullptr)
			{
				prevFreeBlock = reinterpret_cast<FreeBlock*>(blockStart);
				prevFreeBlock->size = blockSize;
				prevFreeBlock->next = _freeBlocks;		
				_freeBlocks = prevFreeBlock;			// ?!!
			}
			else if (reinterpret_cast<MUptr>(prevFreeBlock) + static_cast<MUptr>(prevFreeBlock->size) == blockStart)
			{
				// PrevFreeBlock just eats up size which used to belong to our freed block. Omnomnom.
				prevFreeBlock->size += blockSize;
			}
			else
			{
				FreeBlock* temp = reinterpret_cast<FreeBlock*>(blockStart);
				temp->size = blockSize;
				temp->next = prevFreeBlock->next;
				prevFreeBlock->next = temp;
				prevFreeBlock = temp;				// ?!!
			}

			if (freeBlock != nullptr && reinterpret_cast<MUptr>(freeBlock) == blockEnd)
			{
				prevFreeBlock->size += freeBlock->size;
				prevFreeBlock->next = freeBlock->next;
			}

			_numAllocations--;
			_usedMemory -= blockSize;

			ME_ASSERT(_usedMemory <= _sizeTotal, "Cos sie zjebalo");
		}

		void RandomFreeListAllocator::ClearInternal()
		{
			_numAllocations = 0;
			_usedMemory = 0;
			_freeBlocks = reinterpret_cast<FreeBlock*>(_start);
			_freeBlocks->size = _sizeTotal;
			_freeBlocks->next = nullptr;
		}
	}
}
#pragma once

#include "Allocator.h"

namespace morphEngine
{
	namespace memoryManagement
	{

		class RandomFreeListAllocator :
			public Allocator
		{
		protected:

#pragma region StructProtected

			struct AllocationHeader
			{
				MSize size;
				MUint8 adjustment;
			};

			struct FreeBlock
			{
				MSize size;
				FreeBlock* next;
			};

#pragma endregion

#pragma region protected

			FreeBlock* _freeBlocks;

#pragma endregion

#pragma region functionsProtected

			virtual void* AllocateInternal(MSize size, MUint8 alignment = ME_BYTE_ALIGNMENT) override;
			virtual void DeallocateInternal(void* p) override;
			virtual void ClearInternal() override;

#pragma endregion

		public:

#pragma region functionsPublic

			RandomFreeListAllocator(MSize size);
			RandomFreeListAllocator(const RandomFreeListAllocator& c);
			virtual ~RandomFreeListAllocator();

			virtual MSize GetFreeMemory() const override;

#pragma endregion

		};

	}
}
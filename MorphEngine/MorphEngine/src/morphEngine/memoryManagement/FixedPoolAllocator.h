#pragma once

#include "memoryManagement/Allocator.h"

namespace morphEngine
{
	namespace memoryManagement
	{

		class FixedPoolAllocator :
			public Allocator
		{
		protected:

#pragma region Protected

			MSize _objectSize;
			MUint8 _objectAlignment;

			void** _freeList;

#pragma endregion

#pragma region Functions Protected

			virtual void* AllocateInternal(MSize size, MUint8 alignment = ME_BYTE_ALIGNMENT) override;
			virtual void DeallocateInternal(void* p) override;
			virtual void ClearInternal() override;

#pragma endregion

		public:

#pragma region Functions Public

			FixedPoolAllocator(MSize size, MSize objectSize, MUint8 objectAlignment = ME_BYTE_ALIGNMENT);
			FixedPoolAllocator(const FixedPoolAllocator& c);
			virtual ~FixedPoolAllocator();

#pragma endregion
		};

	}
}
#pragma once

#include "memoryManagement/Allocator.h"

namespace morphEngine
{
	namespace memoryManagement
	{

		class RandomLinearAllocator :
			public Allocator
		{
		protected:

#pragma region Protected

			void* _currentPtr;

#pragma endregion

#pragma region FunctionsProtected

			virtual void* AllocateInternal(MSize size, MUint8 alignment = ME_BYTE_ALIGNMENT) override;
			virtual void DeallocateInternal(void* p) override;
			virtual void ClearInternal() override;

#pragma endregion

		public:
			RandomLinearAllocator(MSize size);
			RandomLinearAllocator(const RandomLinearAllocator& c);
			~RandomLinearAllocator();
		};

	}
}
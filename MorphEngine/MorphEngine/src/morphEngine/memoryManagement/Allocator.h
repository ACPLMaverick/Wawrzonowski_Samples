#pragma once

#include "core/GlobalDefines.h"

#ifdef _DEBUG

#include <string>
#include <map>

#endif // _DEBUG


#ifdef PLATFORM_WINDOWS
#define ME_BYTE_ALIGNMENT 8
#endif

namespace morphEngine
{
	namespace reflection
	{
		class TypeIterator;
		class TypeIteratorPolymorphic;
	}

	namespace memoryManagement
	{
		
		/// <summary>
		/// Abstract class.
		/// </summary>
		class Allocator
		{
			friend class reflection::TypeIterator;
			friend class reflection::TypeIteratorPolymorphic;
		protected:

#if _TRACE_CALL_STACK

			struct CallInfo
			{
				std::string CallText;

				CallInfo() : CallText("") { }
				CallInfo(const std::string& fName) : CallText(fName) { }

				inline void Print()
				{
					printf((">>>>>>>>>>>>>>>>>>>>>>>>> AllocationInfo >> \n" + CallText + "\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n").c_str());
				}
			};

#endif // _TRACE_CALL_STACK


#pragma region Protected

			void* _start = nullptr;
			MSize _sizeTotal = 0;
			MSize _usedMemory = 0;
			MSize _numAllocations = 0;

#if _TRACE_CALL_STACK

			std::map<void*, CallInfo> _callInfos;

#endif // _TRACE_CALL_STACK


#pragma endregion

#pragma region FunctionsProtected

			inline void* AlignForward(void* address, MUint8 alignment) const;
			inline MUint8 AlignForwardAdjustment(const void* address, MUint8 alignment) const;
			inline MUint8 AlignForwardAdjustmentWithHeader(const void* address, MUint8 alignment, MUint8 headerSize) const;

			virtual void* AllocateInternal(MSize size, MUint8 alignment = ME_BYTE_ALIGNMENT) = 0;
			virtual void DeallocateInternal(void* p) = 0;
			virtual void ClearInternal() = 0;

#pragma endregion

		public:

#pragma region FunctionsPublic

			/// <summary>
			/// Allocates *size* memory on heap while constructing Allocator. Does not allocate any memory if value 0 is passed.
			/// </summary>
			/// <param name="size"></param>
			Allocator(MSize size);
			Allocator(const Allocator& c);
			virtual ~Allocator();

			/// <summary>
			/// Allocate given bytes in allocator's own memory space.
			/// </summary>
			/// <param name="size">Size of memory chunk to allocate.</param>
			/// <param name="alignment">Memory alignment, usually 8, necessary to keep CPU operations efficent.</param>
			/// <returns>Pointer to allocated memory of given size. CAN BE nullptr when not enough memory.</returns>
			virtual void* Allocate(MSize size, MUint8 alignment = ME_BYTE_ALIGNMENT);
			/// <summary>
			/// Deallocate memory held under given address.
			/// </summary>
			/// <param name="p">Pointer to memory.</param>
			virtual void Deallocate(void* p);

			/// <summary>
			/// Updates the allocator somehow. Usually empty function.
			/// </summary>
			virtual void Update();

			/// <summary>
			/// Purges whole allocator memory space, without deleting allocator itself.
			/// </summary>
			virtual void Clear();

			/// <summary>
			/// Checks if given address belongs to allocator's memory space.
			/// </summary>
			/// <param name="p"></param>
			/// <returns></returns>
			virtual bool IsMemoryAddressInRange(void* p) const;

#pragma region Accessors

			virtual void* GetStart() { return _start; }
			virtual MSize GetSizeTotal() const { return _sizeTotal; }
			virtual MSize GetUsedMemory() const { return _usedMemory; }
			virtual MSize GetFreeMemory() const { return GetSizeTotal() - GetUsedMemory(); }
			virtual MSize GetNumAllocations() const { return _numAllocations; }

#pragma endregion

#pragma endregion
		};

#pragma region Inline

		void * Allocator::AlignForward(void * address, MUint8 alignment) const
		{
			/*
			To n-byte align a memory address x we need to mask off the log2(n) least significant bits from x.
			Simply masking off bits will return the first n-byte aligned address before x, so in order to find
			the first after x we just need to add alignment-1 to x and mask that address.
			*/
			return (void*)((reinterpret_cast<MUptr>(address) + static_cast<MUptr>(alignment - 1)) & static_cast<MUptr>(~(alignment - 1)));
		}

		MUint8 Allocator::AlignForwardAdjustment(const void * address, MUint8 alignment) const
		{
			MUint8 adjustment = alignment - (reinterpret_cast<MUptr>(address) & static_cast<MUptr>(alignment - 1));

			if (adjustment == alignment)
			{
				return 0; //already aligned
			}

			return adjustment;
		}

		MUint8 Allocator::AlignForwardAdjustmentWithHeader(const void * address, MUint8 alignment, MUint8 headerSize) const
		{
			MUint8 adjustment = AlignForwardAdjustment(address, alignment);

			MUint8 neededSpace = headerSize;

			if (adjustment < neededSpace)
			{
				neededSpace -= adjustment;

				//Increase adjustment to fit header
				adjustment += alignment * (neededSpace / alignment);

				if (neededSpace % alignment > 0)
				{
					adjustment += alignment;
				}
			}

			return adjustment;
		}

#pragma endregion

	}
}
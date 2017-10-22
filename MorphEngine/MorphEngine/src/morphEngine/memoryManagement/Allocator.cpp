#include "Allocator.h"

#if _TRACE_CALL_STACK

#include "debugging\Debug.h"
using namespace morphEngine::debugging;

#endif

namespace morphEngine
{
	namespace memoryManagement
	{

		Allocator::Allocator(MSize size)
		{
			_sizeTotal = size;
			if (size != 0)
			{
				_start = malloc(_sizeTotal);
			}

			_usedMemory = 0;
			_numAllocations = 0;
		}

		Allocator::Allocator(const Allocator & c) : 
			Allocator(c._sizeTotal)
		{
		}


		Allocator::~Allocator()
		{
#if _TRACE_CALL_STACK

			if (!(_numAllocations == 0 && _usedMemory == 0))
			{
				for (auto it = _callInfos.begin(); it != _callInfos.end(); ++it)
				{
					(*it).second.Print();
				}

				ME_ASSERT(false , "Allocator deallocation error. " + std::to_string(_numAllocations) + " allocations, " + std::to_string(_usedMemory) + " used memory.");
			}

#else
			ME_ASSERT(_numAllocations == 0 && _usedMemory == 0, "Allocator deallocation error.");
#endif

			if (_start != nullptr)
			{
				free(_start);
				_start = nullptr;
			}

			_sizeTotal = 0;
		}

		void* Allocator::Allocate(MSize size, MUint8 alignment)
		{
#if _TRACE_CALL_STACK

			std::string cs;
			Debug::GetCallStack(cs);

			void* mem = AllocateInternal(size, alignment);

			_callInfos.emplace(mem, CallInfo(cs));

			return mem;

#else

			return AllocateInternal(size, alignment);

#endif // _TRACE_CALL_STACK
		}

		void Allocator::Deallocate(void * p)
		{
#if _TRACE_CALL_STACK

			_callInfos.erase(p);

#endif // _TRACE_CALL_STACK

			DeallocateInternal(p);
		}

		void Allocator::Update()
		{
		}

		void Allocator::Clear()
		{
			ClearInternal();
		}

		bool Allocator::IsMemoryAddressInRange(void * p) const
		{
			MUptr adr = reinterpret_cast<MUptr>(p);
			MUptr adrMin = reinterpret_cast<MUptr>(_start);
			MUptr adrMax = reinterpret_cast<MUptr>(_start) + static_cast<MUptr>(_sizeTotal);

			if (adr >= adrMin && adr < adrMax)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}
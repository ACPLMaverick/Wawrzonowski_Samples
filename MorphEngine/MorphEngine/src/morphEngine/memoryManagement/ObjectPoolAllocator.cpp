#include "ObjectPoolAllocator.h"
#include "reflection/Type.h"
#include "core/Engine.h"
#include "core/MObject.h"

namespace morphEngine
{
	namespace memoryManagement
	{
		ObjectPoolAllocator::ObjectPoolAllocator(const reflection::Type* type, MSize size, MSize objectSize, MUint8 objectAlignment) :
			Allocator(size),
			_temporaryAllocator(new FixedPoolAllocator(TEMPORARY_ALLOCATOR_SIZE * objectSize, objectSize, objectAlignment), TEMPORARY_ALLOCATOR_SIZE * objectSize),
			_type(type),
			_objectSize(objectSize),
			_objectAlignment(objectAlignment)
		{

		}

		ObjectPoolAllocator::ObjectPoolAllocator(const ObjectPoolAllocator& c) :
			Allocator(c),
			_temporaryAllocator(c._temporaryAllocator),
			_objectSize(c._objectSize),
			_objectAlignment(c._objectAlignment)
		{

		}

		ObjectPoolAllocator::~ObjectPoolAllocator()
		{
			if (_helper != nullptr)
			{
				ClearInternal();
				delete _helper;
			}
		}

		void ObjectPoolAllocator::Update()
		{
			// solve temp deallocations
			for (auto it = _tempDeallocations.begin(); it != _tempDeallocations.end(); ++it)
			{
				DeallocateInternalInOwnMemory(*it);
			}
			_tempDeallocations.clear();

			// solve temp allocations
			for (auto it = _tempAllocations.begin(); it != _tempAllocations.end(); ++it)
			{
				void* newMem = AllocateInternalInOwnMemory(_type->GetSize(), _type->GetAlignment());
				_helper->CopyObject(newMem, (*it));
			}
			_tempAllocations.clear();

			_temporaryAllocator.Clear();
		}

		bool ObjectPoolAllocator::IsMemoryAddressInRange(void * p) const
		{
			return Allocator::IsMemoryAddressInRange(p) || _temporaryAllocator.IsMemoryAddressInRange(p);
		}

		MSize ObjectPoolAllocator::GetHelperSize()
		{
			return sizeof(ObjectPoolAllocatorTemplateHelper<core::MObject>);
		}

		MUint8 ObjectPoolAllocator::GetHelperAlign()
		{
			return alignof(ObjectPoolAllocatorTemplateHelper<core::MObject>);;
		}

		void * ObjectPoolAllocator::AllocateInternal(MSize size, MUint8 alignment)
		{
			// do not need to check whether allocator is full if we are in AllocatorCollection.

			if (morphEngine::core::Engine::GetInstance()->GetIsGomUpdate())
			{
				void* mem = _temporaryAllocator.Allocate(size, alignment);
				_tempAllocations.push_back(mem);
				return mem;
			}
			else
			{
				return AllocateInternalInOwnMemory(size, alignment);
			}
		}

		void ObjectPoolAllocator::DeallocateInternal(void * p)
		{
			if (_numAllocations != 0 || _temporaryAllocator.GetNumAllocations() != 0)
			{
				if (morphEngine::core::Engine::GetInstance()->GetIsGomUpdate())
				{
					for (auto it = _tempAllocations.begin(); it != _tempAllocations.end(); ++it)
					{
						if (p == (*it))
						{
							_helper->DestroyObject(p);
							_temporaryAllocator.Deallocate(p);
							_tempAllocations.erase(it);
							return;
						}
					}
					_tempDeallocations.push_back(p);
				}
				else
				{
					DeallocateInternalInOwnMemory(p);
				}
			}

		}

		void ObjectPoolAllocator::ClearInternal()
		{
			_bIsClearInProgress = true;
			_helper->ClearAllocator();
			_bIsClearInProgress = false;
		}

		void* ObjectPoolAllocator::AllocateInternalInOwnMemory(MSize size, MUint8 alignment)
		{
			void* mem = reinterpret_cast<void*>(reinterpret_cast<MUint8*>(_start) + (_objectSize * _numAllocations));
			++_numAllocations;
			_usedMemory += _objectSize;

			return mem;
		}

		void ObjectPoolAllocator::DeallocateInternalInOwnMemory(void * p)
		{
			MUint8* startByte = reinterpret_cast<MUint8*>(_start);
			void* last = reinterpret_cast<void*>(startByte + (_objectSize * (_numAllocations - 1)));
			if (p != last)
			{
				// need to reallocate last object onto deallocation place to keep memory continuity
				_helper->CopyObject(p, last);
			}
			else
			{
				_helper->DestroyObject(p);
			}

			--_numAllocations;
			_usedMemory -= _objectSize;
		}

		ObjectPoolAllocator::ObjectPoolAllocatorTemplateHelperBase::~ObjectPoolAllocatorTemplateHelperBase()
		{
		}
	}
}
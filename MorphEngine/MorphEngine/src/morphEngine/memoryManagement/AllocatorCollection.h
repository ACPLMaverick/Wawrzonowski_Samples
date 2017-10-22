#pragma once

#include "memoryManagement/Allocator.h"
#include <vector>

namespace morphEngine
{
	namespace memoryManagement
	{
		template <class T>
		class AllocatorCollection :
			public Allocator
		{
			friend class reflection::TypeIterator;
			friend class reflection::TypeIteratorPolymorphic;
		protected:

#pragma region Protected

			/// <summary>
			/// Should be the only class using std::vector as it is initialized before any Memory Allocator starts.
			/// </summary>
			std::vector<Allocator*> _allocArray;
			MSize _maxChunks;

#pragma endregion

#pragma region FunctionsProtected

			virtual void* AllocateInternal(MSize size, MUint8 alignment = ME_BYTE_ALIGNMENT) override;
			virtual void DeallocateInternal(void* p) override;
			virtual void ClearInternal() override;

#pragma endregion

		public:

#pragma region FunctionsPublic

			/// <summary>
			/// Creates empty collection of 
			/// </summary>
			/// <param name="size">Size of one chunk of memory</param>
			/// <param name="maxChunks">If 0, no limit is given</param>
			
			AllocatorCollection(Allocator* firstAllocator, MSize size, MSize maxChunks = 0) :
				Allocator::Allocator(size)
			{
				ME_ASSERT(firstAllocator->GetSizeTotal() == size /*&& sizeof(firstAllocator) == sizeof(T)*/,
					"AllocatorCollection: Wrong constructor parameters given.");
				_allocArray.push_back(firstAllocator);
				_maxChunks = maxChunks;
			}

			
			AllocatorCollection(const AllocatorCollection& c) : Allocator(c) {};

			virtual ~AllocatorCollection()
			{
				for (std::vector<Allocator*>::iterator it = _allocArray.begin(); it != _allocArray.end(); ++it)
				{
					delete (*it);
				}
				_allocArray.clear();
			}

			virtual void Update() override;

			/// <summary>
			/// Check if given address belongs to memory space of any of held allocators.
			/// </summary>
			/// <param name="p"></param>
			/// <returns></returns>
			virtual bool IsMemoryAddressInRange(void* p) const override;

			virtual MSize GetSizeTotal() const override;
			virtual MSize GetUsedMemory() const override;
			virtual MSize GetNumAllocations() const override;

			virtual T* GetFirstFreeAllocator(MSize size);
			virtual T* GetAllocatorByAddress(void* ptr);

#pragma endregion
		};

	}
}

#pragma region FunctionDefinitions

namespace morphEngine
{
	namespace memoryManagement
	{
		template<class T>
		void * AllocatorCollection<T>::AllocateInternal(MSize size, MUint8 alignment)
		{
			ME_ASSERT(size <= GetSizeTotal(), "AllocatorCollection: Given allocation size too big for a single allocator.");

			// Searching for allocator with enough free memory.
			for (std::vector<Allocator*>::reverse_iterator it = _allocArray.rbegin(); it != _allocArray.rend(); ++it)
			{
				// Each allocator should be able to compute remaining memory, excluding any neccesary metadata for one allocation
				if ((*it)->GetFreeMemory() >= size)
				{
					void *p = (*it)->Allocate(size, alignment);
					// Just for safety, if allocation fails, nullptr will be returned.
					if (p != nullptr)
					{
						return p;
					}
					else
					{
						continue;
					}
				}
			}

			// Such allocator wasn't found. Create new one.

			if (_maxChunks != 0 && _allocArray.size() >= _maxChunks)
			{
				ME_ASSERT(false, "AllocatorCollection: Out of memory");
				return nullptr;
			}

			const T& ref = reinterpret_cast<T&>(*_allocArray[0]);
			_allocArray.push_back(new T(ref));

			return _allocArray.back()->Allocate(size, alignment);
		}

		template<class T>
		void AllocatorCollection<T>::DeallocateInternal(void * p)
		{
			// First find which allocator could the memory block belong to.
			// Assuming each memory block is continous.

			for (std::vector<Allocator*>::iterator it = _allocArray.begin(); it != _allocArray.end(); ++it)
			{
				MUptr adr = reinterpret_cast<MUptr>(p);
				if ((*it)->IsMemoryAddressInRange(p))
				{
					(*it)->Deallocate(p);

					// If this allocator is now empty, destroy it.
					if (_allocArray.size() > 1 && (*it)->GetUsedMemory() == 0)
					{
						delete (*it);
						_allocArray.erase(it);
					}
					
					return;
				}
			}

			ME_ASSERT(false, "AllocatorCollection: Given address doesn't belong here. You probably called Deallocate on wrong allocator.")
		}

		template<class T>
		void AllocatorCollection<T>::ClearInternal()
		{
			for (std::vector<Allocator*>::iterator it = _allocArray.begin(); it != _allocArray.end(); ++it)
			{
				(*it)->Clear();
			}

			if (_allocArray.size() > 1)
			{
				Allocator* tmp = _allocArray[0];

				for (std::vector<Allocator*>::iterator it = _allocArray.begin() + 1; it != _allocArray.end(); ++it)
				{
					delete (*it);
				}

				_allocArray.clear();
				_allocArray.push_back(tmp);
			}
		}

		template<class T>
		inline void AllocatorCollection<T>::Update()
		{
			for (std::vector<Allocator*>::iterator it = _allocArray.begin(); it != _allocArray.end(); ++it)
			{
				(*it)->Update();
			}
		}

		template<class T>
		bool AllocatorCollection<T>::IsMemoryAddressInRange(void * p) const
		{
			for (std::vector<Allocator*>::const_iterator it = _allocArray.begin(); it != _allocArray.end(); ++it)
			{
				if ((*it)->IsMemoryAddressInRange(p))
				{
					return true;
				}
			}
			return false;
		}

		template<class T>
		MSize AllocatorCollection<T>::GetSizeTotal() const
		{
			return _allocArray.size() * _allocArray[0]->GetSizeTotal();
		}

		template<class T>
		MSize AllocatorCollection<T>::GetUsedMemory() const
		{
			MSize mem = 0;
			for (std::vector<Allocator*>::const_iterator it = _allocArray.begin(); it != _allocArray.end(); ++it)
			{
				mem += (*it)->GetUsedMemory();
			}
			return mem;
		}

		template<class T>
		MSize AllocatorCollection<T>::GetNumAllocations() const
		{
			MSize allocs = 0;
			for (std::vector<Allocator*>::const_iterator it = _allocArray.begin(); it != _allocArray.end(); ++it)
			{
				allocs += (*it)->GetNumAllocations();
			}
			return allocs;
		}
		template<class T>
		inline T * AllocatorCollection<T>::GetFirstFreeAllocator(MSize size)
		{
			for (std::vector<Allocator*>::iterator it = _allocArray.begin(); it != _allocArray.end(); ++it)
			{
				if ((*it)->GetFreeMemory() >= size)
				{
					return reinterpret_cast<T*>((*it));
				}
			}
			const T& ref = reinterpret_cast<T&>(*_allocArray[0]);
			T* nAlloc = new T(ref);
			_allocArray.push_back(nAlloc);
			return nAlloc;
		}
		template<class T>
		inline T * AllocatorCollection<T>::GetAllocatorByAddress(void * ptr)
		{
			for (std::vector<Allocator*>::iterator it = _allocArray.begin(); it != _allocArray.end(); ++it)
			{
				if ((*it)->IsMemoryAddressInRange(ptr))
				{
					return reinterpret_cast<T*>((*it));
				}
			}
			return nullptr;
		}
	}
}
	
#pragma endregion
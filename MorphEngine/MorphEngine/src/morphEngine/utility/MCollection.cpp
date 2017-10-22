#include "MCollection.h"
#include "memoryManagement/MemoryManager.h"

void * morphEngine::utility::MCollectionHelper::Allocate(MSize size, MUint8 align)
{
	return memoryManagement::MemoryManager::GetInstance()->GetCollectionsAllocator()
		->Allocate(size, align);
}

void morphEngine::utility::MCollectionHelper::Deallocate(void * ptr)
{
	memoryManagement::MemoryManager::GetInstance()->GetCollectionsAllocator()->Deallocate(reinterpret_cast<void*>(ptr));
}

void * morphEngine::utility::MCollectionHelper::AllocateL(MSize size, MUint8 align)
{
	return memoryManagement::MemoryManager::GetInstance()->GetLinkedCollectionsAllocator()
		->Allocate(size, align);
}

void morphEngine::utility::MCollectionHelper::DeallocateL(void * ptr)
{
	memoryManagement::MemoryManager::GetInstance()->GetLinkedCollectionsAllocator()->Deallocate(reinterpret_cast<void*>(ptr));
}

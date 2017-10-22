#include "Handle.h"
#include "MemoryManager.h"

void * morphEngine::memoryManagement::HandleHelper::Allocate(MSize size, MUint8 alignment)
{
	return MemoryManager::GetInstance()->GetHandleAllocator()->Allocate(size, alignment);
}

void morphEngine::memoryManagement::HandleHelper::Deallocate(void * ptr)
{
	MemoryManager::GetInstance()->GetHandleAllocator()->Deallocate(ptr);
}

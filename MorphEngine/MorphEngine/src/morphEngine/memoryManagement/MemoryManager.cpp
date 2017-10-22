#include "MemoryManager.h"
#include "reflection/Type.h"
#include "gom/Scene.h"

namespace morphEngine
{
	namespace memoryManagement
	{

		MemoryManager::MemoryManager()
		{
			_alocResources = new AllocatorCollection<RandomFreeListAllocator>(new RandomFreeListAllocator(_dims._sizeResources), _dims._sizeResources);
			_alocCollections = new AllocatorCollection<RandomFreeListAllocator>(new RandomFreeListAllocator(_dims._sizeCollections), _dims._sizeCollections);
			_alocLinkedCollections = new AllocatorCollection<RandomFreeListAllocator>(new RandomFreeListAllocator(_dims._sizeLinkedCollections), _dims._sizeLinkedCollections);
			_alocPhysics = new AllocatorCollection<RandomFreeListAllocator>(new RandomFreeListAllocator(_dims._sizePhysics), _dims._sizePhysics);
			_alocHandles = new AllocatorCollection<FixedPoolAllocator>(new FixedPoolAllocator(_dims._sizeHandles, sizeof(HandleInternal<MInt32>), alignof(HandleInternal<MInt32>)), _dims._sizeHandles);
			_alocScenes = new AllocatorCollection<FixedPoolAllocator>(new FixedPoolAllocator(_dims._sizeScenes, sizeof(gom::Scene), alignof(gom::Scene)), _dims._sizeScenes);

			MSize helperSize = (_dims._sizeObjectPoolOneType / sizeof(ObjectPoolAllocator)) * sizeof(ObjectPoolAllocator::GetHelperSize());
			_alocObjectPoolHelpers = new AllocatorCollection<FixedPoolAllocator>(new FixedPoolAllocator(helperSize, ObjectPoolAllocator::GetHelperSize(), ObjectPoolAllocator::GetHelperAlign()), helperSize);

			_alocTemp = new AllocatorCollection<RandomLinearAllocator>(new RandomLinearAllocator(_dims._sizeTempOneFrame), _dims._sizeTempOneFrame);
			_alocTempTwoFrames = new AllocatorCollection<RandomLinearAllocator>(new RandomLinearAllocator(_dims._sizeTempTwoFrames), _dims._sizeTempTwoFrames);
		}


		MemoryManager::~MemoryManager()
		{
		}

		void MemoryManager::Shutdown()
		{
			for (auto it = _objectPoolMap.begin(); it != _objectPoolMap.end(); ++it)
			{
				delete (*it).second;
			}
			_objectPoolMap.clear();

			delete _alocResources;
			_alocResources = nullptr;

			delete _alocPhysics;
			_alocPhysics = nullptr;

			delete _alocCollections;
			_alocCollections = nullptr;

			delete _alocLinkedCollections;
			_alocLinkedCollections = nullptr;

			delete _alocScenes;
			_alocScenes = nullptr;

			delete _alocHandles;
			_alocHandles = nullptr;

			delete _alocObjectPoolHelpers;
			_alocObjectPoolHelpers = nullptr;

			delete _alocTemp;
			_alocTemp = nullptr;

			delete _alocTempTwoFrames;
			_alocTempTwoFrames = nullptr;
		}

		void MemoryManager::Update()
		{
			for (auto it = _objectPoolMap.begin(); it != _objectPoolMap.end(); ++it)
			{
				(*it).second->Update();
			}
			_alocResources->Update();
			_alocCollections->Update();
			_alocLinkedCollections->Update();
			_alocHandles->Update();
			_alocObjectPoolHelpers->Update();
			_alocTemp->Update();
			_alocTempTwoFrames->Update();

			_alocTemp->Clear();

			if (_bEverySecondFrame)
			{
				_alocTempTwoFrames->Clear();
				_bEverySecondFrame = false;
			}
			else
			{
				_bEverySecondFrame = true;
			}
		}

		AllocatorCollection<ObjectPoolAllocator> * MemoryManager::RegisterType(const reflection::Type* type)
		{
			AllocatorCollection<ObjectPoolAllocator>* alloc = _objectPoolMap[type];
			if (alloc == nullptr)
			{
				// TODO: enter size and alignment information here.
				alloc = new AllocatorCollection<ObjectPoolAllocator>(new ObjectPoolAllocator(type, _dims._sizeObjectPoolOneType, type->GetSize(), static_cast<MUint8>(type->GetAlignment())), _dims._sizeObjectPoolOneType);
				_objectPoolMap[type] = alloc;
			}
			return alloc;
		}
	}
}
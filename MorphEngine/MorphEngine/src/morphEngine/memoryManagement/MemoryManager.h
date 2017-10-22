#pragma once

namespace morphEngine
{
	namespace reflection
	{
		const class Type;
	}
}

#include "core/Singleton.h"
//#include "gom/BaseObject.h"

#include "memoryManagement/RandomFreeListAllocator.h"
#include "memoryManagement/RandomLinearAllocator.h"
#include "memoryManagement/FixedPoolAllocator.h"
#include "memoryManagement/AllocatorCollection.h"
#include "memoryManagement/ObjectPoolAllocator.h"
#include "memoryManagement/Handle.h"


#include <map>

namespace morphEngine
{
	namespace memoryManagement
	{
		
		/// <summary>
		/// Singleton.
		/// </summary>
		class MemoryManager :
			public core::Singleton<MemoryManager>
		{
		public:

#pragma region Structs Public

			struct AllocatorDimensions
			{
				MSize _sizeObjectPoolOneType;
				MSize _sizeResources;
				MSize _sizeCollections;
				MSize _sizeLinkedCollections;
				MSize _sizeHandles;
				MSize _sizeScenes;
				MSize _sizePhysics;
				MSize _sizeTempOneFrame;
				MSize _sizeTempTwoFrames;

				AllocatorDimensions(
					MSize sizeObjectPoolOneType = Megabytes(1),
					MSize sizeResources = Megabytes(512),
					MSize sizeCollections = Megabytes(128),
					MSize sizeLinkedCollections = Megabytes(1),
					MSize sizeHandles = Megabytes(64),
					MSize sizeScenes = Megabytes(32),
					MSize sizePhysics = Megabytes(128),
					MSize sizeTempOneFrame = Megabytes(1),
					MSize sizeTempTwoFrames = Megabytes(1)) :

					_sizeObjectPoolOneType(sizeObjectPoolOneType),
					_sizeResources(sizeResources),
					_sizeCollections(sizeCollections),
					_sizeLinkedCollections(sizeLinkedCollections),
					_sizeScenes(sizeScenes),
					_sizePhysics(sizePhysics),
					_sizeHandles(sizeHandles),
					_sizeTempOneFrame(sizeTempOneFrame),
					_sizeTempTwoFrames(sizeTempTwoFrames)
				{

				}
			};

#pragma endregion

		protected:

#pragma region Protected

			AllocatorDimensions _dims;

			std::map<const reflection::Type*, AllocatorCollection<ObjectPoolAllocator>*> _objectPoolMap;

			AllocatorCollection<RandomFreeListAllocator>* _alocResources = nullptr;
			AllocatorCollection<RandomFreeListAllocator>* _alocCollections = nullptr;
			AllocatorCollection<RandomFreeListAllocator>* _alocLinkedCollections = nullptr;
			AllocatorCollection<RandomFreeListAllocator>* _alocPhysics = nullptr;
			AllocatorCollection<FixedPoolAllocator>* _alocScenes = nullptr;
			AllocatorCollection<FixedPoolAllocator>* _alocHandles = nullptr;
			AllocatorCollection<FixedPoolAllocator>* _alocObjectPoolHelpers = nullptr;
			AllocatorCollection<RandomLinearAllocator>* _alocTemp = nullptr;
			AllocatorCollection<RandomLinearAllocator>* _alocTempTwoFrames = nullptr;

			bool _bEverySecondFrame = false;

#pragma endregion

#pragma region FunctionsProtected


#pragma endregion

		public:

#pragma region FunctionsPublic
			MemoryManager();
			MemoryManager(const MemoryManager& copy) {};
			~MemoryManager();

			/// <summary>
			/// Destroys all memory allocators making dynamic memory allocation impossible.
			/// </summary>
			void Shutdown();

			/// <summary>
			/// Must be called last at the end of the frame. Clears temporary memory allocators.
			/// </summary>
			void Update();

			AllocatorCollection<ObjectPoolAllocator>* RegisterType(const reflection::Type* type);
			template <typename T> Handle<T> NewObject(const gom::ObjectInitializer& initializer);

#pragma region Accessors

			const AllocatorDimensions& GetAllocatorDimensions() const { return _dims; }

			ME_DEPRECATED_MSG("This function yields no effect, as serialization and deserialization of MemoryManager is not yet implemented.")
			/// <summary>
			/// This function will change amount of memory each allocator is assigned on engine startup. Engine needs to be restarted in order for these
			/// changes to take effect.
			/// </summary>
			/// <param name="dimensions"></param>
			void SetAllocatorDimensions(const AllocatorDimensions& dimensions) { _dims = dimensions; }

			Allocator* const GetAssetAllocator() const { return _alocResources; }
			Allocator* const GetCollectionsAllocator() const { return _alocCollections; }
			Allocator* const GetLinkedCollectionsAllocator() const { return _alocLinkedCollections; }
			Allocator* const GetSceneAllocator() const { return _alocScenes; }
			Allocator* const GetPhysicsAllocator() const { return _alocPhysics; }
			Allocator* const GetHandleAllocator() const { return _alocHandles; }
			Allocator* const GetObjectPoolHelpersAllocator() const { return _alocObjectPoolHelpers; }
			Allocator* const GetTemporaryOneFrameAllocator() const { return _alocTemp; }
			Allocator* const GetTemporaryTwoFramesAllocator() const { return _alocTempTwoFrames; }

#pragma endregion

#pragma endregion

		};

#pragma region Function Definitions

		template<typename T>
		inline Handle<T> MemoryManager::NewObject(const gom::ObjectInitializer & initializer)
		{
			return T::Type.GetAllocator()->NewObject<T>(initializer);
		}

#pragma endregion


}
}
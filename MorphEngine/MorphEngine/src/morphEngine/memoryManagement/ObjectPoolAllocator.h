#pragma once

#include "memoryManagement/Allocator.h"
#include "memoryManagement/Handle.h"
#include "memoryManagement/FixedPoolAllocator.h"
#include "memoryManagement/AllocatorCollection.h"
#include "gom/ObjectInitializer.h"

#include <map>

namespace morphEngine
{
	namespace reflection
	{
		class Type;
	}

	namespace memoryManagement
	{
		/// <summary>
		/// This allocator maintains continous memory block, allowing to iterate through its objects.
		/// </summary>
		class ObjectPoolAllocator :
			public Allocator
		{
#pragma region Classes Protected
		protected:

			class ObjectPoolAllocatorTemplateHelperBase
			{
			public:
				virtual ~ObjectPoolAllocatorTemplateHelperBase();

				virtual void RemoveHandleAndObject(void* vHandle) = 0;
				virtual void DestroyObject(void* vObject) = 0;
				virtual void CopyObject(void* vDestination, void* vSource) = 0;
				virtual void ClearAllocator() = 0;
			};

			template <class T>
			class ObjectPoolAllocatorTemplateHelper : 
				public ObjectPoolAllocatorTemplateHelperBase
			{
			public:
				ObjectPoolAllocator& _ref;

				ObjectPoolAllocatorTemplateHelper(ObjectPoolAllocator& ref);
				virtual ~ObjectPoolAllocatorTemplateHelper();

				void* operator new(MSize size);
				void operator delete(void* ptr, MSize size);

				virtual void RemoveHandleAndObject(void* vHandle) override;
				virtual void DestroyObject(void* vObject);
				virtual void CopyObject(void* vDestination, void* vSource) override;
				virtual void ClearAllocator() override;
			};

#pragma endregion
		public:
			template <typename T>
			friend class HandleInternal;
			template <typename T>
			friend class ObjectPoolAllocatorTemplateHelper;
			friend class reflection::Type;
			friend class reflection::TypeIterator;
			friend class reflection::TypeIteratorPolymorphic;
		protected:


#pragma region Protected

			static const MSize TEMPORARY_ALLOCATOR_SIZE = 64;

			AllocatorCollection<FixedPoolAllocator> _temporaryAllocator;

			ObjectPoolAllocatorTemplateHelperBase* _helper = nullptr;

			// key - pointer to object memory (T*), value - pointer to handle to that object (HandleInternal*)
			std::map<void*, void*> _handleTable;
			std::vector<void*> _tempAllocations;
			std::vector<void*> _tempDeallocations;

			MSize _objectSize;
			const reflection::Type* _type;
			MUint8 _objectAlignment;
			bool _bIsClearInProgress = false;

#pragma endregion

#pragma region Functions Protected

			virtual void* AllocateInternal(MSize size, MUint8 alignment = ME_BYTE_ALIGNMENT) override;
			virtual void DeallocateInternal(void* p) override;
			virtual void ClearInternal() override;

			void* AllocateInternalInOwnMemory(MSize size, MUint8 alignment = ME_BYTE_ALIGNMENT);
			void DeallocateInternalInOwnMemory(void *p);

#pragma endregion

		public:

#pragma region Functions Public

			ObjectPoolAllocator(const reflection::Type* type, MSize size, MSize objectSize, MUint8 objectAlignment = ME_BYTE_ALIGNMENT);
			ObjectPoolAllocator(const ObjectPoolAllocator& c);
			virtual ~ObjectPoolAllocator();

			virtual void Update() override;

			virtual bool IsMemoryAddressInRange(void* p) const;
			inline bool IsClearInProgress() const { return _bIsClearInProgress; }
			
			template <typename T> Handle<T> NewObject(const gom::ObjectInitializer& initializer);
			template <typename T> void DestroyObject(Handle<T> handle);
			template <typename T> Handle<T> CloneObject(Handle<T> reference, bool bDeepCopy = true);
			//template <typename T> void DestroyObjectImmediate(Handle<T> handle);

			static MSize GetHelperSize();
			static MUint8 GetHelperAlign();

#pragma endregion
		};

#pragma region Definitions

		template<typename T>
		inline Handle<T> ObjectPoolAllocator::NewObject(const gom::ObjectInitializer & initializer)
		{
			// this is a hack of hacks and I know it 
			if (_helper == nullptr)
			{
				_helper = new ObjectPoolAllocatorTemplateHelper<T>(*this);
			}

			void* mem = Allocate(sizeof(T), alignof(T));
			HandleInternal<T>* handle = new HandleInternal<T>(reinterpret_cast<T*>(mem), &T::Type);
			const_cast<gom::ObjectInitializer &>(initializer)._this = reinterpret_cast<HandleInternal<core::MObject>*>(handle);
			T* newObject = new(mem) T(initializer);

			_handleTable.emplace(reinterpret_cast<void*>(newObject), reinterpret_cast<void*>(handle));

			return Handle<T>(handle);
		}

		//template<typename T>
		//inline void ObjectPoolAllocator::DestroyObject(Handle<T> handle)
		//{
		//	_tempDestructions.push_back(reinterpret_cast<void*>(*handle));
		//}

		template<typename T>
		inline void ObjectPoolAllocator::DestroyObject(Handle<T> handle)
		{
			handle->~T();
			void* mem = reinterpret_cast<void*>(handle.GetPointer());
			auto hIterator = _handleTable.find(mem);
			if(hIterator != _handleTable.end())
			{
				delete reinterpret_cast<HandleInternal<T>*>((*hIterator).second);
				_handleTable.erase(hIterator);
			}
			Deallocate(mem);
		}

		template<typename T>
		inline Handle<T> ObjectPoolAllocator::CloneObject(Handle<T> reference, bool bDeepCopy)
		{
			// this is a hack of hacks and I know it 
			if (_helper == nullptr)
			{
				_helper = new ObjectPoolAllocatorTemplateHelper<T>(*this);
			}

			void* mem = Allocate(sizeof(T), alignof(T));
			HandleInternal<T>* handle = new HandleInternal<T>(reinterpret_cast<T*>(mem), &T::Type);
			Handle<T> oldThis(true);
			oldThis = reference.GetPointer()->_this;
			reference.GetPointer()->_this = Handle<T>(handle, true);
			T* newObject = new(mem) T(*reference, bDeepCopy);
			((core::MObject*)newObject)->_uniqueID = gom::ObjectInitializer::GenerateUID();
			reference.GetPointer()->_this = oldThis;

			_handleTable.emplace(reinterpret_cast<void*>(newObject), reinterpret_cast<void*>(handle));

			return Handle<T>(handle);
		}

#pragma endregion

#pragma region HelperDefinitions

		template<class T>
		inline ObjectPoolAllocator::ObjectPoolAllocatorTemplateHelper<T>::ObjectPoolAllocatorTemplateHelper(ObjectPoolAllocator & ref) : 
			_ref(ref)
		{
		}

		template<class T>
		inline ObjectPoolAllocator::ObjectPoolAllocatorTemplateHelper<T>::~ObjectPoolAllocatorTemplateHelper()
		{
		}

		template<class T>
		inline void * ObjectPoolAllocator::ObjectPoolAllocatorTemplateHelper<T>::operator new(MSize size)
		{
			return MemoryManager::GetInstance()->GetObjectPoolHelpersAllocator()->Allocate(size, alignof(ObjectPoolAllocatorTemplateHelper<T>));
		}

		template<class T>
		inline void ObjectPoolAllocator::ObjectPoolAllocatorTemplateHelper<T>::operator delete(void * ptr, MSize size)
		{
			MemoryManager::GetInstance()->GetObjectPoolHelpersAllocator()->Deallocate(ptr);
		}

		template<class T>
		inline void ObjectPoolAllocator::ObjectPoolAllocatorTemplateHelper<T>::RemoveHandleAndObject(void * vHandle)
		{
			HandleInternal<T>* handle = reinterpret_cast<HandleInternal<T>*>(vHandle);
			_ref._handleTable.erase(handle->_object);
			handle->_object->~T();
		}

		template<class T>
		inline void ObjectPoolAllocator::ObjectPoolAllocatorTemplateHelper<T>::DestroyObject(void * vObject)
		{
			reinterpret_cast<T*>(vObject)->~T();
		}

		template<class T>
		inline void ObjectPoolAllocator::ObjectPoolAllocatorTemplateHelper<T>::CopyObject(void * vDestination, void * vSource)
		{
			T* destination = reinterpret_cast<T*>(vDestination);
			T* source = reinterpret_cast<T*>(vSource);


			// update handle here
			HandleInternal<T>* hndl = reinterpret_cast<HandleInternal<T>*>(_ref._handleTable[reinterpret_cast<void*>(source)]);
			if (hndl != nullptr)
			{
				hndl->_object = destination;
				_ref._handleTable.emplace(hndl->_object, hndl);
				Handle<T> oldThis(true);
				oldThis = source->_this;
				source->_this = Handle<T>(hndl, true);
				destination = new(destination) T(*source, false);
				source->_this = oldThis;
				source->~T();
			}
			_ref._handleTable.erase(reinterpret_cast<void*>(source));
		}

		template<class T>
		inline void ObjectPoolAllocator::ObjectPoolAllocatorTemplateHelper<T>::ClearAllocator()
		{
			if (_ref._numAllocations != 0 || _ref._temporaryAllocator.GetNumAllocations() != 0)
			{
				//for (auto it = _ref._tempDeallocations.begin(); it != _ref._tempDeallocations.end(); ++it)
				//{
				//	reinterpret_cast<T*>((*it))->~T();
				//	_ref.DeallocateInternalInOwnMemory(*it);
				//}

				for (auto it = _ref._tempAllocations.begin(); it != _ref._tempAllocations.end(); ++it)
				{
					reinterpret_cast<T*>((*it))->~T();
					//auto iter = _ref._handleTable.find(*it);
					//delete reinterpret_cast<HandleInternal<T>*>((*iter).second);
					//_ref._handleTable.erase(iter);
				}

				//for (auto it = _ref._handleTable.begin(); it != _ref._handleTable.end(); ++it)
				//{
				//	reinterpret_cast<T*>((*it).first)->~T();
				//	delete reinterpret_cast<HandleInternal<T>*>((*it).second);
				//	--_ref._numAllocations;
				//	_ref._usedMemory -= _ref._objectSize;
				//}
				for (auto it = _ref._handleTable.begin(); it != _ref._handleTable.end(); ++it)
				{
					delete reinterpret_cast<HandleInternal<T>*>((*it).second);
				}
				for (MSize i = 0; i < _ref._numAllocations; ++i)
				{
					T* obj = reinterpret_cast<T*>(_ref._start) + i;
					obj->~T();
				}
				_ref._numAllocations = _ref._usedMemory = 0;

				//ME_ASSERT(_ref._numAllocations == 0 && _ref._usedMemory == 0, "ObjectPoolAllocator: Clearing went wrong.");
				_ref._handleTable.clear();
				_ref._tempAllocations.clear();
				_ref._tempDeallocations.clear();
				_ref._temporaryAllocator.Clear();
			}
		}

#pragma endregion
}
}


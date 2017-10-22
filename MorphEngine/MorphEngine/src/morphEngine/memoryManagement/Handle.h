#pragma once

#include "../core/GlobalDefines.h"
#include "HandleInfo.h"

#include <vector>

namespace morphEngine
{
	namespace reflection
	{
		class Type;
	}

	namespace memoryManagement
	{
		class ObjectPoolAllocator;

		template <class T> class Handle;

		namespace HandleHelper
		{
			extern void* Allocate(MSize size, MUint8 alignment);
			extern void Deallocate(void* ptr);
		}

		template <class T> 
		class HandleInternal
		{
			friend class ObjectPoolAllocator;
			friend class Handle<T>;
		protected:

#pragma region Protected

			std::vector<Handle<T>*> _refs;
			T* _object;
			const reflection::Type* _type;

#pragma endregion

#pragma region Functions Protected

			inline HandleInternal(T* object, const reflection::Type* type);

			inline HandleInternal(const HandleInternal<T>& copy);
			template <typename D> inline HandleInternal(const HandleInternal<D>& copy);
			inline ~HandleInternal();

			inline void EraseRef(Handle<T>* ref);

			inline HandleInternal<T>& operator=(const HandleInternal<T>& copy);
			template <typename D> inline HandleInternal<T>& operator=(const HandleInternal<D>& copy);

			inline void IncrementRefCounter(Handle<T>* ref);
			inline void DecrementRefCounter(Handle<T>* ref);

#pragma endregion

		public:

#pragma region Functions Public

			void* operator new(MSize size);
			void operator delete(void* ptr, MSize size);

#pragma endregion
		};

		template <class T> class Handle
		{
			template <typename D> friend class Handle;
			friend class HandleInternal<T>;
		protected:

			union
			{
				HandleInternal<T>* _hRef;
				MUint64 _storedID = 0;
			};
			bool _bIsStoringID;
			bool _bIsShadowHandle;

			inline void Copy(const Handle<T>& copy);
			template <typename D> inline void Copy(const Handle<D>& copy);
			inline void IncrementHRef(HandleInternal<T>* hRef);
			inline void DecrementHRef(HandleInternal<T>* hRef);

			inline void InvalidateInternal();

		public:

			inline Handle(bool isShadowHandle = false);
			inline Handle(HandleInternal<T>* hRef, bool isShadowHandle = false);
			inline Handle(const Handle<T>& copy);
			inline Handle(MUint64 storedID);
			template <typename D> inline Handle(const Handle<D>& copy);
			inline ~Handle();

			inline void Invalidate();

			inline void StoreID(MUint64 id);
			inline MUint64 GetStoredID() const;
			inline bool IsStoringID() const;
			inline bool IsShadowHandle() const;

			inline T* GetPointer() const;
			inline const reflection::Type* GetType() const;
			inline MUint64 GetObjectID() const;
			inline bool IsValid() const;

			// Looks up through polymorphic type iterator and linearly searches for an object of ID being stored.
			// Returns true if object was found or this handle already stores object reference.
			inline bool LoadObjectFromStoredID();

			// Saves referenced object's ID into data of this structure. 
			// Beware, as after this operation, "connection" with the object will be broken.
			inline bool LoadIDFromObject();

			inline T& operator*() const;
			inline T* operator->() const;

			inline bool operator==(const Handle<T>& rhs) const;
			inline bool operator!=(const Handle<T>& rhs) const;
			inline bool operator<(const Handle<T>& rhs) const;
			inline bool operator<=(const Handle<T>& rhs) const;
			inline bool operator>(const Handle<T>& rhs) const;
			inline bool operator>=(const Handle<T>& rhs) const;

			inline Handle<T>& operator=(const Handle<T>& copy);
			template <typename D> inline Handle<T>& operator=(const Handle<D>& copy);
			template <typename D> inline operator Handle<D>();
			template <typename D> inline operator Handle<D>&();
		};

#pragma region Definitions HandleInternal

		template <class T> 
		HandleInternal<T>::HandleInternal(T* object, const reflection::Type* type) :
			_object(object),
			_type(type)
		{

		}

		template<class T>
		HandleInternal<T>::HandleInternal(const HandleInternal<T>& c) :
			_object(c._object),
			_refs(c._refs),
			_type(c._type)
		{
			
		}

		template <class T>
		template <typename D> 
		HandleInternal<T>::HandleInternal(const HandleInternal<D>& c) :
			_object(reinterpret_cast<T*>(c._object)),
			_refs(c._refs),
			_type(c._type)
		{

		}

		template <class T> 
		HandleInternal<T>::~HandleInternal()
		{
			// this will occur when HandleInternal (i.e. object is destroyed externally by Destroy call)
			for (auto it = _refs.begin(); it != _refs.end(); ++it)
			{
				(*it)->InvalidateInternal();
			}
		}

		template<class T>
		inline void HandleInternal<T>::EraseRef(Handle<T>* ref)
		{
			MSize* refFakePtr = reinterpret_cast<MSize*>(ref);
			for (auto it = _refs.begin(); it != _refs.end(); ++it)
			{
				MSize* fakePtr = reinterpret_cast<MSize*>(*it);
				if (refFakePtr == fakePtr)
				{
					(*it)->InvalidateInternal();
					_refs.erase(it);
					return;
				}
			}
		}

		template<class T>
		inline HandleInternal<T>& HandleInternal<T>::operator=(const HandleInternal<T>& copy)
		{
			_object = copy._object;
			_refs = copy._refs;
			_type = copy._type;

			return *this;
		}

		template<class T>
		inline void HandleInternal<T>::IncrementRefCounter(Handle<T>* ref)
		{
			_refs.push_back(ref);
		}

		template<class T>
		inline void HandleInternal<T>::DecrementRefCounter(Handle<T>* ref)
		{
			if (_refs.size() != 0)
			{
				EraseRef(ref);

				if (_refs.size() == 0)
				{
					// Destroy object and myself
					ObjectPoolAllocator* alloc = _type->GetAllocator()->GetAllocatorByAddress(reinterpret_cast<void*>(_object));
					alloc->_helper->RemoveHandleAndObject(reinterpret_cast<void*>(this));
					alloc->Deallocate(reinterpret_cast<void*>(_object));
					delete this;
				}
			}
		}

		template<class T>
		inline void * HandleInternal<T>::operator new(MSize size)
		{
			return HandleHelper::Allocate(size, alignof(HandleInternal<T>));
		}

		template<class T>
		inline void HandleInternal<T>::operator delete(void * ptr, MSize size)
		{
			HandleHelper::Deallocate(ptr);
		}

		template<class T>
		template <typename D>
		inline HandleInternal<T>& HandleInternal<T>::operator=(const HandleInternal<D>& copy)
		{
			_object = reinterpret_cast<T*>(copy._object);
			_refs = copy._refs;
			_type = copy._type;

			return *this;
		}

#pragma endregion

#pragma region Definitions Handle

		template<class T>
		inline Handle<T>::Handle(bool isShadowHandle) :
			_hRef(nullptr),
			_bIsStoringID(false),
			_bIsShadowHandle(isShadowHandle)
		{
		}

		template<class T>
		inline Handle<T>::Handle(HandleInternal<T>* hRef, bool isShadowHandle) :
			_hRef(hRef),
			_bIsStoringID(false),
			_bIsShadowHandle(isShadowHandle)
		{
			IncrementHRef(_hRef);
		}

		template<class T>
		inline Handle<T>::Handle(const Handle<T>& copy) : 
			_bIsShadowHandle(copy._bIsShadowHandle)
		{
			Copy(copy);
		}

		template<class T>
		inline Handle<T>::Handle(MUint64 storedID) :
			_storedID(storedID),
			_bIsStoringID(true),
			_bIsShadowHandle(false)
		{
		}

		template<class T>
		template<typename D>
		inline Handle<T>::Handle(const Handle<D>& copy) : 
			_bIsShadowHandle(copy._bIsShadowHandle)
		{
			Copy(copy);
		}

		template<class T>
		inline Handle<T>::~Handle()
		{
			DecrementHRef(_hRef);
			_hRef = nullptr;
		}

		template<class T>
		inline void Handle<T>::Invalidate()
		{
			DecrementHRef(_hRef);
			InvalidateInternal();
		}

		template<class T>
		inline void Handle<T>::InvalidateInternal()
		{
			_hRef = nullptr;
		}

		template<class T>
		inline void Handle<T>::IncrementHRef(HandleInternal<T>* hRef)
		{
			if (hRef != nullptr && !_bIsStoringID && !_bIsShadowHandle)
			{
				hRef->IncrementRefCounter(this);
			}
		}

		template<class T>
		inline void Handle<T>::DecrementHRef(HandleInternal<T>* hRef)
		{
			if (hRef != nullptr && !_bIsStoringID && !_bIsShadowHandle)
			{
				hRef->DecrementRefCounter(this);
			}
		}

		template<class T>
		inline void Handle<T>::StoreID(MUint64 id)
		{
			_bIsStoringID = true;
			_storedID = id;
		}

		template<class T>
		inline bool Handle<T>::IsStoringID() const
		{
			return _bIsStoringID;
		}

		template<class T>
		inline bool Handle<T>::IsShadowHandle() const
		{
			return _bIsShadowHandle;
		}

		template<class T>
		inline MUint64 Handle<T>::GetStoredID() const
		{
			ME_ASSERT(_bIsStoringID, "Handle: Retrieving stored id, but this handle is not storing it.");
			return _storedID;
		}

		template<class T>
		inline T * Handle<T>::GetPointer() const
		{
			ME_ASSERT(_hRef != nullptr, "Handle: Reference to internal handle is null! You should initialize it with a copy or call CreateInstance onto it.");
			ME_ASSERT(!_bIsStoringID, "Handle: Using handle that is storing ID as an object storing handle.");
			return _hRef->_object;
		}

		template<class T>
		inline const reflection::Type * Handle<T>::GetType() const
		{
			if(_hRef != nullptr && !_bIsStoringID)
			{
				return _hRef->_type;
			}
			return T::GetType();
		}

		template<class T>
		inline MUint64 Handle<T>::GetObjectID() const
		{
			ME_ASSERT(_hRef != nullptr, "Handle: Reference to internal handle is null! You should initialize it with a copy or call CreateInstance onto it.");
			if (_bIsStoringID)
			{
				return _storedID;
			}
			else
			{
				return _hRef->_object->GetUniqueID();
			}
		}

		template<class T>
		inline bool Handle<T>::IsValid() const
		{
			return _hRef != nullptr;
		}

		template<class T>
		inline bool Handle<T>::LoadObjectFromStoredID()
		{
			if (!_bIsStoringID)
			{
				return true;
			}

			const morphEngine::reflection::Type* t = GetType();
			if(t == nullptr)
			{
				return false;
			}

			for (auto it = t->GetIteratorPolymorphic(); it.IsValid(); ++it)
			{
				Handle<T> h = it.Get<T>()->This();
				if (_storedID == h->GetUniqueID())
				{
					_bIsStoringID = false;

					_hRef = h._hRef;
					_hRef->IncrementRefCounter(this);

					return true;
				}
			}

			return false;
		}

		template<class T>
		inline bool Handle<T>::LoadIDFromObject()
		{
			if (_bIsStoringID)
			{
				return true;
			}

			if (_hRef == nullptr)
			{
				return false;
			}

			_bIsStoringID = true;
			_hRef->DecrementRefCounter(this);
			_storedID = _hRef->_object->GetUniqueID();

			return true;
		}

		template<class T>
		inline T & Handle<T>::operator*() const
		{
			ME_ASSERT(_hRef != nullptr, "Handle: Reference to internal handle is null! You should initialize it with a copy or call CreateInstance onto it.");
			ME_ASSERT(!_bIsStoringID, "Handle: Using handle that is storing ID as an object storing handle.");
			return *(_hRef->_object);
		}

		template<class T>
		inline T * Handle<T>::operator->() const
		{
			ME_ASSERT(_hRef != nullptr, "Handle: Reference to internal handle is null! You should initialize it with a copy or call CreateInstance onto it.");
			ME_ASSERT(!_bIsStoringID, "Handle: Using handle that is storing ID as an object storing handle.");
			return _hRef->_object;
		}

		template<class T>
		inline bool Handle<T>::operator==(const Handle<T>& rhs) const
		{
			return this->_hRef == rhs._hRef;
		}

		template<class T>
		inline bool Handle<T>::operator!=(const Handle<T>& rhs) const
		{
			return this->_hRef != rhs._hRef;
		}

		template<class T>
		inline bool Handle<T>::operator<(const Handle<T>& rhs) const
		{
			return _hRef < rhs._hRef;
		}

		template<class T>
		inline bool Handle<T>::operator<=(const Handle<T>& rhs) const
		{
			return operator<(rhs) || operator==(rhs);
		}

		template<class T>
		inline bool Handle<T>::operator>(const Handle<T>& rhs) const
		{
			return _hRef > rhs._hRef;
		}

		template<class T>
		inline bool Handle<T>::operator>=(const Handle<T>& rhs) const
		{
			return operator>(rhs) || operator==(rhs);
		}

		template<class T>
		inline Handle<T>& Handle<T>::operator=(const Handle<T>& copy)
		{
			Copy(copy);

			return *this;
		}

		template<class T>
		template<typename D>
		inline Handle<T>& Handle<T>::operator=(const Handle<D>& copy)
		{
			Copy(copy);

			return *this;
		}

		template<class T>
		template<typename D>
		inline Handle<T>::operator Handle<D>&()
		{
			return reinterpret_cast<Handle<D>&>(*this);
		}

		template<class T>
		template<typename D>
		inline Handle<T>::operator Handle<D>()
		{
			return Handle<D>(*this);
		}

		template<class T>
		inline void Handle<T>::Copy(const Handle<T>& copy)
		{
			_bIsStoringID = copy._bIsStoringID;
			if (_bIsStoringID)
			{
				_storedID = copy._storedID;
			}
			else
			{
				IncrementHRef(copy._hRef);
				DecrementHRef(_hRef);
				_hRef = copy._hRef;
			}
		}

		template<class T>
		template<typename D>
		inline void Handle<T>::Copy(const Handle<D>& copy)
		{
			_bIsStoringID = copy._bIsStoringID;
			if (_bIsStoringID)
			{
				_storedID = copy._storedID;
			}
			else
			{
				IncrementHRef(reinterpret_cast<HandleInternal<T>*>(copy._hRef));
				DecrementHRef(_hRef);
				_hRef = reinterpret_cast<HandleInternal<T>*>(copy._hRef);
			}
		}

#pragma endregion

}
}
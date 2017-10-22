#pragma once

#include <map>
#include <vector>
#include <string>

#include "TypeIterator.h"

#include "../core/GlobalDefines.h"
#include "../core/Singleton.h"
#include "../utility/MString.h"

#include "../memoryManagement/AllocatorCollection.h"
#include "../memoryManagement/ObjectPoolAllocator.h"
#include "../memoryManagement/MemoryManager.h"
#include "../memoryManagement/Handle.h"
#include "../gom/ObjectInitializer.h"

namespace morphEngine
{
	using namespace utility;

	namespace gom
	{
		class BaseObject;
	}

	//template <> class memoryManagement::Handle<gom::BaseObject>;

	namespace reflection
	{
		template<typename T>
		void CreateInstance(const gom::ObjectInitializer& initializer, memoryManagement::Handle<gom::BaseObject>& outObject)
		{
			outObject = T::GetType()->GetAllocator()->GetFirstFreeAllocator(T::GetType()->GetSize())->NewObject<T>(initializer);
		}

		template<typename T>
		void CloneInstance(memoryManagement::Handle<gom::BaseObject> handle, memoryManagement::Handle<gom::BaseObject>& outObject, bool bDeepCopy = true)
		{
			outObject = T::GetType()->GetAllocator()->GetFirstFreeAllocator(T::GetType()->GetSize())->CloneObject<T>(handle, bDeepCopy);
		}

		template<typename T>
		void DestroyInstance(memoryManagement::Handle<gom::BaseObject> handle)
		{
			void* ptr = reinterpret_cast<void*>(handle.GetPointer());
			T::GetType()->GetAllocator()->GetAllocatorByAddress(ptr)->DestroyObject<T>(handle);
		}

		class Assembly;

		class Type
		{
			friend class memoryManagement::ObjectPoolAllocator;
			friend class TypeIterator;
			friend class TypeIteratorPolymorphic;
			friend class Assembly;
		protected:
			typedef void(*CreateInstanceFunctionPointer)(const gom::ObjectInitializer&, memoryManagement::Handle<gom::BaseObject>&);
			typedef void(*DestroyInstanceFunctionPointer)(memoryManagement::Handle<gom::BaseObject>);
			typedef void(*CloneInstanceFunctionPointer)(memoryManagement::Handle<gom::BaseObject>, memoryManagement::Handle<gom::BaseObject>&, bool);
			
			MString _name;
			MArray<Type*> _children;
			MArray<Type*> _polymorphicArray;
			MSize _size;
			CreateInstanceFunctionPointer _createInstanceFunction;
			DestroyInstanceFunctionPointer _destroyInstanceFunction;
			CloneInstanceFunctionPointer _cloneInstanceFunction;
			Type* _parent;
			memoryManagement::AllocatorCollection<memoryManagement::ObjectPoolAllocator>* _allocator;
			MUint8 _alignment;

			void Initialize();
			void ConstructPolymorphicArray();

		public:
			Type(MSize size, MUint8 alignment, const MString& name, CreateInstanceFunctionPointer creator, DestroyInstanceFunctionPointer destroyer, CloneInstanceFunctionPointer cloner, const Type* parent);
			inline Type(const Type& other) : Type(other._size, other._alignment, other._name, other._createInstanceFunction, other._destroyInstanceFunction, other._cloneInstanceFunction, other._parent)
			{
				_allocator = other._allocator;
				_parent->AddChild(this);
			}
			virtual inline ~Type()
			{
				_parent = nullptr;
			}

			inline void CreateInstance(const gom::ObjectInitializer& initializer, memoryManagement::Handle<gom::BaseObject>& outObject) const
			{
				ME_ASSERT(_createInstanceFunction != nullptr, "CreateInstanceFunctionPointer for this type is null");
				_createInstanceFunction(initializer, outObject);
			}

			inline void CloneInstance(memoryManagement::Handle<gom::BaseObject> instanceHandle, memoryManagement::Handle<gom::BaseObject>& outObject, bool bDeepCopy = true) const
			{
				ME_ASSERT(_cloneInstanceFunction != nullptr, "CloneInstanceFunctionPointer for this type is null");
				_cloneInstanceFunction(instanceHandle, outObject, bDeepCopy);
			}

			inline void DestroyInstance(memoryManagement::Handle<gom::BaseObject> instanceHandle) const
			{
				ME_ASSERT(_destroyInstanceFunction != nullptr, "DestroyInstanceFunctionPointer for this type is null");
				_destroyInstanceFunction(instanceHandle);
			}


			inline void AddChild(Type* type) { _children.Add(type); }

			inline MSize GetSize() const { return _size; }
			inline MUint8 GetAlignment() const { return _alignment; }
			inline memoryManagement::AllocatorCollection<memoryManagement::ObjectPoolAllocator>* GetAllocator() const { return _allocator; }
			inline const MString& GetName() const { return _name; }
			inline Type* GetParentType() const { return _parent; }
			inline const MArray<Type*>& GetChildTypes() const { return _children; }

			inline TypeIterator GetIterator() const { return TypeIterator(this); }
			inline TypeIteratorPolymorphic GetIteratorPolymorphic() const { return TypeIteratorPolymorphic(this); }

			inline void Shutdown()
			{
				_name.Shutdown();
				_children.Shutdown();
				_polymorphicArray.Shutdown();
			}

			bool IsA(const Type* type) const;
			bool IsA(const  MString& typeName) const;

			Type& operator=(const Type& other);
		};

		class Assembly : public core::Singleton<Assembly>
		{
			friend class Type;

		protected:
			std::vector<Type*> _types;

			inline void AddType(Type* type)
			{
				auto it = std::find(_types.begin(), _types.end(), type);
				if(it == _types.end())
				{
					_types.push_back(type);
				}
			}

			inline void RemoveType(Type* type)
			{
				auto it = std::find(_types.begin(), _types.end(), type);
				if(it != _types.end())
				{
					_types.erase(it);
				}

				if(_types.size() == 0)
				{
					DestroyInstance();
				}
			}

		public:
			inline const Type* GetType(MString name) const
			{
				auto it = _types.begin();
				auto end = _types.end();
				for(it; it != end; ++it)
				{
					if((*it) != nullptr && ((*it)->GetName() == name))
					{
						return (*it);
					}
				}

				return nullptr;
			}

			inline const std::vector<Type*>& GetTypes() const
			{
				return _types;
			}

			inline void Initialize()
			{
				auto it = _types.begin();
				auto end = _types.end();

				for (it; it != end; ++it)
				{
					(*it)->Initialize();
				}

				it = _types.begin();

				for (it; it != end; ++it)
				{
					(*it)->ConstructPolymorphicArray();
				}
			}

			inline void Shutdown()
			{
				auto it = _types.begin();
				auto end = _types.end();
				for(it; it != end; ++it)
				{
					(*it)->Shutdown();
				}
				_types.clear();
			}
		};
	}
}

#define GetTypeIter(type) type::Type.GetIterator();
#define GetTypeIterPoly(type) type::GetType()->GetIteratorPolymorphic();

#define ME_TYPE \
friend class morphEngine::memoryManagement::ObjectPoolAllocator; \
private: \
static const morphEngine::reflection::Type Type; \
public: \
inline static const morphEngine::reflection::Type* GetType() { return &Type; } \
inline static const MString& GetTypeName() {return Type.GetName(); } \
private:

#define REGISTER_ME_TYPE(type, parentType) const morphEngine::reflection::Type type::Type(sizeof(type), alignof(type), #type, morphEngine::reflection::CreateInstance<type>, morphEngine::reflection::DestroyInstance<type>, morphEngine::reflection::CloneInstance<type>, parentType)
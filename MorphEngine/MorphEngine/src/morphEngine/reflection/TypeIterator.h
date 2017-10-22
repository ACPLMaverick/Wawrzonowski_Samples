#pragma once

#include "core/GlobalDefines.h"
#include "utility/MString.h"
#include "utility/MArray.h"
using namespace morphEngine::utility;

namespace morphEngine
{
	namespace reflection
	{
		class Type;

		class TypeIterator
		{
			friend class Type;
		protected:

			MSize _allocIndex;
			MSize _allocCollectionIndex;
			MSize _typeIndex;
			Type* _type;
			MUint8* _dataPtr;
			bool _bIsSomethingAllocated;
			bool _bReachedFinish;
			bool _bReachedStart;

			TypeIterator(const Type* type);

			virtual inline Type* GetCurrentType() { return _type; }

		public:
			TypeIterator(const TypeIterator& c);
			virtual ~TypeIterator();

			template <typename T> inline T* Get() 
			{ 
				ME_ASSERT(T::GetType() == _type, "TypeIterator: Calling get for a wrong type.")
				return reinterpret_cast<T*>(_dataPtr);
			}

			bool IsValid();

			void Rewind();

			virtual TypeIterator& operator=(const TypeIterator& c);

			virtual TypeIterator& operator++();
			virtual TypeIterator& operator--();
			virtual TypeIterator& operator+=(MSize i);
			virtual TypeIterator& operator-=(MSize i);

			bool operator==(const TypeIterator& rhs);
			bool operator!=(const TypeIterator& rhs);
			virtual bool operator>=(const TypeIterator& rhs);
			virtual bool operator<=(const TypeIterator& rhs);
			virtual bool operator>(const TypeIterator& rhs);
			virtual bool operator<(const TypeIterator& rhs);
		};

		class TypeIteratorPolymorphic : public TypeIterator
		{
			friend class Type;
		protected:

			TypeIteratorPolymorphic(const Type* type);

		public:

			TypeIteratorPolymorphic(const TypeIteratorPolymorphic& c);
			virtual ~TypeIteratorPolymorphic();

			virtual TypeIteratorPolymorphic& operator++();
			virtual TypeIteratorPolymorphic& operator--();
		};
	}
}

#define GetTypeIterPtr(type, iterator) iterator.Get<type>();
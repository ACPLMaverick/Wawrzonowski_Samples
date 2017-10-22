#include "TypeIterator.h"
#include "Type.h"

namespace morphEngine
{
	namespace reflection
	{
#pragma region TypeIterator

		TypeIterator::TypeIterator(const Type * type) :
			_allocIndex(0),
			_allocCollectionIndex(0),
			_typeIndex(0),
			_type((Type*)type),
			_dataPtr(reinterpret_cast<MUint8*>(_type->_allocator->_allocArray[0]->_start)),
			_bIsSomethingAllocated(false),
			_bReachedStart(false),
			_bReachedFinish(false)
		{
			for (auto it = _type->_polymorphicArray.GetIterator(); it.IsValid(); ++it)
			{
				if ((*it)->_allocator->_allocArray[0]->GetNumAllocations() != 0)
				{
					_bIsSomethingAllocated = true;
					_dataPtr = reinterpret_cast<MUint8*>(_type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[0]->_start);
					break;
				}
				++_typeIndex;
			}
		}

		TypeIterator::TypeIterator(const TypeIterator & c) :
			_allocIndex(c._allocIndex),
			_allocCollectionIndex(c._allocCollectionIndex),
			_typeIndex(c._typeIndex),
			_bIsSomethingAllocated(c._bIsSomethingAllocated),
			_type(c._type),
			_dataPtr(c._dataPtr),
			_bReachedStart(c._bReachedFinish)
		{
		}

		TypeIterator::~TypeIterator()
		{
		}

		bool TypeIterator::IsValid()
		{
			return
				!_bReachedStart && !_bReachedFinish && _bIsSomethingAllocated;
		}

		void TypeIterator::Rewind()
		{
			_allocIndex = 0;
			_allocCollectionIndex = 0;
			_typeIndex = 0;
			_dataPtr = reinterpret_cast<MUint8*>(_type->_polymorphicArray[0]->_allocator->_allocArray[0]->_start);

			for (auto it = _type->_polymorphicArray.GetIterator(); it.IsValid(); ++it)
			{
				if ((*it)->_allocator->_allocArray[0]->GetNumAllocations() != 0)
				{
					_bIsSomethingAllocated = true;
					_dataPtr = reinterpret_cast<MUint8*>(_type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[0]->_start);
					break;
				}
				++_typeIndex;
			}

			_bReachedFinish = false;
			_bReachedStart = false;
		}

		TypeIterator & TypeIterator::operator=(const TypeIterator & c)
		{
			_allocIndex = c._allocIndex;
			_allocCollectionIndex = c._allocCollectionIndex;
			_typeIndex = c._typeIndex;
			_type = c._type;
			_dataPtr = c._dataPtr;
			_bReachedStart = c._bReachedStart;
			_bReachedFinish = c._bReachedFinish;
			_bIsSomethingAllocated = c._bIsSomethingAllocated;

			return *this;
		}

		TypeIterator & TypeIterator::operator++()
		{
			if (!_bReachedFinish &&
				_type->_polymorphicArray[_typeIndex]->_allocator->GetNumAllocations() != 0 &&
				_type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[_allocCollectionIndex]->GetNumAllocations() != 0
				)
			{
				if (_allocIndex < _type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[_allocCollectionIndex]->GetNumAllocations() - 1)
				{
					_dataPtr += _type->_polymorphicArray[_typeIndex]->GetSize();
					++_allocIndex;
				}
				else if (_allocCollectionIndex < _type->_polymorphicArray[_typeIndex]->_allocator->_allocArray.size() - 1 &&
						_type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[++_allocCollectionIndex]->GetNumAllocations() != 0)
				{
					_dataPtr = reinterpret_cast<MUint8*>(_type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[_allocCollectionIndex]->_start);
					_allocIndex = 0;
				}
				else
				{
					_bReachedFinish = true;
				}
			}
			else
			{
				_bReachedFinish = true;
			}

			return *this;
		}

		TypeIterator & TypeIterator::operator--()
		{
			if (!_bReachedStart &&
				_type->_polymorphicArray[_typeIndex]->_allocator->GetNumAllocations() != 0 &&
				(_allocIndex != 0 || _allocCollectionIndex != 0) )
			{
				if (_allocIndex == 0 && _allocCollectionIndex != 0)
				{
					--_allocCollectionIndex;
					_allocIndex = _type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[_allocCollectionIndex]->GetNumAllocations() - 1;	// let's say num allocations can't be zero here.
					_dataPtr = reinterpret_cast<MUint8*>(_type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[_allocCollectionIndex]->_start) + _allocIndex * _type->GetSize();
				}
				else
				{
					--_allocIndex;
					_dataPtr -= _type->GetSize();
				}
			}
			else
			{
				_bReachedStart = true;
			}

			return *this;
		}

		TypeIterator & TypeIterator::operator+=(MSize ind)
		{
			for (MSize i = 0; i < ind; ++i)
			{
				operator++();
			}
			return *this;
		}

		TypeIterator & TypeIterator::operator-=(MSize ind)
		{
			for (MSize i = 0; i < ind; ++i)
			{
				operator--();
			}
			return *this;
		}

		bool TypeIterator::operator==(const TypeIterator & rhs)
		{
			return _dataPtr == rhs._dataPtr;
		}

		bool TypeIterator::operator!=(const TypeIterator & rhs)
		{
			return _dataPtr != rhs._dataPtr;
		}

		bool TypeIterator::operator>=(const TypeIterator & rhs)
		{
			return !operator<(rhs);
		}

		bool TypeIterator::operator<=(const TypeIterator & rhs)
		{
			return !operator>(rhs);
		}

		bool TypeIterator::operator>(const TypeIterator & rhs)
		{
			return (_typeIndex > rhs._typeIndex) || (_typeIndex == rhs._typeIndex && _allocCollectionIndex > rhs._allocCollectionIndex) ||
				(_typeIndex == rhs._typeIndex && _allocCollectionIndex == rhs._allocCollectionIndex && _allocIndex > rhs._allocIndex);
		}

		bool TypeIterator::operator<(const TypeIterator & rhs)
		{
			return (_typeIndex < rhs._typeIndex) || (_typeIndex == rhs._typeIndex && _allocCollectionIndex < rhs._allocCollectionIndex) ||
				(_typeIndex == rhs._typeIndex && _allocCollectionIndex == rhs._allocCollectionIndex && _allocIndex < rhs._allocIndex);
		}

#pragma endregion

#pragma region TypeIteratorExclusive

		TypeIteratorPolymorphic::TypeIteratorPolymorphic(const Type * type) :
			TypeIterator((Type*)type)
		{
		}

		TypeIteratorPolymorphic::TypeIteratorPolymorphic(const TypeIteratorPolymorphic & c) :
			TypeIterator(c)
		{
		}

		TypeIteratorPolymorphic::~TypeIteratorPolymorphic()
		{
		}

		TypeIteratorPolymorphic & TypeIteratorPolymorphic::operator++()
		{
			if (!_bReachedFinish &&
				_typeIndex < _type->_polymorphicArray.GetSize() - 1 &&
				_allocCollectionIndex == _type->_polymorphicArray[_typeIndex]->_allocator->_allocArray.size() - 1 && 
				_type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[_allocCollectionIndex]->GetNumAllocations() > 0 &&
				_allocIndex == _type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[_allocCollectionIndex]->GetNumAllocations() - 1)
			{
				MSize tempIndex = _typeIndex;
				while (tempIndex < _type->_polymorphicArray.GetSize())
				{
					++tempIndex;
					if (_type->_polymorphicArray[tempIndex]->_allocator->GetNumAllocations() != 0)
					{
						break;
					}
				}
				if (_typeIndex != tempIndex && tempIndex < _type->_polymorphicArray.GetSize())
				{
					_typeIndex = tempIndex;
					_allocCollectionIndex = 0;
					_allocIndex = 0;

					_dataPtr = reinterpret_cast<MUint8*>(_type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[_allocCollectionIndex]->_start);
				}
				else
				{
					_bReachedFinish = true;
				}
			}
			else
			{
				TypeIterator::operator++();
			}

			return *this;
		}

		TypeIteratorPolymorphic & TypeIteratorPolymorphic::operator--()
		{
			if (!_bReachedStart &&
				_typeIndex != 0 &&
				_allocCollectionIndex == 0 && 
				_allocIndex == 0)
			{
				MSize tempIndex = _typeIndex;
				while (tempIndex != 0)
				{
					--tempIndex;
					if (_type->_polymorphicArray[tempIndex]->_allocator->GetNumAllocations() != 0)
					{
						break;
					}
				}
				if (_typeIndex != tempIndex && tempIndex < _type->_polymorphicArray.GetSize())
				{
					_typeIndex = tempIndex;
					_allocCollectionIndex = _type->_polymorphicArray[_typeIndex]->_allocator->_allocArray.size() - 1;
					_allocIndex = _type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[_allocCollectionIndex]->GetNumAllocations() - 1;

					_dataPtr = reinterpret_cast<MUint8*>(_type->_polymorphicArray[_typeIndex]->_allocator->_allocArray[_allocCollectionIndex]->_start) + _allocIndex;
				}
				else
				{
					_bReachedStart = true;
				}
			}
			else
			{
				TypeIterator::operator--();
			}

			return *this;
		}

#pragma endregion


	}
}
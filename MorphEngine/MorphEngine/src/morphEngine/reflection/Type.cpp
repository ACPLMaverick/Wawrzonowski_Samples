#include "Type.h"
#include "../debugging/Debug.h"
#include "../utility/MStack.h"
using namespace morphEngine::utility;
#include <cstdio>

namespace morphEngine
{
	namespace reflection
	{
		Type::Type(MSize size, MUint8 alignment, const MString & name, CreateInstanceFunctionPointer creator, DestroyInstanceFunctionPointer destroyer, CloneInstanceFunctionPointer cloner, const Type* parent) :
			_size(size), _alignment(alignment), _name(name), _createInstanceFunction(creator), _destroyInstanceFunction(destroyer), _parent((Type*)parent), _cloneInstanceFunction(cloner),
			_allocator(nullptr)
		{
			const char* tmp = name;
			Assembly::GetInstance()->AddType(this);
		}

		bool Type::IsA(const Type* type) const
		{
			if (type == this)
			{
				return true;
			}

			Type* parent = _parent;
			while (parent)
			{
				if (parent == type)
				{
					return true;
				}
				parent = parent->_parent;
			}

			return false;
		}

		bool Type::IsA(const MString& typeName) const
		{
			if (_name == typeName)
			{
				return true;
			}

			Type* parent = _parent;
			while (parent)
			{
				if (parent->_name == typeName)
				{
					return true;
				}
				parent = parent->_parent;
			}

			return false;
		}

		Type& Type::operator=(const Type & other)
		{
			{
				_name = other._name;
				//_createInstanceFunction = other._createInstanceFunction;
				_parent = other._parent;
				Assembly::GetInstance()->AddType(this);

				return *this;
			}
		}

		void Type::Initialize()
		{
			_allocator = memoryManagement::MemoryManager::GetInstance()->RegisterType(this);

			if (_parent != nullptr)
			{
				_parent->AddChild(this);
			}
		}

		void Type::ConstructPolymorphicArray()
		{
			MStack<Type*> stack;
			stack.Push(this);

			while (stack.GetSize() != 0)
			{
				Type* t = stack.Pop();
				_polymorphicArray.Add(t);
				for (auto it = t->_children.GetIterator(); it.IsValid(); ++it)
				{
					stack.Push(*it);
				}
			}
		}

	}
}

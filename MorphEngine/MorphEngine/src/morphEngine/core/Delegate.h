#pragma once

#include "reflection\Type.h"
#include "memoryManagement\Handle.h"
#include "utility/MArray.h"

namespace morphEngine
{
	namespace core
	{
		template<typename Return, typename ...Parameters>
		class Event
		{
		public:
#pragma region Inner classes for delegates
			class BaseDelegate
			{
			public:
				inline BaseDelegate() { }
				inline virtual ~BaseDelegate() { }
				
				inline virtual Return operator()(Parameters ... parameters) const = 0;

				inline virtual bool Equals(const BaseDelegate* other) const = 0;
			};

			//Delegate class for global functions (i.e. functions that are not methods of any class)
			class Delegate : public BaseDelegate
			{
			protected:
				typedef Return (*FunctionPointer)(Parameters... parameters);

				FunctionPointer _function;

			public:
				inline Delegate(FunctionPointer function) { _function = function; }
				inline virtual ~Delegate() { _function = 0; }
				inline virtual Return operator()(Parameters ... parameters) const
				{
					return _function(parameters...);
				}

				inline virtual bool Equals(const BaseDelegate* other) const
				{
					const Delegate* d = reinterpret_cast<const Delegate*>(other);
					
					if (d == nullptr)
					{
						return false;
					}

					return d->_function == _function;
				}
			};

			//Delegate class for classes' methods
			//NOTE: Only for classes that has handle (_this for example).
			//DO NOT PASS RAW POINTER TO CONSTRUCTOR (it will generate some problems...)
			template<typename Class>
			class ClassDelegate : public BaseDelegate
			{
			protected:
				typedef Return(Class::*FunctionPointer)(Parameters... parameters);
				memoryManagement::Handle<Class> _handle;
				FunctionPointer _function;

			public:
				inline ClassDelegate(const memoryManagement::Handle<Class>& object, FunctionPointer function) : _handle(object), _function(function)
				{

				}

				inline virtual ~ClassDelegate()
				{}

				inline virtual Return operator()(Parameters... parameters) const
				{
					if(!_handle.IsValid())
					{
						return Return();
					}
					Class* object = _handle.GetPointer();
					return (object->*_function)(parameters...);
				}

				inline virtual bool Equals(const BaseDelegate* other) const
				{
					const ClassDelegate* d = reinterpret_cast<const ClassDelegate*>(other);
					if(d == nullptr)
					{
						return false;
					}

					return d->_function == _function && d->_handle == _handle;
				}
			};

			//Delegate class for classes' methods
			//NOTE: For classes that don't have _this handle (Engine, WindowWin32, etc.) and have to use delegates with raw pointer
			//DO NOT PASS HANDLE TO CONSTRUCTOR
			template<typename Class>
			class ClassRawDelegate : public BaseDelegate
			{
			protected:
				typedef Return(Class::*FunctionPointer)(Parameters... parameters);
				Class* _object;
				FunctionPointer _function;

			public:
				inline ClassRawDelegate(Class* object, FunctionPointer function) : _function(function), _object(object) { }
				inline virtual ~ClassRawDelegate() { }

				inline virtual Return operator()(Parameters... parameters) const
				{
					return (_object->*_function)(parameters...);
				}

				inline virtual bool Equals(const BaseDelegate* other) const
				{
					const ClassRawDelegate* d = reinterpret_cast<const ClassRawDelegate*>(other);
					if (d == nullptr)
					{
						return false;
					}

					return d->_function == _function && d->_object == _object;
				}
			};

#pragma endregion

		protected:
#pragma region Procted variables
			utility::MArray<const BaseDelegate*> _delegates;
			bool _bHasCopyBeenMade = false;
#pragma endregion

		public:
			inline Event() { }

			inline Event(const Event& c) : 
				_delegates(c._delegates)
			{
				const_cast<Event&>(c)._bHasCopyBeenMade = true;
			}

			~Event()
			{
				Shutdown();
			}

			inline void Shutdown()
			{
				if (!_bHasCopyBeenMade)
				{
					MSize size = _delegates.GetSize();
					for (MSize i = 0; i < size; ++i)
					{
						delete (_delegates[i]);
					}
				}
				_delegates.Shutdown();
			}

			inline const utility::MArray<const BaseDelegate*>& GetDelegates() const { return _delegates; }

#pragma region Overloaded operators for adding and removing delegates and for invoking event

			inline void operator +=(const BaseDelegate* delegatePtr)
			{
				_delegates.Add(delegatePtr);
				const BaseDelegate* delegateAdded = _delegates[_delegates.GetSize() - 1];
				if(delegateAdded != delegatePtr)
				{
					delete delegatePtr;
				}
			}

			inline void operator-=(const BaseDelegate* delegatePtr)
			{
				if (delegatePtr == nullptr)
				{
					return;
				}

				MSize size = _delegates.GetSize();
				
				for (MSize i = 0; i < size; ++i)
				{
					const BaseDelegate* bd = _delegates[i];
					if(delegatePtr->Equals(bd))
					{
						//Compare adresses and if they are equal then return after deleting (*it) to avoid null pointer deletion after for
						bool areEqual = delegatePtr == bd;
						delete bd;
						_delegates.Remove(bd);
						if (areEqual)
						{
							return;
						}
						break;
					}
				}

				delete delegatePtr;
				return;
			}

			inline bool operator==(const Event<Return, Parameters ...>& other) const
			{
				return this == &other;
			}

			inline bool operator!=(const Event<Return, Parameters ...>& other) const
			{
				return !operator==(other);
			}

			inline Return operator()(Parameters ... parameters)
			{
				MSize size = _delegates.GetSize();
				for (MSize i = 0; i < size; ++i)
				{
					const BaseDelegate* bd = _delegates[i];
					ME_WARNING(bd != nullptr, "Null pointer detected in event's delegates collection");
					if (bd != nullptr)
					{
						if (i == size - 1)
						{
							return (*bd)(parameters...);
						}
						else
						{
							(*bd)(parameters...);
						}
					}
				}
			}

#pragma endregion
		};
	}
}
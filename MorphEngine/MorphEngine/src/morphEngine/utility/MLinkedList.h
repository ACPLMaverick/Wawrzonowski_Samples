#pragma once

#include "utility/MCollection.h"

namespace morphEngine
{
	namespace utility
	{

#pragma region Structs

		template <class T>
		class MLinkedList;

		template <class T>
		struct MLinkedListNode
		{
			friend class MLinkedList<T>;

			T _obj;
			MLinkedListNode* _next;

			MLinkedListNode(const T& obj, MLinkedListNode* nxt) :
				_obj(obj),
				_next(nxt)
			{

			}

			virtual const T& operator * () const
			{
				return _obj;
			}

			virtual T& operator * ()
			{
				return _obj;
			}

			void* operator new(MSize size)
			{
				return MCollectionHelper::AllocateL(size, alignof(MLinkedListNode<T>));
			}
			void operator delete(void* p, MSize size)
			{
				MCollectionHelper::DeallocateL(p);
			}
		};

#pragma endregion

		template <class T>
		class MLinkedList :
			public MCollection<MLinkedListNode<T>>
		{
		protected:

#pragma region Protected

			MSize _size;
			MLinkedListNode<T>* _first;

#pragma endregion

#pragma region Functions Protected

			void CopyAllNodes(const MLinkedList<T>& c);
			MLinkedListNode<T>* GetLastNode();
			void GetNeighbouringNodes(MSize position, MLinkedListNode<T>*& one, MLinkedListNode<T>*& two);

#pragma endregion

		public:

#pragma region Classes Public

			class MIteratorLinkedList : public MIteratorBase
			{
				friend class MLinkedList<T>;
			protected:
				MLinkedListNode<T>* _dataPtr;
				const MLinkedList<T>& _list;


				MIteratorLinkedList(const MLinkedList<T>& ref, MLinkedListNode<T>* data) :
					MIteratorBase(),
					_dataPtr(data),
					_list(ref)
				{

				}

			public:

				MIteratorLinkedList(const MIteratorLinkedList& c) :
					MIteratorBase(c),
					_dataPtr(c._dataPtr),
					_list(c._list)
				{

				}

				virtual ~MIteratorLinkedList()
				{
				}

				virtual bool IsValid() const override
				{
					return _dataPtr != nullptr;
				}

				virtual bool operator==(const MIteratorLinkedList& other) const
				{
					return _dataPtr == other._dataPtr;
				}

				virtual bool operator!=(const MIteratorLinkedList& other) const
				{
					return _dataPtr != other._dataPtr;
				}

				virtual const MLinkedListNode<T>& operator * () const
				{
					ME_ASSERT(IsValid(), "MLinkedList: Used dereference operator on invalid iterator.");
					return *_dataPtr;
				}

				virtual MLinkedListNode<T>& operator * ()
				{
					ME_ASSERT(IsValid(), "MLinkedList: Used dereference operator on invalid iterator.");
					return *_dataPtr;
				}

				virtual MIteratorLinkedList operator+ (const MSize& rhs) const
				{
					MIteratorLinkedList result(*this);
					result += rhs;
					return result;
				}

				virtual MIteratorLinkedList& operator+= (const MSize& rhs)
				{
					for (int i = 0; i < rhs; ++i)
					{
						++_dataPtr;
						if (!IsValid())
						{
							break;
						}
					}
					return *this;
				}

				MIteratorLinkedList& operator++()
				{
					ME_ASSERT(_dataPtr != nullptr, "MLinkedList: Incremented an invalid iterator.");
					_dataPtr = _dataPtr->_next;

					return *this;
				}

				MIteratorLinkedList operator++(MInt32)
				{
					MIteratorLinkedList result(*this);
					++(*this);
					return result;
				}

			};

#pragma endregion

#pragma region Functions Public

			MLinkedList();
			MLinkedList(const MLinkedList<T>& c);
			~MLinkedList();

			virtual void Add(const T& obj);
			virtual void Add(const T& obj, MSize position);
			virtual void Remove(const T& obj);
			virtual void Remove(MSize position);
			virtual void Clear() override;

			virtual bool IsEmpty() const override;
			virtual MSize GetSize() const override;
			virtual MSize GetCapacity() const override;

			virtual MIteratorLinkedList GetIterator() const;

			virtual bool Contains(const MLinkedListNode<T>& node) const override;
			virtual bool Contains(const T& obj) const;

			MLinkedList<T>& operator=(const MLinkedList<T>& c);
			virtual T operator[](const MSize index) const;
			virtual T& operator[](const MSize index);

#pragma endregion

		};

#pragma region Definitions

		template <class T> MLinkedList<T>::MLinkedList() :
			MCollection<MLinkedListNode<T>>(0, GrowthStrategy::NO_GROW, ShrinkStrategy::NO_SHRINK),
			_size(0),
			_first(nullptr)
		{
			_type = CollectionType::LINKED_LIST;
		}

		template<class T>
		inline MLinkedList<T>::MLinkedList(const MLinkedList<T>& c) :
			MLinkedList<T>(),
			_size(c._size)
		{
		}


		template <class T> MLinkedList<T>::~MLinkedList()
		{
			Clear();
		}

		template<class T>
		inline void MLinkedList<T>::Add(const T & obj)
		{
			MLinkedListNode<T>* last = GetLastNode();

			if (last != nullptr)
			{
				last->_next = new MLinkedListNode<T>(obj, nullptr);
			}
			else
			{
				_first = new MLinkedListNode<T>(obj, nullptr);
			}

			++_size;
		}

		template<class T>
		inline void MLinkedList<T>::Add(const T & obj, MSize position)
		{
			MLinkedListNode<T>* prev = nullptr;
			MLinkedListNode<T>* nxt = nullptr;
			GetNeighbouringNodes(position, prev, nxt);

			MLinkedListNode<T>* node = new MLinkedListNode<T>(obj, nxt);

			if (nxt == nullptr)
			{
				_first = node;
			}

			if (prev != nullptr)
			{
				prev->_next = node;
			}
			++_size;
		}

		template<class T>
		inline void MLinkedList<T>::Remove(const T & obj)
		{
			MLinkedListNode<T>* node = _first;
			MLinkedListNode<T>* prev = nullptr;
			while (node != nullptr)
			{
				if (node->_obj == obj)
				{
					prev->_next = node->_next;
					delete node;
				}

				prev = node;
				node = node->_next;
			}

			--_size;

			if (_size == 0)
			{
				_first = nullptr;
			}
		}

		template<class T>
		inline void MLinkedList<T>::Remove(MSize position)
		{
			ME_ASSERT(position < _size, "MLinkedList: index overflow.");

			MLinkedListNode<T>* node = _first;
			MLinkedListNode<T>* prev = nullptr;
			for(MSize i = 0; i < position; ++i)
			{
				prev = node;
				node = node->_next;
			}

			if (prev != nullptr)
			{
				prev->_next = node->_next;
			}

			if (node == _first)
			{
				_first = _first->_next;
			}

			--_size;
			delete node;

			if (_size == 0)
			{
				_first = nullptr;
			}
		}

		template<class T>
		inline void MLinkedList<T>::Clear()
		{
			MLinkedListNode<T>* node = _first;
			MLinkedListNode<T>* temp = nullptr;
			while (node != nullptr)
			{
				temp = node;
				node = node->_next;
				delete temp;
				--_size;
			}

			_first = nullptr;
			ME_ASSERT(_size == 0, "MLinkedList: Clear has deleted incorrect number of nodes.");
		}

		template<class T>
		inline bool MLinkedList<T>::IsEmpty() const
		{
			return _size == 0;
		}

		template<class T>
		inline MSize MLinkedList<T>::GetSize() const
		{
			return _size;
		}

		template<class T>
		inline MSize MLinkedList<T>::GetCapacity() const
		{
			return -1;
		}

		template<class T>
		inline typename MLinkedList<T>::MIteratorLinkedList MLinkedList<T>::GetIterator() const
		{
			return MIteratorLinkedList(*this, _first);
		}

		template<class T>
		inline bool MLinkedList<T>::Contains(const MLinkedListNode<T>& nodeOther) const
		{
			MLinkedListNode<T>* node = _first;
			while (node != nullptr)
			{
				if (node == &nodeOther)
				{
					return true;
				}
				node = node->_next;
			}
			return false;
		}

		template<class T>
		inline bool MLinkedList<T>::Contains(const T & obj) const
		{
			MLinkedListNode<T>* node = _first;
			while (node != nullptr)
			{
				if (node->_obj == obj)
				{
					return true;
				}
				node = node->_next;
			}
			return false;
		}

		template<class T>
		inline MLinkedList<T>& MLinkedList<T>::operator=(const MLinkedList<T>& c)
		{
			CopyAllNodes(c);
		}

		template<class T>
		inline T MLinkedList<T>::operator[](const MSize index) const
		{
			ME_ASSERT(index < _size, "MLinkedList: index overflow.");
			MLinkedListNode<T>* node = _first;
			for (MSize i = 0; i < index; ++i)
			{
				node = node->_next;
			}
			return node->_obj;
		}

		template<class T>
		inline T & MLinkedList<T>::operator[](const MSize index)
		{
			ME_ASSERT(index < _size, "MLinkedList: index overflow.");
			MLinkedListNode<T>* node = _first;
			for (MSize i = 0; i < index; ++i)
			{
				node = node->_next;
			}
			return node->_obj;
		}

		template<class T>
		inline void MLinkedList<T>::CopyAllNodes(const MLinkedList<T>& c)
		{
			MLinkedListNode<T>* node = c._first;
			MLinkedListNode<T>* prev = nullptr;
			
			if (node != nullptr)
			{
				_first = new MLinkedListNode<T>(node->_obj, nullptr);
				prev = _first;
				node = node->_next;
			}

			while (node != nullptr)
			{
				MLinkedListNode<T>* newNode = new MLinkedListNode<T>(node->_obj, nullptr);
				prev->_next = newNode;
				prev = newNode;
				node = node->_next;
			}
		}

		template<class T>
		inline MLinkedListNode<T>* MLinkedList<T>::GetLastNode()
		{
			if (_first == nullptr)
				return _first;

			MLinkedListNode<T>* last = _first;

			while (last->_next != nullptr)
			{
				last = last->_next;
			}

			return last;
		}

		template<class T>
		inline void MLinkedList<T>::GetNeighbouringNodes(MSize position, MLinkedListNode<T>*& one, MLinkedListNode<T>*& two)
		{
			ME_ASSERT(position <= _size, "MLinkedList: index overflow.");

			two = nullptr;
	
			one = _first;
			if (position != 0)
			{
				for (MSize i = 0; i < position - 1; ++i)
				{
					one = one->_next;
				}
			}

			if (one != nullptr)
			{
				two = one->_next;
			}
		}

#pragma endregion

	}
}
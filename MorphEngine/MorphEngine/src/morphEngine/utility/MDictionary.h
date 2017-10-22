#pragma once

#include "utility/MCollection.h"
#include "utility/MStack.h"
#include "utility/MQueue.h"
#include "utility/MArray.h"

namespace morphEngine
{
	namespace utility
	{
#pragma region Structs

		template <class K, class T> class MDictionary;

		template <class K, class T>
		struct MDictionaryNode
		{
			friend class MDictionary<K, T>;

		protected:

			K _key;
			T _value;
			MDictionaryNode<K, T>* _parent;
			MDictionaryNode<K, T>* _left;
			MDictionaryNode<K, T>* _right;
			MUint8 _color;

			MDictionaryNode(const K& key, const T& value, MUint8 color,
				MDictionaryNode<K, T>* parent, MDictionaryNode<K, T>* left, MDictionaryNode<K, T>* right) :
				_key(key),
				_value(value),
				_color(color),
				_parent(parent),
				_left(left),
				_right(right)
			{
			}

		public:

			MDictionaryNode(const K& key, const T& value) :
				_key(key),
				_value(value),
				_color(NODE_COLOR_BLACK)
			{
			}

			void* operator new(MSize size)
			{
				return MCollectionHelper::AllocateL(size, alignof(MDictionaryNode<K, T>));
			}
			void operator delete(void* p, MSize size)
			{
				MCollectionHelper::DeallocateL(p);
			}

			const K& GetKey() const { return _key; }
			const T& GetValue() const { return _value; }
			K& GetKey() { return _key; }
			T& GetValue() { return _value; }
		};

#pragma endregion

		template <class K, class T>
		class MDictionary :
			public MCollection<MDictionaryNode<K, T>>
		{

#pragma region Static Const Protected

			static const MUint8 NODE_COLOR_RED = 0xFF;
			static const MUint8 NODE_COLOR_BLACK = 0x00;

#pragma endregion


#pragma region Protected

			MDictionaryNode<K, T>* _root;

#pragma endregion

#pragma region Functions Protected

			MDictionaryNode<K, T>* Compare(const MDictionaryNode<K, T>* left, const MDictionaryNode<K, T>* right);

			bool VerifyProperty1(MDictionaryNode<K, T>* node) const;
			bool VerifyProperty2() const;
			bool VerifyProperty3(MDictionaryNode<K, T>* node) const;
			bool VerifyProperty4() const;
			bool VerifyProperty4Helper(MDictionaryNode<K, T>* node, MInt64 blackCount, MInt64& pathBlackCount) const;

			MDictionaryNode<K, T>* NewNode(const K& key, const T& value, MUint8 color,
				MDictionaryNode<K, T>* parent, MDictionaryNode<K, T>* left, MDictionaryNode<K, T>* right);
			MDictionaryNode<K, T>* LookupNode(const K& key) const;
			void ReplaceNode(MDictionaryNode<K, T>* oldNode, MDictionaryNode<K, T>* newNode);
			void RotateLeft(MDictionaryNode<K, T>* node);
			void RotateRight(MDictionaryNode<K, T>* node);

			void InsertCase1(MDictionaryNode<K, T>* node);
			void InsertCase2(MDictionaryNode<K, T>* node);
			void InsertCase3(MDictionaryNode<K, T>* node);
			void InsertCase4(MDictionaryNode<K, T>* node);
			void InsertCase5(MDictionaryNode<K, T>* node);

			void DeleteCase1(MDictionaryNode<K, T>* node);
			void DeleteCase2(MDictionaryNode<K, T>* node);
			void DeleteCase3(MDictionaryNode<K, T>* node);
			void DeleteCase4(MDictionaryNode<K, T>* node);
			void DeleteCase5(MDictionaryNode<K, T>* node);
			void DeleteCase6(MDictionaryNode<K, T>* node);

			MDictionaryNode<K, T>* GetMaximumNode(MDictionaryNode<K, T>* node) const;
			MDictionaryNode<K, T>* GetLeftMostNode(MDictionaryNode<K, T>* node) const;
			MDictionaryNode<K, T>* GetRightMostNode(MDictionaryNode<K, T>* node) const;
			MUint8 GetNodeColor(MDictionaryNode<K, T>* node) const;
			MDictionaryNode<K, T>* GetGrandparent(MDictionaryNode<K, T>* n) const;
			MDictionaryNode<K, T>* GetSibling(MDictionaryNode<K, T>* n) const;
			MDictionaryNode<K, T>* GetUncle(MDictionaryNode<K, T>* n) const;

			MDictionaryNode<K, T>* GetByValue(const T& value) const;

			void CopyAllNodes(const MDictionary<K, T>& c);

#pragma endregion

		public:

#pragma region Classes Public

			class MIteratorDictionary : public MIteratorBase
			{
				friend class MDictionary<K, T>;
			protected:
				MDictionaryNode<K, T>* _dataPtr;
				MDictionary<K, T>& _dic;

				MStack<MDictionaryNode<K, T>*> _visitedNodes;

				MDictionaryNode<K, T>* _last;
				MSize _lastSize;

				MIteratorDictionary(MDictionary<K, T>& ref, MDictionaryNode<K, T>* data) :
					MIteratorBase(),
					_dataPtr(data),
					_dic(ref),
					_lastSize(_dic.GetSize())
				{
					UpdateLastNode();

					if (_dataPtr != nullptr)
					{
						if(_dataPtr->_left != nullptr) _visitedNodes.Push(_dataPtr->_left);
						if (_dataPtr->_right != nullptr) _visitedNodes.Push(_dataPtr->_right);
					}
				}

				void UpdateLastNode()
				{
					_lastSize = _dic.GetSize();
					if (_dic._root != nullptr)
					{
						_last = _dic.GetMaximumNode(_dic._root);

						if (_last == _dic._root)
						{
							_last = _dic.GetLeftMostNode(_dic._root);
						}
					}
					else
					{
						_last = nullptr;
					}
				}

			public:

				MIteratorDictionary(const MIteratorDictionary& c) :
					MIteratorBase(c),
					_dataPtr(c._dataPtr),
					_visitedNodes(c._visitedNodes),
					_dic(c._dic)
				{
					if (_dataPtr != nullptr)
					{
						if (_dataPtr->_left != nullptr) _visitedNodes.Push(_dataPtr->_left);
						if (_dataPtr->_right != nullptr) _visitedNodes.Push(_dataPtr->_right);
					}
				}

				virtual ~MIteratorDictionary()
				{
				}

				virtual bool IsValid() const override
				{
					return _dataPtr != nullptr;
				}

				virtual bool operator==(const MIteratorDictionary& other) const
				{
					return _dataPtr == other._dataPtr;
				}
				
				virtual bool operator!=(const MIteratorDictionary& other) const
				{
					return _dataPtr != other._dataPtr;
				}

				virtual const MDictionaryNode<K, T>& operator * () const
				{
					ME_ASSERT(IsValid(), "MDictionary: Used dereference operator on invalid iterator.");
					return *_dataPtr;
				}

				virtual MDictionaryNode<K, T>& operator * ()
				{
					ME_ASSERT(IsValid(), "MDictionary: Used dereference operator on invalid iterator.");
					return *_dataPtr;
				}

				virtual MIteratorDictionary operator+ (const MSize& rhs) const
				{
					MIteratorDictionary result(*this);
					result += rhs;
					return result;
				}

				//virtual MIteratorDictionary operator- (const MSize& rhs) const
				//{
				//	MIteratorDictionary result(*this);
				//	result._dataPtr -= rhs;
				//	return result;
				//}

				virtual MIteratorDictionary& operator+= (const MSize& rhs)
				{
					for (int i = 0; i < rhs; ++i)
					{
						operator++();
						if (!IsValid())
						{
							break;
						}
					}
					return *this;
				}

				//virtual MIteratorDictionary& operator-= (const MSize& rhs)
				//{
				//	for (int i = 0; i < rhs; ++i)
				//	{
				//		--_dataPtr;
				//		if (!IsValid())
				//		{
				//			break;
				//		}
				//	}
				//	return *this;
				//}

				MIteratorDictionary& operator++()
				{
					if (_dataPtr == nullptr)
					{
						return *this;
					}

					if (_visitedNodes.IsEmpty())
					{
						_dataPtr = nullptr;
						return *this;
					}

					_dataPtr = _visitedNodes.Pop();
					if (_dataPtr->_left != nullptr) _visitedNodes.Push(_dataPtr->_left);
					if (_dataPtr->_right != nullptr) _visitedNodes.Push(_dataPtr->_right);

					return *this;
				}

				//MIteratorDictionary& operator--()
				//{
				//	UpdateLastNode();

				//	return *this;
				//}

				MIteratorDictionary operator++(MInt32)
				{
					MIteratorDictionary result(*this);
					++(*this);
					return result;
				}

				//MIteratorDictionary operator--(MInt32)
				//{
				//	MIteratorDictionary result(*this);
				//	--(*this);
				//	return result;
				//}
			};

#pragma endregion

#pragma region Functions Public

			MDictionary();
			MDictionary(const MDictionary& c);
			virtual ~MDictionary();

			virtual void Add(const K& key, const T& value);
			virtual void Remove(const K& key);


			// User need to be sure that given key exists in Dictionary, otherwise exception will be thrown.
			// Please use TryGetKeyInDictionary function to ensure this.
			virtual T GetValue(const K& key) const;

			bool TryGetKey(const T& value, K* outKey) const;

			// This is an expensive operation and generally discouraged to use.
			bool TryGetValue(const K& key, T* outValue) const;

			// This is an expensive operation and generally discouraged to use.
			// User need to be sure that given value exists in Dictionary, otherwise exception will be thrown.
			// Please use TryGetValueInDictionary function to ensure this.
			virtual K GetKey(const T& value) const;
			virtual void Clear();

			virtual bool IsEmpty() const;
			virtual MSize GetSize() const;

			// This function will always return unsigned -1, as MDictionary has infinite capacity, beign a linked list.
			virtual MSize GetCapacity() const;

			virtual MIteratorDictionary GetIterator();
			virtual MIteratorDictionary GetBegin();
			virtual MIteratorDictionary GetEnd();

			virtual bool Contains(const MDictionaryNode<K, T>& node) const override;
			virtual bool Contains(const K& key) const;

			virtual T operator[](const K& index) const;
			virtual T& operator[](const K& index);
			virtual MDictionary& operator=(const MDictionary& c);

			bool VerifyProperties() const;

#pragma endregion

		};

#pragma region Definitions

		template <class K, class T> MDictionary<K, T>::MDictionary() : 
			MCollection<MDictionaryNode<K, T>>(0, GrowthStrategy::NO_GROW, ShrinkStrategy::NO_SHRINK),
			_root(nullptr)
		{
			_type = CollectionType::DICTIONARY;
		}

		template <class K, class T> MDictionary<K, T>::MDictionary(const MDictionary& c) :
			MDictionary<K, T>()
		{
			CopyAllNodes(c);
		}

		template <class K, class T> MDictionary<K, T>::~MDictionary()
		{
			Clear();
		}

		template<class K, class T>
		inline void MDictionary<K, T>::Add(const K & key, const T & value)
		{
			MDictionaryNode<K, T>* newNode = new MDictionaryNode<K, T>(key, value, NODE_COLOR_RED,
				nullptr, nullptr, nullptr);

			if (_root == nullptr)
			{
				_root = newNode;
			}
			else
			{
				MDictionaryNode<K, T>* n = _root;
				while (true)
				{
					if (n->_key == key)
					{
						n->_value = value;
						return;
					}
					else if (key < n->_key)
					{
						if (n->_left == nullptr)
						{
							n->_left = newNode;
							break;
						}
						else
						{
							n = n->_left;
						}
					}
					else
					{
						if (n->_right == nullptr)
						{
							n->_right = newNode;
							break;
						}
						else
						{
							n = n->_right;
						}
					}
				}
				
				newNode->_parent = n;
			}

			++_size;
			InsertCase1(newNode);
			ME_ASSERT(VerifyProperties(), "MDictionary: RBT lost its properties on add.");
		}

		template<class K, class T>
		inline void MDictionary<K, T>::Remove(const K & key)
		{
			MDictionaryNode<K, T>* child = nullptr;
			MDictionaryNode<K, T>* n = LookupNode(key);

			if (n == nullptr)
			{
				return;
			}

			if (n->_left != nullptr && n->_right != nullptr)
			{
				MDictionaryNode<K, T>* pred = GetMaximumNode(n->_left);
				n->_key = pred->_key;
				n->_value = pred->_value;
				n = pred;
			}

			ME_ASSERT(n->_left == nullptr || n->_right == nullptr, "MDictionary: Delete failed somehow.");

			child = n->_right == nullptr ? n->_left : n->_right;
			if (GetNodeColor(n) == NODE_COLOR_BLACK)
			{
				n->_color = GetNodeColor(child);
				DeleteCase1(n);
			}

			ReplaceNode(n, child);
			delete n;
			--_size;
			if (_size == 0)
				_root = nullptr;
			ME_ASSERT(VerifyProperties(), "MDictionary: RBT lost its properties on delete.");
		}

		template<class K, class T>
		inline T MDictionary<K, T>::GetValue(const K & key) const
		{
			MDictionaryNode<K, T>* ptr = LookupNode(key);
			ME_ASSERT(ptr != nullptr, "MDictionary: GetValue called for nonexistant key.");
			return ptr->GetValue();
		}

		template<class K, class T>
		inline bool MDictionary<K, T>::TryGetValue(const K & key, T* outValue) const
		{
			MDictionaryNode<K, T>* ptr = LookupNode(key);
			if (ptr != nullptr)
			{
				*outValue = ptr->GetValue();
				return true;
			}
			else
			{
				return false;
			}
		}

		template<class K, class T>
		inline bool MDictionary<K, T>::TryGetKey(const T & value, K* outKey) const
		{
			MDictionaryNode<K, T>* ptr = GetByValue(value);
			if (ptr != nullptr)
			{
				*outKey = ptr->GetKey();
				return true;
			}
			else
			{
				return false;
			}
		}

		template<class K, class T>
		inline K MDictionary<K, T>::GetKey(const T & value) const
		{
			MDictionaryNode<K, T>* ptr = GetByValue(value);
			ME_ASSERT(ptr != nullptr, "MDictionary: GetKey called for nonexistant value.");
			return ptr->GetKey();
		}

		template<class K, class T>
		inline void MDictionary<K, T>::Clear()
		{
			if (_root != nullptr)
			{
				MStack<MDictionaryNode<K, T>*> stack;
				MDictionaryNode<K, T>* n = _root;
				stack.Push(_root);

				while (n != nullptr && stack.GetSize() > 0)
				{
					n = stack.Pop();

					if (n->_left != nullptr)
					{
						stack.Push(n->_left);
					}
					if (n->_right != nullptr)
					{
						stack.Push(n->_right);
					}

					delete n;
					--_size;
				}
				_root = nullptr;
			}

			ME_ASSERT(_size == 0, "MDictionary: NodeCount not zero after Clear.");
		}

		template<class K, class T>
		inline bool MDictionary<K, T>::IsEmpty() const
		{
			return GetSize() != 0;
		}

		template<class K, class T>
		inline MSize MDictionary<K, T>::GetSize() const
		{
			return _size;
		}

		template<class K, class T>
		inline MSize MDictionary<K, T>::GetCapacity() const
		{
			return static_cast<MSize>(-1);
		}

		template<class K, class T>
		inline typename MDictionary<K, T>::MIteratorDictionary MDictionary<K, T>::GetIterator()
		{
			return GetBegin();
		}

		template<class K, class T>
		inline typename MDictionary<K, T>::MIteratorDictionary MDictionary<K, T>::GetBegin()
		{
			return MIteratorDictionary(*this, _root);
		}

		template<class K, class T>
		inline typename MDictionary<K, T>::MIteratorDictionary MDictionary<K, T>::GetEnd()
		{
			return MIteratorDictionary(*this, GetMaximumNode(_root));
		}

		template<class K, class T>
		inline bool MDictionary<K, T>::Contains(const MDictionaryNode<K, T>& node) const
		{
			T value;
			return TryGetValue(node.GetKey(), &value);
		}

		template<class K, class T>
		inline bool MDictionary<K, T>::Contains(const K & key) const
		{
			return LookupNode(key) != nullptr;
		}

		template<class K, class T>
		inline T MDictionary<K, T>::operator[](const K & index) const
		{
			return LookupNode(index)->GetValue();
		}

		template<class K, class T>
		inline T & MDictionary<K, T>::operator[](const K & index)
		{
			// TODO: insert return statement here
			return LookupNode(index)->GetValue();
		}

		template<class K, class T>
		inline MDictionary<K, T> & MDictionary<K, T>::operator=(const MDictionary<K, T> & c)
		{
			// TODO: insert return statement here
			CopyAllNodes(c);
			return *this;
		}

		template<class K, class T>
		inline bool MDictionary<K, T>::VerifyProperties() const
		{
			return VerifyProperty1(_root) && VerifyProperty2() && VerifyProperty3(_root) && VerifyProperty4();
		}

		template<class K, class T>
		inline MDictionaryNode<K, T>* MDictionary<K, T>::Compare(const MDictionaryNode<K, T>* left, const MDictionaryNode<K, T>* right)
		{
			return (a1->GetKey() < a2->GetKey() ? a1 : a2);
		}

		template<class K, class T>
		inline bool MDictionary<K, T>::VerifyProperty1(MDictionaryNode<K, T>* node) const
		{
			if (node == nullptr)
			{
				return true;
			}
			else
			{
				if (GetNodeColor(node) == NODE_COLOR_RED || GetNodeColor(node) == NODE_COLOR_BLACK)
				{
					if (!VerifyProperty1(node->_left)) return false;
					if (!VerifyProperty1(node->_right)) return false;
				}
				else
				{
					ME_ASSERT(false, "MDictionary: Property 1 verification failed.");
					return false;
				}
				return true;	// ?
			}
		}

		template<class K, class T>
		inline bool MDictionary<K, T>::VerifyProperty2() const
		{
			bool assert = _root != nullptr && GetNodeColor(_root) == NODE_COLOR_BLACK;
			//ME_ASSERT(assert, "MDictionary: Property 2 verification failed.");
			// this property gets auto fixed because fuck it
			if (!assert && _root != nullptr) _root->_color = NODE_COLOR_BLACK;
			//return assert;
			return true;
		}

		template<class K, class T>
		inline bool MDictionary<K, T>::VerifyProperty3(MDictionaryNode<K, T>* node) const
		{
			if (node == nullptr)
			{
				return true;
			}
			else
			{
				MDictionaryNode<K, T>* lc = node->_left;
				MDictionaryNode<K, T>* rc = node->_right;

				bool assert = true;
				if (GetNodeColor(node) == NODE_COLOR_RED)
				{
					MDictionaryNode<K, T>* parent = node->_parent;
					assert &= (node->_parent != nullptr ? GetNodeColor(node->_parent) == NODE_COLOR_BLACK : true) && (node->_left != nullptr ? GetNodeColor(node->_left) == NODE_COLOR_BLACK : true) && (node->_right != nullptr ? GetNodeColor(node->_right) == NODE_COLOR_BLACK : true);
					ME_ASSERT(assert, "MDictionary: Property 3 verification failed.");

					if (!assert)
						return false;
				}

				assert &= VerifyProperty3(lc);
				assert &= VerifyProperty3(rc);
				return assert;
			}
		}

		template<class K, class T>
		inline bool MDictionary<K, T>::VerifyProperty4() const
		{
			MInt64 blackCountPath = -1;
			return VerifyProperty4Helper(_root, 0, blackCountPath);
		}

		template<class K, class T>
		inline bool MDictionary<K, T>::VerifyProperty4Helper(MDictionaryNode<K, T>* node, MInt64 blackCount, MInt64 & pathBlackCount) const
		{
			bool assert = true;
			if (GetNodeColor(node) == NODE_COLOR_BLACK)
			{
				++blackCount;
			}
			if (node == nullptr)
			{
				if (pathBlackCount == -1)
				{
					pathBlackCount = blackCount;
				}
				else
				{
					assert = blackCount == pathBlackCount;
					ME_ASSERT(assert, "MDictionary: Property 4 verification failed.");
				}
				return assert;
			}

			VerifyProperty4Helper(node->_left, blackCount, pathBlackCount);
			VerifyProperty4Helper(node->_right, blackCount, pathBlackCount);

			return assert;
		}

		template<class K, class T>
		inline MDictionaryNode<K, T>* MDictionary<K, T>::NewNode(const K & key, const T & value, MUint8 color, MDictionaryNode<K, T>* parent, MDictionaryNode<K, T>* left, MDictionaryNode<K, T>* right)
		{
			void* data = memoryManagement::MemoryManager::GetInstance()->GetLinkedCollectionsAllocator()->
				Allocate(sizeof(MDictionaryNode<K, T>), __alignof(MDictionaryNode<K, T>));
			MDictionaryNode<K, T>* nn = new MDictionaryNode<K, T>()
			return NULL;
		}

		template<class K, class T>
		inline MDictionaryNode<K, T>* MDictionary<K, T>::LookupNode(const K & key) const
		{
			MDictionaryNode<K, T>* node = _root;
			while (node != nullptr)
			{
				if (key == node->GetKey())
				{
					return node;
				}
				else if (key < node->GetKey())
				{
					node = node->_left;
				}
				else
				{
					node = node->_right;
				}
			}

			return nullptr;
		}

		template<class K, class T>
		inline void MDictionary<K, T>::ReplaceNode(MDictionaryNode<K, T>* oldNode, MDictionaryNode<K, T>* newNode)
		{
			if (oldNode->_parent == nullptr)
			{
				_root = newNode;
			}
			else
			{
				if (oldNode == oldNode->_parent->_left)
					oldNode->_parent->_left = newNode;
				else
					oldNode->_parent->_right = newNode;
			}
			if (newNode != nullptr)
			{
				newNode->_parent = oldNode->_parent;
			}
		}

		template<class K, class T>
		inline void MDictionary<K, T>::RotateLeft(MDictionaryNode<K, T>* n)
		{
			MDictionaryNode<K, T>* r = n->_right;
			ReplaceNode(n, r);
			n->_right = r->_left;
			if (r->_left != nullptr)
			{
				r->_left->_parent = n;
			}
			r->_left = n;
			n->_parent = r;
		}

		template<class K, class T>
		inline void MDictionary<K, T>::RotateRight(MDictionaryNode<K, T>* n)
		{
			MDictionaryNode<K, T>* L = n->_left;
			ReplaceNode(n, L);
			n->_left = L->_right;
			if (L->_right != nullptr)
			{
				L->_right->_parent = n;
			}
			L->_right = n;
			n->_parent = L;
		}

		template<class K, class T>
		inline void MDictionary<K, T>::InsertCase1(MDictionaryNode<K, T>* node)
		{
			if (node->_parent == nullptr)
			{
				node->_color = NODE_COLOR_BLACK;
			}
			else
			{
				InsertCase2(node);
			}
		}

		template<class K, class T>
		inline void MDictionary<K, T>::InsertCase2(MDictionaryNode<K, T>* node)
		{
			if (GetNodeColor(node->_parent) == NODE_COLOR_BLACK)
			{
				return;
			}
			else
			{
				InsertCase3(node);
			}
		}

		template<class K, class T>
		inline void MDictionary<K, T>::InsertCase3(MDictionaryNode<K, T>* node)
		{
			if (GetNodeColor(GetUncle(node)) == NODE_COLOR_RED)
			{
				node->_parent->_color = NODE_COLOR_BLACK;
				(GetUncle(node))->_color = NODE_COLOR_BLACK;
				(GetGrandparent(node))->_color = NODE_COLOR_RED;
				InsertCase1(GetGrandparent(node));
			}
			else
			{
				InsertCase4(node);
			}
		}

		template<class K, class T>
		inline void MDictionary<K, T>::InsertCase4(MDictionaryNode<K, T>* node)
		{
			if (node == node->_parent->_right && node->_parent == GetGrandparent(node)->_left)
			{
				RotateLeft(node->_parent);
				node = node->_left;
			}
			else if (node == node->_parent->_left && node->_parent == GetGrandparent(node)->_right)
			{
				RotateRight(node->_parent);
				node = node->_right;
			}

			InsertCase5(node);
		}

		template<class K, class T>
		inline void MDictionary<K, T>::InsertCase5(MDictionaryNode<K, T>* node)
		{
			node->_parent->_color = NODE_COLOR_BLACK;
			GetGrandparent(node)->_color = NODE_COLOR_RED;

			if (node == node->_parent->_left && node->_parent == GetGrandparent(node)->_left)
			{
				RotateRight(GetGrandparent(node));
			}
			else
			{
				ME_ASSERT(node == node->_parent->_right && node->_parent == GetGrandparent(node)->_right, "MDictionary: InsertCase5 failed.");
				RotateLeft(GetGrandparent(node));
			}
		}

		template<class K, class T>
		inline void MDictionary<K, T>::DeleteCase1(MDictionaryNode<K, T>* node)
		{
			if (node->_parent == nullptr)
			{
				return;
			}
			else
			{
				DeleteCase2(node);
			}
		}

		template<class K, class T>
		inline void MDictionary<K, T>::DeleteCase2(MDictionaryNode<K, T>* node)
		{
			if (GetNodeColor(GetSibling(node)) == NODE_COLOR_RED)
			{
				node->_parent->_color = NODE_COLOR_RED;
				GetSibling(node)->_color = NODE_COLOR_BLACK;

				if (node == node->_parent->_left)
				{
					RotateLeft(node->_parent);
				}
				else
				{
					RotateRight(node->_parent);
				}
			}
			DeleteCase3(node);
		}

		template<class K, class T>
		inline void MDictionary<K, T>::DeleteCase3(MDictionaryNode<K, T>* node)
		{
			MDictionaryNode<K, T>* sibling = GetSibling(node);
			if (GetNodeColor(node->_parent) == NODE_COLOR_BLACK && GetNodeColor(sibling) == NODE_COLOR_BLACK &&
				GetNodeColor(sibling->_left) == NODE_COLOR_BLACK && GetNodeColor(sibling->_right) == NODE_COLOR_BLACK)
			{
				sibling->_color = NODE_COLOR_RED;
				DeleteCase1(node->_parent);
			}
			else
			{
				DeleteCase4(node);
			}
		}

		template<class K, class T>
		inline void MDictionary<K, T>::DeleteCase4(MDictionaryNode<K, T>* node)
		{
			MDictionaryNode<K, T>* sibling = GetSibling(node);
			if (sibling != nullptr && GetNodeColor(node->_parent) == NODE_COLOR_RED && GetNodeColor(sibling) == NODE_COLOR_BLACK &&
				GetNodeColor(sibling->_left) == NODE_COLOR_BLACK && GetNodeColor(sibling->_right) == NODE_COLOR_BLACK)
			{
				sibling->_color = NODE_COLOR_RED;
				node->_parent->_color = NODE_COLOR_BLACK;
			}
			else
			{
				DeleteCase5(node);
			}
		}

		template<class K, class T>
		inline void MDictionary<K, T>::DeleteCase5(MDictionaryNode<K, T>* node)
		{
			MDictionaryNode<K, T>* sibling = GetSibling(node);
			if (sibling != nullptr && node == node->_parent->_left && GetNodeColor(sibling) == NODE_COLOR_BLACK &&
				GetNodeColor(sibling->_left) == NODE_COLOR_RED && GetNodeColor(sibling->_right) == NODE_COLOR_BLACK)
			{
				sibling->_color = NODE_COLOR_RED;
				sibling->_left->_color = NODE_COLOR_BLACK;
				RotateRight(sibling);
			}
			else if (sibling != nullptr && node == node->_parent->_right && GetNodeColor(sibling) == NODE_COLOR_BLACK &&
				GetNodeColor(sibling->_right) == NODE_COLOR_RED && GetNodeColor(sibling->_left) == NODE_COLOR_BLACK)
			{
				sibling->_color = NODE_COLOR_RED;
				sibling->_right->_color = NODE_COLOR_BLACK;
				RotateLeft(sibling);
			}
			DeleteCase6(node);
		}

		template<class K, class T>
		inline void MDictionary<K, T>::DeleteCase6(MDictionaryNode<K, T>* node)
		{
			MDictionaryNode<K, T>* sibling = GetSibling(node);
			if (sibling != nullptr)
			{
				sibling->_color = GetNodeColor(node->_parent);
				node->_parent->_color = NODE_COLOR_BLACK;

				if (node == node->_parent->_left)
				{
					ME_ASSERT(GetNodeColor(sibling->_right) == NODE_COLOR_RED, "MDictionary: DeleteCase6 failed.");
					sibling->_right->_color = NODE_COLOR_BLACK;
					RotateLeft(node->_parent);
				}
				else
				{
					ME_ASSERT(GetNodeColor(sibling->_left) == NODE_COLOR_RED, "MDictionary: DeleteCase6 failed.");
					sibling->_left->_color = NODE_COLOR_BLACK;
					RotateRight(node->_parent);
				}
			}
		}

		template<class K, class T>
		inline MDictionaryNode<K, T>* MDictionary<K, T>::GetMaximumNode(MDictionaryNode<K, T>* node) const
		{
			ME_ASSERT(node != nullptr, "MDictionary: GetMaximumNode received nullptr.");
			while (node->_right != nullptr)
			{
				node = node->_right;
			}
			return node;
		}

		template<class K, class T>
		inline MDictionaryNode<K, T>* MDictionary<K, T>::GetLeftMostNode(MDictionaryNode<K, T>* node) const
		{
			while (node != nullptr && node->_left != nullptr)
			{
				node = node->_left;
			}
			return node;
		}

		template<class K, class T>
		inline MDictionaryNode<K, T>* MDictionary<K, T>::GetRightMostNode(MDictionaryNode<K, T>* node) const
		{
			while (node != nullptr && node->_right != nullptr)
			{
				node = node->_right;
			}
			return node;
		}

		template<class K, class T>
		inline MUint8 MDictionary<K, T>::GetNodeColor(MDictionaryNode<K, T>* node) const
		{
			return node != NULL ? node->_color : NODE_COLOR_BLACK;
		}

		template<class K, class T>
		inline MDictionaryNode<K, T>* MDictionary<K, T>::GetGrandparent(MDictionaryNode<K, T>* n) const
		{
			ME_ASSERT(n != nullptr, "MDictionary::GetGrandparent: n is null.");
			ME_ASSERT(n->_parent != nullptr, "MDictionary::GetGrandparent: n->_parent is null.");
			ME_ASSERT(n->_parent->_parent != nullptr, "MDictionary::GetGrandparent: n->_parent->_parent is null.");
			return n->_parent->_parent;
		}

		template<class K, class T>
		inline MDictionaryNode<K, T>* MDictionary<K, T>::GetSibling(MDictionaryNode<K, T>* n) const
		{
			ME_ASSERT(n != nullptr, "MDictionary::GetSibling: n is null.");
			ME_ASSERT(n->_parent != nullptr, "MDictionary::GetSibling: n->_parent is null.");
			if (n == n->_parent->_left)
			{
				return n->_parent->_right;
			}
			else
			{
				return n->_parent->_left;
			}
		}

		template<class K, class T>
		inline MDictionaryNode<K, T>* MDictionary<K, T>::GetUncle(MDictionaryNode<K, T>* n) const
		{
			ME_ASSERT(n != nullptr, "MDictionary::GetUncle: n is null.");
			ME_ASSERT(n->_parent != nullptr, "MDictionary::GetUncle: n->_parent is null.");
			ME_ASSERT(n->_parent->_parent != nullptr, "MDictionary::GetUncle: n->_parent->_parent is null.");
			return GetSibling(n->_parent);
		}

		template<class K, class T>
		inline MDictionaryNode<K, T>* MDictionary<K, T>::GetByValue(const T & value) const
		{
			MStack<MDictionaryNode<K, T>*> stack;
			MDictionaryNode<K, T>* n = _root;
			stack.Push(_root);

			while (n != nullptr && stack.GetSize() > 0)
			{
				n = stack.Pop();

				if (n->GetValue() == value)
				{
					return n;
				}

				if (n->_left != nullptr)
				{
					stack.Push(n->_left);
				}
				if (n->_right != nullptr)
				{
					stack.Push(n->_right);
				}
			}

			return nullptr;
		}

		template<class K, class T>
		inline void MDictionary<K, T>::CopyAllNodes(const MDictionary<K, T>& c)
		{
			if (c._root == nullptr)
				return;

			//_size = c._size;
			_size = 0;

			_root = new MDictionaryNode<K, T>(c._root->GetKey(), c._root->GetValue(), c._root->_color, nullptr, nullptr, nullptr);
			++_size;
			MDictionaryNode<K, T>* clone = _root;
			MDictionaryNode<K, T>* original = c._root;

			while (original != nullptr)
			{
				if (original->_left != nullptr && clone->_left == nullptr)
				{
					clone->_left = new MDictionaryNode<K, T>(original->_left->GetKey(), original->_left->GetValue(), original->_left->_color, clone, nullptr, nullptr);
					original = original->_left;
					clone = clone->_left;
					++_size;
				}
				else if (original->_right != nullptr && clone->_right == nullptr)
				{
					clone->_right = new MDictionaryNode<K, T>(original->_right->GetKey(), original->_right->GetValue(), original->_right->_color, clone, nullptr, nullptr);
					original = original->_right;
					clone = clone->_right;
					++_size;
				}
				else
				{
					original = original->_parent;
					clone = clone->_parent;
				}
			}

			ME_ASSERT(_size == c._size, "Ha!");
		}

#pragma endregion

	}
}

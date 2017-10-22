#pragma once

#include "core/GlobalDefines.h"

namespace morphEngine
{
	namespace utility
	{
		template <class T>
		class MBinaryTree
		{
		protected:

#pragma region Functions Protected

			void CopyNode(T* source, T* destination);

			inline bool VerifyBTIndex(MSize index, MSize dataSize);

			inline void Swap(T* a1, T* a2);

			inline void RotateLeft(T* ptr, T* dataBegin, MSize dataSize);
			inline void RotateRight(T* ptr, T* dataBegin, MSize dataSize);

			inline T* GetParent(MSize i, T* dataBegin, MSize dataSize);
			inline T* GetParent(T* ptr, T* dataBegin, MSize dataSize);
			inline T* GetGrandparent(T* ptr, T* dataBegin, MSize dataSize);
			inline T* GetSibling(T* ptr, T* dataBegin, MSize dataSize);
			inline T* GetUncle(T* ptr, T* dataBegin, MSize dataSize);
			inline T* GetLeftChild(MSize i, T* dataBegin, MSize dataSize);
			inline T* GetLeftChild(T* ptr, T* dataBegin, MSize dataSize);
			inline T* GetRightChild(MSize i, T* dataBegin, MSize dataSize);
			inline T* GetRightChild(T* ptr, T* dataBegin, MSize dataSize);

#pragma endregion

		public:

#pragma region Functions Protected

			MBinaryTree();
			virtual ~MBinaryTree();

#pragma endregion

		};

#pragma region Definitions


		template <class T> MBinaryTree<T>::MBinaryTree()
		{
		}


		template <class T> MBinaryTree<T>::~MBinaryTree()
		{
		}

		template<class T>
		inline void MBinaryTree<T>::CopyNode(T * destination, T * source)
		{
			memcpy(destination, source, sizeof(T));
			*destination = *object;
		}

		template<class T>
		inline bool MBinaryTree<T>::VerifyBTIndex(MSize index, MSize dataSize)
		{
			return index < dataSize;
		}

		template<class T>
		inline void MBinaryTree<T>::Swap(T * a1, T * a2)
		{
			T tmp = *a1;
			*a1 = *a2;
			*a2 = tmp;
		}

		template<class T>
		inline void MBinaryTree<T>::RotateLeft(T * ptr, T * dataBegin, MSize dataSize)
		{
			T* r = GetRightChild(ptr, dataBegin, dataSize);
			Swap(ptr, r);
			
			CopyNode(GetRightChild(ptr, dataBegin, dataSize), GetLeftChild(r, dataBegin, dataSize));

			if (GetLeftChild(ptr, dataBegin, dataSize) != nullptr)
			{
				T* par = GetParent(GetLeftChild(r, dataBegin, dataSize), dataBegin, dataSize);
				CopyNode(par, ptr);
			}
			CopyNode(GetLeftChild(r, dataBegin, dataSize), ptr);
			CopyNode(GetParent(ptr, dataBegin, dataSize), r);
		}

		template<class T>
		inline void MBinaryTree<T>::RotateRight(T * ptr, T * dataBegin, MSize dataSize)
		{
			T* L = GetLeftChild(ptr, dataBegin, dataSize);
			Swap(ptr, L);

			CopyNode(GetRightChild(ptr, dataBegin, dataSize), GetLeftChild(L, dataBegin, dataSize));
			if (GetRightChild(L, dataBegin, dataSize) != nullptr)
			{
				T* par = GetParent(GetRightChild(L, dataBegin, dataSize), dataBegin, dataSize);
				CopyNode(par, ptr);
			}
			CopyNode(GetRightChild(L, dataBegin, dataSize), ptr);
			CopyNode(GetParent(ptr, dataBegin, dataSize), L);
		}

		template<class T>
		inline T* MBinaryTree<T>::GetParent(MSize i, T* dataBegin, MSize dataSize)
		{
			if (dataSize > 1 && VerifyBTIndex(i, dataSize))
			{
				return &dataBegin[i / 2];
			}
			else
			{
				return nullptr;
			}
		}

		template<class T>
		inline T* MBinaryTree<T>::GetParent(T * ptr, T* dataBegin, MSize dataSize)
		{
			return GetParent(ptr - dataBegin, dataBegin, dataSize);
		}

		template<class T>
		inline T * MBinaryTree<T>::GetGrandparent(T * ptr, T * dataBegin, MSize dataSize)
		{
			T* ret = ptr;
			if (ptr != nullptr)
			{
				ret = GetParent(ret, dataBegin, dataSize);
				if (ret != nullptr)
				{
					ret = GetParent(ret, dataBegin, dataSize);
					return ret;
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				return nullptr;
			}
		}

		template<class T>
		inline T * MBinaryTree<T>::GetSibling(T * ptr, T * dataBegin, MSize dataSize)
		{
			if (ptr != nullptr)
			{
				T* ret = GetParent(ptr, dataBegin, dataSize);
				if (ret != nullptr)
				{
					if (ptr == GetLeftChild(ret))
					{
						return GetRightChild(ret);
					}
					else
					{
						return GetLeftChild(ret);
					}
				}
				else
				{
					return nullptr;
				}
			}
			else return nullptr;
		}

		template<class T>
		inline T * MBinaryTree<T>::GetUncle(T * ptr, T * dataBegin, MSize dataSize)
		{
			T* p = nullptr
			if(ptr != nullptr && GetGrandparent(ptr) != nullptr && (p = GetParent(ptr)) != nullptr)
			{
				return GetSibling(p, dataBegin, dataSize);
			}
			else
			{
				return nullptr;
			}
		}

		template<class T>
		inline T* MBinaryTree<T>::GetLeftChild(MSize i, T* dataBegin, MSize dataSize)
		{
			MSize newPos = 2 * i;

			if (VerifyBTIndex(newPos, dataSize))
			{
				return &dataBegin[newPos];
			}
			else
			{
				return nullptr;
			}
		}

		template<class T>
		inline T* MBinaryTree<T>::GetLeftChild(T * ptr, T* dataBegin, MSize dataSize)
		{
			return GetLeftChild(ptr - dataBegin, dataBegin, dataSize);
		}

		template<class T>
		inline T* MBinaryTree<T>::GetRightChild(MSize i, T* dataBegin, MSize dataSize)
		{
			MSize newPos = 2 * i + 1;

			if (VerifyBTIndex(newPos, dataSize))
			{
				return &dataBegin[newPos];
			}
			else
			{
				return nullptr;
			}
		}

		template<class T>
		inline T* MBinaryTree<T>::GetRightChild(T * ptr, T* dataBegin, MSize dataSize)
		{
			return GetRightChild(ptr - dataBegin, dataBegin, dataSize);
		}

#pragma endregion

	}
}
#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Containers.Pair.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Threading.DataFlowControler.h"
#include "Seed.Core.SettingsAPI.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			///<summary>Пріоритетна черга для обробки даних в порядку приходу з урахуванням пріоритету
			///<para>Працює через фібоначеву кучу. Ітерація відсутня</para></summary>
			template <class THeap, typename TPriority, typename TValue>
			class SeedKernelCall BasePriorityQueue final :
			public Memory::Allocator<THeap>,
				public LINQ::IPriorityQueue<BasePriorityQueue, THeap, TPriority, TValue, BasePair<THeap, TPriority, TValue>, true>,
				protected IDisposable
			{
			private:
				using Pair = BasePair<THeap, TPriority, TValue>;
			private:
				///<summary>Внутрішня структура елемента в черзі</summary>
				struct Node : public Memory::Allocator<THeap> {
					///<summary>Попередній елемент</summary>
					Node* prev;
					///<summary>Наступний елемент</summary>
					Node* next;
					///<summary>Вкладений елемент</summary>
					Node* child;
					///<summary>Батьківський елемент</summary>
					Node* parent;
					//<summary>Дані елемента</summary>
					Pair data;
					///<summary></summary>
					int degree;
					///<summary></summary>
					bool marked;

					///Гетери

					__forceinline Node* getPrev() { return prev; }
					__forceinline Node* getNext() { return next; }
					__forceinline Node* getChild() { return child; }
					__forceinline Node* getParent() { return parent; }
					__forceinline const TPriority& getValue() { return data.Key; }
					__forceinline TValue& getData() { return data.Value; }
					__forceinline bool isMarked() { return marked; }

					__forceinline bool hasChildren() { return child; }
					__forceinline bool hasParent() { return parent; }

					//Конструктори
#pragma warning(push)
//Visual Studio bug, next and prev init as this
#pragma warning(disable: 26495)
					Node(Pair&& data) :
						data(MoveRValue(data)),
						prev(this),
						next(this),
						degree(0),
						marked(false),
						child(nullptr),
						parent(nullptr)
					{}
#pragma warning(pop)
				};
			private:
				struct Tree : public Memory::Allocator<THeap> {
					Node* heap;
					RefCountT Count;
					RefCountT RefCount;
					SizeT MaxSize;

					Tree(SizeT MaxSize) :
						heap(nullptr),
						Count(0),
						RefCount(1),
						MaxSize(MaxSize),
						flowControler()
					{}
				};
				Tree* This;
			protected:
				virtual void Dispose() {
					if (This->heap != nullptr) {
						deleteAll(const_cast<Node*>(This->heap));
						This->heap = nullptr;
						This->Count = 0;
					}
				}
			public:
				BasePriorityQueue() : This(new Tree(CurrentSettings().MemoryManager.DefaultPriorityQueueMaxSize)) {}
				BasePriorityQueue(SizeT MaxSize) : This(new Tree(MaxSize)) {}

				void Insert(const TPriority& priority, const TValue& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(priority, key));
					This->heap = merge(const_cast<Node*>(This->heap), ret);
					++this->This->Count;
				}

				void Insert(TPriority&& priority, const TValue& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(MoveRValue(priority), key));
					This->heap = merge(const_cast<Node*>(This->heap), ret);
					++this->This->Count
				}

				void Insert(const TPriority& priority, TValue&& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(priority, MoveRValue(key)));
					This->heap = merge(const_cast<Node*>(This->heap), ret);
					++this->This->Count;
				}

				void Insert(TPriority&& priority, TValue&& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(MoveRValue(priority), MoveRValue(key)));
					This->heap = merge(This->heap, ret);
					++this->This->Count;
				}

				void Insert(const Pair& itemValue) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(itemValue));
					This->heap = merge(This->heap, ret);
					++this->This->Count;
				}

				void Insert(Pair&& itemValue) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(MoveRValue(itemValue));
					This->heap = merge(This->heap, ret);
					++this->This->Count;
				}

				bool getIsEmpty() {
					return This->heap == nullptr;
				}

				SizeT getCount() {
					return this->This->Count;
				}

				const TPriority& getFirstItemPriority() {
					return This->heap->data.Key;
				}

				const TPriority& discardFirstItem() {
					Node* old = This->heap;
					This->heap = removeMinimum(This->heap);
					const TPriority& ret = old->data.Key;
					delete old;
					return ret;
				}

				TValue getFirstWhenAquire() {
					while (this->This->Count == 0) {
						Throw(NullPointException(ExceptionPosition));
					}
					Node* old = This->heap;
					This->heap = removeMinimum(This->heap);
					TValue ret(MoveRValue(old->data.Value));
					--this->This->Count;
					delete old;
					return ret;
				}

				///<summary>Оптимізований обмін</summary>
				///<param name = 'A'>Перший елемент</param>
				///<param name = 'B'>Другий елемент</param>
				__forceinline friend void Swap(BasePriorityQueue& A, BasePriorityQueue& B);

				Boolean Property(getIsEmpty) IsEmpty;
				SizeT Property(getCount) Count;
			private:
				Node* merge(Node* a, Node* b) {
					if (a == nullptr) return b;
					if (b == nullptr) return a;
					if (a->data.Key > b->data.Key) {
						Node* temp = a;
						a = b;
						b = temp;
					}
					Node* an = a->next;
					Node* bp = b->prev;
					a->next = b;
					b->prev = a;
					an->prev = bp;
					bp->next = an;
					return a;
				}

				void deleteAll(Node* n) {
					if (n != nullptr) {
						Node* c = n;
						do {
							Node* d = c;
							c = c->next;
							deleteAll(d->child);
							delete d;
						} while (c != n);
					}
				}

				void addChild(Node* parent, Node* child) {
					child->prev = child->next = child;
					child->parent = parent;
					parent->degree++;
					parent->child = merge(parent->child, child);
				}

				void _unMarkAndUnParentAll(Node* n) {
					if (n == nullptr)return;
					Node* c = n;
					do {
						c->marked = false;
						c->parent = nullptr;
						c = c->next;
					} while (c != n);
				}

				Node* removeMinimum(Node* n) {
					_unMarkAndUnParentAll(n->child);
					if (n->next == n) {
						n = n->child;
					}
					else {
						n->next->prev = n->prev;
						n->prev->next = n->next;
						n = merge(n->next, n->child);
					}
					if (n == nullptr)
						return n;
					Node* trees[128] = { nullptr };

					while (true) {
						if (trees[n->degree] != nullptr) {
							Node* t = trees[n->degree];
							if (t == n)break;
							trees[n->degree] = nullptr;
							if (n->data.Key < t->data.Key) {
								t->prev->next = t->next;
								t->next->prev = t->prev;
								addChild(n, t);
							}
							else {
								t->prev->next = t->next;
								t->next->prev = t->prev;
								if (n->next == n) {
									t->next = t->prev = t;
									addChild(t, n);
									n = t;
								}
								else {
									n->prev->next = t;
									n->next->prev = t;
									t->next = n->next;
									t->prev = n->prev;
									addChild(t, n);
									n = t;
								}
							}
							continue;
						}
						else {
							trees[n->degree] = n;
						}
						n = n->next;
					}
					Node* min = n;
					do {
						if (n->data.Key < min->data.Key) min = n;
						n = n->next;
					} while (min != n);
					return min;
				}

				Node* cut(Node* heap, Node* n) {
					if (n->next == n) {
						n->parent->child = nullptr;
					}
					else {
						n->next->prev = n->prev;
						n->prev->next = n->next;
						n->parent->child = n->next;
					}
					n->next = n->prev = n;
					n->marked = false;
					return merge(heap, n);
				}

				Node* decreaseKey(Node* heap, Node* n, const TPriority& priority) {
					if (n->data.Key < priority)return heap;
					n->data = Pair(priority, MoveRValue(n->data.Value));
					if (n->data.Key < n->parent->data.Key) {
						heap = cut(heap, n);
						Node* parent = n->parent;
						n->parent = nullptr;
						while (parent != nullptr && parent->marked) {
							heap = cut(heap, parent);
							n = parent;
							parent = n->parent;
							n->parent = nullptr;
						}
						if (parent != nullptr && parent->parent != nullptr)
							parent->marked = true;
					}
					return heap;
				}

				Node* find(Node* heap, const TPriority& priority) {
					Node* n = heap;
					if (n == nullptr) return nullptr;
					do {
						if (n->data.Key == priority) return n;
						Node* ret = find(n->child, priority);
						if (ret)return ret;
						n = n->next;
					} while (n != heap);
					return nullptr;
				}
			protected:
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				friend class LINQ::IAssociableCollection;
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				friend class LINQ::IPriorityQueue;
#pragma region ForEach Pair
				template<typename Lambda>
				__forceinline static void ForEachImplWithIndex(Node* node, SizeT& Index, Lambda& lambda, Node* startNode = nullptr) {
					if (startNode = nullptr)
						startNode = node;
					else
						if (startNode == node)
							return;

					if (node->left != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					lambda(node->data, Index); Index++;
					if (node->right != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					if (node->next != nullptr)
						ForEachImplWithIndex(node->next, Index, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachImplWithoutIndex(Node* node, Lambda& lambda, Node* startNode = nullptr) {
					if (startNode = nullptr)
						startNode = node;
					else
						if (startNode == node)
							return;

					if (node->left != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					lambda(node->data);
					if (node->right != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					if (node->next != nullptr)
						ForEachImplWithIndex(node->next, Index, lambda);
				}

				template<typename LambdaT>
				__forceinline const BasePriorityQueue& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TKey, TValue>&>) {
						auto node = this->This->root;
						if (node == nullptr)
						{
							ForEachImplWithoutIndex(node, lambda);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TKey, TValue>&, SizeT>) {
						auto node = this->This->root;
						if (node != nullptr)
						{
							SizeT index = 0;
							ForEachImplWithIndex(node, index, lambda);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline BasePriorityQueue& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, BasePair<THeap, TKey, TValue>&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TKey, TValue>&>) {
							auto node = this->This->heap;
							if (node == nullptr)
							{
								ForEachImplWithoutIndex(node, lambda);
							}
							return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, BasePair<THeap, TKey, TValue>&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TKey, TValue>&, SizeT>) {
							auto node = this->This->heap;
							if (node != nullptr)
							{
								SizeT index = 0;
								ForEachImplWithIndex(node, index, lambda);
							}
							return *this;
					}
					else {
						static_assert(false);
					}
				}
#pragma endregion
#pragma region For Each Key
				template<typename Lambda>
				__forceinline static void ForEachKeyImplWithIndex(Node* node, SizeT& Index, Lambda& lambda, Node* startNode = nullptr) {
					if (startNode = nullptr)
						startNode = node;
					else
						if (startNode == node)
							return;

					if (node->left != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					lambda(node->data.Key, Index); Index++;
					if (node->right != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					if (node->next != nullptr)
						ForEachImplWithIndex(node->next, Index, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachKeyImplWithoutIndex(Node* node, Lambda& lambda, Node* startNode = nullptr) {
					if (startNode = nullptr)
						startNode = node;
					else
						if (startNode == node)
							return;

					if (node->left != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					lambda(node->data.Key);
					if (node->right != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					if (node->next != nullptr)
						ForEachImplWithIndex(node->next, Index, lambda);
				}

				template<typename LambdaT>
				__forceinline BasePriorityQueue& ForEachKeyImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TKey&>) {
						auto node = this->This->heap;
						if (node == nullptr)
						{
							ForEachKeyImplWithoutIndex(node, lambda);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TKey&, SizeT>) {
						auto node = this->This->heap;
						if (node != nullptr)
						{
							SizeT index = 0;
							ForEachKeyImplWithIndex(node, index, lambda);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BasePriorityQueue& ForEachKeyImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TKey&>) {
						auto node = this->This->heap;
						if (node == nullptr)
						{
							ForEachKeyImplWithoutIndex(node, lambda);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TKey&, SizeT>) {
						auto node = this->This->heap;
						if (node != nullptr)
						{
							SizeT index = 0;
							ForEachKeyImplWithIndex(node, index, lambda);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}
#pragma endregion
#pragma region For Each Value
				template<typename Lambda>
				__forceinline static void ForEachValueImplWithIndex(Node* node, SizeT& Index, Lambda& lambda, Node* startNode = nullptr) {
					if (startNode = nullptr)
						startNode = node;
					else
						if (startNode == node)
							return;

					if (node->left != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					lambda(node->data.Value, Index); Index++;
					if (node->right != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					if (node->next != nullptr)
						ForEachImplWithIndex(node->next, Index, lambda);
				}

				template<typename Lambda>
				__forceinline static void ForEachValueImplWithoutIndex(Node* node, Lambda& lambda, Node* startNode = nullptr) {
					if (node->left != nullptr)
						ForEachValueImplWithoutIndex(node->left, lambda);
					lambda(node->data.Value);
					if (node->right != nullptr)
						ForEachValueImplWithoutIndex(node->left, lambda);

					if (startNode = nullptr)
						startNode = node;
					else
						if (startNode == node)
							return;

					if (node->left != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					lambda(node->data.Value);
					if (node->right != nullptr)
						ForEachImplWithIndex(node->left, Index, lambda);
					if (node->next != nullptr)
						ForEachImplWithIndex(node->next, Index, lambda);
				}

				template<typename LambdaT>
				__forceinline const BasePriorityQueue& ForEachValueImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TValue>& > ) {
							auto node = this->This->heap;
							if (node == nullptr)
							{
								ForEachValueImplWithoutIndex(node, lambda);
							}
							return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TValue&, SizeT>) {
							auto node = this->This->heap;
							if (node != nullptr)
							{
								SizeT index = 0;
								ForEachValueImplWithIndex(node, index, lambda);
							}
							return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline BasePriorityQueue& ForEachValueImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, TValue&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TValue&>) {
						auto node = this->This->root;
						if (node == nullptr)
						{
							ForEachValueImplWithoutIndex(node, lambda);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, TValue&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TValue&, SizeT>) {
							auto node = this->This->root;
							if (node != nullptr)
							{
								SizeT index = 0;
								ForEachValueImplWithIndex(node, index, lambda);
							}
							return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename Lambda>
				void deleteAll(Node* n, Lambda& lambda) {
					if (n != nullptr) {
						Node* c = n;
						do {
							Node* d = c;
							c = c->next;
							deleteAll(d->child);
							lambda(MoveRValue(d->data));
							delete d;
						} while (c != n);
					}
				}

				__forceinline BasePriorityQueue& RemoveAllImpl() {
					this->Dispose();
					return *this;
				}

				template<typename Lambda>
				__forceinline BasePriorityQueue& RemoveAllWithImpl(Lambda&& lambda) {
					if (This->heap != nullptr)
						deleteAll(This->heap, lambda);
					This->heap = nullptr;
					This->count = 0;
					return *this;
				}
#pragma endregion
			public:
				RefCountClass(BasePriorityQueue, This)
			};

			template<typename THeap, typename TKey, typename TVal>
			__forceinline void Swap(BasePriorityQueue<THeap, TKey, TVal>& A, BasePriorityQueue<THeap, TKey, TVal>& B) {
				Swap(A.This, B.This);
			}

			namespace {
				template<typename TKey, typename TVal>
				using SecuredPriorityQueue = BasePriorityQueue<Seed::Core::Memory::SensitiveDataHeap, TKey, TVal>;
			}

			namespace {
				template<typename TKey, typename TVal>
				using PriorityQueue = BasePriorityQueue<Seed::Core::Memory::Heap, TKey, TVal>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename TKey, typename TVal>
				struct IsPriorityQueueT<Seed::Core::Containers::BasePriorityQueue<THeap, TKey, TVal>> : LogicTrue {};
			}
		}
	}
}
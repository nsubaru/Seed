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
			class BasePriorityQueue final :
			public Memory::Allocator<THeap>,
				public LINQ::IPriorityQueue<BasePriorityQueue, THeap, TPriority, TValue, BasePair<THeap, TPriority, TValue>, true>,
				protected IDisposable,
				public IClonable<BasePriorityQueue<THeap, TPriority, TValue>>
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

					forceinline Node* getPrev() { return prev; }
					forceinline Node* getNext() { return next; }
					forceinline Node* getChild() { return child; }
					forceinline Node* getParent() { return parent; }
					forceinline const TPriority& getValue() { return data.Key; }
					forceinline TValue& getData() { return data.Value; }
					forceinline bool isMarked() { return marked; }

					forceinline bool hasChildren() { return child; }
					forceinline bool hasParent() { return parent; }

					//Конструктори
#pragma warning(push)
//Visual Studio bug, next and prev init as this
#pragma warning(disable: 26495)
					forceinline Node(Pair&& data) :
						data(MoveRValue(data)),
						prev(this),
						next(this),
						degree(0),
						marked(false),
						child(nullptr),
						parent(nullptr)
					{}

					forceinline Node(const Pair& data) :
						data(data),
						prev(this),
						next(this),
						degree(0),
						marked(false),
						child(nullptr),
						parent(nullptr)
					{}

					forceinline Node(const Pair& data, Node* prev) :
						data(data),
						prev(prev),
						next(this),
						degree(0),
						marked(false),
						child(nullptr),
						parent(nullptr)
					{}

					forceinline Node(Pair&& data, Node* prev) :
						data(MoveRValue(data)),
						prev(prev),
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

					forceinline Tree(SizeT MaxSize) :
						heap(nullptr),
						Count(0),
						RefCount(1),
						MaxSize(MaxSize)
					{}
				};
				Tree* This;
			protected:
				forceinline virtual void Dispose() {
					if (This->heap != nullptr) {
						deleteAll(const_cast<Node*>(This->heap));
						This->heap = nullptr;
						This->Count = 0;
					}
				}
			public:
				forceinline BasePriorityQueue() : This(new Tree(CurrentSettings().MemoryManager.DefaultPriorityQueueMaxSize)) {}
				forceinline BasePriorityQueue(SizeT MaxSize) : This(new Tree(MaxSize)) {}
				forceinline BasePriorityQueue(InitilizerList<Pair>&& list) :
					BasePriorityQueue(CurrentSettings().MemoryManager.DefaultPriorityQueueMaxSize + list.size()){
					for (auto&& item : list) {
						This->heap = merge(This->heap, new Node(MoveRValue(item)));
					}
					This->Count = list.size();
				}
				forceinline BasePriorityQueue(SizeT MaxSize, InitilizerList<Pair>&& list) : BasePriorityQueue(MaxSize) {
					for (auto&& item : list) {
						This->heap = merge(This->heap, new Node(MoveRValue(item)));
					}
					This->Count = list.size();
				}

				forceinline void Insert(const TPriority& priority, const TValue& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(priority, key));
					This->heap = merge(const_cast<Node*>(This->heap), ret);
					++this->This->Count;
				}

				forceinline void Insert(TPriority&& priority, const TValue& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(MoveRValue(priority), key));
					This->heap = merge(const_cast<Node*>(This->heap), ret);
					++this->This->Count;
				}

				forceinline void Insert(const TPriority& priority, TValue&& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(priority, MoveRValue(key)));
					This->heap = merge(const_cast<Node*>(This->heap), ret);
					++this->This->Count;
				}

				forceinline void Insert(TPriority&& priority, TValue&& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(MoveRValue(priority), MoveRValue(key)));
					This->heap = merge(This->heap, ret);
					++this->This->Count;
				}

				forceinline void Insert(const Pair& itemValue) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(itemValue));
					This->heap = merge(This->heap, ret);
					++this->This->Count;
				}

				forceinline void Insert(Pair&& itemValue) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(MoveRValue(itemValue));
					This->heap = merge(This->heap, ret);
					++this->This->Count;
				}

				forceinline bool getIsEmpty() const {
					return this->This->heap == nullptr || this->This->Count == 0;
				}

				forceinline SizeT getCount() const {
					return this->This->Count;
				}

				forceinline const TPriority& getFirstItemPriority() {
					return This->heap->data.Key;
				}

				forceinline const TPriority& discardFirstItem() {
					Node* old = This->heap;
					This->heap = removeMinimum(This->heap);
					const TPriority& ret = old->data.Key;
					delete old;
					return ret;
				}

				forceinline TValue Dequeu() {
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

				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);

				forceinline virtual BasePriorityQueue Clone() const override final {
					BasePriorityQueue result;

					result.This->Count = This->Count;
					result.This->MaxSize = This->MaxSize;
					result.This->heap = copyTree(This->heap, nullptr);

					return result;
				}

				Boolean Property(getIsEmpty) IsEmpty;
				SizeT Property(getCount) Count;
			private:
				forceinline Node* merge(Node* a, Node* b) {
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

				forceinline static void deleteAll(Node* n) {
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

				forceinline void addChild(Node* parent, Node* child) {
					child->prev = child->next = child;
					child->parent = parent;
					parent->degree++;
					parent->child = merge(parent->child, child);
				}

				forceinline void _unMarkAndUnParentAll(Node* n) {
					if (n == nullptr)return;
					Node* c = n;
					do {
						c->marked = false;
						c->parent = nullptr;
						c = c->next;
					} while (c != n);
				}

				forceinline Node* removeMinimum(Node* n) {
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

				forceinline Node* cut(Node* heap, Node* n) {
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

				forceinline Node* decreaseKey(Node* heap, Node* n, const TPriority& priority) {
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

				forceinline Node* find(Node* heap, const TPriority& priority) {
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

				forceinline Node* copyTree(Node* orig, Node* parrent) const {
					Node* newNode = nullptr;
					Node* newNodeFirst = nullptr;
					try {
						if (orig == nullptr)
							return nullptr;

						Node* cpOrig = orig;
						newNode = new Node(cpOrig->data); newNode->parent = parrent; newNode->marked = cpOrig->marked;
						newNodeFirst = newNode;

						newNode->child = copyTree(cpOrig->child, newNode);
						cpOrig = cpOrig->next;

						while (true) {
							newNode = newNode->next = new Node(cpOrig->data, newNode); newNode->parent = parrent; newNode->marked = cpOrig->marked;
							newNode->child = copyTree(cpOrig->child, newNode);

							if (cpOrig->next == orig)
								break;
							else
								cpOrig = cpOrig->next;
						}

						newNode->next = newNodeFirst;
						newNodeFirst->prev = newNode;

						return newNodeFirst;
					}
					catch (...) {
						Node* tmp;
						while (newNode != nullptr) {
							tmp = newNode; newNode = newNode->next;
							deleteAll(tmp->child);
							delete tmp;
						}
						ReThrow;
					}
				}
			protected:
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				friend class LINQ::IAssociableCollection;
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				friend class LINQ::IPriorityQueue;
                template<typename THeap, typename T, typename Enable>
                friend struct Seed::Core::NewSerilization::Details::BaseSerlizationHandler;
#pragma region ForEach Pair
				template<typename Lambda>
				forceinline static void ForEachImplWithIndex(Node* node, SizeT& Index, Lambda& lambda) {
					Node* it = node;
					do {
						lambda(it->data, Index); Index++;
						if (it->child != nullptr)
							ForEachImplWithIndex(it->child, Index, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename Lambda>
				forceinline static void ForEachImplWithoutIndex(Node* node, Lambda& lambda) {
					Node* it = node;
					do {
						lambda(it->data);
						if (it->child != nullptr)
							ForEachImplWithoutIndex(it->child, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename LambdaT>
				forceinline const BasePriorityQueue& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TPriority, TValue>&>) {
						auto node = this->This->heap;
						if (node != nullptr)
						{
							ForEachImplWithoutIndex(node, lambda);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TPriority, TValue>&, SizeT>) {
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

				template<typename LambdaT>
				forceinline BasePriorityQueue& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, BasePair<THeap, TPriority, TValue>&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TPriority, TValue>&>) {
							auto node = this->This->heap;
							if (node != nullptr)
							{
								ForEachImplWithoutIndex(node, lambda);
							}
							return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, BasePair<THeap, TPriority, TValue>&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const BasePair<THeap, TPriority, TValue>&, SizeT>) {
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
				forceinline static void ForEachKeyImplWithIndex(Node* node, SizeT& Index, Lambda& lambda) {
					Node* it = node;
					do{
						lambda(it->data.Key, Index); Index++;
						if (it->child != nullptr)
							ForEachKeyImplWithIndex(it->child, Index, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename Lambda>
				forceinline static void ForEachKeyImplWithoutIndex(Node* node, Lambda& lambda) {
					Node* it = node;
					do {
						lambda(it->data.Key);
						if (it->child != nullptr)
							ForEachKeyImplWithoutIndex(it->child, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename LambdaT>
				forceinline BasePriorityQueue& ForEachKeyImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TPriority&>) {
						auto node = this->This->heap;
						if (node != nullptr)
						{
							ForEachKeyImplWithoutIndex(node, lambda);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TPriority&, SizeT>) {
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
				forceinline const BasePriorityQueue& ForEachKeyImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TPriority&>) {
						auto node = this->This->heap;
						if (node != nullptr)
						{
							ForEachKeyImplWithoutIndex(node, lambda);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TPriority&, SizeT>) {
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
				forceinline static void ForEachValueImplWithIndex(Node* node, SizeT& Index, Lambda& lambda, Node* startNode = nullptr) {
					Node* it = node;
					do {
						lambda(it->data.Value, Index); Index++;
						if (it->child != nullptr)
							ForEachValueImplWithIndex(it->child, Index, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename Lambda>
				forceinline static void ForEachValueImplWithoutIndex(Node* node, Lambda& lambda, Node* startNode = nullptr) {
					Node* it = node;
					do {
						lambda(it->data.Value);
						if (it->child != nullptr)
							ForEachValueImplWithoutIndex(it->child, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename LambdaT>
				forceinline const BasePriorityQueue& ForEachValueImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TValue&> ) {
							auto node = this->This->heap;
							if (node != nullptr)
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
				forceinline BasePriorityQueue& ForEachValueImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, TValue&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TValue&>) {
						auto node = this->This->heap;
						if (node != nullptr)
						{
							ForEachValueImplWithoutIndex(node, lambda);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, TValue&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const TValue&, SizeT>) {
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

				template<typename Lambda>
				forceinline void deleteAll(Node* n, Lambda& lambda) {
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

				forceinline BasePriorityQueue& RemoveAllImpl() {
					this->Dispose();
					return *this;
				}

				template<typename Lambda>
				forceinline BasePriorityQueue& RemoveAllWithImpl(Lambda&& lambda) {
					if (This->heap != nullptr)
						deleteAll(This->heap, lambda);
					This->heap = nullptr;
					This->count = 0;
					return *this;
				}

				forceinline void UnsafeAdd(Pair&& pair){
					Insert(MoveRValue(pair));
				}
#pragma endregion
			public:
				RefCountClass(BasePriorityQueue, This)
			};

			namespace {
				template<typename TKey, typename TVal>
				using SensitivePriorityQueue = BasePriorityQueue<Seed::Core::Memory::SensitiveDataHeap, TKey, TVal>;
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

		template<typename THeap, typename TKey, typename TVal>
		forceinline void Swap(Containers::BasePriorityQueue<THeap, TKey, TVal>& A, Containers::BasePriorityQueue<THeap, TKey, TVal>& B) {
			Swap(A.This, B.This);
		}
	}
}
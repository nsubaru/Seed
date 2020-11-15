#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Containers.Pair.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Threading.DataFlowControler.h"
#include "Seed.Core.SettingsAPI.h"

#pragma warning(push)
//Visual Studio bug, next and prev init as this
#pragma warning(disable: 26495)

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

					ForceInline Node* getPrev() { return prev; }
					ForceInline Node* getNext() { return next; }
					ForceInline Node* getChild() { return child; }
					ForceInline Node* getParent() { return parent; }
					ForceInline const TPriority& getValue() { return data.Key; }
					ForceInline TValue& getData() { return data.Value; }
					ForceInline bool isMarked() { return marked; }

					ForceInline bool hasChildren() { return child; }
					ForceInline bool hasParent() { return parent; }

					//Конструктори
					ForceInline Node(Pair&& data) :
						data(MoveRValue(data)),
						prev(this),
						next(this),
						degree(0),
						marked(false),
						child(nullptr),
						parent(nullptr)
					{}

					ForceInline Node(const Pair& data) :
						data(data),
						prev(this),
						next(this),
						degree(0),
						marked(false),
						child(nullptr),
						parent(nullptr)
					{}

					ForceInline Node(const Pair& data, Node* prev) :
						data(data),
						prev(prev),
						next(this),
						degree(0),
						marked(false),
						child(nullptr),
						parent(nullptr)
					{}

					ForceInline Node(Pair&& data, Node* prev) :
						data(MoveRValue(data)),
						prev(prev),
						next(this),
						degree(0),
						marked(false),
						child(nullptr),
						parent(nullptr)
					{}
				};
			private:
				struct Tree : public Memory::Allocator<THeap> {
					Node* heap;
					RefCountT Count;
					RefCountT RefCount;
					SizeT MaxSize;

					ForceInline Tree(SizeT MaxSize) :
						heap(nullptr),
						Count(0),
						RefCount(1),
						MaxSize(MaxSize)
					{}
				};
				Tree* This;
			protected:
				ForceInline virtual void Dispose() {
					if (This->heap != nullptr) {
						deleteAll(const_cast<Node*>(This->heap));
						This->heap = nullptr;
						This->Count = 0;
					}
				}
			public:
				/// <summary>
				/// Пустий конструктор, максимальний розмір черги береться з налаштування ядра
				/// </summary>
				ForceInline BasePriorityQueue() : This(new Tree(CurrentSettings().MemoryManager.DefaultPriorityQueueMaxSize)) {}
				/// <summary>
				/// Конструктор з максимальним розміром
				/// </summary>
				/// <param name="MaxSize">Максимальний розмір</param>
				ForceInline BasePriorityQueue(SizeT MaxSize) : This(new Tree(MaxSize)) {}
				// <summary>
				/// Конструктор з списку ініціалізації, 
				/// розмір береться як сума кількості елементів і стандартний розмір з налаштувань
				/// </summary>
				/// <param name="list">Список елементів</param>
				ForceInline BasePriorityQueue(InitilizerList<Pair>&& list) :
					BasePriorityQueue(CurrentSettings().MemoryManager.DefaultPriorityQueueMaxSize + list.size()){
					for (auto&& item : list) {
						This->heap = merge(This->heap, new Node(MoveRValue(item)));
					}
					This->Count = list.size();
				}
				// <summary>
				/// Конструктор з макимальний розмір і списком елементів
				/// </summary>
				/// <param name="MaxSize">Максимальний розмір, якщо в спику більше елементів то вони проігноруються</param>
				/// <param name="list">Список елементів</param>
				ForceInline BasePriorityQueue(SizeT MaxSize, InitilizerList<Pair>&& list) : 
					BasePriorityQueue(
						MaxSize > list.size() ? 
						MaxSize : 
						list.size()
						) 
				{
					auto it = list.begin();
					auto copyItemsCount = MaxSize > list.size() ? list.size() : MaxSize;
					for (SizeT i = 0; i <copyItemsCount; i++, ++it)
					{
						This->heap = merge(This->heap, new Node(MoveRValue(*it)));
					}
					This->Count = list.size();
				}

				/// <summary>
				/// Вставка елемента з пріоритетом і значенням
				/// </summary>
				/// <param name="priority">Пріоритет</param>
				/// <param name="value">Значення</param>
				ForceInline void Insert(const TPriority& priority, const TValue& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(priority, key));
					This->heap = merge(const_cast<Node*>(This->heap), ret);
					++this->This->Count;
				}

				/// <summary>
				/// Вставка елемента з пріоритетом і значенням
				/// </summary>
				/// <param name="priority">Пріоритет</param>
				/// <param name="value">Значення</param>
				ForceInline void Insert(TPriority&& priority, const TValue& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(MoveRValue(priority), key));
					This->heap = merge(const_cast<Node*>(This->heap), ret);
					++this->This->Count;
				}

				/// <summary>
				/// Вставка елемента з пріоритетом і значенням
				/// </summary>
				/// <param name="priority">Пріоритет</param>
				/// <param name="value">Значення</param>
				ForceInline void Insert(const TPriority& priority, TValue&& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(priority, MoveRValue(key)));
					This->heap = merge(const_cast<Node*>(This->heap), ret);
					++this->This->Count;
				}

				/// <summary>
				/// Вставка елемента з пріоритетом і значенням
				/// </summary>
				/// <param name="priority">Пріоритет</param>
				/// <param name="value">Значення</param>
				ForceInline void Insert(TPriority&& priority, TValue&& key) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(MoveRValue(priority), MoveRValue(key)));
					This->heap = merge(This->heap, ret);
					++this->This->Count;
				}

				/// <summary>
				/// Вставка елемента з пріоритетом і значенням
				/// </summary>
				/// <param name="priority">Пріоритет</param>
				/// <param name="value">Значення</param>
				ForceInline void Insert(const Pair& itemValue) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(Pair(itemValue));
					This->heap = merge(This->heap, ret);
					++this->This->Count;
				}

				/// <summary>
				/// Вставка елемента з пріоритетом і значенням
				/// </summary>
				/// <param name="priority">Пріоритет</param>
				/// <param name="value">Значення</param>
				ForceInline void Insert(Pair&& itemValue) {
					if (this->This->Count == this->This->MaxSize)
						Throw(NullPointException(ExceptionPosition));
					Node* ret = new Node(MoveRValue(itemValue));
					This->heap = merge(This->heap, ret);
					++this->This->Count;
				}

				/// <summary>
				/// Чи черга пуста
				/// </summary>
				/// <returns>Вертає true якщо черга пуста</returns>
				ForceInline bool GetIsEmpty() const {
					return this->This->heap == nullptr || this->This->Count == 0;
				}

				/// <summary>
				/// Вертає кількість елементів
				/// </summary>
				/// <returns>Кількість елементів в черзі</returns>
				ForceInline SizeT GetCount() const {
					return this->This->Count;
				}

				/// <summary>
				/// Вертає самий верщий пріоритет який ж активним в черзі
				/// </summary>
				/// <returns>Значення пріоритету</returns>
				ForceInline const TPriority& TopKey() {
					return This->heap->data.Key;
				}

				/// <summary>
				/// Забирає верхній елемент і вертаж його ключ лиш
				/// </summary>
				/// <returns>Ключ верхнього елементу</returns>
				ForceInline const TPriority& DiscardFirstItem() {
					Node* old = This->heap;
					This->heap = removeMinimum(This->heap);
					const TPriority& ret = old->data.Key;
					delete old;
					return ret;
				}

				/// <summary>
				/// Виймає верхній елемент і вертає його значення
				/// </summary>
				/// <returns>Значення елемента</returns>
				ForceInline TValue Dequeue() {
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

				/// <summary>
				/// Клонування об'єкта
				/// </summary>
				/// <returns>Новий об'єкт з копією всіх елементів</returns>
				ForceInline virtual BasePriorityQueue Clone() const override final {
					BasePriorityQueue result;

					result.This->Count = This->Count;
					result.This->MaxSize = This->MaxSize;
					result.This->heap = copyTree(This->heap, nullptr);

					return result;
				}

				/// <summary>
				/// True якщо черга пуста
				/// </summary>
				Boolean Property(GetIsEmpty) IsEmpty;

				/// <summary>
				/// Поточна кількість елементів в пріоритетній черзі
				/// </summary>
				SizeT Property(GetCount) Count;

				/// <summary>
				/// Вертає максимальну кількість елементів
				/// </summary>
				/// <returns>Максимальна кількість елементів</returns>
				SizeT GetMaxSize() const {
					return This->MaxSize;
				}

				/// <summary>
				/// Встановлює максимальну кількість елементів яка не можу бути меншою ніж поточна кількість і та яка є стандартною
				/// </summary>
				/// <param name="newMaxSize">Нова максимальна кількість</param>
				void SetMaxSize(SizeT newMaxSize) {
					if (newMaxSize < This->Count || newMaxSize < CurrentSettings().MemoryManager.DefaultPriorityQueueMaxSize)
						Throw(
							InvalidArgumentException(
								ExceptionPosition,
								L"newMaxSize",
								LR"(Нова максимальна кількість має бути не меншою ніж поточна кількість елементів і не меншою ніж станадартне значення)"
								)
							);
					This->MaxSize = newMaxSize;
				}

				/// <summary>
				/// Максимальний розмір черги
				/// </summary>
				SizeT Property(GetMaxSize, SetMaxSize) MaxSize;
			private:
				ForceInline static Node* merge(Node* a, Node* b) {
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

				ForceInline static void deleteAll(Node* n) {
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

				ForceInline static void addChild(Node* parent, Node* child) {
					child->prev = child->next = child;
					child->parent = parent;
					parent->degree++;
					parent->child = merge(parent->child, child);
				}

				ForceInline static void _unMarkAndUnParentAll(Node* n) {
					if (n == nullptr)return;
					Node* c = n;
					do {
						c->marked = false;
						c->parent = nullptr;
						c = c->next;
					} while (c != n);
				}

				static Node* removeMinimum(Node* n) {
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

				ForceInline static Node* cut(Node* heap, Node* n) {
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

				static Node* decreaseKey(Node* heap, Node* n, const TPriority& priority) {
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

				static Node* find(Node* heap, const TPriority& priority) {
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

				static Node* copyTree(Node* orig, Node* parrent) {
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
                friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
#pragma region ForEach Pair
				template<typename Lambda>
				static void ForEachImplWithIndex(Node* node, SizeT& Index, Lambda& lambda) {
					Node* it = node;
					do {
						lambda(it->data, Index); Index++;
						if (it->child != nullptr)
							ForEachImplWithIndex(it->child, Index, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename Lambda>
				static void ForEachImplWithoutIndex(Node* node, Lambda& lambda) {
					Node* it = node;
					do {
						lambda(it->data);
						if (it->child != nullptr)
							ForEachImplWithoutIndex(it->child, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename LambdaT>
				ForceInline const BasePriorityQueue& ForEachImpl(LambdaT&& lambda) const {
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
				ForceInline BasePriorityQueue& ForEachImpl(LambdaT&& lambda) {
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
				static void ForEachKeyImplWithIndex(Node* node, SizeT& Index, Lambda& lambda) {
					Node* it = node;
					do{
						lambda(it->data.Key, Index); Index++;
						if (it->child != nullptr)
							ForEachKeyImplWithIndex(it->child, Index, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename Lambda>
				static void ForEachKeyImplWithoutIndex(Node* node, Lambda& lambda) {
					Node* it = node;
					do {
						lambda(it->data.Key);
						if (it->child != nullptr)
							ForEachKeyImplWithoutIndex(it->child, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename LambdaT>
				ForceInline BasePriorityQueue& ForEachKeyImpl(LambdaT&& lambda) {
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
				ForceInline const BasePriorityQueue& ForEachKeyImpl(LambdaT&& lambda) const {
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
				static void ForEachValueImplWithIndex(Node* node, SizeT& Index, Lambda& lambda, Node* startNode = nullptr) {
					Node* it = node;
					do {
						lambda(it->data.Value, Index); Index++;
						if (it->child != nullptr)
							ForEachValueImplWithIndex(it->child, Index, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename Lambda>
				static void ForEachValueImplWithoutIndex(Node* node, Lambda& lambda, Node* startNode = nullptr) {
					Node* it = node;
					do {
						lambda(it->data.Value);
						if (it->child != nullptr)
							ForEachValueImplWithoutIndex(it->child, lambda);
						it = it->next;
					} while (it != node);
				}

				template<typename LambdaT>
				ForceInline const BasePriorityQueue& ForEachValueImpl(LambdaT&& lambda) const {
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
				ForceInline BasePriorityQueue& ForEachValueImpl(LambdaT&& lambda) {
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
				static void deleteAll(Node* n, Lambda& lambda) {
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

				ForceInline BasePriorityQueue& RemoveAllImpl() {
					if (This->heap != nullptr) {
						deleteAll(const_cast<Node*>(This->heap));
						This->heap = nullptr;
						This->Count = 0;
					}
					return *this;
				}

				template<typename Lambda>
				ForceInline BasePriorityQueue& RemoveAllWithImpl(Lambda&& lambda) {
					if (This->heap != nullptr)
						deleteAll(This->heap, lambda);
					This->heap = nullptr;
					This->count = 0;
					return *this;
				}

				ForceInline void UnsafeAdd(Pair&& pair){
					Insert(MoveRValue(pair));
				}

				ForceInline void UnsafeAdd(const Pair& pair){
					Insert(pair);
				}
#pragma endregion
			public:
				RefCountClass(BasePriorityQueue, This)
			};

			///<summary>Пріоритетна черга для обробки даних в порядку приходу з урахуванням пріоритету в чутливій кучі
			///<para>Працює через фібоначеву кучу. Ітерація відсутня</para></summary>
			namespace {
				template<typename TKey, typename TVal>
				using SensitivePriorityQueue = BasePriorityQueue<Seed::Core::Memory::SensitiveDataHeap, TKey, TVal>;
			}

			///<summary>Пріоритетна черга для обробки даних в порядку приходу з урахуванням пріоритету в простій кучі
			///<para>Працює через фібоначеву кучу. Ітерація відсутня</para></summary>
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
		ForceInline void Swap(Containers::BasePriorityQueue<THeap, TKey, TVal>& A, Containers::BasePriorityQueue<THeap, TKey, TVal>& B) {
			Swap(A.This, B.This);
		}
	}
}

#pragma warning(pop)

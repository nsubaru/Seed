#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Threading.h"

namespace Seed {
	inline namespace Core {
		///<summary>Пріоритетна черга для обробки даних в порядку приходу з урахуванням пріоритету
		///<para>Працює через фібоначеву кучу. Ітерація відсутня</para></summary>
		template <class THeap, typename TPriority, typename TValue>
		class BasePriorityQueue : public Memory::Allocator<THeap>, IDisposable {
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
				///<summary>Значення пріоритету</summary>
				TPriority priority;
				///<summary>Значення елемента</summary>
				TValue value;
				///<summary></summary>
				int degree;
				///<summary></summary>
				bool marked;

				///<summary>Конструктор</summary>
				__forceinline Node(const TPriority& Key, const TValue& Value) :
					priority(Key),
					value(Value),
					prev(this),
					next(this),
					degree(0),
					marked(false),
					child(nullptr),
					parent(nullptr)
				{}

				///<summary>Конструктор</summary>
				__forceinline Node(const TPriority& Key, TValue&& Value) :
					priority(MoveRValue(Key)),
					value(Value),
					prev(this),
					next(this),
					degree(0),
					marked(false),
					child(nullptr),
					parent(nullptr)
				{}

				///<summary>Конструктор</summary>
				__forceinline Node(TPriority&& Key, const TValue& Value) :
					priority(MoveRValue(Key)),
					value(Value),
					prev(this),
					next(this),
					degree(0),
					marked(false),
					child(nullptr),
					parent(nullptr)
				{}

				///<summary>Конструктор</summary>
				__forceinline Node(TPriority&& Key, TValue&& Value) :
					priority(MoveRValue(Key)),
					value(MoveRValue(Value)),
					prev(this),
					next(this),
					degree(0),
					marked(false),
					child(nullptr),
					parent(nullptr)
				{}

				friend class BasePriorityQueue<THeap, TPriority, TValue>;
				
				///Гетери
				
				__forceinline Node* getPrev() { return prev; }
				__forceinline Node* getNext() { return next; }
				__forceinline Node* getChild() { return child; }
				__forceinline Node* getParent() { return parent; }
				__forceinline TPriority getValue() { return priority; }
				__forceinline TValue getData() { return value; }
				__forceinline bool isMarked() { return marked; }

				__forceinline bool hasChildren() { return child; }
				__forceinline bool hasParent() { return parent; }
			};
		private:
			struct Tree {
				Node* heap;
				RefCountT Count;
				RefCountT RefCount;
				Threading::SRWLock lock;
				Threading::ConditionVariable condVariable;
				Tree() : heap(nullptr), Count(0), lock(), RefCount(1) {}
			};
			Tree* tree;
		protected:
			virtual void Dispose() final override {
				if (tree->heap != nullptr) {
					deleteAll(tree->heap);
					tree->heap = nullptr;
				}
			}
		public:
			BasePriorityQueue() : tree(new Tree) {}

			RefCountClass(BasePriorityQueue, tree);

			void Insert(const TPriority& priority, const TValue& key) {
				this->tree->lock.ExclusiveLock([&]() {
					Node* ret = new Node(priority, key);
					tree->heap = merge(tree->heap, ret);
					tree->Count++;
					tree->condVariable.WakeOne();
					});
			}
			void Insert(const TPriority& priority, TValue&& key) {
				this->tree->lock.ExclusiveLock([&, key = MoveRValue(key)]() mutable{
					Node* ret = new Node(priority, key);
					tree->heap = merge(tree->heap, ret);
					tree->Count++;
					tree->condVariable.WakeOne();
				});
			}
			void Insert(TPriority&& priority, const TValue& key) {
				this->tree->lock.ExclusiveLock([&, priority = MoveRValue(priority)]() mutable{
					Node* ret = new Node(priority, key);
					tree->heap = merge(tree->heap, ret);
					tree->Count++;
					tree->condVariable.WakeOne();
				});
			}
			void Insert(TPriority&& priority, TValue&& key) {
				this->tree->lock.ExclusiveLock([&, priority = MoveRValue(priority), key = MoveRValue(key)]() mutable {
					Node* ret = new Node(priority, key);
					tree->heap = merge(tree->heap, ret);
					tree->Count++;
					tree->condVariable.WakeOne();
				});
			}

			void Merge(const BasePriorityQueue& other) {
				other.tree->lock.ExclusiveLock([&]() {
					this->tree->lock.ExclusiveLock([&]() {
						this->merge(this->tree->heap, other.tree->heap);
						this->tree->Count = this->tree->Count + other.tree->Count;
						this->tree->condVariable.WakeOne();
						});
					other.tree->Count = 0;
					});
			}

			BasePriorityQueue& operator+=(BasePriorityQueue&& other) {
				return this->tree->lock.ExclusiveLock([&, other = MoveRValue(other)]() mutable {
					this->merge(this->tree->heap, other.tree->heap);
					this->tree->Count = this->tree->Count + other.tree->Count;
					this->tree->condVariable.WakeOne();
					other.tree->heap = nullptr;
					other.~BasePriorityQueue();
					return *this;
					});
			}

			bool getIsEmpty() {
				return tree->heap == nullptr;
			}

			SizeT getCount() { 
				return this->tree->Count; 
			}

			TPriority getFirstItemPriority() {
				return tree->heap->priority;
			}

			Boolean Property(getIsEmpty) IsEmpty;
			SizeT Property(getCount) Count;

			TPriority discardFirstItem() {
				Node* old = tree->heap;
				tree->heap = removeMinimum(tree->heap);
				TPriority ret = old->priority;
				delete old;
				return ret;
			}

			TValue getFirstWhenAquire() {
				if(this->tree->Count == 0)
					tree->condVariable.Wait();
				return tree->lock.ExclusiveLock([&]() {
					Node* old = tree->heap;
					tree->heap = removeMinimum(tree->heap);
					TValue ret(MoveRValue(old->value));
					this->tree->Count--;
					delete old;
					return ret;
					});
			}
		private:
			Node* merge(Node* a, Node* b) {
				if (a == nullptr) return b;
				if (b == nullptr) return a;
				if (a->priority > b->priority) {
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
						if (n->priority < t->priority) {
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
					if (n->priority < min->priority)min = n;
					n = n->next;
				} while (n != n);
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

			Node* decreaseKey(Node* heap, Node* n, TPriority priority) {
				if (n->priority < priority)return heap;
				n->priority = priority;
				if (n->priority < n->parent->priority) {
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

			Node* find(Node* heap, TPriority priority) {
				Node* n = heap;
				if (n == nullptr) return nullptr;
				do {
					if (n->priority == priority)return n;
					Node* ret = find(n->child, priority);
					if (ret)return ret;
					n = n->next;
				} while (n != heap);
				return nullptr;
			}
		};

		namespace {
			template<typename TKey, typename TVal>
			using SecuredPriorityQueue = BasePriorityQueue<Seed::Core::Memory::SensitiveDataHeap, TKey, TVal>;
		}

		namespace {
			template<typename TKey, typename TVal>
			using PriorityQueue = BasePriorityQueue<Seed::Core::Memory::Heap, TKey, TVal>;
		}
	}
}
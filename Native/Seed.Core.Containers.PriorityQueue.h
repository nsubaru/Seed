#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Memory.h"

namespace Seed {
	inline namespace Core {
		template <class THeap, typename TPriority, typename TValue>
		class BasePriorityQueue : public Memory::Allocator<THeap> {
		private:
			struct Node : public Memory::Allocator<THeap> {
				Node* prev;
				Node* next;
				Node* child;
				Node* parent;
				TPriority priority;
				TValue value;
				int degree;
				bool marked;

				Node(const TValue& Key, const TPriority& Value) :
					priority(Value),
					value(Key),
					prev(this),
					next(this),
					degree(0),
					marked(false),
					child(nullptr),
					parent(nullptr)
				{}

				Node(const TValue& Key, TPriority&& Value) :
					priority(MoveRValue(Value)),
					value(Key),
					prev(this),
					next(this),
					degree(0),
					marked(false),
					child(nullptr),
					parent(nullptr)
				{}

				Node(TValue&& Key, const TPriority& Value) :
					priority(MoveRValue(Value)),
					value(Key),
					prev(this),
					next(this),
					degree(0),
					marked(false),
					child(nullptr),
					parent(nullptr)
				{}

				Node(TValue&& Key, TPriority&& Value) :
					priority(MoveRValue(Value)),
					value(MoveRValue(Key)),
					prev(this),
					next(this),
					degree(0),
					marked(false),
					child(nullptr),
					parent(nullptr)
				{}

				friend class BasePriorityQueue<THeap, TPriority, TValue>;
				Node* getPrev() { return prev; }
				Node* getNext() { return next; }
				Node* getChild() { return child; }
				Node* getParent() { return parent; }
				TPriority getValue() { return priority; }
				TValue getData() { return value; }
				bool isMarked() { return marked; }

				bool hasChildren() { return child; }
				bool hasParent() { return parent; }
			};
		protected:
			Node* heap;
		public:

			BasePriorityQueue() {
				heap = nullptr;
			}
			virtual ~BasePriorityQueue() {
				if (heap) {
					deleteAll(heap);
				}
			}
			Node* insert(const TPriority& priority, const TValue& key) {
				Node* ret = new Node(priority, key);
				heap = merge(heap, ret);
				return ret;
			}
			Node* insert(const TPriority& priority, TValue&& key) {
				Node* ret = new Node(priority, key);
				heap = merge(heap, ret);
				return ret;
			}
			Node* insert(TPriority&& priority, const TValue& key) {
				Node* ret = new Node(priority, key);
				heap = merge(heap, ret);
				return ret;
			}
			Node* insert(TPriority&& priority, TValue&& key) {
				Node* ret = new Node(priority, key);
				heap = merge(heap, ret);
				return ret;
			}
			void merge(BasePriorityQueue& other) {

			}

			bool isEmpty() {
				return heap == NULL;
			}

			TPriority getFirstItemPriority() {
				return heap->priority;
			}

			TPriority discardFirstItem() {
				Node* old = heap;
				heap = removeMinimum(heap);
				TPriority ret = old->priority;
				delete old;
				return ret;
			}

			TValue getFirst() {
				Node* old = heap;
				heap = removeMinimum(heap);
				TValue ret = old->value;
				delete old;
				return ret;
			}

			//void decreaseKey(Node* n, TPriority priority) {
			//	heap = decreaseKey(heap, n, priority);
			//}

			//Node* find(TPriority priority) {
			//	return find(heap, priority);
			//}

			const BasePriorityQueue& operator+=(BasePriorityQueue&& other) {
				heap = merge(heap, other.heap);
				other.heap = nullptr;
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
	}
}
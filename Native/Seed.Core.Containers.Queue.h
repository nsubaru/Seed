#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Threading.h"

namespace Seed {
	inline namespace Core {
		///<summary>Пріоритетна черга для обробки даних в порядку приходу з урахуванням пріоритету
		///<para>Працює через фібоначеву кучу. Ітерація відсутня</para></summary>
		template <class THeap, typename TValue>
		class SeedKernelCall BaseQueue final : public Memory::Allocator<THeap>, protected IDisposable{
		private:
			///<summary>Внутрішня структура елемента в черзі</summary>
			struct Node : public Memory::Allocator<THeap> {
				///<summary>Попередній елемент</summary>
				Node* prev;
				///<summary>Наступний елемент</summary>
				Node* next;
				///<summary>Значення елемента</summary>
				TValue value;

				///<summary>Конструктор</summary>
				__forceinline Node(const TValue& Value) :
					value(Value),
					prev(this),
					next(this),
				{}

				///<summary>Конструктор</summary>
				__forceinline Node(TValue&& Value) :
					value(Value),
					prev(this),
					next(this)
				{}

				friend class BaseQueue<THeap, TValue>;

				///Гетери

				__forceinline Node* getPrev() { return prev; }
				__forceinline Node* getNext() { return next; }
				__forceinline TValue getData() { return value; }
			};
		private:
			struct LinkedList : public Memory::Allocator<THeap> {
				volatile Node* head;
				volatile Node* tile;
				RefCountT Count;
				RefCountT RefCount;
				SizeT MaxSize;
				struct InternalPtr { ImplSpecificT Ptr; };
				InternalPtr Lock;
				InternalPtr QueueNotFull;
				InternalPtr QueueNotEmpty;

				LinkedList(SizeT MaxSize) :
					head(nullptr),
					Count(0),
					RefCount(1),
					MaxSize(MaxSize),
					Lock({ nullptr }),
					QueueNotFull({ nullptr }),
					QueueNotEmpty({ nullptr })
				{}

				void PushBack(Node* n)
				{
					Count++;
					if (head == nullptr)
						head = tail = n;
					else if (tail != nullptr)
					{
						tail->prev = n;
						n->next = tail;
						tail = n;
					}
				}
				Node* PickHeap()
				{
					Node& ret = head;
					if (tail == head)
					{
						Count--; 
						head = tail = nullptr;
						return ret;
					}
					if (Count > 1)
					{
						head->prev->next = nullptr;
						Count--;
						return ret;
					}
				}

			};
			LinkedList* list;
		protected:
			virtual void Dispose() {
				if (list->head != nullptr) {
					deleteAll(const_cast<Node*>(list->head));
					list->head = nullptr;
				}
			}
		public:
			BaseQueue() : list(new LinkedList(CurrentSettings().MemoryManager.QueueMaxSize)) { //-V730
				extern void InitSRWLock(void* PSRWLock);
				extern void InitCondVal(void* handle);

				InitSRWLock(&list->Lock);
				InitCondVal(&list->QueueNotFull);
				InitCondVal(&list->QueueNotEmpty);
			}

			BaseQueue(SizeT MaxSize) : list(new LinkedList(MaxSize)) { //-V730
				extern void InitSRWLock(void* PSRWLock);
				extern void InitCondVal(void* handle);

				InitSRWLock(&list->Lock);
				InitCondVal(&list->QueueNotFull);
				InitCondVal(&list->QueueNotEmpty);
			}

			RefCountClass(BaseQueue, list); //-V730

			void Insert(const TValue& value) {
				extern void AcquireLockExclusiveSRWLock(void* PSRWLock);
				extern void ReleaseLockExclusiveSRWLock(void* PSRWLock);
				extern void WaitCondVal(void* condVar, void* srwLock);
				extern void WakeOneCondVal(void* condVar);

				try {
					AcquireLockExclusiveSRWLock(&this->list->Lock);
					if (++this->list->Count > this->list->MaxSize)
						WaitCondVal(&this->list->QueueNotFull, &this->list->Lock);
					Node* ret = new Node(value);
					/*list->head = merge(const_cast<Node*>(list->head), ret);
					++list->Count;*/
					list->PushBack(ret);
					ReleaseLockExclusiveSRWLock(&this->list->Lock);
					WakeOneCondVal(&this->list->QueueNotEmpty);
				}
				catch (...) {
					ReleaseLockExclusiveSRWLock(&this->list->Lock);
					ReThrow;
				}
			}
			void Insert(TValue&& value) {
				extern void AcquireLockExclusiveSRWLock(void* PSRWLock);
				extern void ReleaseLockExclusiveSRWLock(void* PSRWLock);
				extern void WaitCondVal(void* condVar, void* srwLock);
				extern void WakeOneCondVal(void* condVar);

				try {
					AcquireLockExclusiveSRWLock(&this->list->Lock);
					if (++this->list->Count > this->list->MaxSize)
						WaitCondVal(&this->list->QueueNotFull, &this->list->Lock);
					Node* ret = new Node(MoveRValue(value));
					/*list->head = merge(const_cast<Node*>(list->head), ret);
					++list->Count;*/
					list->PushBack(ret);
					ReleaseLockExclusiveSRWLock(&this->list->Lock);
					WakeOneCondVal(&this->list->QueueNotEmpty);
				}
				catch (...) {
					ReleaseLockExclusiveSRWLock(&this->list->Lock);
					ReThrow;
				}
			}

			TValue& Pick()
			{
				extern void AcquireLockExclusiveSRWLock(void* PSRWLock);
				extern void ReleaseLockExclusiveSRWLock(void* PSRWLock);
				extern void WaitCondVal(void* condVar, void* srwLock);
				extern void WakeOneCondVal(void* condVar);
				TValue& ret;

				try {
					AcquireLockExclusiveSRWLock(&this->list->Lock);
					while (this->list->Count == 0)
					{
						WaitCondVal(&this->list->QueueNotEmpty, &this->list->Lock);
					}
					ret = list->PickHeap();
					ReleaseLockExclusiveSRWLock(&this->list->Lock);
					WakeOneCondVal(&this->list->QueueNotEmpty);
				}
				catch (...) {
					ReleaseLockExclusiveSRWLock(&this->list->Lock);
					ReThrow;
				}
				return ret;
			}
			/*
			void Merge(const BaseQueue& other) {
				this->merge(const_cast<Node*>(list->head), const_cast<Node*>(other.list->head));
				this->list->Count = this->list->Count + other.list->Count;
				other.list->Count = 0;
			}

			BaseQueue& operator+=(BaseQueue&& other) {
				this->merge(const_cast<Node*>(list->head), const_cast<Node*>(other.list->head));
				this->list->Count = this->list->Count + other.list->Count;
				other.list->head = nullptr;
				other.~BasePriorityQueue();
				return *this;
			}*/

			bool getIsEmpty() {
				return list->head == nullptr;
			}

			SizeT getCount() {
				return this->list->Count;
			}

			Boolean Property(getIsEmpty) IsEmpty;
			SizeT Property(getCount) Count;

			/*TPriority discardFirstItem() {
				volatile Node* old = list->head;
				list->head = removeMinimum(const_cast<Node*>(list->head));
				TPriority ret = old->priority;
				delete old;
				return ret;
			}*/

			/*TValue getFirstWhenAquire() {
				extern void AcquireLockExclusiveSRWLock(void* PSRWLock);
				extern void ReleaseLockExclusiveSRWLock(void* PSRWLock);
				extern void WaitCondVal(void* condVar, void* srwLock);
				extern void WakeOneCondVal(void* condVar);

				try {
					AcquireLockExclusiveSRWLock(&this->list->Lock);
					while (this->list->Count == 0)
					{
						WaitCondVal(&this->list->QueueNotEmpty, &this->list->Lock);
					}
					Node* old = const_cast<Node*>(list->head);
					list->head = removeMinimum(const_cast<Node*>(list->head));
					TValue ret(MoveRValue(old->value));
					--this->list->Count;
					delete old;
					ReleaseLockExclusiveSRWLock(&this->list->Lock);
					WakeOneCondVal(&this->list->QueueNotFull);
					return ret;
				}
				catch (...) {
					ReleaseLockExclusiveSRWLock(&this->list->Lock);
					ReThrow;
				}
			}*/
		private:
			/*Node* merge(Node* a, Node* b) {
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
			}*/

			/*void deleteAll(Node* n) {
				if (n != nullptr) {
					volatile  Node* c = n;
					do {
						volatile Node* d = c;
						c = c->next;
						deleteAll(d->child);
						delete d;
					} while (c != n);
				}
			}*/

			/*void addChild(Node* parent, Node* child) {
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
			}*/

			/*Node* removeMinimum(Node* n) {
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
					if (n->priority < min->priority) min = n;
					n = n->next;
				} while (min != n);
				return min;
			}*/

			/*Node* cut(Node* head, Node* n) {
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
				return merge(head, n);
			}

			Node* decreaseKey(Node* head, Node* n, TPriority priority) {
				if (n->priority < priority)return head;
				n->priority = priority;
				if (n->priority < n->parent->priority) {
					head = cut(head, n);
					Node* parent = n->parent;
					n->parent = nullptr;
					while (parent != nullptr && parent->marked) {
						head = cut(head, parent);
						n = parent;
						parent = n->parent;
						n->parent = nullptr;
					}
					if (parent != nullptr && parent->parent != nullptr)
						parent->marked = true;
				}
				return head;
			}

			Node* find(Node* head, TPriority priority) {
				Node* n = head;
				if (n == nullptr) return nullptr;
				do {
					if (n->priority == priority)return n;
					Node* ret = find(n->child, priority);
					if (ret)return ret;
					n = n->next;
				} while (n != head);
				return nullptr;
			}*/
		};

		namespace {
			template<typename TVal>
			using SecuredQueue = BaseQueue<Seed::Core::Memory::SensitiveDataHeap, TVal>;
		}

		namespace {
			template<typename TKey, typename TVal>
			using Queue = BaseQueue<Seed::Core::Memory::Heap, TVal>;
		}
	}
}
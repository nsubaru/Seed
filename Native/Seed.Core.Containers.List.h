#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Base.Interlocked.h"
#include "Seed.Core.Containers.Array.h"

namespace Seed {
	inline namespace Core {
		class EndNodeNotDeferencable : public Exception {
		public:
			//Помилка: Ітератор на кінець не є визначеним
			__forceinline EndNodeNotDeferencable(const AChar * File, const AChar * FunctName, SizeT Line) noexcept
				: Exception(File, FunctName, Line, L"End node of list not deferencable") {};
		};
#pragma warning(push)
#pragma warning(disable: 4251)
		//Шаблон двозвязний список
		// для кожного елемента відбувається підрахунок силок,
		// що дозволяє корректно його знищити навіть
		// якщо десь використовується ітератор на елемент
		template<typename Heap, typename T>
		class BaseList : virtual public Memory::Allocator<Heap> {
		private:
			//Дані списку
			struct list_data;
			//Структурв елемента списку
			struct elem : virtual public Memory::Allocator<Heap> {
			public:
				//Покажчик на список який є власником елемента
				list_data * father;
				//Покажчик на наступний елемент
				elem * next;
				//Покажчик на попередній елемент
				elem * prev;

				//Лічиьник силок на даний елемент
				mutable RefCountT RefCount;

				//Значення елементів
				T value;

				//Пустий конструктор
				elem() :
					father(nullptr), next(nullptr), prev(nullptr),
					RefCount(1), value() {};
				//Конструктор з присвоєнням значення
				elem(const T & obj, list_data * data) :
					father(data), next(nullptr), prev(nullptr),
					RefCount(1), value(obj) {};
				//Конструктор з переміщенням значення
				elem(T && obj, list_data * data) :
					father(data), next(nullptr), prev(nullptr),
					RefCount(1), value(MoveRValue(obj)) {};
				//Конструктор з присвоєнням значення
				elem(const T & obj, list_data * data, elem * prev) : //-V730
					father(data), prev(prev), next(nullptr),
					RefCount(1), value(obj) {};
				//Конструктор з переміщенням значення
				elem(T && obj, list_data * data, elem * prev) : //-V730
					father(data), prev(prev), next(nullptr),
					RefCount(1), value(MoveRValue(obj)) {};
				//Конструктор з присвоєнням значення
				elem(const T & obj, list_data * data, elem * prev, elem * next) : //-V730
					father(data), prev(prev), next(next),
					RefCount(1), value(obj) {};
				//Конструктор з переміщенням значення
				elem(T && obj, list_data * data, elem * prev, elem * next) : //-V730
					father(data), prev(prev), next(next),
					RefCount(1), value(MoveRValue(obj)) {};
			};
			//Дані списку
			struct list_data : virtual public Memory::Allocator<Heap> {
				//Покажчик на перший  елемент
				elem *root = nullptr;
				//Покажчик на останй елемент
				elem *last = nullptr;
				//Кількість елементів
				RefCountT size = 0;
			};
			//Об'єкти списку
			list_data * data;
		public:
			//Ітератор
			class Node : virtual public Memory::Allocator<Heap> {
			private:
				//Елемент списка
				mutable elem * This;
			public:
				//Пустий конструктор
				__forceinline Node() :This(nullptr) {};

				//Конструктор з елемента списку
				__forceinline Node(elem * This) :This(This) {
					if (This != nullptr) ++This->RefCount;
				}

				//Копіювальний конструктор
				__forceinline Node(const Node & orig) : This(orig.This) {
					if (this->This != nullptr) ++This->RefCount;
				}

				//Конструктор переміщення
				__forceinline Node(Node && orig) noexcept : This(MoveRValue(orig.This)) {
					orig.This = nullptr;
				}

				//Оператор присвоєння
				__forceinline Node & operator=(const Node & orig) {
					this->~Node();
					This = orig.This;
					if (this->This != nullptr) ++This->RefCount;
					return *this;
				}
				//Оператор переміщення
				__forceinline Node & operator=(Node && orig) noexcept {
					this->~Node();
					This = orig.This;
					orig.This = nullptr;
					return *this;
				}

				//Наступний елемент
				__forceinline const Node Next() const {
					if (This == nullptr) Throw(EndNodeNotDeferencable(ExceptionPosition));
					return Node(This->next);
				}

				//Попередній елемент
				__forceinline const Node Prev() const {
					if (This == nullptr) Throw(EndNodeNotDeferencable(ExceptionPosition));
					return Node(This->prev);
				}

				//Константний постфіксний інкремент
				const Node operator++(int) const {
					Node node(*this);
					if (This == nullptr) Throw(EndNodeNotDeferencable(ExceptionPosition));
					This->RefCount--;
					this->This = this->This->next;
					if (This != nullptr) This->RefCount++;
					return node;
				}
				//Константний постфіксний декремент
				const Node operator--(int) const {
					Node node(*this);
					if (This == nullptr) Throw(EndNodeNotDeferencable(ExceptionPosition));
					This->RefCount--;
					this->This = this->This->prev;
					if (This != nullptr) This->RefCount++;
					return node;
				}
				//Префіксний інкремент
				Node & operator++() {
					if (This == nullptr) Throw(EndNodeNotDeferencable(ExceptionPosition));
					This->RefCount--;
					this->This = this->This->next;
					if (This != nullptr) This->RefCount++;
					return *this;
				}
				//Префіксний декремент
				Node & operator--() {
					if (This == nullptr)
						Throw(EndNodeNotDeferencable(ExceptionPosition));
					This->RefCount--;
					this->This = this->This->prev;
					if (This != nullptr) This->RefCount++;
					return *this;
				}
				//Оператор рівності двох ітераторів
				__forceinline bool operator==(const Node & second) const {
					return This == second.This;
				}
				//Оператор не рівності двох ітераторів
				__forceinline bool operator!=(const Node & second) const {
					return This != second.This;
				}
				//Оператор рівності з nullptr
				__forceinline bool operator==(const std::nullptr_t) const {
					return This == nullptr;
				}
				//Оператор не рівності з nullptr
				__forceinline bool operator!=(const std::nullptr_t) const {
					return This != nullptr;
				}
				//Оператор розіменування ітератора
				__forceinline T & operator*() {
					if (This == nullptr)
						Throw(EndNodeNotDeferencable(ExceptionPosition));
					return This->value;
				}
				//Константний оператор розіменування ітератора
				__forceinline const T & operator*() const {
					if (This == nullptr)
						Throw(EndNodeNotDeferencable(ExceptionPosition));
					return This->value;
				}
				//Оператор доступу до полів/методів структури/класу
				__forceinline T * operator->() {
					if (This == nullptr)
						Throw(EndNodeNotDeferencable(ExceptionPosition));
					return &This->value;
				}
				//Константний оператор доступу до полів/методів структури/класу
				__forceinline const T * operator->() const {
					if (This == nullptr)
						Throw(EndNodeNotDeferencable(ExceptionPosition));
					return &This->value;
				}
				//Видалення значення ітератора, ітератор отримує значення наступного елемента
				void Remove() {
					if (this->This == nullptr)
						Throw(EndNodeNotDeferencable(ExceptionPosition));
					list_data * data = this->This->father;
					--((This)->RefCount);
					if (this->This->next == nullptr && this->This->prev == nullptr) {
						if (this->This->father != nullptr) {
							this->This->father->root = this->This->father->last = nullptr;
						}
					}
					else if (this->This->next != nullptr && this->This->prev != nullptr) {
						this->This->prev->next = this->This->next;
						this->This->next->prev = this->This->prev;
					}
					else if (this->This->next == nullptr) {
						this->This->prev->next = nullptr; //-V522
						this->This->father->last = this->This->prev;
					}
					else if (this->This->prev == nullptr) {
						this->This->next->prev = nullptr;
						this->This->father->root = this->This->next;
					}
					if (this->This->father != nullptr)
						--((This)->RefCount);
					if (this->This->RefCount == 0) {
						delete this->This;
					}
					else {
						this->This->next = nullptr;
						this->This->prev = nullptr;
						this->This = nullptr;
					}
				}
				//Деструктор ітератора, якщо елемент є останьою силкою на об'єкт то відбувається знищення об'єкта
				//завдяки цьому ітератори можна безпечно передавати не слідкуючи за життямм списку який є
				//власником об'єкта
				__forceinline ~Node() noexcept {
					if (This == nullptr) return;
					--This->RefCount;
					if (This->RefCount == 0) {
						delete This;
					}
					This = nullptr;
				}
				//Вставка елемента після заданого
				__forceinline Node Insert(const T & value) {
					if (This == nullptr)
						Throw(NullPointException(ExceptionPosition, 0, NullPointException::Types::Read));
					auto newElem = new elem(value, This->father, This->prev, This->next);
					auto cp = newElem;
					Synchonize::MemoryBarrier();
					Synchonize::InterlockedExchangePointer((void * volatile *)&newElem, This->next->prev);
					Synchonize::InterlockedExchangePointer((void * volatile *)&cp, This->next->prev);
					Synchonize::MemoryBarrier();
					return Node(This->next->next);
				}
				//Оптимізована вставка елемента після заданого
				__forceinline Node Insert(T && value) {
					if (This == nullptr)
						Throw(NullPointException(ExceptionPosition, 0, NullPointException::Types::Read));
					auto newElem = new elem(MoveRValue(value), This->father, This->prev, This->next);
					auto cp = newElem;
					Synchonize::MemoryBarrier();
					Synchonize::InterlockedExchangePointer((void * volatile *)&newElem, This->next->prev);
					Synchonize::InterlockedExchangePointer((void * volatile *)&cp, This->next);
					Synchonize::MemoryBarrier();
					return Node(This->next->next);
				}
			};
		public:
			//Пустий конструктор
			__forceinline BaseList() noexcept :data(nullptr) {};
			//Конструктор з значенням першого елемента
			__forceinline BaseList(T & Value) {
				data = new list_data;
				data->root = new elem(Value, data);
				data->last = data->root;
				data->size = 1;
			}
			//Конструктор з масиву та його розміру
			BaseList(T * Array, SizeT Size) {
				data = new list_data;
				if (Array == nullptr)
				{
					data = nullptr;
					return;
				}
				data->root = new elem(Array[0], data);
				data->last = data->root;
				if (Size != 1) {
					for (SizeT i = 1; i < Size; i++) {
						data->last->next = new elem(Array[i], data, data->last);
						data->last = data->last->next;
					}
					data->last->next = nullptr;
				}
				data->size = Size;
			}

			//Конструктор з масиву та його розміру
			BaseList(const Array<T> & array) {
				data = new list_data;
				if (array.Size() == 0) return;
				data->root = new elem(array[0], data);
				data->last = data->root;
				if (array.Size() != 1) {
					for (SizeT i = 1; i < array.Size(); i++) {
						data->last->next = new elem(array[i], data, data->last);
						data->last = data->last->next;
					}
					data->last->next = nullptr;
				}
				data->size = array.Size();
			}

			//Конструктор через list-initializer
			BaseList(CRT::initializer_list<T> list) {
				data = new list_data;
				if (list.size() == 0) return;
				this->data->size = list.size();
				auto it = list.begin();
				data->root = new elem(*it, data);
				data->root->value = *it;
				data->last = data->root;
				if (this->data->size != 1) {
					for (SizeT i = 0; i < this->data->size; i++) {
						it++;
						data->last->next = new elem(*it, data, data->last);
						data->last = data->last->next;
					}
				}
			}

			//Копіювальний конструктор
			BaseList(const BaseList&);
			//Конструктор переміщення
			BaseList(BaseList&&) noexcept;
			//Оператор присвоєння
			BaseList& operator=(const BaseList & Orig) {
				if (this->data == Orig.data) return *this;
				this->~BaseList();
				if (Orig.data == nullptr) {
					this->data = nullptr;
					return *this;
				}
				this->data = new list_data;
				this->data->size = Orig.data->size;
				elem * root_orig = Orig.data->root;
				this->data->root = new elem(root_orig->value, data);
				this->data->last = this->data->root;
				root_orig = root_orig->next;
				if (root_orig == nullptr) {
					this->data->root->next = nullptr;
					return *this;
				}
				while (root_orig != nullptr) {
					this->data->last->next = new elem(root_orig->value, data, this->data->last);
					this->data->last = this->data->last->next;
					root_orig = root_orig->next;
				}
				this->data->last->next = nullptr;
				return *this;
			}
			//Оператор переміщення
			__forceinline BaseList& operator=(BaseList && Orig) noexcept {
				if (this->data == Orig.data) return *this;
				this->~BaseList();
				this->data = Orig.data;
				Orig.data = nullptr;
				return *this;
			}

			//Вертає true якщо список пустий
			__forceinline Boolean IsEmpty() const noexcept { return (this->data == nullptr || this->data->size == 0); }

			__forceinline SizeT Size() const noexcept { return IsEmpty() ? 0ull : (SizeT)data->size; }

			//Одержання ітератора на перший елемент
			__forceinline Node Root() {
				if (data == nullptr) return End();
				return Node(data->root);
			}
			//Одержання ітератора на останій елемент
			__forceinline Node Last() {
				if (data == nullptr) return End();
				return Node(data->last);
			}
			//Константне одержання ітератора на перший елемент
			__forceinline const Node Root() const {
				if (data == nullptr) return End();
				return Node(data->root);
			}
			//Константне одержання ітератора на останій елемент
			__forceinline const Node Last() const {
				if (data == nullptr) return End();
				return Node(data->last);
			}
			//Нуль ітератора на кінець
			__forceinline static Node End() {
				return Node(nullptr);
			}
			//Додавання нового елемента до списку
			Node Add(const T & Value) {
				if (data == nullptr) {
					data = new list_data;
					data->root = new elem(Value, data);
					data->last = data->root;
					data->size = 1;
					return data->last;
				}
				if (data->root == nullptr) {
					data->root = new elem(Value, data);
					data->last = data->root;
					data->size = 1;
					return data->last;
				}
				auto new_last = new elem(Value, data, data->last);
				InterlockedExchangePointerNoFence((void * volatile *)&data->last, new_last);
				new_last = data->last;
				InterlockedExchangePointerNoFence((void * volatile *)&data->last->prev->next, new_last);
				++data->size;
				return data->last;
			}
			//Оптимізоване додавання елемента до списку
			Node Add(const T && Value) {
				if (data == nullptr) {
					data = new list_data;
					data->root = new elem(Value, data);
					data->last = data->root;
					data->size = 1;
					return data->last;
				}
				if (data->root == nullptr) {
					data->root = new elem(Value, data);
					data->last = data->root;
					data->size = 1;
					return data->last;
				}
				auto new_end = new elem(MoveRValue(Value), data, data->last);
				InterlockedExchangePointerNoFence((void * volatile *)&data->last, new_end);
				new_end = data->last;
				InterlockedExchangePointerNoFence((void * volatile *)&data->last->prev->next, new_end);
				++data->size;
				return data->last;
			}

			//Перетворення в масив
			Array<T> toArray() const {
				if (data == nullptr)
					throw new NullPointExeption(ExeptionPosition);
				if (data->root == nullptr)
					return Array<T>();
				T * __array = new T[data->size];
				SizeT i = 0;
				auto it = data->root;
				while (it != nullptr) {
					__array[i] = it->value;
					it = it->next;
				}
				return Array<T>(__array, i);
			}

			//Оператор додавання значення до списку
			__forceinline BaseList & operator+=(const T & Value) { this->Add(Value); return *this; }
			//Оптимізований оператор додавання до списку
			__forceinline BaseList & operator+=(T && Value) { this->Add(Value); return *this; }

			//Вертає ітератор на початок для циклу з перебором
			__forceinline const Node begin() const {
				return Root();
			}
			//Вертає ітератор на кінець для циклу з перебором
			__forceinline const Node end() const {
				return nullptr;
			}
			//Видалення всіх елементів які рівні значенню
			void Remove(const T & value) {
				elem * it = data->root;
				elem * tmp;
				while (it != nullptr) {
					if (it->value == value) {
						if (it->next == nullptr && it->prev == nullptr) {
							data->root = nullptr;
							data->last = nullptr;
							tmp = nullptr;
						}
						else if (it->next == nullptr) {
							tmp = nullptr;
							it->prev->next = nullptr; //-V522
							data->last = it->prev;
						}
						else if (it->prev == nullptr) {
							tmp = it->next;
							it->next->prev = nullptr;
							data->root = it->next;
						}
						else {
							tmp = it->next;
							tmp->prev = it->prev;
							it->prev->next = tmp;
						}
						--it->RefCount;
						if (it->RefCount == 0) {
							delete it;
						}
						--data->size;
						it = tmp;
					}
					else  it = it->next;
				}
			}

			//Видалення всіх елементів які рівні значенню
			//Якщо вказано IsRemoveAll false то видаленно буде тільки перше включення
			void Remove(const T & value, bool IsRemoveAll = true) {
				elem * it = data->root;
				elem * tmp;
				while (it != nullptr) {
					if (it->value == value) {
						if (it->next == nullptr && it->prev == nullptr) {
							data->root = nullptr;
							data->last = nullptr;
							tmp = nullptr;
						}
						else if (it->next == nullptr) {
							tmp = nullptr;
							it->prev->next = nullptr; //-V522
							data->last = it->prev;
						}
						else if (it->prev == nullptr) {
							tmp = it->next;
							it->next->prev = nullptr;
							data->root = it->next;
						}
						else {
							tmp = it->next;
							tmp->prev = it->prev;
							it->prev->next = tmp;
						}
						--it->RefCount;
						if (it->RefCount == 0) {
							delete it;
						}
						--data->size;
						it = tmp;
						if (!IsRemoveAll)
							break;
					}
					else  it = it->next;
				}
			}
			template<UInt8(Comparator)(const T&, const T&)>
			void Sort() {
				return 1;
			}

			void Sort() {
				static_assert(Seed::StaticAsserts::IsBasedOn<IComparable<T>, T>(), "Cant sort class witch not implemented sort")
			}

			//Видалення елементів які задовільняють умову функції Comp
			template<typename Filter>
			void RemoveIf(const Filter & filter) {
				static_assert(!StaticAsserrts::CheckSignature<Filter, Boolean, const T&>(), "Non valid filter");
				elem * it = data->root;
				elem * tmp;
				while (it != nullptr) {
					if (filter(it->value)) {
						if (it->next == nullptr && it->prev == nullptr) {
							data->root = nullptr;
							data->last = nullptr;
							tmp = nullptr;
						}
						else if (it->next == nullptr) {
							tmp = nullptr;
							it->prev->next = nullptr; //-V522
							data->last = it->prev;
						}
						else if (it->prev == nullptr) {
							tmp = it->next;
							it->next->prev = nullptr;
							data->root = it->next;
						}
						else {
							tmp = it->next;
							tmp->prev = it->prev;
							it->prev->next = tmp;
						}
						--it->RefCount;
						if (it->RefCount == 0) {
							delete it;
						}
						--data->size;
						it = tmp;
					}
					else  it = it->next;
				}
			}

			void AddAfter();
			void AddBefore();
			void AddLast();

			Node Find();
			Node FindLast();

			//Видалення елемента по ітератору
			__forceinline void Remove(Node & node) {
				node.Remove();
			}
			//Деструктор звільняє пам'ять виділену під елементи але якщо елемент використовується через ітератор
			//то його знищення оминається
			~BaseList() {
				if (data == nullptr) return;
				elem * node;
				while (data->root != nullptr) {
					node = data->root;
					data->root = data->root->next;
					--node->RefCount;
					if (node->RefCount == 0) {
						delete node;
						continue;
					}
					node->next = nullptr;
					node->father = nullptr;
					node->prev = nullptr;
				}
				data->last = nullptr;
				data->size = 0;
				delete data;
				data = nullptr;
			}
		};

		//Копіювальний конструктор
		template<typename Allocator, typename T>
		inline BaseList<Allocator, T>::BaseList(const BaseList & Orig) {
			if (this == &Orig)return;
			if (Orig.data == nullptr) {
				this->data = nullptr;
				return;
			}
			this->data = new list_data;
			this->data->size = Orig.data->size;
			elem * root_orig = Orig.data->root;
			this->data->root = new elem(root_orig->value, this->data);
			this->data->last = this->data->root;
			root_orig = root_orig->next;
			if (root_orig == nullptr) {
				this->data->root->next = nullptr;
				return;
			}
			while (root_orig != nullptr) {
				this->data->last->next = new elem(root_orig->value, this->data, this->data->last);
				this->data->last = this->data->last->next;
				root_orig = root_orig->next;
			}
			this->data->last->next = nullptr;
		}
		//Конструктор переміщення
		template<typename Allocator, typename T>
		__forceinline BaseList<Allocator, T>::BaseList(BaseList && Orig) noexcept {
			this->data = Orig.data; Orig.data = nullptr;
		}

		//Двохзвязний список з стандартним алокатором
		template<typename T>
		using List = BaseList<Seed::Core::Memory::Heap, T>;

		//Двохзвязний список з захищеним алокатором
		template<typename T>
		using SecuredList = BaseList<Seed::Core::Memory::SensitiveDataHeap, T>;

#pragma warning(pop)
	}
}
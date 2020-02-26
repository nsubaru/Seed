#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Containers.Internals.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			template<class THeap, typename T>
			class BaseList final :
				protected IDisposable,
				public Memory::Allocator<THeap>,
				public LINQ::IList<BaseList, THeap, T, true>,
				public IClonable<BaseList<THeap, T>>
			{
			private:
#pragma region Internal storage struct
				///<summary>Дані списку</summary>
				struct list_data;
				///<summary>Структурв елемента списку</summary>
				struct elem : public Memory::Allocator<THeap>
				{
				public:
					///<summary>Покажчик на об'єкт списка який є власником елемента</summary>
					list_data* father;
					///<summary>Покажчик на наступний елемент</summary>
					elem* next;
					///<summary>Покажчик на попередній елемент</summary>
					elem* prev;

					///<summary>Лічиьник силок на даний елемент</summary>
					mutable RefCountT RefCount;

					///<summary>Значення елементів</summary>
					T value;

					///<summary>Пустий конструктор</summary>
					elem() :
						father(nullptr), next(nullptr), prev(nullptr),
						RefCount(1), value()
					{};
					///<summary>Конструктор з присвоєнням значення</summary>
					///<param name = "obj">Значення елемента</param>
					///<param name = "This">Покажчик на об'єкт списка який є власником нового елемента</param>
					elem(const T& obj, list_data* This) :
						father(This), next(nullptr), prev(nullptr),
						RefCount(1), value(obj)
					{};
					///<summary>Конструктор з переміщенням значення</summary>
					///<param name = "obj">Значення елемента</param>
					///<param name = "This">Покажчик на об'єкт списка який є власником нового елемента</param>
					elem(T&& obj, list_data* This) :
						father(This), next(nullptr), prev(nullptr),
						RefCount(1), value(MoveRValue(obj))
					{};
					///<summary>Конструктор з присвоєнням значення і виставленням покажчика на попередній елемент</summary>
					///<param name = "obj">Значення елемента</param>
					///<param name = "This">Покажчик на об'єкт списка який є власником нового елемента</param>
					///<param name = "prev">Покажчик на попередній елемент</param>
					elem(const T& obj, list_data* This, elem* prev) : //-V730
						father(This), prev(prev), next(nullptr),
						RefCount(1), value(obj)
					{};
					///<summary>Конструктор з переміщенням значення і виставленням покажчика на попередній елемент</summary>
					///<param name = "obj">Значення елемента</param>
					///<param name = "This">Покажчик на об'єкт списка який є власником нового елемента</param>
					///<param name = "prev">Покажчик на попередній елемент</param>
					elem(T&& obj, list_data* This, elem* prev) : //-V730
						father(This), prev(prev), next(nullptr),
						RefCount(1), value(MoveRValue(obj))
					{};
					///<summary>Конструктор з присвоєнням значення і встановлення покажчиків на сусідні елементи</summary>
					///<param name = "obj">Значення елемента</param>
					///<param name = "This">Покажчик на об'єкт списка який є власником нового елемента</param>
					///<param name = "prev">Покажчик на попередній елемент</param>
					///<param name = "next">Покажчик на наступний елемент</param>
					elem(const T& obj, list_data* This, elem* prev, elem* next) :
						father(This), prev(prev), next(next),
						RefCount(1), value(obj)
					{};
					///<summary>Конструктор з переміщенням значення і встановлення покажчиків на сусідні елементи</summary>
					///<param name = "obj">Значення елемента</param>
					///<param name = "This">Покажчик на об'єкт списка який є власником нового елемента</param>
					///<param name = "prev">Покажчик на попередній елемент</param>
					///<param name = "next">Покажчик на наступний елемент</param>
					elem(T&& obj, list_data* This, elem* prev, elem* next) :
						father(This), prev(prev), next(next),
						RefCount(1), value(MoveRValue(obj))
					{};
				};
				///<summary>Дані списку</summary>
				struct list_data : virtual public Memory::Allocator<THeap>
				{
					///<summary>Покажчик на перший  елемент</summary>
					elem* root;
					///<summary>Покажчик на останй елемент</summary>
					elem* last;
					///<summary>Кількість елементів</summary>
					RefCountT size;
					///<summary>Кількість силок на даний екземпляр списку</summary>
					RefCountT RefCount;

					///<summary>Конструктор, заповнює початкові дані</summary>
					list_data() :
						root(nullptr),
						last(nullptr),
						size(0),
						RefCount(1)
					{}
				};
				///<summary>Об'єкти списку</summary>
				list_data* This;
			protected:
				virtual void Dispose() override final {
					elem* node;
					while (this->This->root != nullptr)
					{
						node = this->This->root;
						this->This->root = This->root->next;
						if (--node->RefCount == 0)
						{
							delete node;
							continue;
						}
						node->next = nullptr;
						node->father = nullptr;
						node->prev = nullptr;
					}
				}
			private:
				template<typename Lambda>
				elem* merge(elem* first, elem* second, Lambda& lambda)
				{
					// If first linked list is empty
					if (!first)
						return second;

					// If second linked list is empty
					if (!second)
						return first;

					// Pick the smaller value
					if (lambda(first->value, second->value))
					{
						first->next = merge(first->next, second, lambda);
						first->next->prev = first;
						first->prev = nullptr;
						return first;
					}
					else
					{
						second->next = merge(first, second->next, lambda);
						second->next->prev = second;
						second->prev = nullptr;
						return second;
					}
				}

				template<typename Lambda>
				// Function to do merge sort
				elem* mergeSort(elem* head, Lambda& lambda)
				{
					if (!head || !head->next)
						return head;
					elem* second = split(head, lambda);

					// Recur for left and right halves
					head = mergeSort(head, lambda);
					second = mergeSort(second, lambda);

					// Merge the two sorted halves
					return merge(head, second, lambda);
				}

				template<typename Lambda>
				// Split a doubly linked list (DLL) into 2 DLLs of
				// half sizes
				elem* split(elem* head, Lambda& lambda)
				{
					elem* fast = head, * slow = head;
					while (fast->next && fast->next->next)
					{
						fast = fast->next->next;
						slow = slow->next;
					}
					elem* temp = slow->next;
					slow->next = nullptr;
					return temp;
				}
#pragma endregion
			public:
#pragma region Iterator
				///<summary>Ітератор
				///<para> Особливістю ітератора в списку є те що елементи існуватимуть
				///після знищення списку який був власником елементів.
				///Саме для цього аідбувається підрахунок силок для елементів списка, а не списку в цілому
				///</para>
				///</summary>
				class Iterator : IDisposable
				{
					friend class BaseList;
				private:
					///<summary>Елемент списка</summary>
					mutable elem* This;
				private:
					///<summary>Конструктор з елемента списку</summary>
					///<param name = "This">Покажчик на елемент</param>
					__forceinline Iterator(elem* This) :This(This)
					{
						if (This != nullptr) ++This->RefCount;
					}
				protected:
                    virtual void Dispose() override {};
				public:
					///<summary>Пустий конструктор</summary>
					__forceinline Iterator() :This(nullptr)
					{};
#pragma region Operators
#pragma region Iterator Ariphmetic
					///<summary>Константний постфіксний інкремент</summary>
					const Iterator operator++(int) const
					{
						Iterator node(*this);
						if (This == nullptr)
							Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
						This->RefCount--;
						if (this->This->father == nullptr)
						{
							//Якщо батька в елемента немає значить контейнер знищений і відповідно сусідніх елементів немає
							this->This = nullptr;
							return node;
						}
						else
						{
							this->This = this->This->next;
							if (This != nullptr) This->RefCount++;
							return node;
						}
					}
					///<summary>Константний постфіксний декремент</summary>
					const Iterator operator--(int) const
					{
						Iterator node(*this);
						if (This == nullptr)
							Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
						This->RefCount--;
						if (This->father == nullptr)
						{
							//Якщо батька в елемента немає значить контейнер знищений і відповідно сусідніх елементів немає
							this->This = nullptr;
							return node;
						}
						else
						{
							this->This = this->This->prev;
							if (This != nullptr) This->RefCount++;
							return node;
						}
					}
					///<summary>Префіксний інкремент</summary>
					Iterator& operator++()
					{
						if (This == nullptr)
							Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
						This->RefCount--;
						if (This->father == nullptr)
						{
							//Якщо батька в елемента немає значить контейнер знищений і відповідно сусідніх елементів немає
							if (This->RefCount == 0)
								delete This;
							This = nullptr;
						}
						else
						{
							this->This = this->This->next;
							if (This != nullptr) This->RefCount++;
						}
						return *this;
					}
					///<summary>Префіксний декремент</summary>
					Iterator& operator--()
					{
						if (This == nullptr)
							Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
						This->RefCount--;
						if (this->This->father == nullptr)
						{
							//Якщо батька в елемента немає значить контейнер знищений і відповідно сусідніх елементів немає
							this->This = nullptr;
							return *this;
						}
						else
						{
							this->This = this->This->prev;
							if (This != nullptr) This->RefCount++;
							return *this;
						}
					}
#pragma endregion
#pragma region Comparing
					///<summary>Оператор рівності двох ітераторів</summary>
					///<param name = "seconde">Правий ітератор</param>
					__forceinline bool operator==(const Iterator& second) const
					{
						return reinterpret_cast<SizeT>(This) == reinterpret_cast<SizeT>(second.This);
					}
					///<summary>Оператор не рівності двох ітераторів</summary>
					///<param name = "seconde">Правий ітератор</param>
					__forceinline bool operator!=(const Iterator& second) const
					{
						return reinterpret_cast<SizeT>(This) != reinterpret_cast<SizeT>(second.This);
					}

					///<summary>Оператор рівності з nullptr</summary>
					__forceinline bool operator==(const Seed::NullptrT) const
					{
						return reinterpret_cast<SizeT>(This) == reinterpret_cast<SizeT>(nullptr);
					}
					///<summary>Оператор не рівності з nullptr</summary>
					__forceinline bool operator!=(const Seed::NullptrT) const
					{
						return This != nullptr;
					}

#pragma endregion
#pragma region Data access operators
					///<summary>Оператор розіменування ітератора</summary>
					///<returns>Значення елемента</returns>
					__forceinline T& operator*()
					{
						if (This == nullptr)
							Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
						return This->value;
					}
					///<summary>Константний оператор розіменування ітератора</summary>
					///<returns>Константне значення елемента</returns>
					__forceinline const T& operator*() const
					{
						if (This == nullptr)
							Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
						return This->value;
					}
					///<summary>Оператор доступу до полів/методів структури/класу</summary>
					///<returns>Покажчик на елемент</returns>
					__forceinline T* operator->()
					{
						if (This == nullptr)
							Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
						return &This->value;
					}
					///<summary>Константний оператор доступу до полів/методів структури/класу</summary>
					///<returns>Константний покажчик на елемент</returns>
					__forceinline const T* operator->() const
					{
						if (This == nullptr)
							Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
						return &This->value;
					}
#pragma endregion
#pragma endregion
#pragma region Iterator move
					///<summary>Вертає наступний елемент</summary>
					__forceinline const Iterator Next() const
					{
						if (This == nullptr)
							Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
						if (This->father == nullptr)
							//Якщо батька в елемента немає значить контейнер знищений і відповідно сусідніх елементів немає
							return Iterator(nullptr);
						else
							return Iterator(This->next);
					}

					///<summary>Попередній елемент</summary>
					__forceinline const Iterator Prev() const
					{
						if (This == nullptr)
							Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
						if (This->father == nullptr)
							//Якщо батька в елемента немає значить контейнер знищений і відповідно сусідніх елементів немає
							return Iterator(nullptr);
						else
							return Iterator(This->prev);
					}
#pragma endregion
#pragma region List element manipulations
					///<summary>Видалення значення ітератора, ітератор отримує значення наступного елемента</summary>
					Iterator Remove()
					{
						if (this->This == nullptr)
							Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
						if (this->This->father == nullptr)
						{
							if (--This->RefCount == 0)
								delete This;
							This = nullptr;
							return *this;
						}
						else
						{
							__assume(this->This->father != nullptr); //-V547
							if (this->This->next == nullptr && this->This->prev == nullptr)
							{
								this->This->father->root = this->This->father->last = nullptr;
							}
							else if (this->This->next != nullptr && this->This->prev != nullptr)
							{
								this->This->prev->next = this->This->next;
								this->This->next->prev = this->This->prev;
							}
							else if (this->This->next == nullptr)
							{
								if (this->This->prev != nullptr)
									this->This->prev->next = nullptr;
								this->This->father->last = this->This->prev;
							}
							else if (this->This->prev == nullptr) //-V2516
							{
								this->This->next->prev = nullptr;
								this->This->father->root = this->This->next;
							}

							--((This)->father->size);

							if (--this->This->RefCount == 1)
							{
								auto newThis = this->This->next;
								delete this->This;
								this->This = newThis;
								if (this->This != nullptr)
									++this->This->RefCount;
							}
							else
							{
								auto newThis = this->This->next;
								this->This->next = nullptr;
								this->This->prev = nullptr;
								this->This = newThis;
								if (this->This != nullptr)
									++this->This->RefCount;
							}
						}
						return *this;
					}
#pragma region Adding element to list by iterator
					///<summary>Вставка елемента після заданого</summary>
					///<param name = "Value">Значення нового елемента</summary>
					__forceinline Iterator Insert(const T& Value)
					{
						return AddAfter(Value);
					}
					///<summary>Оптимізована вставка елемента після заданого</summary>
					///<param name = "Value">rvalue силка на елемент для ідеальної передачі значення в ново створений елемент</param>
					__forceinline Iterator Insert(T&& Value)
					{
						return AddAfter(MoveRValue(Value));
					}
					///<summary>Вставка елемента після поточного</summary>
					///<param name = "Value">Значення нового елемента</summary>
					__forceinline Iterator AddAfter(const T& Value)
					{

						if (This == nullptr)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__);
						if (This->father == nullptr)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This->father), 0);
						//Створення нового елемента
						auto newAfter = new elem(Value, This->father, This, this->This->next);
						if (this->This->next == nullptr)
						{
							this->This->father->last = this->This->next = newAfter;
							newAfter->next->prev = newAfter;
						}
						else
						{
							this->This->next = newAfter;
							newAfter->next->prev = newAfter;
						}
						return newAfter;
					}
					///<summary>Оптимізована вставка елемента після поточного</summary>
					///<param name = "Value">rvalue силка на елемент для ідеальної передачі значення в ново створений елемент</param>
					__forceinline Iterator AddAfter(T&& Value)
					{
						if (This == nullptr)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__);
						if (This->father == nullptr)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This->father), 0);
						//Створення нового елемента
						auto newAfter = new elem(MoveRValue(Value), This->father, This, this->This->next);
						if (this->This->next == nullptr)
						{
							this->This->father->last = this->This->next = newAfter;
							newAfter->next->prev = newAfter;
						}
						else
						{
							this->This->next = newAfter;
							newAfter->next->prev = newAfter;
						}
						return newAfter;
					}
					///<summary>Вставка елемента перед поточним</summary>
					///<param name = "Value">Значення нового елемента</summary>
					__forceinline Iterator AddBefore(const T& Value)
					{
						if (This == nullptr)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__);
						if (This->father == nullptr)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This->father), 0);
						//Створення нового елемента
						auto newBefore = new elem(Value, This->father, nullptr, this->This->father->root);
						if (this->This->prev == nullptr)
						{
							this->This->father->root = this->This->prev = newBefore;
							newBefore->next->prev = newBefore;
						}
						else
						{
							this->This->prev = newBefore;
							newBefore->prev->next = newBefore;
						}
						return newBefore;
					}
					///<summary>Оптимізована вставка елемента перед поточним</summary>
					///<param name = "Value">rvalue силка на елемент для ідеальної передачі значення в ново створений елемент</param>
					__forceinline Iterator AddBefore(T&& Value)
					{
						if (This == nullptr)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						if (This->father == nullptr)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This->father), 0);
						//Створення нового елемента
						auto newBefore = new elem(MoveRValue(Value), This->father, nullptr, this->This->father->root);
						if (this->This->prev == nullptr)
						{
							this->This->father->root = this->This->prev = newBefore;
							newBefore->next->prev = newBefore;
						}
						else
						{
							this->This->prev = newBefore;
							newBefore->prev->next = newBefore;
						}
						return newBefore;
					}
#pragma endregion
#pragma endregion
					template<typename U>
					friend void Seed::Core::Swap(U& A, U& B);

                    RefCountClass(Iterator, This);
				};
			public:
				friend Iterator;

				///<summary>Пустий конструктор</summary>
				__forceinline BaseList() :This(new list_data)
				{};

				///<summary>Конструктор через list-initializer</summary>
				///<param name = "list">Список ініціалізації</param>
				BaseList(InitilizerList<T>&& list)
				{
					This = new list_data;
					if (list.size() == 0) return;
					this->This->size = list.size();
					auto it = list.begin();
					This->root = new elem(MoveRValue(*it), This);
					This->root->value = *it;
					This->last = This->root;
					it++;
					if (this->This->size != 1)
					{
						for (SizeT i = 1; i < this->This->size; i++)
						{
							This->last->next = new elem(MoveRValue(*it), This, This->last);
							This->last = This->last->next;
							it++;
						}
					}
				}

				///<summary>Оператор додавання значення в кінець списку</summary>
				///<param name = "Value">Значення нового елемента</param>
				__forceinline BaseList& operator+=(const T& Value)
				{
					this->AddLast(Value); return *this;
				}
				///<summary>Оптимізований оператор додавання значення в кінець списку</summary>
				///<param name = "Value">rvalue силка на значення нового елемента</param>
				__forceinline BaseList& operator+=(T&& Value)
				{
					this->AddLast(MoveRValue(Value)); return *this;
				}

				///<summary>Додавання елементів списку OtherList в кінець цього списку</summary>
				///<param name = "OtherList">Другий список</param>
				///<returns>Силку на поточний список</returns>
				__forceinline BaseList& operator+=(const BaseList& OtherList) {
					if (OtherList.This == this->This)
						return *this;

					if (OtherList.This->size == 0)
						return *this;

					auto node = OtherList.This->root;
					while (node != nullptr) {
						this->This->last->next = new elem(node->value, this->This, this->This->last);
						this->This->last = this->This->last->next;
						node = node->next;
					}
					this->This->size = this->This->size + OtherList.This->size;
					return *this;
				}

				///<summary>Оптимізована додавання елементів до даного списку</summary>
				///<param name = "OtherList">Другий список</param>
				///<returns>Силку на поточний список</returns>
				__forceinline BaseList& operator+=(BaseList&& OtherList) {
					if (OtherList.This == this->This)
						return *this;

					if (OtherList.This->size == 0)
						return *this;
					//Ignore OtherList.This->root == null as if it true than OtherList.This->size == 0
#pragma warning(push)
#pragma warning(disable: 28182)
					if (OtherList.This->RefCount == 1) {
						__assume(OtherList.This->root != nullptr);
						auto item = OtherList.This->root;
						while (item != nullptr)
						{
							item->father = this->This;
							item = item->next;
						}

						OtherList.This->root->prev = this->This->last;
						this->This->last = this->This->last->next = OtherList.This->root;
						this->This->size = this->This->size + OtherList.This->size;
						OtherList.This->root = OtherList.This->last = nullptr;
						OtherList.This->size = 0;
						return *this;
					}
#pragma warning(pop)
					else {
						return *this += OtherList;
					}
					return *this;
				}

				Boolean operator==(const BaseList& second) {
					if (this->Count == second.Count) {
						auto it1 = this->This->root;
						auto it2 = second.This->root;
						while (it1 != nullptr)
						{
							if (it1->value != it2->value)
								return false;
							it1 = it1->next;
							it2 = it2->next;
						}
						return true;
					}
					return false;
				}
#pragma endregion
#pragma region Item count methods
				///<summary>Вертає true якщо список пустий</summary>
				__forceinline Boolean IsEmpty() const
				{
					return (this->This == nullptr || this->This->size == 0);
				}

				///<summary>Вертає кількість елементів в списку</summary>
				virtual inline SizeT GetCount() const
				{
					return IsEmpty() ? 0ull : (SizeT)This->size; //-V2517
				}

				///<summary>Кількість елементів в списку</summary>
				SizeT Property(GetCount) Count;
#pragma endregion
#pragma region Iterators
#pragma region Gettter
				////TODO: Maybe implement as property

				///<summary>Одержання ітератора на перший елемент</summary>
				__forceinline Iterator GetRoot()
				{
					if (This == nullptr) return GetEnd();
						return Iterator(This->root);
				}
				///<summary>Одержання ітератора на останій елемент</summary>
				__forceinline Iterator GetLast()
				{
					if (This == nullptr) return GetEnd();
						return Iterator(This->last);
				}
				///<summary>Константне одержання ітератора на перший елемент</summary>
				__forceinline const Iterator GetRoot() const
				{
					if (This == nullptr) return GetEnd();
						return Iterator(This->root);
				}
				///<summary>Константне одержання ітератора на останій елемент</summary>
				__forceinline const Iterator GetLast() const
				{
					if (This == nullptr) return GetEnd();
						return Iterator(This->last);
				}
				///<summary>Одержання нуль ітератора на кінець списку</summary>
				__forceinline static Iterator GetEnd()
				{
					return Iterator(nullptr);
				}

				///<summary>Ітератора на перший елемент</summary>
				Iterator Property(GetRoot) FirstIt;
				///<summary>Ітератора на останій елемент</summary>
				Iterator Property(GetLast) LastIt;
				///<summary>Нуль ітератора на кінець списку</summary>
				Iterator Property(GetEnd) End;
#pragma endregion
#pragma region Add new value by iterator
				///<summary>Додавання нового елемента в кінець списку</summary>
				///<param name = "Value">Значення нового елемента</param>
				Iterator AddLast(const T& Value)
				{
					if (This == nullptr)
					{
						This = new list_data;
						This->last = This->root = new elem(Value, This);
						This->size = 1;
						return This->last;
					}
					if (This->root == nullptr)
					{
						This->last = This->root = new elem(Value, This);
						This->size = 1;
						return This->last;
					}
					this->This->last = this->This->last->next = new elem(Value, This, This->last);
					++This->size;
					return This->last;
				}

				///<summary>Оптимізоване додавання елемента до списку</summary>
				///<param name = "Value">rvalue силка на значення нового елемента</param>
				Iterator AddLast(T&& Value)
				{
					if (This == nullptr)
					{
						This = new list_data;
						This->last = This->root = new elem(MoveRValue(Value), This);
						This->size = 1;
						return This->last;
					}
					if (This->root == nullptr)
					{
						This->last = This->root = new elem(MoveRValue(Value), This);
						This->size = 1;
						return This->last;
					}
					this->This->last = this->This->last->next = new elem(MoveRValue(Value), This, This->last);
					++This->size;
					return This->last;
				}
				///<summary>Додавання нового елемента після заданого ітератором</summary>
				///<param name = "Item">Ітератор на елмент</param>
				///<param name = "Value">Значення нового елемента</param>
				Iterator AddAfter(Iterator Item, const T& Value)
				{
					return Item.AddAfter(Value);
				}
				///<summary>Оптимізоване додавання елемента після заданого ітератором</summary>
				///<param name = "Item">Ітератор на елмент</param>
				///<param name = "Value">rvalue силка на значення нового елемента</param>
				Iterator AddAfter(Iterator Item, T&& Value)
				{
					return Item.AddAfter(MoveRValue(Value));
				}
				///<summary>Додавання нового елемента перед елементом заданим ітератором</summary>
				///<param name = "Item">Ітератор на елмент</param>
				///<param name = "Value">Значення нового елемента</param>
				Iterator AddBefore(Iterator Item, const T& Value)
				{
					return Item.AddBefore(Value);
				}
				///<summary>Додавання нового елемента перед елементом заданим ітератором</summary>
				///<param name = "Item">Ітератор на елмент</param>
				///<param name = "Value">rvalue силка на значення нового елемента</param>
				Iterator AddBefore(Iterator Item, T&& Value)
				{
					return Item.AddBefore(MoveRValue(Value));
				}
#pragma endregion
			public:
#pragma region For each support methods
				///<summary>Вертає константний ітератор на початок для циклу з перебором</summary>
				__forceinline const Iterator begin() const
				{
					return GetRoot();
				}
				///<summary>Вертає константний ітератор на кінець для циклу з перебором</summary>
				__forceinline const Iterator end() const
				{
					return nullptr;
				}
				///<summary>Вертає ітератор на початок для циклу з перебором</summary>
				__forceinline Iterator begin()
				{
					return GetRoot();
				}
#pragma endregion
			public:
				///<summary>Видалення елемента по ітератору. Ітератор обнулює</summary>
				__forceinline void Remove(Iterator& node)
				{
					if (node.This == nullptr)
						Internals::Exceptions::ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
					if (node.This->father == nullptr)
					{
						if (--node.This->RefCount == 0)
							delete node.This;
						node.This = nullptr;
					}
					else
					{
						__assume(node.This->father != nullptr); //-V547
						if (node.This->next == nullptr && node.This->prev == nullptr)
						{
							node.This->father->root = node.This->father->last = nullptr;
						}
						else if (node.This->next != nullptr && node.This->prev != nullptr)
						{
							node.This->prev->next = node.This->next;
							node.This->next->prev = node.This->prev;
						}
						else if (node.This->next == nullptr)
						{
							if (node.This->prev != nullptr)
								node.This->prev->next = nullptr;
							node.This->father->last = node.This->prev;
						}
						else if (node.This->prev == nullptr) //-V2516
						{
							node.This->next->prev = nullptr;
							node.This->father->root = node.This->next;
						}

						--((node.This)->father->size);

						if (--node.This->RefCount == 1)
						{
							auto newThis = node.This->next;
							delete node.This;
							node.This = nullptr;
						}
						else
						{
							node.This->next = nullptr;
							node.This->prev = nullptr;
							node.This = nullptr;
						}
					}
				}

				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);
			public:
				RefCountClass(BaseList, This)
			public:
				virtual BaseList Clone() const override final {
					BaseList result;
					result.This->size = This->size;
					elem* node = This->root;
					if (node != nullptr) {
						This->root = This->last = new elem(node->value, result.This);
						node = node->next;
						while (node != nullptr) {
							This->last = This->last->prev = new elem(node->value, result.This, This->last);
							node = node->next;
						}
					}
					return result;
				}
			protected:
				template<template<typename, typename> typename, typename, typename, Boolean, Boolean>
				friend class LINQ::ICollection;
				template<template<typename, typename> typename, typename, typename, Boolean>
				friend class LINQ::IList;
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				friend class LINQ::IFixedCollectionBase;
				template<template<typename THeap, typename T>typename TList, typename THeap, typename T, bool TIsDynamic, bool TIsConcurencySuport>
				friend class LINQ::IOrderable;
                template<typename THeap, typename T, typename Enable>
                friend struct Seed::Core::NewSerilization::Details::BaseSerlizationHandler;
			protected:
#pragma warning(push)
#pragma warning(disable: 4172)
				template<typename LambdaT>
				__forceinline BaseList& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&> || 
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
							auto node = this->This->root;
							while (node != nullptr) {
								lambda(node->value);
								node = node->next;
							}
							return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
							auto node = this->This->root;
							SizeT index = 0;
							while (node != nullptr) {
								lambda(node->value, index);
								node = node->next;
								index++;
							}
							return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BaseList& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						auto node = this->This->root;
						while (node != nullptr) {
							lambda(node->value);
							node = node->next;
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						auto node = this->This->root;
						SizeT index = 0;
						while (node != nullptr) {
							lambda(node->value, index);
							node = node->next;
							index++;
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline BaseList& ForEachImpl(LambdaT&& lambda, SizeT count) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&> || 
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
							if (count > this->This->size)
							{
								auto node = this->This->root;
								while (node != nullptr) {
									lambda(node->value);
									node = node->next;
								}
								return *this;
							}
							else {
								SizeT i = 0;
								auto node = this->This->root;
								while (node != nullptr) {
									lambda(node->value);
									node = node->next;
									i++;
									if (i == count)
										return *this;
								}
								return *this;
							}
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						if (count > this->This->size)
						{
							auto node = this->This->root;
							SizeT index = 0;
							while (node != nullptr) {
								lambda(node->value, index);
								node = node->next;
								index++;
							}
							return *this;
						}
						else {
							SizeT index = 0;
							auto node = this->This->root;
							while (node != nullptr) {
								lambda(node->value, index);
								node = node->next;
								index++;
								if (index == count)
									return *this;
							}
							return *this;
						}
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BaseList& ForEachImpl(LambdaT&& lambda, SizeT count) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						if (count > this->This->size)
						{
							auto node = this->This->root;
							while (node != nullptr) {
								lambda(node->value);
								node = node->next;
							}
							return *this;
						}
						else {
							SizeT i = 0;
							auto node = this->This->root;
							while (node != nullptr) {
								lambda(node->value);
								node = node->next;
								i++;
								if (i == count)
									return *this;
							}
							return *this;
						}
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						if (count > this->This->size)
						{
							auto node = this->This->root;
							SizeT index = 0;
							while (node != nullptr) {
								lambda(node->value, index);
								node = node->next;
								index++;
							}
							return *this;
						}
						else {
							SizeT index = 0;
							auto node = this->This->root;
							while (node != nullptr) {
								lambda(node->value, index);
								node = node->next;
								index++;
								if (index == count)
									return *this;
							}
							return *this;
						}
					}
					else {
						static_assert(false);
					}
				}

			protected:

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda) const {
					U value;
					auto node = this->This->root;
					SizeT index = 0;
					while (node != nullptr) {
						lambda(value, node->value);
						node = node->next;
						index++;
					}
					return MoveRValue(value);
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, const U& Value) const {
					auto node = this->This->root;
					U result = Value;
					SizeT index = 0;
					while (node != nullptr) {
						lambda(result, node->value);
						node = node->next;
						index++;
					}
					return MoveRValue(result);
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, U&& Value) const {
					auto node = this->This->root;
					SizeT index = 0;
					while (node != nullptr) {
						lambda(Value, node->value);
						node = node->next;
						index++;
					}
					return MoveRValue(Value);
				}

				__forceinline bool AnyImpl() const { return this->This->size > 0; }

				template<typename Lambda>
				__forceinline bool AnyImpl(Lambda lambda) const {
					auto node = this->This->root;
					SizeT index = 0;
					while (node != nullptr) {
						if (lambda(node->value))
							return true;
						node = node->next;
						index++;
					}
					return false;
				}

				template<typename Lambda>
				__forceinline bool AllImpl(Lambda lambda) const {
					auto node = this->This->root;
					SizeT index = 0;
					while (node != nullptr) {
						if (!lambda(node->value))
							return false;
						node = node->next;
						index++;
					}
					return true;
				}

				__forceinline BaseList& SortByImpl(bool IsDescending = true) {
					if (IsDescending) {
						auto l = [](const T& V1, const T& V2) { return V1 < V2; };
						This->root = mergeSort(This->root, l);
					}
					else {
						auto l = [](const T& V1, const T& V2) { return V1 > V2; };
						This->root = mergeSort(This->root, l);
					}
					This->last = [&]() { //-V1047
						elem* node = This->root;
						while (node->next != nullptr)
							node = node->next;
						return node;
					}();
					return *this;
				}

				template<typename Lambda>
				__forceinline T& FirstImpl(Lambda&& lambda) {
					elem* node = This->root;
					while (node != nullptr)
						if (lambda(node->value))
							return node->value;
						else
							node = node->next;

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				__forceinline T& LastImpl(Lambda&& lambda) {
					elem* node = This->root;
					elem* selected = nullptr;
					while (node != nullptr) {
						if (lambda(node->value))
							selected = node;
						node = node->next;
					}
					if (selected != nullptr)
						return selected->value;

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				__forceinline const T& FirstImpl(Lambda&& lambda) const {
					elem* node = This->root;
					while (node != nullptr)
						if (lambda(node->value))
							return node->value;
						else
							node = node->next;

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				__forceinline const T& LastImpl(Lambda&& lambda) const {
					elem* node = This->root;
					elem* selected = nullptr;
					while (node != nullptr) {
						if (lambda(node->value))
							selected = node;
						node = node->next;
					}

					if (selected != nullptr)
						return selected->value;

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				__forceinline BaseList& ReverseImpl() {
					elem* temp = nullptr;
					elem* current = This->root;

					while (current != nullptr) {
						temp = current->prev;
						current->prev = current->next;
						current->next = temp;
						current = current->prev;
					}
					Seed::Swap(This->root, This->last);
					return *this;
				}

				__forceinline void UnsafeAdd(const T& Value) {
					if (this->This->root == nullptr) {
						this->This->root = this->This->last = new elem(Value, this->This);
					}
					else {
						this->This->last->prev->next = this->This->last = new elem(Value, this->This, this->This->last);
					}
					++this->This->size;
				}

				__forceinline BaseList& RemoveAllImpl() {
					this->Dispose();
					return *this;
				}

				template<typename Lambda>
				__forceinline BaseList& RemoveAllImpl(Lambda&& lambda) {
						elem* node;
						while (this->This->root != nullptr)
						{
							node = this->This->root;
							this->This->root = This->root->next;
							lambda(MoveRValue(node->value));
							if (--node->RefCount == 0)
							{
								delete node;
								continue;
							}
							node->next = nullptr;
							node->father = nullptr;
							node->prev = nullptr;
						}
						return *this;
				}

				template<typename FilterLambda, typename Lambda>
				__forceinline BaseList& RemoveIfFilterAndLambdaWithIndex(FilterLambda&& filter, Lambda&& lambda) {
						if (This == nullptr)
							return *this;
						elem* it = This->root;
						elem* tmp;
						SizeT Index = 0;
						while (it != nullptr)
						{
							if (filter(it->value, Index)) {

								lambda(it->value, Index);

								if (it->next == nullptr && it->prev == nullptr)
								{
									This->root = nullptr;
									This->last = nullptr;
									tmp = nullptr;
								}
								else if (it->next == nullptr)
								{
									tmp = nullptr;
									if (it->prev != nullptr)
										it->prev->next = nullptr;
									This->last = it->prev;
								}
								else if (it->prev == nullptr)
								{
									tmp = it->next;
									it->next->prev = nullptr;
									This->root = it->next;
								}
								else
								{
									tmp = it->next;
									tmp->prev = it->prev;
									it->prev->next = tmp;
								}
								--it->RefCount;
								if (it->RefCount == 0)
								{
									delete it;
								}
								--This->size;
								it = tmp;

							}
							else  it = it->next;
						}
						return *this;
				}

				template<typename FilterLambda, typename Lambda>
				__forceinline BaseList& RemoveIfFilterWithIndex(FilterLambda&& filter, Lambda&& lambda) {
						if (This == nullptr)
							return *this;
						elem* it = This->root;
						elem* tmp;
						SizeT Index = 0;
						while (it != nullptr)
						{
							if (filter(it->value, Index)) {

								lambda(it->value);

								if (it->next == nullptr && it->prev == nullptr)
								{
									This->root = nullptr;
									This->last = nullptr;
									tmp = nullptr;
								}
								else if (it->next == nullptr)
								{
									tmp = nullptr;
									if (it->prev != nullptr)
										it->prev->next = nullptr;
									This->last = it->prev;
								}
								else if (it->prev == nullptr)
								{
									tmp = it->next;
									it->next->prev = nullptr;
									This->root = it->next;
								}
								else
								{
									tmp = it->next;
									tmp->prev = it->prev;
									it->prev->next = tmp;
								}
								--it->RefCount;
								if (it->RefCount == 0)
								{
									delete it;
								}
								--This->size;
								it = tmp;

							}
							else  it = it->next;
						}
						return *this;
				}

				template<typename FilterLambda, typename Lambda>
				__forceinline BaseList& RemoveIfLambdaWithIndex(FilterLambda&& filter, Lambda&& lambda) {
						if (This == nullptr)
							return *this;
						elem* it = This->root;
						elem* tmp;
						SizeT Index = 0;
						while (it != nullptr)
						{
							if (filter(it->value)) {

								lambda(it->value, Index);

								if (it->next == nullptr && it->prev == nullptr)
								{
									This->root = nullptr;
									This->last = nullptr;
									tmp = nullptr;
								}
								else if (it->next == nullptr)
								{
									tmp = nullptr;
									if (it->prev != nullptr)
										it->prev->next = nullptr;
									This->last = it->prev;
								}
								else if (it->prev == nullptr)
								{
									tmp = it->next;
									it->next->prev = nullptr;
									This->root = it->next;
								}
								else
								{
									tmp = it->next;
									tmp->prev = it->prev;
									it->prev->next = tmp;
								}
								--it->RefCount;
								if (it->RefCount == 0)
								{
									delete it;
								}
								--This->size;
								it = tmp;

							}
							else  it = it->next;
						}
						return *this;
				}

				template<typename FilterLambda, typename Lambda>
				__forceinline BaseList& RemoveIfFilterAndLambdaWithoutIndex(FilterLambda&& filter, Lambda&& lambda) {
						if (This == nullptr)
							return *this;
						elem* it = This->root;
						elem* tmp;
						SizeT Index = 0;
						while (it != nullptr)
						{
							if (filter(it->value)) {

								lambda(it->value);

								if (it->next == nullptr && it->prev == nullptr)
								{
									This->root = nullptr;
									This->last = nullptr;
									tmp = nullptr;
								}
								else if (it->next == nullptr)
								{
									tmp = nullptr;
									if (it->prev != nullptr)
										it->prev->next = nullptr;
									This->last = it->prev;
								}
								else if (it->prev == nullptr)
								{
									tmp = it->next;
									it->next->prev = nullptr;
									This->root = it->next;
								}
								else
								{
									tmp = it->next;
									tmp->prev = it->prev;
									it->prev->next = tmp;
								}
								--it->RefCount;
								if (it->RefCount == 0)
								{
									delete it;
								}
								--This->size;
								it = tmp;

							}
							else  it = it->next;
						}
						return *this;
				}

				template<typename FilterLambda, typename Lambda>
				__forceinline BaseList& RemoveIfImpl(FilterLambda&& filter, Lambda&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleT<Lambda, void, T&&>::State&&
						TypeTraits::IsLambdaConvertibleT<FilteringLambda, Boolean, const T&>::State) {
						return RemoveIfFilterAndLambdaWithoutIndex(MoveRValue(filter), MoveRValue(lambda));
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleT<Lambda, void, T&&>::State&&
						TypeTraits::IsLambdaConvertibleT<FilteringLambda, Boolean, const T&, SizeT>::State) {
						return RemoveIfFilterWithIndex(MoveRValue(filter), MoveRValue(lambda));
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleT<Lambda, void, T&&, SizeT>::State&&
						TypeTraits::IsLambdaConvertibleT<FilteringLambda, Boolean, const T&>::State) {
						return RemoveIfLambdaWithIndex(MoveRValue(filter), MoveRValue(lambda));
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleT<Lambda, void, T&&, SizeT>::State&&
						TypeTraits::IsLambdaConvertibleT<FilteringLambda, Boolean, const T&, SizeT>::State) {
						return RemoveIfFilterAndLambdaWithIndex(MoveRValue(filter), MoveRValue(lambda));
					}
					else
						static_assert(false);
				}

				template<typename Lambda>
				__forceinline SizeT GetCountIfImpl(Lambda&& lambda) {
					SizeT selectedCount = 0;

					auto node = this->This->root;
					while (node != nullptr) {
						if (lambda(node->value))
							selectedCount++;
						node = node->next;
					}

					return selectedCount;
				}
#pragma warning(pop)
			};

#pragma endregion
#pragma region Partical implementation for default and sensitive heaps

			///<summary>Двохзвязний список з стандартним алокатором
			///<para>
			/// Для кожного елемента відбувається підрахунок силок,
			/// що дозволяє корректно його знищити навіть
			/// якщо десь використовується ітератор на елемент
			///</para>
			///</summary>
			///<param name = "T">Тип значеннь</param>
			namespace {
				template<typename T>
				using List = BaseList<Seed::Core::Memory::Heap, T>;
			}

			///<summary>Двохзвязний список з захищеним алокатором
			///<para>
			/// Для кожного елемента відбувається підрахунок силок,
			/// що дозволяє корректно його знищити навіть
			/// якщо десь використовується ітератор на елемент
			///</para>
			///</summary>
			///<param name = "T">Тип значеннь</param>
			namespace {
				template<typename T>
				using SensitiveList = BaseList<Seed::Core::Memory::SensitiveDataHeap, T>;
			}
#pragma endregion
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename T>
				struct IsListT<Seed::Core::Containers::BaseList<THeap, T>> : LogicTrue {};
			}
		}

		template<typename THeap, typename T>
		__forceinline void Swap(Containers::BaseList<THeap, T>& A, Containers::BaseList<THeap, T>& B) {
			Swap(A.This, B.This);
		}

		template<typename THeap, typename T>
		__forceinline void Swap(typename Containers::BaseList<THeap, T>::Iterator& A, typename Containers::BaseList<THeap, T>::Iterator& B) {
			Swap(A.This, B.This);
		}
	}
}
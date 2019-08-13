#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Base.Interlocked.h"
#include "Seed.Core.Containers.Array.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Containers.IEnumerable.h"

namespace Seed {
	inline namespace Core {
#pragma warning(push)
#pragma warning(disable: 4251)
		///<summary>Шаблон потоко-захищеного двозвязний список
		///<para>
		/// Для кожного елемента відбувається підрахунок силок,
		/// що дозволяє корректно його знищити навіть
		/// якщо десь використовується ітератор на елемент
		///</para>
		///</summary>
		///<param name = "Heap">Класс кучі з якої буде алокуватись пам'ять для елементів ліста</param>
		///<param name = "T">Тип значеннь</param>
		template<typename Heap, typename T>
		class SeedKernelCall BaseList final : public Memory::Allocator<Heap>, public Queryable::IEnumerable<BaseList<Heap, T>, T>, public IDisposable
		{
		private:
			///<summary>Дані списку</summary>
			struct list_data;
			///<summary>Структурв елемента списку</summary>
			struct elem : virtual public Memory::Allocator<Heap>
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
				///<param name = "data">Покажчик на об'єкт списка який є власником нового елемента</param>
				elem(const T& obj, list_data* data) :
					father(data), next(nullptr), prev(nullptr),
					RefCount(1), value(obj)
				{};
				///<summary>Конструктор з переміщенням значення</summary>
				///<param name = "obj">Значення елемента</param>
				///<param name = "data">Покажчик на об'єкт списка який є власником нового елемента</param>
				elem(T&& obj, list_data* data) :
					father(data), next(nullptr), prev(nullptr),
					RefCount(1), value(MoveRValue(obj))
				{};
				///<summary>Конструктор з присвоєнням значення і виставленням покажчика на попередній елемент</summary>
				///<param name = "obj">Значення елемента</param>
				///<param name = "data">Покажчик на об'єкт списка який є власником нового елемента</param>
				///<param name = "prev">Покажчик на попередній елемент</param>
				elem(const T& obj, list_data* data, elem* prev) : //-V730
					father(data), prev(prev), next(nullptr),
					RefCount(1), value(obj)
				{};
				///<summary>Конструктор з переміщенням значення і виставленням покажчика на попередній елемент</summary>
				///<param name = "obj">Значення елемента</param>
				///<param name = "data">Покажчик на об'єкт списка який є власником нового елемента</param>
				///<param name = "prev">Покажчик на попередній елемент</param>
				elem(T&& obj, list_data* data, elem* prev) : //-V730
					father(data), prev(prev), next(nullptr),
					RefCount(1), value(MoveRValue(obj))
				{};
				///<summary>Конструктор з присвоєнням значення і встановлення покажчиків на сусідні елементи</summary>
				///<param name = "obj">Значення елемента</param>
				///<param name = "data">Покажчик на об'єкт списка який є власником нового елемента</param>
				///<param name = "prev">Покажчик на попередній елемент</param>
				///<param name = "next">Покажчик на наступний елемент</param>
				elem(const T& obj, list_data* data, elem* prev, elem* next) :
					father(data), prev(prev), next(next),
					RefCount(1), value(obj)
				{};
				///<summary>Конструктор з переміщенням значення і встановлення покажчиків на сусідні елементи</summary>
				///<param name = "obj">Значення елемента</param>
				///<param name = "data">Покажчик на об'єкт списка який є власником нового елемента</param>
				///<param name = "prev">Покажчик на попередній елемент</param>
				///<param name = "next">Покажчик на наступний елемент</param>
				elem(T&& obj, list_data* data, elem* prev, elem* next) :
					father(data), prev(prev), next(next),
					RefCount(1), value(MoveRValue(obj))
				{};
			};
			///<summary>Дані списку</summary>
			struct list_data : virtual public Memory::Allocator<Heap>
			{
				///<summary>Покажчик на перший  елемент</summary>
				elem* root;
				///<summary>Покажчик на останй елемент</summary>
				elem* last;
				///<summary>Кількість елементів</summary>
				RefCountT size;
				//Кількість силок на даний екземпляр списку
				RefCountT RefCount;
				///<summary>RWLock - для синхронізації читання запису
				///<para>Він захищає структуру списка, а не значення елемента</para>
				///</summary>
				Threading::SRWLock lock;

				///<summary>Конструктор, заповнює початкові дані</summary>
				list_data() :
					root(nullptr), 
					last(nullptr), 
					size(0), 
					RefCount(1), 
					lock()
				{}
			};
			///<summary>Об'єкти списку</summary>
			list_data* data;
		private:
			elem* RemoveByElemPtr(elem* it, SizeT MinCountRef) {
				elem* tmp;
				if (it->next == nullptr && it->prev == nullptr)
				{
					data->root = nullptr;
					data->last = nullptr;
					tmp = nullptr;
				}
				else if (it->next == nullptr)
				{
					tmp = nullptr;
					it->prev->next = nullptr;  //-V522
					data->last = it->prev;
				}
				else if (it->prev == nullptr)
				{
					tmp = it->next;
					it->next->prev = nullptr;
					data->root = it->next;
				}
				else
				{
					tmp = it->next;
					tmp->prev = it->prev;
					it->prev->next = tmp;
				}
				--it->RefCount;
				if (it->RefCount == MinCountRef)
				{
					delete it;
				}
				--data->size;
				it = tmp;
				return it;
			}
		protected:
			virtual void Dispose() override final {
				this->data->lock.ExclusiveLock([&]()
					{
						elem* node;
						while (this->data->root != nullptr)
						{
							node = this->data->root;
							this->data->root = data->root->next;
							if (--node->RefCount == 0)
							{
								delete node;
								continue;
							}
							node->next = nullptr;
							node->father = nullptr;
							node->prev = nullptr;
						}
					});
			}
		protected:
			 virtual BaseList & ForEachImpl(Queryable::LambdaHolder<void, T&>& lambda) override final {
				this->data->lock.ShareLock([&]() mutable {
					auto node = this->data->root;
					while (node != nullptr) {
						lambda(node->value);
						node = node->next;
					}
					});
				return *this;
			}

			 virtual BaseList & ForEachImpl(Queryable::LambdaHolder<void, T&, SizeT>& lambda) override final {
				this->data->lock.ShareLock([&]() {
					auto node = this->data->root;
					SizeT i = 0;
					while (node != nullptr) {
						lambda(node->value, ForwardReference<SizeT&&>(i)); i++;
						node = node->next;
					}
					});
				return *this;
			}

			 virtual BaseList& ForEachImpl(Queryable::LambdaHolder<void, const T&>& lambda) override final {
				this->data->lock.ShareLock([&]() mutable {
					auto node = this->data->root;
					while (node != nullptr) {
						lambda(node->value);
						node = node->next;
					}
					});
				return *this;
			}

			 virtual BaseList& ForEachImpl(Queryable::LambdaHolder<void, const T&, SizeT>& lambda) override final {
				this->data->lock.ShareLock([&]() {
					auto node = this->data->root;
					SizeT i = 0;
					while (node != nullptr) {
						lambda(node->value, ForwardReference<SizeT&&>(i)); i++;
						node = node->next;
					}
					});
				return *this;
			}

			 virtual const BaseList& ForEachImpl(Queryable::LambdaHolder<void, const T&>& lambda) const override final {
				this->data->lock.ShareLock([&]() mutable {
					auto node = this->data->root;
					while (node != nullptr) {
						lambda(node->value);
						node = node->next;
					}
					});
				return *this;
			}

			 virtual const BaseList& ForEachImpl(Queryable::LambdaHolder<void, const T&, SizeT>& lambda) const override final {
				this->data->lock.ShareLock([&]() {
					auto node = this->data->root;
					SizeT i = 0;
					while (node != nullptr) {
						lambda(node->value, ForwardReference<SizeT&&>(i)); i++;
						node = node->next;
					}
					});
				return *this;
			}

			virtual BaseList& FillImpl(Queryable::LambdaHolder<T>& lambda, SizeT ItemsCount) override final {
				if (ItemsCount == 0)
					return *this;
				if (this->data->size == 0) {
					this->data->last = this->data->root = new elem(lambda(), this->data);
					if (ItemsCount != 1) {
						for (SizeT i = 1; i < ItemsCount; i++) {
							this->data->last = this->data->last->next = new elem(lambda(), this->data, this->data->last);
						}
					}
					this->data->size = ItemsCount;
					return *this;
				}
				return *this;
			}

			virtual BaseList& FillImpl(Queryable::LambdaHolder<T, SizeT>& lambda, SizeT ItemsCount) override final {
				if (ItemsCount == 0)
					return *this;
				if (this->data->size == 0) {
					this->data->last = this->data->root = new elem(lambda(0), this->data);
					if (ItemsCount != 1) {
						for (SizeT i = 1; i < ItemsCount; i++) {
							this->data->last = this->data->last->next = new elem(lambda(ForwardReference<SizeT&&>(i)), this->data, this->data->last);
						}
					}
					this->data->size = ItemsCount;
					return *this;
				}
				return *this;
			}

			 virtual void TransferTo(T&& Value, [[maybe_unused]] SizeT Index) override final{
				if (this->data->root == nullptr) {
					this->data->root = this->data->last = new elem(MoveRValue(Value), this->data);
				}
				else {
					this->data->last = this->data->last->next = new elem(MoveRValue(Value), this->data, this->data->last);
				}
			}

			inline virtual void PrepareCopyToOrMove(SizeT NewCount) override final {
				this->data->size = NewCount;
			}

			 virtual void RemoveAllImpl(Queryable::LambdaHolder<void, T&&>& lambda) override final {
				this->data->lock.ExclusiveLock([&]() {
					elem* item = this->data->root;
					elem* tmp = item;
					while (item != nullptr) {
						tmp = item;
						item = item->next;
						lambda(MoveRValue(tmp->value));
						if (--tmp->RefCount == 0) {
							delete tmp;
						}
						else {
							tmp->next = tmp->prev = nullptr;
							tmp->father = nullptr;
						}
					}
					this->data->root = this->data->last = nullptr;
					this->data->size = 0;
				});
			}

			 virtual void RemoveAllImpl(Queryable::LambdaHolder<void, T&&, SizeT>& lambda) override final {
				this->data->lock.ExclusiveLock([&]() {
					elem* item = this->data->root;
					elem* tmp = item;
					SizeT Index = 0;
					while (item != nullptr) {
						tmp = item;
						item = item->next;
						lambda(MoveRValue(tmp->value), ForwardReference<SizeT&&>(Index));
						if (--tmp->RefCount == 0) {
							delete tmp;
						}
						else {
							tmp->next = tmp->prev = nullptr;
							tmp->father = nullptr;
						}
						Index++;
					}
					this->data->root = this->data->last = nullptr;
					this->data->size = 0;
				});
			}

			 virtual BaseList& SortImpl(Queryable::LambdaHolder<bool, const T&, const T&>& lambda, bool IsAscendingOrder) override final  {
				this->data->lock.ExclusiveLock([&]()
					{
						if (IsAscendingOrder)
							MergeSort(&data->root, [&](const T& V1, const T& V2)
								{
									return lambda(V1, V2) == true;
								});
						else
							MergeSort(&data->root, [&](const T& V1, const T& V2)
								{
									return lambda(V1, V2) == false;
								});
						data->last = data->root;
						while (data->last->next != nullptr) data->last = data->last->next;
					});
				return *this;
			}

			 virtual BaseList& RemoveIfImpl(Queryable::LambdaHolder<bool, const T&>& lambda) override final {
				this->data->lock.ExclusiveLock([&]() {
					elem* item = this->data->root;
					while (item != nullptr) {
						if (!lambda(item->value)) {
							item = RemoveByElemPtr(item, 1);
							--this->data->size;
						}
						else {
							item = item->next;
						}
					}
					this->data->root = this->data->last = nullptr;
					this->data->size = 0;
				});
				return *this;
			}

			 virtual BaseList& RemoveIfImpl(Queryable::LambdaHolder<bool, const T&, SizeT>& lambda) override final {
				this->data->lock.ExclusiveLock([&]() {
					elem* item = this->data->root;
					SizeT Index = 0;
					while (item != nullptr) {
						if (!lambda(item->value, ForwardReference<SizeT&&>(Index))) {
							item = RemoveByElemPtr(item, 1);
							--this->data->size;
						}
						else {
							item = item->next;
						}
						Index++;
					}
					this->data->root = this->data->last = nullptr;
					this->data->size = 0;
				});
				return *this;
			}
		public:
			virtual BaseList& RemoveAll() {
				this->data->lock.ExclusiveLock([&]() {
					elem* item = this->data->root;
					elem* tmp;
					while (item != nullptr) {
						if (--item->RefCount == 0) {
							tmp = item;
							item = item->next;
							delete tmp;
						}
						else {
							tmp = item;
							item = item->next;
							tmp->next = tmp->prev = nullptr;
							tmp->father = nullptr;
						}
					}
				});
				return *this;
			}
			virtual BaseList& Reverse() {
				this->data->lock.ExclusiveLock([&]() {
					if (this->data->size == 0 || this->data->size == 1)
						return;
					elem* first = this->data->root;
					elem* last = this->data->last;
					while (first != last || first->next != last) {
						Swap(first->value, last->value);
					}
				});
				return *this;
			}
		public:
			///<summary>Ітератор
			///<para> Особливістю ітератора в списку є те що елементи існуватимуть
			///після знищення списку який був власником елементів.
			///Саме для цього аідбувається підрахунок силок для елементів списка, а не списку в цілому
			///</para>
			///</summary>
			class Iterator
			{
				friend BaseList;
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
			public:
				///<summary>Пустий конструктор</summary>
				__forceinline Iterator() :This(nullptr)
				{};

				///<summary>Копіювальний конструктор</summary>
				///<param name = "orig">Оригинал ітератора</param>
				__forceinline Iterator(const Iterator & orig) : This(orig.This)
				{
					if (this->This != nullptr) ++This->RefCount;
				}

				///<summary>Конструктор переміщення</summary>
				///<param name = "orig">Оригинал ітератора</summary>
				__forceinline Iterator(Iterator && orig) noexcept : This(MoveRValue(orig.This))
				{
					orig.This = nullptr;
				}
#pragma region Operators
#pragma region Copy and Move
				///<summary>Оператор присвоєння</summary>
				///<param name = "orig">Оригинал ітератора</summary>
				__forceinline Iterator& operator=(const Iterator & orig)
				{
					this->~Iterator();
					This = orig.This;
					if (this->This != nullptr) ++This->RefCount;
					return *this;
				}
				///<summary>Оператор переміщення</summary>
				///<param name = "orig">Оригинал ітератора</summary>
				__forceinline Iterator& operator=(Iterator && orig) noexcept
				{
					this->~Iterator();
					This = orig.This;
					orig.This = nullptr;
					return *this;
				}
#pragma endregion
#pragma region Iterator Ariphmetic
				///<summary>Константний постфіксний інкремент</summary>
				const Iterator operator++(int) const
				{
					extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					Iterator node(*this);
					if (This == nullptr)
						ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
					This->RefCount--;
					if (this->This->father == nullptr)
					{
						//Якщо батька в елемента немає значить контейнер знищений і відповідно сусідніх елементів немає
						this->This = nullptr;
						return node;
					}
					else
					{
						return this->This->father->lock.ShareLock([&]()
							{
								this->This = this->This->next;
								if (This != nullptr) This->RefCount++;
								return node;
							});
					}
				}
				///<summary>Константний постфіксний декремент</summary>
				const Iterator operator--(int) const
				{
					extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					Iterator node(*this);
					if (This == nullptr)
						ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
					This->RefCount--;
					if (This->father == nullptr)
					{
						//Якщо батька в елемента немає значить контейнер знищений і відповідно сусідніх елементів немає
						this->This = nullptr;
						return node;
					}
					else
					{
						return this->This->father->lock.ShareLock([&]()
							{
								this->This = this->This->prev;
								if (This != nullptr) This->RefCount++;
								return node;
							});
					}
				}
				///<summary>Префіксний інкремент</summary>
				Iterator& operator++()
				{
					extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					if (This == nullptr)
						ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
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
						this->This->father->lock.ShareLock([&]()
							{
								this->This = this->This->next;
								if (This != nullptr) This->RefCount++;
							});
					}
					return *this;
				}
				///<summary>Префіксний декремент</summary>
				Iterator& operator--()
				{
					extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					if (This == nullptr)
						ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
					This->RefCount--;
					if (this->This->father == nullptr)
					{
						//Якщо батька в елемента немає значить контейнер знищений і відповідно сусідніх елементів немає
						this->This = nullptr;
						return *this;
					}
					else
					{
						return this->This->father->lock.ShareLock([&]()
							{
								this->This = this->This->prev;
								if (This != nullptr) This->RefCount++;
								return *this;
							});
					}
				}
#pragma endregion
#pragma region Comparing
				///<summary>Оператор рівності двох ітераторів</summary>
				///<param name = "seconde">Правий ітератор</param>
				__forceinline bool operator==(const Iterator & second) const
				{
					return reinterpret_cast<SizeT>(This) == reinterpret_cast<SizeT>(second.This);
				}
				///<summary>Оператор не рівності двох ітераторів</summary>
				///<param name = "seconde">Правий ітератор</param>
				__forceinline bool operator!=(const Iterator & second) const
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
					extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					if (This == nullptr)
						ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
					return This->value;
				}
				///<summary>Константний оператор розіменування ітератора</summary>
				///<returns>Константне значення елемента</returns>
				__forceinline const T& operator*() const
				{
					extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					if (This == nullptr)
						ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
					return This->value;
				}
				///<summary>Оператор доступу до полів/методів структури/класу</summary>
				///<returns>Покажчик на елемент</returns>
				__forceinline T* operator->()
				{
					extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					if (This == nullptr)
						ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
					return &This->value;
				}
				///<summary>Константний оператор доступу до полів/методів структури/класу</summary>
				///<returns>Константний покажчик на елемент</returns>
				__forceinline const T* operator->() const
				{
					extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					if (This == nullptr)
						ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
					return &This->value;
				}
#pragma endregion
#pragma endregion
#pragma region Iterator move
				///<summary>Вертає наступний елемент</summary>
				__forceinline const Iterator Next() const
				{
					extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					if (This == nullptr)
						ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
					if (This->father == nullptr)
						//Якщо батька в елемента немає значить контейнер знищений і відповідно сусідніх елементів немає
						return Iterator(nullptr);
					else
						return This->father->lock.ShareLock([&]()
							{
								return Iterator(This->next);
							});
				}

				///<summary>Попередній елемент</summary>
				__forceinline const Iterator Prev() const
				{
					extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					if (This == nullptr)
						ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
					if (This->father == nullptr)
						//Якщо батька в елемента немає значить контейнер знищений і відповідно сусідніх елементів немає
						return Iterator(nullptr);
					else
						return This->father->lock.ShareLock([&]()
							{
								return Iterator(This->prev);
							});
				}
#pragma endregion
#pragma region List element manipulations
				///<summary>Видалення значення ітератора, ітератор отримує значення наступного елемента</summary>
				Iterator Remove()
				{
					extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					if (this->This == nullptr)
						ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
					if (this->This->father == nullptr)
					{
						if (--This->RefCount == 0)
							delete This;
						This = nullptr;
						return *this;
					}
					else
						//Якщо власник існує то видалення виконується в екслюзивному блокуванні
						this->This->father->lock.ExclusiveLock([&]()
							{
								__assume(this->This->father != nullptr);
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
									if(this->This->prev != nullptr)
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
									if(this->This != nullptr)
										++this->This->RefCount;
								}
								else
								{
									this->This->next = nullptr;
									this->This->prev = nullptr;
									this->This = this->This->next;
								}
							});
					return *this;
				}
#pragma region Adding element to list by iterator
				///<summary>Вставка елемента після заданого</summary>
				///<param name = "Value">Значення нового елемента</summary>
				__forceinline Iterator Insert(const T & Value)
				{
					return AddAfter(Value);
				}
				///<summary>Оптимізована вставка елемента після заданого</summary>
				///<param name = "Value">rvalue силка на елемент для ідеальної передачі значення в ново створений елемент</param>
				__forceinline Iterator Insert(T && Value)
				{
					return AddAfter(MoveRValue(Value));
				}
				///<summary>Вставка елемента після поточного</summary>
				///<param name = "Value">Значення нового елемента</summary>
				__forceinline Iterator AddAfter(const T & Value)
				{
					extern __declspec(noreturn) void ThrowNullPointException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line,
						const Seed::SizeT Address,
						const Seed::UInt8 Mode);

					if (This == nullptr)
						ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					if (This->father == nullptr)
						ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This->father), 0);
					//Виконується в екслюзивному блокуванні
					return This->father->lock.ExclusiveLock([&]() mutable
						{
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
						});
				}
				///<summary>Оптимізована вставка елемента після поточного</summary>
				///<param name = "Value">rvalue силка на елемент для ідеальної передачі значення в ново створений елемент</param>
				__forceinline Iterator AddAfter(T && Value)
				{
					extern __declspec(noreturn) void ThrowNullPointException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line,
						const Seed::SizeT Address,
						const Seed::UInt8 Mode);
					if (This == nullptr)
						ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					if (This->father == nullptr)
						ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This->father), 0);
					//Виконується в екслюзивному блокуванні
					return This->father->lock.ExclusiveLock([&, Value = MoveRValue(Value)] () mutable
						{
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
						});
				}
				///<summary>Вставка елемента перед поточним</summary>
				///<param name = "Value">Значення нового елемента</summary>
				__forceinline Iterator AddBefore(const T & Value)
				{
					extern __declspec(noreturn) void ThrowNullPointException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line,
						const Seed::SizeT Address,
						const Seed::UInt8 Mode);
					if (This == nullptr)
						ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					if (This->father == nullptr)
						ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This->father), 0);
					//Виконується в екслюзивному блокуванні
					return This->father->lock.ExclusiveLock([&]() mutable
						{
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
						});
				}
				///<summary>Оптимізована вставка елемента перед поточним</summary>
				///<param name = "Value">rvalue силка на елемент для ідеальної передачі значення в ново створений елемент</param>
				__forceinline Iterator AddBefore(T && Value)
				{
					extern __declspec(noreturn) void ThrowNullPointException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line,
						const Seed::SizeT Address,
						const Seed::UInt8 Mode);
					if (This == nullptr)
						ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					if (This->father == nullptr)
						ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, reinterpret_cast<SizeT>(This->father), 0);
					//Виконується в екслюзивному блокуванні
					return This->father->lock.ExclusiveLock([&]() mutable
						{
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
						});
				}
#pragma endregion
#pragma endregion

				///<summary>Деструктор ітератора
				///<para>Якщо елемент є останьою силкою на об'єкт то відбувається знищення об'єкта
				///завдяки цьому ітератори можна безпечно передавати не слідкуючи за життямм списку який є
				///власником об'єкта
				///</para></summary>
				__forceinline ~Iterator() noexcept
				{
					if (This == nullptr) return;
					if (--This->RefCount == 0)
					{
						delete This;
					}
					This = nullptr;
				}
			};
		public:
			friend Iterator;
#pragma region Ctors
			///<summary>Пустий конструктор</summary>
			__forceinline BaseList() :data(new list_data)
			{};
			/////<summary>Конструктор з значенням першого елемента</summary>
			/////<param name = "Value">Значення першого елемента</summary>
			//__forceinline BaseList(const T & Value)
			//{
			//	data = new list_data;
			//	data->last = data->root = new elem(Value, data);
			//	data->size = 1;
			//}
			/////<summary>Конструктор з значенням першого елемента</summary>
			/////<param name = "Value">rvalue силка на перший елемент</summary>
			//__forceinline BaseList(T && Value)
			//{
			//	data = new list_data;
			//	data->last = data->root = new elem(MoveRValue(Value), data);
			//	data->size = 1;
			//}
			///<summary>Конструктор з масиву та його розміру</summary>
			///<param name = "Array">С-подібний масив</summary>
			///<param name = "Count">Кількість елементів в ньому</summary>
			InternalApiCall BaseList(T * Array, SizeT NewCount)
			{
				data = new list_data;
				if (Array == nullptr)
				{
					data = nullptr;
					return;
				}
				data->root = new elem(Array[0], data);
				data->last = data->root;
				if (NewCount != 1)
				{
					for (SizeT i = 1; i < NewCount; i++)
					{
						data->last->next = new elem(Array[i], data, data->last);
						data->last = data->last->next;
					}
					data->last->next = nullptr;
				}
				data->size = NewCount;
			}

			///<summary>Конструктор з масиву та його розміру</summary>
			///<param name = "array"> Масив елементів</param>
			BaseList(const Array<T> & array)
			{
				data = new list_data;
				if (array.Count == 0) return;
				data->root = new elem(array[0], data);
				data->last = data->root;
				if (array.Count != 1)
				{
					for (SizeT i = 1; i < array.Count; i++)
					{
						data->last->next = new elem(array[i], data, data->last);
						data->last = data->last->next;
					}
					data->last->next = nullptr;
				}
				data->size = array.Count;
			}

			///<summary>Конструктор через list-initializer</summary>
			///<param name = "list">Список ініціалізації</param>
			BaseList(InitilizerList<T> && list)
			{
				////TODO: Write optimized InitilizerList ctors
				data = new list_data;
				if (list.size() == 0) return;
				this->data->size = list.size();
				auto it = list.begin();
				data->root = new elem(MoveRValue(*it), data);
				data->root->value = *it;
				data->last = data->root;
				if (this->data->size != 1)
				{
					for (SizeT i = 0; i < this->data->size; i++)
					{
						data->last->next = new elem(MoveRValue(*it), data, data->last);
						data->last = data->last->next;
						it++;
					}
				}
			}

			/////<summary>Копіювальний конструктор</sumary>
			/////<param name = "orig">Оригінал списку</param>
			//BaseList(const BaseList & orig);
			/////<summary>Конструктор переміщення</sumary>
			/////<param name = "orig">Оригінал списку</param>
			//BaseList(BaseList && orig) noexcept;

			RefCountClass(BaseList, data);

#pragma endregion
#pragma region Operators
			/////<summary>Оператор присвоєння, виконує копіювання елементів списку</sumary>
			/////<param name = "orig">Оригінал списку</param>
			//BaseList& operator=(const BaseList & Orig)
			//{
			//	if (this->data == Orig.data) return *this;
			//	this->~BaseList();
			//	if (Orig.data == nullptr)
			//	{
			//		this->data = nullptr;
			//		return *this;
			//	}
			//	//Виконується в спільній блокировці оригиналу
			//	return Orig.data->lock.ShareLock([&]()
			//		{
			//			this->data = new list_data;
			//			this->data->size = Orig.data->size;
			//			elem* root_orig = Orig.data->root;
			//			this->data->root = new elem(root_orig->value, data);
			//			this->data->last = this->data->root;
			//			root_orig = root_orig->next;
			//			if (root_orig == nullptr)
			//			{
			//				this->data->root->next = nullptr;
			//				return *this;
			//			}
			//			while (root_orig != nullptr)
			//			{
			//				this->data->last->next = new elem(root_orig->value, data, this->data->last);
			//				this->data->last = this->data->last->next;
			//				root_orig = root_orig->next;
			//			}
			//			this->data->last->next = nullptr;
			//			return *this;
			//		});
			//}
			/////<summary>Оператор переміщення, виконує переміщення об'єкта списку</sumary>
			/////<param name = "orig">Оригінал списку</param>
			//__forceinline BaseList& operator=(BaseList && Orig) noexcept
			//{
			//	if (this->data == Orig.data) return *this;
			//	this->~BaseList();
			//	this->data = Orig.data;
			//	Orig.data = nullptr;
			//	return *this;
			//}

			///<summary>Оператор додавання значення в кінець списку</summary>
			///<param name = "Value">Значення нового елемента</param>
			__forceinline BaseList& operator+=(const T & Value)
			{
				this->AddLast(Value); return *this;
			}
			///<summary>Оптимізований оператор додавання значення в кінець списку</summary>
			///<param name = "Value">rvalue силка на значення нового елемента</param>
			__forceinline BaseList& operator+=(T && Value)
			{
				this->AddLast(Value); return *this;
			}

			Boolean operator==(const BaseList& second) {
				if (this->Count == second.Count) {
					return this->data->lock.ShareLock([&]()
						{
							auto it1 = this->data->root;
							auto it2 = second.data->root;
							while (it1 != nullptr)
							{
								if (it1->value != it2->value)
									return false;
								it1 = it1->next;
								it2 = it2->next;
							}
							return true;
						});
				}
				return false;
			}
#pragma endregion
#pragma region Item count methods
			///<summary>Вертає true якщо список пустий</summary>
			__forceinline Boolean IsEmpty() const
			{
				return (this->data == nullptr || this->data->size == 0);
			}

			///<summary>Вертає кількість елементів в списку</summary>
			virtual inline SizeT GetCount() const
			{
				return IsEmpty() ? 0ull : (SizeT)data->size; //-V2517
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
				if (data == nullptr) return GetEnd();
				return this->data->lock.ShareLock([&]()
					{
						return Iterator(data->root);
					});
			}
			///<summary>Одержання ітератора на останій елемент</summary>
			__forceinline Iterator GetLast()
			{
				if (data == nullptr) return GetEnd();
				return this->data->lock.ShareLock([&]()
					{
						return Iterator(data->last);
					});
			}
			///<summary>Константне одержання ітератора на перший елемент</summary>
			__forceinline const Iterator GetRoot() const
			{
				if (data == nullptr) return GetEnd();
				return this->data->lock.ShareLock([&]()
					{
						return Iterator(data->root);
					});
			}
			///<summary>Константне одержання ітератора на останій елемент</summary>
			__forceinline const Iterator GetLast() const
			{
				if (data == nullptr) return GetEnd();
				return this->data->lock.ShareLock([&]()
					{
						return Iterator(data->last);
					});
			}
			///<summary>Одержання нуль ітератора на кінець списку</summary>
			__forceinline static Iterator GetEnd()
			{
				return Iterator(nullptr);
			}

			///<summary>Ітератора на перший елемент</summary>
			Iterator Property(GetRoot) First;
			///<summary>Ітератора на останій елемент</summary>
			Iterator Property(GetLast) Last;
			///<summary>Нуль ітератора на кінець списку</summary>
			Iterator Property(GetEnd) End;
#pragma endregion
#pragma region Add new value by iterator
			///<summary>Додавання нового елемента в кінець списку</summary>
			///<param name = "Value">Значення нового елемента</param>
			Iterator AddLast(const T & Value)
			{
				if (data == nullptr)
				{
					data = new list_data;
					data->root = new elem(Value, data);
					data->last = data->root;
					data->size = 1;
					return data->last;
				}
				if (data->root == nullptr)
				{
					data->root = new elem(Value, data);
					data->last = data->root;
					data->size = 1;
					return data->last;
				}
				//Виконується в ексклюзивній блокировці
				return this->data->lock.ExclusiveLock([&]() mutable
					{
						auto newLast = new elem(Value, data, data->last);
						auto cp = newLast;
						this->data->last = newLast;
						this->data->last->prev->next = newLast;
						++data->size;
						return newLast;
					});
			}

			///<summary>Оптимізоване додавання елемента до списку</summary>
			///<param name = "Value">rvalue силка на значення нового елемента</param>
			Iterator AddLast(const T && Value)
			{
				if (data == nullptr)
				{
					data = new list_data;
					data->root = new elem(Value, data);
					data->last = data->root;
					data->size = 1;
					return data->last;
				}
				if (data->root == nullptr)
				{
					data->root = new elem(Value, data);
					data->last = data->root;
					data->size = 1;
					return data->last;
				}
				//Виконується в ексклюзивній блокировці
				return this->data->lock.ExclusiveLock([&]() mutable
					{
						auto newLast = new elem(MoveRValue(Value), data, data->last);
						auto cp = newLast;
						this->data->last = newLast;
						this->data->last->prev->next = newLast;
						++data->size;
						return newLast;
					});
			}
			///<summary>Додавання нового елемента після заданого ітератором</summary>
			///<param name = "Item">Ітератор на елмент</param>
			///<param name = "Value">Значення нового елемента</param>
			Iterator AddAfter(Iterator Item, const T & Value)
			{
				return Item.AddAfter(Value);
			}
			///<summary>Оптимізоване додавання елемента після заданого ітератором</summary>
			///<param name = "Item">Ітератор на елмент</param>
			///<param name = "Value">rvalue силка на значення нового елемента</param>
			Iterator AddAfter(Iterator Item, T && Value)
			{
				return Item.AddAfter(MoveRValue(Value));
			}
			///<summary>Додавання нового елемента перед елементом заданим ітератором</summary>
			///<param name = "Item">Ітератор на елмент</param>
			///<param name = "Value">Значення нового елемента</param>
			Iterator AddBefore(Iterator Item, const T & Value)
			{
				return Item.AddBefore(Value);
			}
			///<summary>Додавання нового елемента перед елементом заданим ітератором</summary>
			///<param name = "Item">Ітератор на елмент</param>
			///<param name = "Value">rvalue силка на значення нового елемента</param>
			Iterator AddBefore(Iterator Item, T && Value)
			{
				return Item.AddBefore(MoveRValue(Value));
			}
#pragma endregion
#pragma region Convertors

			///<summary>Отримання масиву елементів списку
			///<para>На час перетворення список блокується</para>
			///</summary>
			///<returns>Масив елементів</returns>
			Array<T> toArray() const
			{
				if (data == nullptr)
					return Array<T>();
				if (data->root == nullptr)
					return Array<T>();
				T * __array = new T[data->size];
				SizeT i = 0;
				this->data->lock.ShareLock([&]()
					{
						auto it = data->root;
						while (it != nullptr)
						{
							__array[i] = it->value;
							it = it->next;
						}
					});
				return Array<T>(__array, i);
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
#pragma region Removing
			///<summary>Видалення всіх елементів які рівні значенню
			///<para>Якщо вказано IsRemoveAll false то видаленно буде тільки перше включення</para>
			///</summary>
			///<param name = "Value">Значення яке вказує що треба видалити елемент</param>
			///<param name = "IsRemoveAll">Не обов'язово. Якщо вказано false то видаленно буде тільки перше включення</param>
			///<returns>Силку на список</returns>
			BaseList& Remove(const T & Value, bool IsRemoveAll = true)
			{
				elem* it = data->root;
				elem* tmp;
				while (it != nullptr)
				{
					if (it->value == Value) //Видалення виконуєтсья в екслюзивній блокировці
						this->data->lock.ExclusiveLock([&]()
							{
								if (it->next == nullptr && it->prev == nullptr)
								{
									data->root = nullptr;
									data->last = nullptr;
									tmp = nullptr;
								}
								else if (it->next == nullptr)
								{
									tmp = nullptr;
									it->prev->next = nullptr; //-V522
									data->last = it->prev;
								}
								else if (it->prev == nullptr)
								{
									tmp = it->next;
									it->next->prev = nullptr;
									data->root = it->next;
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
								--data->size;
								it = tmp;
							});
					else  it = it->next;
				}
				return *this;
			}

			////TODO: Write removing all with speicific function which execute for each item

			///<summary>Видалення елементів які задовільняють умову функції Comp</summary>
			///<param name = "Filter">Функція-фільтр якщо вертає true то елемент видаляється</param>
			///<returns>Силку на список</returns>
			template<typename TFilter>
			BaseList& RemoveIf(const TFilter & Filter)
			{
				static_assert(!StaticAsserrts::CheckSignature<TFilter, Boolean, const T&>(), "Invalid filter");
				if (data == nullptr)
					return;
				elem * it = data->root;
				elem * tmp;
				while (it != nullptr)
				{
					if (Filter(it->value)) {
						this->data->lock.ExclusiveLock([&]()
							{
								if (it->next == nullptr && it->prev == nullptr)
								{
									data->root = nullptr;
									data->last = nullptr;
									tmp = nullptr;
								}
								else if (it->next == nullptr)
								{
									tmp = nullptr;
									if (it->prev != nullptr)
										it->prev->next = nullptr;
									data->last = it->prev;
								}
								else if (it->prev == nullptr)
								{
									tmp = it->next;
									it->next->prev = nullptr;
									data->root = it->next;
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
								--data->size;
								it = tmp;
							}
						);
					}
					else  it = it->next;
				}
				return *this;
			}

			///<summary>Видалення першого елемента який задовільняє умову функції Comp</summary>
			///<param name = "Filter">Функція-фільтр якщо вертає true то елемент видаляється</param>
			///<returns>Силку на список</returns>
			template<typename TFilter>
			BaseList& RemoveIfFirstOnly(const TFilter& Filter)
			{
				static_assert(!StaticAsserrts::CheckSignature<TFilter, Boolean, const T&>(), "Invalid filter");
				if (data == nullptr)
					return;
				elem * it = data->root;
				elem * tmp;
				while (it != nullptr)
				{
					if (Filter(it->value)) {
						this->data->lock.ExclusiveLock([&]()
							{
								if (it->next == nullptr && it->prev == nullptr)
								{
									data->root = nullptr;
									data->last = nullptr;
									tmp = nullptr;
								}
								else if (it->next == nullptr)
								{
									tmp = nullptr;
									if (it->prev != nullptr)
										it->prev->next = nullptr;
									data->last = it->prev;
								}
								else if (it->prev == nullptr)
								{
									tmp = it->next;
									it->next->prev = nullptr;
									data->root = it->next;
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
								--data->size;
								it = tmp;
							}
						);
						return *this;
					}
					else  it = it->next;
				}
				return *this;
			}

			///<summary>Видалення елемента по ітератору. Ітератор обнулює</summary>
			__forceinline void Remove(Iterator & node)
			{
				extern __declspec(noreturn) void ThrowEndNodeNotDeferencable(
					const Seed::ConstAString & File,
					const Seed::ConstAString & Function,
					const Seed::SizeT Line);
				if (node.This == nullptr)
					ThrowEndNodeNotDeferencable(__FILE__, __FUNCSIG__, __LINE__);
				if (node.This->father == nullptr)
				{
					if (--node.This->RefCount == 0)
						delete node.This;
					node.This = nullptr;
				}
				else
					//Якщо власник існує то видалення виконується в екслюзивному блокуванні
					node.This->father->lock.ExclusiveLock([&]()
						{
							__assume(node.This->father != nullptr);
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
								node.This = node.This->next;
							}
						});
			}
#pragma endregion
#pragma region Sorting
		private:
			///<summary>Внутріщшня функція сортування списку</summary>
			///<param name = "Comparator">Фукція\лямбда яка приймає дві константні силки
			///на значення і вертає true якщо перше значення більше другого</param>
			template<typename TComparator>
			elem* SortedMerge(elem * a, elem * b, TComparator Comparator)
			{
				elem* result = nullptr;

				if (a == nullptr)
					return(b);
				else if (b == nullptr)
					return(a);

				if (Comparator(a->value, b->value))
				{
					result = a;
					result->next = SortedMerge(a->next, b, Comparator);
				}
				else
				{
					result = b;
					result->next = SortedMerge(a, b->next, Comparator);
				}
				return(result);
			}

			/* UTILITY FUNCTIONS */
			/* Split the nodes of the given list into front and back halves,
				and return the two lists using the reference parameters.
				If the length is odd, the extra node should go in the front list.
				Uses the fast/slow pointer strategy. */
			void FrontBackSplit(elem * source, elem * *frontRef, elem * *backRef)
			{
				elem* fast = source->next;
				elem* slow = source;

				/* Advance 'fast' two nodes, and advance 'slow' one node */
				while (fast != nullptr)
				{
					fast = fast->next;
					if (fast != nullptr)
					{
						slow = slow->next;
						fast = fast->next;
					}
				}

				/* 'slow' is before the midpoint in the list, so split it in two
				at that point. */
				*frontRef = source;
				*backRef = slow->next;
				slow->next = nullptr;
			}
			///<summary>Внутріщшня функція сортування списку</summary>
			///<param name = "Comparator">Фукція\лямбда яка приймає дві константні силки
			///на значення і вертає true якщо перше значення більше другого</param>
			template<typename Comparator>
			void MergeSort(elem * *headRef, Comparator cmp)
			{
				elem* head = *headRef;
				elem* a;
				elem* b;

				if ((head == nullptr) || (head->next == nullptr))
				{
					return;
				}

				FrontBackSplit(head, &a, &b);

				/* Recursively sort the sublists */
				MergeSort(&a, cmp);
				MergeSort(&b, cmp);

				/* answer = merge the two sorted lists together */
				*headRef = SortedMerge(a, b, cmp);
			}

			///<summary>Функція сортування списку</summary>
			///<param name = "Comparator">Фукція\лямбда яка приймає дві константні силки
			///на значення і вертає true якщо перше значення більше другого</param>
			///<returns>Силку на список</returns>
			template<typename TComparator>
			BaseList& Sort(TComparator Compator)
			{
				//static_assert(!StaticAsserts::IsFunction<TComparator, Boolean, const T&, const T&>(), "Invalid comparator");
				this->data->lock.ExclusiveLock([&]()
					{
						MergeSort(&data->root, Compator);
						data->last = data->root;
						while (data->last->next != nullptr) data->last = data->last->next;
					});
				return *this;
			}
		public:
			///<summary>Функція сортування списку</summary>
			///<param name = "Ascend">Якщо true то сортування відбувається по зростанню</param>
			///<returns>Силку на список</returns>
			virtual BaseList& Sort(bool IsAscendingOrder = true) override final
			{
				this->data->lock.ExclusiveLock([&]()
					{
						if (IsAscendingOrder)
							MergeSort(&data->root, [](const T & V1, const T & V2)
								{
									return V1 >= V2;
								});
						else
							MergeSort(&data->root, [](const T & V1, const T & V2)
								{
									return V1 <= V2;
								});
						data->last = data->root;
						while (data->last->next != nullptr) data->last = data->last->next;
					});
				return *this;
			}
#pragma endregion
#pragma region Search

			///<summary>Пошук елемента з певним значенням. Вертає ітератор на перший збіг</summary>
			///<param name = "Value">Значення по якому відбувається пошук<param>
			Iterator FindFirst(const T & Value)
			{
				if (this->data == nullptr || this->data->root == nullptr)
					return Iterator(nullptr);

				return this->data->lock.ShareLock([&]()
					{
						auto it = this->data->root;
						while (it != nullptr)
						{
							if (it->value == Value)
								return Iterator(it);
							it = it->next;
						}
						return Iterator(nullptr);
					});
			}
			///<summary>Пошук елемента з певним значенням. Вертає ітератор на оастаній збіг</summary>
			///<param name = "Value">Значення по якому відбувається пошук<param>
			Iterator FindLast(const T & Value)
			{
				if (this->data == nullptr || this->data->root == nullptr)
					return Iterator(nullptr);

				return this->data->lock.ShareLock([&]()
					{
						auto it = this->data->root;
						elem* found = nullptr;
						while (it != nullptr)
						{
							if (it->value == Value)
								found = it;
							it = it->next;
						}
						return Iterator(found);
					});
			}
			///<summary>Пошук елемента по фільтру. Вертає список елементів</summary>
			///<param name = "Filter">Функція\лямбда фільтр яка приймає константну силку на елемент і вертає true якщо елемент потрібний</param>
			typedef bool(*TFilterExample)(const T&);
			template<typename TFilter>
			BaseList Find(TFilter Filter)
			{
				static_assert(!StaticAsserrts::CheckSignature<Filter, Boolean, const T&>(), "Invalid filter");
				if (data == nullptr)
					return BaseList();

				BaseList list;

				this->data->lock.ShareLock([&]()
					{
						it = data->root;
						list.data = new list_data();
						while (it != nullptr)
						{
							if (filter(it->value))
							{
								elem* elemInNewList = new elem(value, list.data, list.data->last, nullptr);
								if (list.data->root == nullptr)
									list.data->root = elemInNewList;
								list.data->last->next = elemInNewList;
							}
							it = it->next;
						}
					});

				return list;
			}
#pragma endregion
#pragma region Dtor

			/////<summary>Деструктор звільняє пам'ять виділену під елементи
			/////<para>Але якщо елемент використовується через ітератор
			/////то його знищення оминається але елемент втрачає покажчик на сусідні елементи
			/////</para></summary>
			//~BaseList()
			//{
			//	if (this->data == nullptr) return;
			//	this->data->lock.ExclusiveLock([&]()
			//		{
			//			elem* node;
			//			while (this->data->root != nullptr)
			//			{
			//				node = this->data->root;
			//				this->data->root = data->root->next;
			//				if (--node->RefCount == 0)
			//				{
			//					delete node;
			//					continue;
			//				}
			//				node->next = nullptr;
			//				node->father = nullptr;
			//				node->prev = nullptr;
			//			}
			//			this->data->last = nullptr;
			//			this->data->size = 0;
			//			delete data;
			//			this->data = nullptr;
			//		});
			//}
#pragma endregion
		};
#pragma region Copy & move ctors implementation
		/////<summary>Копіювальний конструктор</sumary>
		/////<param name = "orig">Оригінал списку<</param>
		//template<typename Allocator, typename T>
		//inline BaseList<Allocator, T>::BaseList(const BaseList & Orig)
		//{
		//	if (this == &Orig)return;
		//	if (Orig.data == nullptr)
		//	{
		//		this->data = nullptr;
		//		return;
		//	}
		//	Orig.data->lock.ShareLock([&]()
		//		{
		//			this->data = new list_data;
		//			this->data->size = Orig.data->size;
		//			elem* root_orig = Orig.data->root;
		//			this->data->root = new elem(root_orig->value, this->data);
		//			this->data->last = this->data->root;
		//			root_orig = root_orig->next;
		//			if (root_orig == nullptr)
		//			{
		//				this->data->root->next = nullptr;
		//				return;
		//			}
		//			while (root_orig != nullptr)
		//			{
		//				this->data->last->next = new elem(root_orig->value, this->data, this->data->last);
		//				this->data->last = this->data->last->next;
		//				root_orig = root_orig->next;
		//			}
		//			this->data->last->next = nullptr;
		//		});
		//}
		/////<summary>Конструктор переміщення</sumary>
		/////<param name = "orig">Оригінал списку<</param>
		//template<typename Allocator, typename T>
		//__forceinline BaseList<Allocator, T>::BaseList(BaseList && Orig) noexcept
		//{
		//	this->data = Orig.data; Orig.data = nullptr;
		//}
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

#pragma warning(pop)
		namespace TypeTraits {
			template<typename Heap, typename U>
			struct TIsResizable<BaseList<Heap, U>> : TypeTraits::LogicTrue {};

			template<typename Heap, typename U>
			struct TIsDynamicSize<BaseList<Heap, U>> : TypeTraits::LogicTrue {};
		}
	}
}
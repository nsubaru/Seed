#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Containers.Internals.h"
#include "Seed.Core.Containers.IEnumerable.h"

namespace Seed {
	inline namespace Core {
		///<summary>Класс динамічного масиву</summary>
		template<typename T>
		class SeedKernelCall Array final : public Queryable::IIndexable<Array<T>, T> {
		private:
			///<summary>Масив</summary>
			T* arr;
			///<summary>Розмір</summary>
			SizeT count;
		private:
			// Function to run quicksort on an array
			// l is the leftmost starting index, which begins at 0
			// r is the rightmost starting index, which begins at array length - 1
			template<typename Lambda>
			void Quicksort(T arr[], SizeT l, SizeT r, Lambda & lambda) //-V688
			{
				// Base case: No need to sort arrays of length <= 1
				if (l >= r)
				{
					return;
				}

				// Choose pivot to be the last element in the subarray
				T pivot = arr[r];

				// Index indicating the "split" between elements smaller than pivot and 
				// elements greater than pivot
				SizeT cnt = l;

				// Traverse through array from l to r
				for (SizeT i = l; i <= r; i++)
				{
					// If an element less than or equal to the pivot is found...
					if (lambda(arr[i], pivot))
					{
						// Then swap arr[cnt] and arr[i] so that the smaller element arr[i] 
						// is to the left of all elements greater than pivot
						Swap(arr[cnt], arr[i]);

						// Make sure to increment cnt so we can keep track of what to swap
						// arr[i] with
						cnt++;
					}
				}

				// NOTE: cnt is currently at one plus the pivot's index 
				// (Hence, the cnt-2 when recursively sorting the left side of pivot)
				Quicksort(arr, l, cnt - 2, lambda); // Recursively sort the left side of pivot
				Quicksort(arr, cnt, r, lambda);   // Recursively sort the right side of pivot
			}
		protected:
			__forceinline virtual Array & ForEachImpl(Queryable::LambdaHolder<void, T&>& lambda) final override {
				for (SizeT i = 0; i < count; i++) {
					lambda(ForwardReference<T&>(arr[i]));
				}
				return *this;
			}
			__forceinline virtual Array & ForEachImpl(Queryable::LambdaHolder<void, T&, SizeT>& lambda) final override {
				for (SizeT i = 0; i < count; i++) {
					lambda(ForwardReference<T&>(arr[i]), ForwardReference<SizeT&&>(i));
				}
				return *this;
			}
			__forceinline virtual Array & ForEachImpl(Queryable::LambdaHolder<void, const T&>& lambda) final override {
				for (SizeT i = 0; i < count; i++) {
					lambda(arr[i]);
				}
				return *this;
			}
			__forceinline virtual Array & ForEachImpl(Queryable::LambdaHolder<void, const T&, SizeT>& lambda) final override {
				for (SizeT i = 0; i < count; i++) {
					lambda(arr[i], ForwardReference<SizeT&&>(i));
				}
				return *this;
			}
			__forceinline virtual const Array& ForEachImpl(Queryable::LambdaHolder<void, const T&>& lambda) const final override {
				for (SizeT i = 0; i < count; i++) {
					lambda(arr[i]);
				}
				return *this;
			}
			__forceinline virtual const Array& ForEachImpl(Queryable::LambdaHolder<void, const T&, SizeT>& lambda) const final override {
				for (SizeT i = 0; i < count; i++) {
					lambda(arr[i], ForwardReference<SizeT&&>(i));
				}
				return *this;
			}
			__forceinline virtual Array & FillImpl(Queryable::LambdaHolder<T>& lambda, SizeT ReqCount) final override {
				if (ReqCount != count) 
					Resize(ReqCount);

				for (SizeT i = 0; i < count; i++) {
					arr[i] = MoveRValue(lambda());
				}

				return *this;
			}
			__forceinline virtual Array& FillImpl(Queryable::LambdaHolder<T, SizeT>& lambda, SizeT ReqCount) final override {
				if (ReqCount != count)
					Resize(ReqCount);

				for (SizeT i = 0; i < count; i++) {
					arr[i] = MoveRValue(lambda(ForwardReference<SizeT&&>(i)));
				}

				return *this;
			}

			__forceinline virtual void TransferTo(T&& Value, SizeT ItemIndex) final override {
				this->arr[ItemIndex] = MoveRValue(Value);
			}

			inline virtual void PrepareCopyToOrMove(SizeT NewSize) final override {
				this->Resize(NewSize);
			}

			virtual Array& SortImpl(Queryable::LambdaHolder<bool, const T&, const T&>& lambda, bool IsAscendingOrder) final override {
				if (IsAscendingOrder) {
					Quicksort(this->arr, 0, this->count - 1,
						[&](const T& Item1, const T& Item2) {
							return lambda(Item1, Item2);
						});
				}
				else {
					Quicksort(this->arr, 0, this->count - 1,
						[&](const T& Item1, const T& Item2) {
							return !lambda(Item1, Item2);
						});
				}
				return *this;
			}

			virtual void RemoveAllImpl(Queryable::LambdaHolder<void, T&&>& lambda) final override {
				for (SizeT i = 0; i < count; i++)
				{
					lambda(MoveRValue(arr[i]));
				}
				delete[] arr;
				arr = nullptr;
				count = 0;
			}
			virtual void RemoveAllImpl(Queryable::LambdaHolder<void, T&&, SizeT>& lambda) final override {
				for (SizeT i = 0; i < count; i++)
				{
					lambda(ForwardReference<T&&>(arr[i]), ForwardReference<SizeT&&>(i));
				}
				delete[] arr;
				arr = nullptr;
				count = 0;
			}
			virtual Array& RemoveAll() final override {
				delete[] arr;
				arr = nullptr;
				count = 0;
				return *this;
			}


		public:
			virtual Array& Sort(bool IsAscendingOrder = true) final override {
				if (IsAscendingOrder) {
					Quicksort(this->arr, 0, this->count - 1,
						[&](const T& Item1, const T& Item2) {
							return Item1 >= Item2;
						});
				}
				else {
					Quicksort(this->arr, 0, this->count - 1,
						[&](const T& Item1, const T& Item2) {
							return Item1 <= Item2;
						});
				}
				return *this;
			}
		public:
			///<summary>Кількість елементів</summary>
			///<returns>Кількість елементів</returns>
			inline virtual SizeT GetCount() const { return count; }

			///<summary>Вертає покажчик на масив</summary>
			///<returns>Вертає покажчик на масив</returns>
			__forceinline const T* const GetRawArray() const { return arr; }

			///<summary>Кількість елементів в масиві</summary>
			Property(GetCount) SizeT Count;
			///<summary>Покажчик на масив</summary>
			Property(GetRawArray) const T* const Raw;

			///<summary>Конструктор</summary>
			__forceinline Array() noexcept : arr(nullptr), count(0) {}

			///<summary>Конструктор з початковим розміром, змінні не ініціалізовуються</summary>
			///<param name = 'Count'>Кількість елементів в масиві</param>
			__forceinline Array(SizeT Count) : arr(new T[Count]), count(Count) {}
#pragma warning(push)
			//initializer_list.Size is count of items not size of array
#pragma warning(disable: 6386)
			///<summary>Конструктор через list-initializer</summary>
			///<param name = 'list'>Статичний список ініціалізація</param>
			__forceinline Array(const CRT::initializer_list<T>& list) : arr(new T[list.size()]), count(list.size()) {
				auto it = list.begin();
				for (SizeT i = 0; i < count; i++)
					this->arr[i] = *it, it++; //-V108 //-V2528
			}
#pragma warning(pop)

			///<summary>Копіювальний конструктор з С подібного масиву і його розміру</summary>
			///<param name = 'RawArray'>С-подібний масив</param>
			///<param name = 'RawArraySize'>Розмір масиву</param>
			__forceinline Array(const T * RawArray, SizeT RawArraySize) : arr(new T[RawArraySize]), count(RawArraySize) {
				Memory::Copy(RawArray, this->arr, RawArraySize);
			}

			///<summary>Конструктор переміщення для С подібного масиву і його розміру</summary>
			///<param name = 'RawArray'>С-подібний масив який буде переміщатись</param>
			///<param name = 'Count'>Кількість елементів в масиві</param>
			__forceinline Array(T * &&RawArray, SizeT Count) : arr(MoveRValue(RawArray)), count(Count) { RawArray = nullptr; }

			///<summary>Копіювальний конструктор</summary>
			///<param name = 'Orig'>Оригинал масиву</param>
			Array(const Array<T> & Orig);
			///<summary>Конструктор переміщення</summary>
			///<param name = 'From'>Оригинал який буде обнулено після виклику конструктора</param>
			Array(Array<T> && From) noexcept;

			///<summary>Оператор присвоєння</summary>
			///<param name = 'From'>Оригинал масиву</param>
			///<returns>Силка на копію масиву</returns>
			__forceinline Array<T>& operator=(const Array<T> & From) {
				if (this == &From) return *this;
				this->~Array();
				this->count = From.count;
				this->arr = new T[count];
#pragma warning(push)
#pragma warning(disable: 4996)
				Memory::Copy(From.arr, this->arr, count);
#pragma warning(pop)
				return *this;
			}

			///<summary>Оператор переміщення</summary>
			///<param name = 'From'>Оригинал який буде обнулено після виклику конструктора</param>
			///<returns>Силка на копію масиву</returns>
			__forceinline Array<T>& operator=(Array<T> && From) {
				this->count = From.count;
				this->arr = From.arr;
				From.arr = nullptr;
				From.count = 0;
				return *this;
			}

			///<summary>Оператор доступу по індексу</summary>
			///<param name = 'Index'>Індекс елемент</param>
			///<returns>Силка на елемент</returns>
			__forceinline virtual T& operator[](SizeT Index) final override {
				extern __declspec(noreturn) void ThrowIndexOutOfRange(
					const Seed::ConstAString & File,
					const Seed::ConstAString & Function,
					const Seed::SizeT Line,
					const Seed::SizeT Required,
					const Seed::SizeT MaxAllowed);
				if (Index < count)
					return arr[Index];
				else
					ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, Index, count);
			}

			///<summary>Констатний оператор доступу по індексу</summary>
			///<param name = 'Index'>Індекс елемент</param>
			///<returns>Константа силка на елемент</returns>
			__forceinline virtual const T& operator[](SizeT index) const final override {
				extern __declspec(noreturn) void ThrowIndexOutOfRange(
					const Seed::ConstAString & File,
					const Seed::ConstAString & Function,
					const Seed::SizeT Line,
					const Seed::SizeT Required,
					const Seed::SizeT MaxAllowed);
				if (index < count)
					return arr[index];
				else
					ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, index, count);
			}

			///<summary>Оператор рівності</summary>
			///<param name = 'Left'>Лівий елемент виразу</param>
			///<returns>True якщо розмір і елементи масивів рівні</returns>
			__forceinline bool operator==(const Array<T> & Left) noexcept {
				if (Left.count != this->count) return false;
				for (SizeT i = 0; i < this->count; i++)
					if (this->arr[i] != Left.arr[i])return false;
				return true;
			}

			///<summary>Оператор нерівності</summary>
			///<param name = 'Left'>Лівий елемент виразу</param>
			///<returns>True якщо кількість або елементи не рівні</returns>
			__forceinline bool operator!=(const Array<T> & Left) noexcept {
				if (Left.count != this->count) return true;
				for (SizeT i = 0; i < this->count; i++)
					if (this->arr[i] != Left.arr[i])return true;
				return false;
			}

			///<summary>Оператор перетворення який вертає константний покажчик на С подібний масив</summary>
			///<returns>Покажчик на внутрішній масив</returns>
			DeprecatedCall("Робота з с-подібнимим масивами") operator const T * () const {
				return arr;
			}

			///<summary>Зміна розміру</summary>
			///<param name = 'NewSize'>Новий розмір масиву</param>
			__forceinline Array& Resize(SizeT NewSize) {
				if (this->arr == nullptr || this->count == 0)
				{
					this->arr = new T[NewSize];
					this->count = NewSize;
				}
				else {
					T* buf = new T[NewSize];
					SizeT CopySize = (NewSize > count) ? count : NewSize;
					for (SizeT i = 0; i < CopySize; i++)
					{
						buf[i] = MoveRValue(this->arr[i]);
					}
					delete[] this->arr;
					count = NewSize;
					this->arr = buf;
				}
				return *this;
			}

			///<summary>Оптимізований обмін динамічних масивів</summary>
			///<param name = 'A'>Перший елемент</param>
			///<param name = 'B'>Другий елемент</param>
			__forceinline friend void Swap(Array & A, Array & B) {
				Swap(A.arr, B.arr);
				Swap(A.count, B.count);
			}

			///<summary>Перевертає елементи масиву</summary>
			///<returns>Силку на поточний масив після зміни порядку елементів</returns>
			__forceinline virtual Array<T>& Reverse() final override {
#pragma warning(push)
#pragma warning(disable: 4996)
				Seed::Reverse(arr, count);
#pragma warning(pop)
				return *this;
			}

			///<summary>Вертаж підмасив який складається з всіх елементів
			///<para>починаючи з елемента StartIndex</para></summary>
			///<param name = 'StartIndex'>Номер першого елемента починаючи
			///<para>з нуля який має бути включений в Slice</para></param>
			///<returns>Підмасив</returns>
			virtual Array<T> Slice(SizeT StartIndex) final override {
				extern __declspec(noreturn) void ThrowNullPointException(
					const Seed::ConstAString & File,
					const Seed::ConstAString & Function,
					const Seed::SizeT Line,
					const Seed::SizeT Address,
					const Seed::UInt8 Mode);
				if (StartIndex > count)
					ThrowNullPointException(
						__FILE__,
						__FUNCSIG__,
						__LINE__,
						reinterpret_cast<SizeT>(this->arr + StartIndex),
						0);
				SizeT SliceCount = count - StartIndex;
				T * __array = new T[SliceCount];

				for (SizeT i = 0; i < SliceCount; i++) {
					__array[i] = arr[StartIndex + i];
				}

				return Array<T>(MoveRValue(__array), SliceCount);
			}

			///<summary>Вертає підмасив який складається з Count елементів починаючи з StartIndex елемента</summary>
			///<param name = 'StartIndex'>Номер першого елемента починаючи
			///<para>з нуля який має бути включений в Slice</para></param>
			///<param name = 'Count'>Кількість елементів</param>
			///<returns>Підмасив</returns>
			virtual Array<T> Slice(SizeT StartIndex, SizeT SliceCount) final override {
				extern __declspec(noreturn) void ThrowNullPointException(
					const Seed::ConstAString & File,
					const Seed::ConstAString & Function,
					const Seed::SizeT Line,
					const Seed::SizeT Address,
					const Seed::UInt8 Mode);
				SliceCount++;
				if (StartIndex + SliceCount > count)
					ThrowNullPointException(
						__FILE__,
						__FUNCSIG__,
						__LINE__,
						reinterpret_cast<SizeT>(this->arr + StartIndex + Count),
						0);
				

				T * __array = new T[SliceCount];

				for (SizeT i = 0; i < SliceCount; i++) {
					__array[i] = arr[StartIndex + i];
				}

				return Array<T>(MoveRValue(__array), SliceCount);
			}

			///<summary>Деструктор</summary>
			__forceinline ~Array() {
				if (this->arr != nullptr) {
					delete[] this->arr;
					count = 0;
					this->arr = nullptr;
				}
			}
		};

		///<summary>Копіювальний конструктор</summary>
		///<param name = 'Orig'>Оригинал масиву</param>
		template<typename T>
		__forceinline Array<T>::Array(const Array & orig) {
			this->arr = new T[orig.count];
#pragma warning(push)
#pragma warning(disable: 4996)
			Memory::Copy(orig.arr, this->arr, sizeof(T) * orig.count);
#pragma warning(pop)
			this->count = orig.count;
		}

		///<summary>Конструктор переміщення</summary>
		///<param name = 'From'>Оригинал який буде обнулено після виклику конструктора</param>
		template<typename T>
		__forceinline Array<T>::Array(Array && orig) noexcept {
			this->arr = orig.arr;
			this->count = orig.count;
			orig.arr = nullptr;
			orig.count = 0;
		}

		using Blob = Array<Byte>;

		namespace TypeTraits {
			template<typename U>
			struct TIsResizable<Array<U>> : TypeTraits::LogicTrue {};

			template<typename U>
			struct TIsDynamicSize<Array<U>> : TypeTraits::LogicFalse {};
		}
	}
}
#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Containers.Internals.h"

namespace Seed {
	inline namespace Core {
		///<summary>Класс динамічного масиву</summary>
		template<typename T>
		class Array {
		private:
			///<summary>Масив</summary>
			T* arr;
			///<summary>Розмір</summary>
			SizeT count;
		public:
			///<summary>Кількість елементів</summary>
			///<returns>Кількість елементів</returns>
			__forceinline SizeT GetCount() const { return count; }

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
			__forceinline Array<T>& operator=(Array<T> && From) noexcept {
				this->count = From.count;
				this->arr = From.arr;
				From.arr = nullptr;
				From.count = 0;
				return *this;
			}

			///<summary>Оператор доступу по індексу</summary>
			///<param name = 'Index'>Індекс елемент</param>
			///<returns>Силка на елемент</returns>
			__forceinline T& operator[](SizeT Index) {
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
			__forceinline const T& operator[](SizeT index) const {
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
			__forceinline void Resize(SizeT NewSize) {
				T* buf = new T[NewSize];
				SizeT CopySize = (NewSize > count) ? count : NewSize;
#pragma warning(push)
#pragma warning(disable: 4996)
				Memory::Copy(this->arr, buf, CopySize);
#pragma warning(pop)
				delete[] this->arr;
				count = NewSize;
				this->arr = buf;
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
			__forceinline Array<T>& Reverse() {
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
			Array<T> Slice(SizeT StartIndex) {
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

				for (SizeT i = 0; i <= SliceCount; i++) {
					__array[i] = arr[StartIndex + i];
				}

				return Array<T>(MoveRValue(__array), SliceCount);
			}

			///<summary>Вертає підмасив який складається з Count елементів починаючи з StartIndex елемента</summary>
			///<param name = 'StartIndex'>Номер першого елемента починаючи
			///<para>з нуля який має бути включений в Slice</para></param>
			///<param name = 'Count'>Кількість елементів</param>
			///<returns>Підмасив</returns>
			Array<T> Slice(SizeT StartIndex, SizeT SliceCount) {
				extern __declspec(noreturn) void ThrowNullPointException(
					const Seed::ConstAString & File,
					const Seed::ConstAString & Function,
					const Seed::SizeT Line,
					const Seed::SizeT Address,
					const Seed::UInt8 Mode);
				if (StartIndex + SliceCount > count)
					ThrowNullPointException(
						__FILE__,
						__FUNCSIG__,
						__LINE__,
						reinterpret_cast<SizeT>(this->arr + StartIndex + Count),
						0);

				T * __array = new T[SliceCount];

				for (SizeT i = 0; i <= SliceCount; i++) {
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
	}
}
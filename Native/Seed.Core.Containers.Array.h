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
			T * arr;
			///<summary>Розмір</summary>
			SizeT size;
		public:
			///<summary>Кількість елементів</summary>
			///<returns>Кількість елементів</returns>
			__forceinline SizeT getSize() const { return size; }

			///<summary>Кількість елементів в масиві</summary>
			Property(getSize) SizeT Size;

			///<summary>Конструктор</summary>
			__forceinline Array() noexcept : arr(nullptr), size(0) {}

			///<summary>Конструктор з початковим розміром, змінні не ініціалізовуються</summary>
			///<param name = 'Size'>Розмір масиву</param>
			__forceinline Array(SizeT Size) : arr(new T[Size]), size(Size) {}

			///<summary>Конструктор через list-initializer</summary>
			///<param name = 'list'>Статичний список ініціалізація</param>
			__forceinline Array(const CRT::initializer_list<T> & list) : arr(new T[list.size()]), size(list.size()) {
				auto it = list.begin();
				for (SizeT i = 0; i < size; i++)
					this->arr[i] = *it, it++; //-V108
			}

			///<summary>Копіювальний конструктор з С подібного масиву і його розміру</summary>
			///<param name = 'RawArray'>С-подібний масив</param>
			///<param name = 'RawArraySize'>Розмір масиву</param>
			__forceinline Array(const T* RawArray, SizeT RawArraySize) : arr(new T[RawArraySize]), size(RawArraySize) {
				Memory::Copy(RawArray, this->arr, RawArraySize);
			}

			///<summary>Конструктор переміщення для С подібного масиву і його розміру</summary>
			///<param name = 'RawArray'>С-подібний масив який буде переміщатись</param>
			///<param name = 'Size'>Розмір даного масиву</param>
			__forceinline Array(T *&& RawArray, SizeT Size) : arr(MoveRValue(RawArray)), size(Size) { RawArray = nullptr; }

			///<summary>Копіювальний конструктор</summary>
			///<param name = 'Orig'>Оригинал масиву</param>
			Array(const Array<T>& Orig);
			///<summary>Конструктор переміщення</summary>
			///<param name = 'From'>Оригинал який буде обнулено після виклику конструктора</param>
			Array(Array<T>&& From) noexcept;

			///<summary>Оператор присвоєння</summary>
			///<param name = 'From'>Оригинал масиву</param>
			///<returns>Силка на копію масиву</returns>
			__forceinline Array<T> & operator=(const Array<T> & From) {
				if (this == &From) return *this;
				this->~Array();
				this->size = From.size;
				this->arr = new T[size];
				Memory::Copy(From.arr, this->arr, size);
				return *this;
			}

			///<summary>Оператор переміщення</summary>
			///<param name = 'From'>Оригинал який буде обнулено після виклику конструктора</param>
			///<returns>Силка на копію масиву</returns>
			__forceinline Array<T> & operator=(Array<T> && From) noexcept {
				this->size = From.size;
				this->arr = From.arr;
				From.arr = nullptr;
				From.size = 0;
				return *this;
			}

			///<summary>Оператор доступу по індексу</summary>
			///<param name = 'Index'>Індекс елемент</param>
			///<returns>Силка на елемент</returns>
			__forceinline T& operator[](SizeT Index) {
				if (Index < size)
					return arr[Index];
				else
					Throw(Seed::IndexOutOfRange(ExeptionPosition, Index, getSize()));
			}

			///<summary>Констатний оператор доступу по індексу</summary>
			///<param name = 'Index'>Індекс елемент</param>
			///<returns>Константа силка на елемент</returns>
			__forceinline const T& operator[](SizeT index) const {
				if (index < size)
					return arr[index];
				else
					Throw(Seed::IndexOutOfRange(ExeptionPosition, index, getSize()));
			}

			///<summary>Оператор рівності</summary>
			///<param name = 'Left'>Лівий елемент виразу</param>
			///<returns>True якщо розмір і елементи масивів рівні</returns>
			__forceinline bool operator==(const Array<T> & Left) noexcept {
				if (Left.size != this->size) return false;
				for (SizeT i = 0; i < this->size; i++)
					if (this->arr[i] != Left.arr[i])return false;
				return true;
			}

			///<summary>Оператор нерівності</summary>
			///<param name = 'Left'>Лівий елемент виразу</param>
			///<returns>True якщо кількість або елементи не рівні</returns>
			__forceinline bool operator!=(const Array<T> & Left) noexcept {
				if (Left.size != this->size) return true;
				for (SizeT i = 0; i < this->size; i++)
					if (this->arr[i] != Left.arr[i])return true;
				return false;
			}


			///<summary>Оператор перетворення який вертає константний покажчик на С подібний масив</summary>
			///<returns>Покажчик на внутрішній масив</returns>
			DeprecatedCall("Робота з с-подібнимим масивами") operator const T*() const {
				return arr;
			}

			//Зміна розміру
			__forceinline void ReSize(SizeT NewSize) {
				T* buf = new T[NewSize];
				SizeT CopySize = (NewSize > size) ? size : NewSize;
				Memory::Copy(this->arr, buf, CopySize);
				delete[] this->arr;
				size = NewSize;
				this->arr = buf;
			}

			//Оптимізований обмін динамічних масивів
			__forceinline friend void Swap(Array & A, Array & B) {
				Swap(A.arr, B.arr);
				Swap(A.size, B.size);
			}

			//Перевертає елементи масиву
			__forceinline void Reverse() {
#pragma warning(push)
#pragma warning(disable: 4996)
				Seed::Reverse(arr, size);
#pragma warning(pop)
			}

			//Вертає підмасив який складається з Count елементів
			//починаючи з StartIndex елемента
			Array<T> Slice(SizeT StartIndex) {
				if(StartIndex > size)
					Throw(
						NullPointExeption(
							ExeptionPosition,
							((SizeT)this->arr) + StartIndex,
							NullPointExeption::Types::Read
						)
					);
				SizeT Count = size - StartIndex;
				T * __array = new T[Count];

				for (SizeT i = 0; i <= Count; i++) {
					__array[i] = arr[StartIndex + i];
				}

				return Array<T>(MoveRValue(__array), Count);
			}

			Array<T> Slice(SizeT StartIndex, SizeT Count) {
				
				if (StartIndex + Count > size)
					Throw(
						NullPointExeption(
							ExeptionPosition, 
							((SizeT)this->arr) + StartIndex + Count, 
							NullPointExeption::Types::Read
						)
					);

				T * __array = new T[Count];

				for (SizeT i = 0; i <= Count; i++) {
					__array[i] = arr[StartIndex + i];
				}

				return Array<T>(MoveRValue(__array), Count);
			}

			//Деструктор
			__forceinline ~Array() {
				if (this->arr != nullptr) {
					delete[] this->arr;
					size = 0;
				}
			}
		};

		//Копіювальний конструктор
		template<typename T>
		__forceinline Array<T>::Array(const Array& orig) {
			this->arr = new T[orig.size];
			Memory::Copy(orig.arr, this->arr, sizeof(T)*orig.size);
			this->size = orig.size;
		}

		//Конструктор переміщення
		template<typename T>
		__forceinline Array<T>::Array(Array&& orig) noexcept {
			this->arr = orig.arr;
			this->size = orig.size;
			orig.arr = nullptr;
			orig.size = 0;
		}
	}
}
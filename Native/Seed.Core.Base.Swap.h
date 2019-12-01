#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Utils.h"

namespace Seed {
	inline namespace Core {
		/// <summary>
		/// Обмін значеннями змінних
		/// </summary>
		/// <typeparam name="T">Тип елементів</typeparam>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Класичний обмін через третю зміну:
		/// Приклад: дано змінні A і B треба обміняти їхні значення
		/// Процедура наступна: робим тимчасову зміну С
		/// C = A; A = B; B = C;
		/// </remarks>
		template<typename T>
		__forceinline void Swap(T& first, T& second) {
			T tmp(MoveRValue(first));
			first = MoveRValue(second);
			second = MoveRValue(tmp);
		}
		
		/// <summary>
		/// Оптимізований обмін значеннями змінних AChar
		/// </summary>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи особливість побітового XOR,
		/// за допомогою якого можна обміняти без третьої змінної.
		/// Псевдокод: A = A ^ B; B = B ^ A; A = A ^ B;
		/// Приклад числа 5 і 10 бітове представлення 0101 і 1010:
		/// 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000; 
		/// </remarks>
		template<>
		__forceinline void Swap(AChar& first, AChar& second) { first ^= second; second ^= first; first ^= second; }

		/// <summary>
		/// Оптимізований обмін значеннями змінних UChar
		/// </summary>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи особливість побітового XOR,
		/// за допомогою якого можна обміняти без третьої змінної.
		/// Псевдокод: A = A ^ B; B = B ^ A; A = A ^ B;
		/// Приклад числа 5 і 10 бітове представлення 0101 і 1010:
		/// 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000; 
		/// </remarks>
		template<>
		__forceinline void Swap(UChar& first, UChar& second) { first ^= second; second ^= first; first ^= second; }

		/// <summary>
		/// Оптимізований обмін значеннями змінних Byte
		/// </summary>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи особливість побітового XOR,
		/// за допомогою якого можна обміняти без третьої змінної.
		/// Псевдокод: A = A ^ B; B = B ^ A; A = A ^ B;
		/// Приклад числа 5 і 10 бітове представлення 0101 і 1010:
		/// 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000; 
		/// </remarks>
		template<>
		__forceinline void Swap(Byte& first, Byte& second) { first ^= second; second ^= first; first ^= second; }

		/// <summary>
		/// Оптимізований обмін значеннями змінних Int16
		/// </summary>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи особливість побітового XOR,
		/// за допомогою якого можна обміняти без третьої змінної.
		/// Псевдокод: A = A ^ B; B = B ^ A; A = A ^ B;
		/// Приклад числа 5 і 10 бітове представлення 0101 і 1010:
		/// 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000; 
		/// </remarks>
		template<>
		__forceinline void Swap(Int16& first, Int16& second) { first ^= second; second ^= first; first ^= second; }

		/// <summary>
		/// Оптимізований обмін значеннями змінних Int32
		/// </summary>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи особливість побітового XOR,
		/// за допомогою якого можна обміняти без третьої змінної.
		/// Псевдокод: A = A ^ B; B = B ^ A; A = A ^ B;
		/// Приклад числа 5 і 10 бітове представлення 0101 і 1010:
		/// 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000; 
		/// </remarks>
		template<>
		__forceinline void Swap(Int32& first, Int32& second) { first ^= second; second ^= first; first ^= second; }

		/// <summary>
		/// Оптимізований обмін значеннями змінних Int64
		/// </summary>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи особливість побітового XOR,
		/// за допомогою якого можна обміняти без третьої змінної.
		/// Псевдокод: A = A ^ B; B = B ^ A; A = A ^ B;
		/// Приклад числа 5 і 10 бітове представлення 0101 і 1010:
		/// 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000; 
		/// </remarks>
		template<>
		__forceinline void Swap(Int64& first, Int64& second) { first ^= second; second ^= first; first ^= second; }

		/// <summary>
		/// Оптимізований обмін значеннями змінних UInt16
		/// </summary>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи особливість побітового XOR,
		/// за допомогою якого можна обміняти без третьої змінної.
		/// Псевдокод: A = A ^ B; B = B ^ A; A = A ^ B;
		/// Приклад числа 5 і 10 бітове представлення 0101 і 1010:
		/// 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000; 
		/// </remarks>
		template<>
		__forceinline void Swap(UInt16& first, UInt16& second) { first ^= second; second ^= first; first ^= second; }

		/// <summary>
		/// Оптимізований обмін значеннями змінних UInt32
		/// </summary>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи особливість побітового XOR,
		/// за допомогою якого можна обміняти без третьої змінної.
		/// Псевдокод: A = A ^ B; B = B ^ A; A = A ^ B;
		/// Приклад числа 5 і 10 бітове представлення 0101 і 1010:
		/// 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000; 
		/// </remarks>
		template<>
		__forceinline void Swap(UInt32& first, UInt32& second) { first ^= second; second ^= first; first ^= second; }

		/// <summary>
		/// Оптимізований обмін значеннями змінних UInt32
		/// </summary>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи особливість побітового XOR,
		/// за допомогою якого можна обміняти без третьої змінної.
		/// Псевдокод: A = A ^ B; B = B ^ A; A = A ^ B;
		/// Приклад числа 5 і 10 бітове представлення 0101 і 1010:
		/// 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000; 
		/// </remarks>
		template<>
		__forceinline void Swap(UInt64& first, UInt64& second) { first ^= second; second ^= first; first ^= second; }

		/// <summary>
		/// Оптимізований обмін адресами покажчиків
		/// </summary>
		/// <param name="first">Перший покажчик</param>
		/// <param name="second">Другий покажчик</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи те що на архітектурі AMD64 покажчиком всього лиш число UInt64
		/// Сам обміну відбувається наступним чином:
		/// Перетворюємо покажчик в число SizeT, тоді обмінюємо про числа і все 
		/// </remarks>
		template<typename T>
		__forceinline void Swap(T*& first, T*& second) {
			Swap(reinterpret_cast<UInt64&>(first), reinterpret_cast<UInt64&>(second)); //-V2005 //-V2533
		}
		static_assert(sizeof(void*) == sizeof(SizeT), "SizeT size is not valid. Must be same as size of pointer");
		static_assert(reinterpret_cast<SizeT>(reinterpret_cast<void*>(0xF0F0F0F0F)) == static_cast<SizeT>(0xF0F0F0F0F), "Error. Can not aquire pointer address to SizeT");  //-V566

		/// <summary>
		/// Оптимізований обмін значеннями змінних Float
		/// </summary>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи особливість побітового XOR та особливістю <c>union</c>
		/// Спочатку представляє дійсні числа як цілі а тоді за допомогою XOR робим обмін без третьої змінної
		/// </remarks>
		template<>
		__forceinline void Swap(Float& first, Float& second) {
			static_assert(sizeof(Float) == sizeof(Int32), "Error. Float and Int32 must have same size");
			union  // NOLINT
			{
				Int32* i;
				Float* f;
			} f1, f2;
			f1.f = &first;
			f2.f = &second;
			*f1.i ^= *f2.i;
			*f2.i ^= *f1.i;
			*f1.i ^= *f2.i;
		}

		/// <summary>
		/// Оптимізований обмін значеннями змінних Double
		/// </summary>
		/// <param name="first">Перше значення</param>
		/// <param name="second">Друге значення</param>
		/// <remarks>
		/// Оптимізований обмін відбувається використовуючи особливість побітового XOR та особливістю <c>union</c>
		/// Спочатку представляє дійсні числа як цілі а тоді за допомогою XOR робим обмін без третьої змінної
		/// </remarks>
		template<>
		__forceinline void Swap(Double& first, Double& second) {
			static_assert(sizeof(Double) == sizeof(Int64), "Error. Double and Int64 must have same size");
			union  // NOLINT
			{
				Int64* i;
				Double* f;
			} F1, F2;
			F1.f = &first;
			F2.f = &second;
			*F1.i ^= *F2.i;
			*F2.i ^= *F1.i;
			*F1.i ^= *F2.i;
		}
	}
}
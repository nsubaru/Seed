#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Utils.h"

namespace Seed {
	inline namespace Core {
		///<summary>Обмін значеннями зміних</summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Класичний обмін через третю зміну:
		///<para>Приклад: дано змінні A і B треба обмінняти їхні значення</para>
		///<para>Процедура наступна: робим тимчасову зміну С</para>
		///<para><code> C = A; A = B; B = C;</code></para>
		///</remarks>
		template<typename T>
		__forceinline void Swap(T & First, T & Second) noexcept { 
			T tmp(MoveRValue(First)); 
			First = MoveRValue(Second);
			Second = MoveRValue(tmp);
		}

		///<summary>Оптимізований обмін значеннями зміних AChar</summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Оптимізований обмін відбувається використовуючи особливість побітового XOR, 
		///<para>задопомогою якого можна обміняти без третьої зміної. </para>
		///<para>Псевдокод:<code> A = A ^ B; B = B ^ A; A = A ^ B;</code></para>
		///<para>Приклад числа 5 і 10 бітове представлення 0101 і 1010:</para>
		///<para><code> 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000;</code> </para>
		///</remarks>
		template<>
		__forceinline void Swap(AChar & First, AChar & Second) noexcept { First ^= Second; Second ^= First; First ^= Second; }

		///<summary>Оптимізований обмін значеннями зміних UChar</summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Оптимізований обмін відбувається використовуючи особливість побітового XOR, 
		///<para>задопомогою якого можна обміняти без третьої зміної. </para>
		///<para>Псевдокод:<code> A = A ^ B; B = B ^ A; A = A ^ B;</code></para>
		///<para>Приклад числа 5 і 10 бітове представлення 0101 і 1010:</para>
		///<para><code> 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000;</code> </para>
		///</remarks>
		template<>
		__forceinline void Swap(UChar & First, UChar & Second) noexcept { First ^= Second; Second ^= First; First ^= Second; }

		///<summary>Оптимізований обмін значеннями зміних Byte</summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Оптимізований обмін відбувається використовуючи особливість побітового XOR, 
		///<para>задопомогою якого можна обміняти без третьої зміної. </para>
		///<para>Псевдокод:<code> A = A ^ B; B = B ^ A; A = A ^ B;</code></para>
		///<para>Приклад числа 5 і 10 бітове представлення 0101 і 1010:</para>
		///<para><code> 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000;</code> </para>
		///</remarks>
		template<>
		__forceinline void Swap(Byte & First, Byte & Second) noexcept { First ^= Second; Second ^= First; First ^= Second; }

		///<summary>Оптимізований обмін значеннями зміних Int16</summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Оптимізований обмін відбувається використовуючи особливість побітового XOR, 
		///<para>задопомогою якого можна обміняти без третьої зміної. </para>
		///<para>Псевдокод:<code> A = A ^ B; B = B ^ A; A = A ^ B;</code></para>
		///<para>Приклад числа 5 і 10 бітове представлення 0101 і 1010:</para>
		///<para><code> 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000;</code> </para>
		///</remarks>
		__forceinline void Swap(Int16 & First, Int16 & Second) noexcept { First ^= Second; Second ^= First; First ^= Second; }

		///<summary>Оптимізований обмін значеннями зміних Int32</summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Оптимізований обмін відбувається використовуючи особливість побітового XOR, 
		///<para>задопомогою якого можна обміняти без третьої зміної. </para>
		///<para>Псевдокод:<code> A = A ^ B; B = B ^ A; A = A ^ B;</code></para>
		///<para>Приклад числа 5 і 10 бітове представлення 0101 і 1010:</para>
		///<para><code> 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000;</code> </para>
		///</remarks>
		template<>
		__forceinline void Swap(Int32 & First, Int32 & Second) noexcept { First ^= Second; Second ^= First; First ^= Second; }

		///<summary>Оптимізований обмін значеннями зміних Int64</summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Оптимізований обмін відбувається використовуючи особливість побітового XOR, 
		///<para>задопомогою якого можна обміняти без третьої зміної. </para>
		///<para>Псевдокод:<code> A = A ^ B; B = B ^ A; A = A ^ B;</code></para>
		///<para>Приклад числа 5 і 10 бітове представлення 0101 і 1010:</para>
		///<para><code> 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000;</code> </para>
		///</remarks>
		template<>
		__forceinline void Swap(Int64 & First, Int64 & Second) noexcept { First ^= Second; Second ^= First; First ^= Second; }

		///<summary>Оптимізований обмін значеннями зміних UInt16</summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Оптимізований обмін відбувається використовуючи особливість побітового XOR, 
		///<para>задопомогою якого можна обміняти без третьої зміної. </para>
		///<para>Псевдокод:<code> A = A ^ B; B = B ^ A; A = A ^ B;</code></para>
		///<para>Приклад числа 5 і 10 бітове представлення 0101 і 1010:</para>
		///<para><code> 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000;</code> </para>
		///</remarks>
		template<>
		__forceinline void Swap(UInt16 & First, UInt16 & Second) noexcept { First ^= Second; Second ^= First; First ^= Second; }

		///<summary>Оптимізований обмін значеннями зміних UInt32</summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Оптимізований обмін відбувається використовуючи особливість побітового XOR, 
		///<para>задопомогою якого можна обміняти без третьої зміної. </para>
		///<para>Псевдокод:<code> A = A ^ B; B = B ^ A; A = A ^ B;</code></para>
		///<para>Приклад числа 5 і 10 бітове представлення 0101 і 1010:</para>
		///<para><code> 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000;</code> </para>
		///</remarks>
		template<>
		__forceinline void Swap(UInt32 & First, UInt32 & Second) noexcept { First ^= Second; Second ^= First; First ^= Second; }

		///<summary>Оптимізований обмін значеннями зміних UInt64</summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Оптимізований обмін відбувається використовуючи особливість побітового XOR, 
		///<para>задопомогою якого можна обміняти без третьої зміної. </para>
		///<para>Псевдокод:<code> A = A ^ B; B = B ^ A; A = A ^ B;</code></para>
		///<para>Приклад числа 5 і 10 бітове представлення 0101 і 1010:</para>
		///<para><code> 0000 = 0101 ^ 1010; 0101 = 1010 ^ 0000; 1010 = 0101^0000;</code> </para>
		///</remarks>
		template<>
		__forceinline void Swap(UInt64 & First, UInt64 & Second) noexcept { First ^= Second; Second ^= First; First ^= Second; }

		///<summary>Оптимізований обмін адресами покажчиків</summary>
		///<param name='First'>Перший покажчик</param>
		///<param name='Second'>Другий покажчик</param>
		///<remarks>Оптимізований обмін відбувається використовуючи 
		///<para>те що на архітектурі AMD64 покачжик всього лиш число UInt64 </para>
		///<para>Сам обміну відбувається настпним чином: </para>
		///<para>Перетворюєм покажчик в число SizeT, тоді обмінюєм про числа і все </para>
		///</remarks>
		template<typename T>
		__forceinline void Swap(T *& First, T *& Second) noexcept {
			Swap((UInt64&)First, (UInt64&)Second); //-V2005
		}
		static_assert(sizeof(void*) == sizeof(SizeT), "Помилка розмір SizeT не є рівним розміру покажчика");
		static_assert((SizeT)(void*)(0xF0F0F0F0F) == (SizeT)0xF0F0F0F0F, "Помилка неможливо правильно перетворити адресс покажчик в SizeT"); //-V566

		///<summary>Оптимізований обмін значеннями зміних Float </summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Оптимізований обмін відбувається використовуючи особливість побітового XOR та особливістю <c>union</c>
		///<para>Спочатку представляєм дійсні числа як цілі а тоді задопомогою XOR робим обмін без третьої зміної</para>
		///</remarks>
		template<>
		__forceinline void Swap(Float & First, Float & Second) noexcept {
			static_assert(sizeof(Float) == sizeof(Int32), "Розміри Float і Int32 різні");
			union
			{
				Int32 * i;
				Float * f;
			} F1, F2;
			F1.f = &First;
			F2.f = &Second;
			*F1.i ^= *F2.i;
			*F2.i ^= *F1.i;
			*F1.i ^= *F2.i;
		}
		///<summary>Оптимізований обмін значеннями зміних Double </summary>
		///<param name='First'>Перше значення</param>
		///<param name='Second'>Друге значення</param>
		///<remarks>Оптимізований обмін відбувається використовуючи особливість побітового XOR та особливістю <c>union<c>
		///<para>Спочатку представляєм дійсні числа як цілі а тоді задопомогою XOR робим обмін без третьої зміної</para>
		///</remarks>
		template<>
		__forceinline void Swap(Double & First, Double & Second) noexcept {
			static_assert(sizeof(Double) == sizeof(Int64), "Розміри Float і Int32 різні");
			union
			{
				Int64 * i;
				Double * f;
			} F1, F2;
			F1.f = &First;
			F2.f = &Second;
			*F1.i ^= *F2.i;
			*F2.i ^= *F1.i;
			*F1.i ^= *F2.i;
		}
	}
}
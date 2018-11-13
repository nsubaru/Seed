#pragma once
#include "Seed.Core.Base.Types.h"

namespace Seed {
	inline namespace Core {
		///<summary>Математичні функції</summary>
		namespace Math {
			///<summary>Максимальні значення для інтегрованих типів</summary>
			namespace Max {
				///<summary>Максимальне значення Int8</summary>
				constexpr Seed::Int8 Int8 = 0x7Fi8;
				///<summary>Максимальне значення Int16</summary>
				constexpr Seed::Int16 Int16 = 0x7FFFi16;
				///<summary>Максимальне значення Int32</summary>
				constexpr Seed::Int32 Int32 = 0x7FFFFFFFi32;
				///<summary>Максимальне значення Int64</summary>
				constexpr Seed::Int64 Int64 = 0x7FFFFFFFFFFFFFFFi64;
				///<summary>Максимальне значення Float </summary>
				constexpr Seed::Float Float = 3.4E38f;
				///<summary>Максимальне значення Double</summary>
				constexpr Seed::Double Double = (Seed::Double)1.7E308;

				///<summary>Максимальне значення UInt8</summary>
				constexpr Seed::UInt8 UInt8 = 0xFFui8;
				///<summary>Максимальне значення UInt16</summary>
				constexpr Seed::UInt16 UInt16 = 0xFFFFui16;
				///<summary>Максимальне значення UInt32</summary>
				constexpr Seed::UInt32 UInt32 = 0xFFFFFFFFui32;
				///<summary>Максимальне значення UInt64</summary>
				constexpr Seed::UInt64 UInt64 = 0xFFFFFFFFFFFFFFFFui64;
				///<summary>Максимальне значення Byte</summary>
				constexpr Seed::Byte Byte = UInt8;
				///<summary>Максимальне значення SizeT</summary>
				constexpr Seed::SizeT SizeT = UInt64;
			}

			///<summary>Мінімальні значення для інтегрованих типів</summary>
			namespace Min {
				///<summary>Мінімальне значення Int8</summary>
				constexpr Seed::Int8 Int8 = 0x80i8;
				///<summary>Мінімальне значення Int16</summary>
				constexpr Seed::Int16 Int16 = 0x8000i16;
				///<summary>Мінімальне значення Int32</summary>
				constexpr Seed::Int32 Int32 = 0x80000000i32;
				///<summary>Мінімальне значення Int64</summary>
				constexpr Seed::Int64 Int64 = 0x8000000000000000i64;
				///<summary>Мінімальне значення Float </summary>
				constexpr Seed::Float Float = (Seed::Float)3.4E-38;
				///<summary>Мінімальне значення Double</summary>
				constexpr Seed::Double Double = 1.7E-308;

				///<summary>Мінімальне значення UInt8</summary>
				constexpr Seed::UInt8 UInt8 = 0x00ui8;
				///<summary>Мінімальне значення UInt16</summary>
				constexpr Seed::UInt16 UInt16 = 0x0000ui16;
				///<summary>Мінімальне значення UInt32</summary>
				constexpr Seed::UInt32 UInt32 = 0x00000000ui32;
				///<summary>Мінімальне значення UInt64</summary>
				constexpr Seed::UInt64 UInt64 = 0x0000000000000000ui64;
				///<summary>Мінімальне значення Byte</summary>
				constexpr Seed::Byte Byte = UInt8;
				///<summary>Мінімальне значення SizeT</summary>
				constexpr Seed::SizeT SizeT = UInt64;
			}

			///<summary>Бескінечність</summary>
			constexpr Double Infinity = 1e+300 * 1e+300;
			///<summary>Невизначене значення</summary>	
			constexpr Double NaN = Infinity * 0.0;

			///<summary>Compile-time функція знаходження модуля Int8</summary>
			///<param name="Number">Число</param>
			///<returns>Число без знаку</returns>
			constexpr __forceinline Seed::Int8 Abs(Seed::Int8 Number) { return Number * ((Number < 0) * (-1i8) + (Number > 0)); }
			static_assert((Int8)100 == Abs((Int8)-100) && (Int8)100 == Abs((Int8)100), "Модуль числа Int32 не коректно працює");
			///<summary>Compile-time функція знаходження модуля Int16</summary>
			///<param name="Number">Число</param>
			///<returns>Число без знаку</returns>
			constexpr __forceinline Seed::Int16 Abs(Seed::Int16 Number) { return Number * ((Number < 0) * (-1i16) + (Number > 0)); }
			static_assert((Int16)100 == Abs((Int16)-100) && (Int16)100 == Abs((Int16)100), "Модуль числа Int32 не коректно працює");
			///<summary>Compile-time функція знаходження модуля Int32</summary>
			///<param name="Number">Число</param>
			///<returns>Число без знаку</returns>
			constexpr __forceinline Seed::Int32 Abs(Seed::Int32 Number) { return Number * ((Number < 0) * (-1) + (Number > 0)); }
			static_assert(100 == Abs(-100) && 100 == Abs(100), "Модуль числа Int32 не коректно працює");
			///<summary>Compile-time функція знаходження модуля Int64</summary>
			///<param name="Number">Число</param>
			///<returns>Число без знаку</returns>
			constexpr __forceinline Seed::Int64 Abs(Seed::Int64 Number) { return Number * ((Number < 0) * (-1i64) + (Number > 0)); }
			static_assert(100L == Abs(-100L) && 100L == Abs(100L), "Модуль числа Int64 не коректно працює");
			///<summary>Знаходження модуля Float</summary>
			///<param name="Number">Число</param>
			///<returns>Число без знаку</returns>
			__forceinline Seed::Float Abs(Seed::Float Number) {
				union {
					Int32 * I;
					Float * D;
				} D;
				D.D = &Number;
				*D.I &= 1 << 31;
				return Number;
			}

			///<summary>Знаходження модуля Double</summary>
			///<param name="Number">Число</param>
			///<returns>Число без знаку</returns>
			__forceinline Seed::Double Abs(Seed::Double Number) {
				union {
					Int64 *  I;
					Double * D;
				} D;
				D.D = &Number;
				*D.I &= 1ULL << 63;
				return Number;
			}

			///<summary>Compile-time функція перевірки чи число Int8 менше нуля</summary>
			///<param name="Number">Число</param>
			///<returns>True якщо число менше нуля</returns>
			constexpr __forceinline Seed::Boolean Sign(Seed::Int8 Number) {
				return ((Number) & ((Seed::Int8)1 << 7));
			}
			static_assert(!Sign((Seed::Int8)100) == Sign((Seed::Int8) - 100), "Функція Sign(Int8) не коректно працює");
			///<summary>Compile-time функція перевірки чи число Int16 менше нуля</summary>
			///<param name="Number">Число</param>
			///<returns>True якщо число менше нуля</returns>
			constexpr __forceinline Seed::Boolean Sign(Seed::Int16 Number) {
				return ((Number) & ((Seed::Int16)1 << 15));
			}
			static_assert(!Sign((Seed::Int16)100) == Sign((Seed::Int16) - 100), "Функція Sign(Int16) не коректно працює");
			///<summary>Compile-time функція перевірки чи число Int32 менше нуля</summary>
			///<param name="Number">Число</param>
			///<returns>True якщо число менше нуля</returns>
			constexpr __forceinline Seed::Boolean Sign(Seed::Int32 Number) {
				return ((Number) & ((Seed::Int32)1 << 31));
			}
			static_assert(!Sign((Seed::Int32)100) == Sign((Seed::Int32) - 100), "Функція Sign(Int32) не коректно працює");
			///<summary>Compile-time функція перевірки чи число Int64 менше нуля</summary>
			///<param name="Number">Число</param>
			///<returns>True якщо число менше нуля</returns>
			constexpr __forceinline Seed::Boolean Sign(Seed::Int64 Number) {
				return ((Number) & ((Seed::Int64)1 << 63));
			}
			static_assert(!Sign((Seed::Int64)100) == Sign((Seed::Int64) - 100), "Функція Sign(Int8) не коректно працює");
			///<summary>Compile-time функція перевірки чи число Float менше нуля</summary>
			///<param name="Number">Число</param>
			///<returns>True якщо число менше нуля</returns>
			__forceinline Seed::Boolean Sign(Seed::Float val) {
				static_assert(sizeof(Float) == sizeof(Int32), "Розміри Float і Int32 різні");
				union
				{
					Int32 *  I;
					Float * D;
				} D;
				D.D = &val;
				return Sign(*D.I);
			}

			///<summary>Compile-time функція перевірки чи число Double менше нуля</summary>
			///<param name="Number">Число</param>
			///<returns>True якщо число менше нуля</returns>
			__forceinline Seed::Boolean Sign(Seed::Double val) {
				static_assert(sizeof(Double) == sizeof(Int64), "Розміри Float і Int32 різні");
				union
				{
					Int64 *  I;
					Double * D;
				} D;
				D.D = &val;
				return Sign(*D.I);
			}

			///<summary>Compile-time функція отримання цілої і дробової частини з Float</summary>
			///<param name="Value">Вхідне дійсне число типу Float</param>
			///<param name="IntPart">На виході ціла частина вхідного числа</param>
			///<results>Дробова частина вхідного числа</results>
			constexpr __forceinline Seed::Float ModD(Seed::Float Value, Seed::Float & IntPart) {
				IntPart = (Float)((Int64)Value);
				return Value - IntPart;
			}

			///<summary>Compile-time функція отримання цілої і дробової частини з Double</summary>
			///<param name="Value">Вхідне дійсне число типу Double</param>
			///<param name="IntPart">На виході ціла частина вхідного числа</param>
			///<results>Дробова частина вхідного числа</results>
			constexpr __forceinline Seed::Double ModD(Seed::Double Value, Seed::Double & IntPart) {
				IntPart = (Double)((Int64)Value);
				return Value - IntPart;
			}

			///<summary>Функція піднесення в степінь Y числа X для дійсних чисел Double</summary>
			///<param name="X">Число яке підноситься в степінь</param>
			///<param name="Y">Степінь в яку підноситься число X</param>
			///<results>Піднесене число</results>
			__forceinline Seed::Double Pow(const Double X, const Double Y) noexcept {
				//static_assert(false, "pow require");
				return NaN;
			}

			///<summary>Функція швидкого піднесення в степінь Y числа X для дійсних чисел UInt64</summary>
			///<param name="X">Число яке підноситься в степінь</param>
			///<param name="Y">Степінь в яку підноситься число X</param>
			///<results>Піднесене число</results>
			__forceinline constexpr Seed::UInt64 Pow(UInt64 X, UInt8 Y) noexcept {
				constexpr UInt8 highest_bit_set[] = {
					0, 1, 2, 2, 3, 3, 3, 3,
					4, 4, 4, 4, 4, 4, 4, 4,
					5, 5, 5, 5, 5, 5, 5, 5,
					5, 5, 5, 5, 5, 5, 5, 5,
					6, 6, 6, 6, 6, 6, 6, 6,
					6, 6, 6, 6, 6, 6, 6, 6,
					6, 6, 6, 6, 6, 6, 6, 6,
					6, 6, 6, 6, 6, 6, 6, 255,
					//все що має 1 в 63 біті при піднесені в степінь X > 0 гарантовано вийде за межі UInt64
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
					255, 255, 255, 255, 255, 255, 255, 255,
				};

				UInt64 result = 1;

				switch (highest_bit_set[Y]) {
				case 255: //ми можем використовувати 255 як маркер переповнення
					if (X == 1) {
						return 1;
					}

					if (X == -1) {
						return 1 - 2 * (Y & 1);
					}

					return 0;
				case 6:
					if (Y & 1) result *= X;
					Y >>= 1;
					X *= X;
				case 5:
					if (Y & 1) result *= X;
					Y >>= 1;
					X *= X;
				case 4:
					if (Y & 1) result *= X;
					Y >>= 1;
					X *= X;
				case 3:
					if (Y & 1) result *= X;
					Y >>= 1;
					X *= X;
				case 2:
					if (Y & 1) result *= X;
					Y >>= 1;
					X *= X;
				case 1:
					if (Y & 1) result *= X;
				default:
					return result;
				}
			}

			///<summary>Заокруглення до верхнього значення при діленні</summary>
			///<param name='What'>Вхідне число</param>
			///<param name='By'>Дільник</param>
			///<results>Ділене заокреглене до верхньої сторони</results>
			///<remarks>Така функція потрібна при очислені кількості кластерів які будуть зайнняті файлом чи даними, 
			///<para>наприклад: розмір кластер 512 байт, розмір даних 1000 байт, кластер може бути або зайннятим або ні,</para>
			///<para>якщо дані не повністю займають кластер все одно вважається що він зайннятий, </para>
			///<para>відповідно 1000 / 512 буде 1 але ж зрозуміло що другий кластер теж буде зайннятий </para>
			///<para>тому і треба заокруглити до більшого тобто до 2</para>
			///</remarks>
			__forceinline constexpr SizeT RoundToUpper(SizeT What, SizeT By) {
				return (What / By) * By != What
					?
					(What / By) + 1
					:
					What / By;
			}
		}
	}
}
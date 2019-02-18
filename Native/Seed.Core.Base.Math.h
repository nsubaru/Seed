#pragma once
#include "Seed.Core.Base.Types.h"

namespace Seed {
	inline namespace Core {
		///<summary>Математичні функції</summary>
		namespace Math {
			template<typename T>
			constexpr T Max() {};

			///<summary>Максимальне значення Int8</summary>
			template<>
			constexpr Int8 Max() { return 0x7Fi8; };
			///<summary>Максимальне значення Int16</summary>
			template<>
			constexpr Int16 Max() { return 0x7FFFi16; };
			///<summary>Максимальне значення Int32</summary>
			template<>
			constexpr Int32 Max() { return 0x7FFFFFFFi32; };
			///<summary>Максимальне значення Int64</summary>
			template<>
			constexpr Int64 Max() { return 0x7FFFFFFFFFFFFFFFi64; };
			///<summary>Максимальне значення Float </summary>
			template<>
			constexpr Float Max() { return 3.4E38f; };
			///<summary>Максимальне значення Double</summary>
			template<>
			constexpr Double Max() { return (Seed::Double)1.7E308; };
			///<summary>Максимальне значення UInt8</summary>
			template<>
			constexpr UInt8 Max() { return 0xFFui8; };
			///<summary>Максимальне значення UInt16</summary>
			template<>
			constexpr UInt16 Max() { return 0xFFFFui16; };
			///<summary>Максимальне значення UInt32</summary>
			template<>
			constexpr UInt32 Max() { return 0xFFFFFFFFui32; };
			///<summary>Максимальне значення UInt64</summary>
			template<>
			constexpr UInt64 Max() { return 0xFFFFFFFFFFFFFFFFui64; };

			template<typename T>
			constexpr T Min() {};

			///<summary>Мінімальне значення Int8</summary>
			template<>
			constexpr Int8 Min() { return 0x80i8; };
			///<summary>Мінімальне значення Int16</summary>
			template<>
			constexpr Int16 Min() { return 0x8000i16; };
			///<summary>Мінімальне значення Int32</summary>
			template<>
			constexpr Int32 Min() { return 0x80000000i32; };
			///<summary>Мінімальне значення Int64</summary>
			template<>
			constexpr Int64 Min() { return 0x8000000000000000i64; };
			///<summary>Мінімальне значення Float </summary>
			template<>
			constexpr Float Min() { return (Seed::Float)3.4E-38; };
			///<summary>Мінімальне значення Double</summary>
			template<>
			constexpr Double Min() { return 1.7E-308; };
			///<summary>Мінімальне значення UInt8</summary>
			template<>
			constexpr UInt8 Min() { return 0x00ui8; };
			///<summary>Мінімальне значення UInt16</summary>
			template<>
			constexpr UInt16 Min() { return 0x0000ui16; };
			///<summary>Мінімальне значення UInt32</summary>
			template<>
			constexpr UInt32 Min() { return 0x00000000ui32; };
			///<summary>Мінімальне значення UInt64</summary>
			template<>
			constexpr UInt64 Min() { return 0x0000000000000000ui64; };

			///<summary>Бескінечність</summary>
			constexpr Double Infinity = 1e+300 * 1e+300;
			///<summary>Невизначене значення</summary>
			constexpr Double NaN = Infinity * 0.0;

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
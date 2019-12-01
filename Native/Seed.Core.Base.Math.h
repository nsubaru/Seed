#pragma once
#include "Seed.Core.Base.Types.h"

namespace Seed {
	inline namespace Core {
		///<summary>Математичні функції</summary>
		namespace Math {
			///<summary>Максимальне значення типу</summary>
			template<typename T>
			constexpr T Max() {
				//static_assert(false, "Invalid type");
				throw 0;
			};

			///<summary>Максимальне значення Int8</summary>
			template<>
			constexpr Int8 Max() { return 0x7Fi8; }; //-V2517
			///<summary>Максимальне значення Int16</summary>
			template<>
			constexpr Int16 Max() { return 0x7FFFi16; }; //-V2517
			///<summary>Максимальне значення Int32</summary>
			template<>
			constexpr Int32 Max() { return 0x7FFFFFFFi32; }; //-V2517
			///<summary>Максимальне значення Int64</summary>
			template<>
			constexpr Int64 Max() { return 0x7FFFFFFFFFFFFFFFi64; }; //-V2517
			///<summary>Максимальне значення Float </summary>
			template<>
			constexpr Float Max() { return 3.4E38f; }; //-V2517
			///<summary>Максимальне значення Double</summary>
			template<>
			constexpr Double Max() { return static_cast<Seed::Double>(1.7E308); }; //-V2533
			///<summary>Максимальне значення UInt8</summary>
			template<>
			constexpr UInt8 Max() { return 0xFFui8; }; //-V2517
			///<summary>Максимальне значення UInt16</summary>
			template<>
			constexpr UInt16 Max() { return 0xFFFFui16; }; //-V2517
			///<summary>Максимальне значення UInt32</summary>
			template<>
			constexpr UInt32 Max() { return 0xFFFFFFFFui32; }; //-V2517
			///<summary>Максимальне значення UInt64</summary>
			template<>
			constexpr UInt64 Max() { return 0xFFFFFFFFFFFFFFFFui64; }; //-V2517

			///<summary>Мінімальне значення типу</summary>
			template<typename T>
			constexpr T Min()
			{
				// ReSharper disable CppStaticAssertFailure
				static_assert(false, "Invalid type");
				// ReSharper restore CppStaticAssertFailure
				return 0;
			};

			///<summary>Мінімальне значення Int8</summary>
			template<>
			constexpr Int8 Min() { return 0x80i8; }; //-V2517
			///<summary>Мінімальне значення Int16</summary>
			template<>
			constexpr Int16 Min() { return 0x8000i16; }; //-V2517
			///<summary>Мінімальне значення Int32</summary>
			template<>
			constexpr Int32 Min() { return 0x80000000i32; }; //-V2517
			///<summary>Мінімальне значення Int64</summary>
			template<>
			constexpr Int64 Min() { return 0x8000000000000000i64; }; //-V2517
			///<summary>Мінімальне значення Float </summary>
			template<>
			constexpr Float Min() { return static_cast<Seed::Float>(3.4E-38); }; //-V2533
			///<summary>Мінімальне значення Double</summary>
			template<>
			constexpr Double Min() { return 1.7E-308; };
			///<summary>Мінімальне значення UInt8</summary>
			template<>
			constexpr UInt8 Min() { return 0x00ui8; }; //-V2517
			///<summary>Мінімальне значення UInt16</summary>
			template<>
			constexpr UInt16 Min() { return 0x0000ui16; }; //-V2517
			///<summary>Мінімальне значення UInt32</summary>
			template<>
			constexpr UInt32 Min() { return 0x00000000ui32; }; //-V2517
			///<summary>Мінімальне значення UInt64</summary>
			template<>
			constexpr UInt64 Min() { return 0x0000000000000000ui64; }; //-V2517

			/// <summary>
			/// Максимальна кількість цифр в числовому типі
			/// </summary>
			/// <typeparam name="T">Тип числа</typeparam>
			template<typename T>
			constexpr UInt8 MaxNumberCount() {
				//Отримуємо максимальне можливе значення для даного числа
				T value = Max<T>();
				//Оголошуємо зміну результату
				UInt8 result = 0;
				//В циклі зменшуємо кількість десяткових розрядів діленням і підраховуємо кількість розрядів поки є розряди
				do {
					value /= 10;
					result++;
				} while (value > 0);
				//Вертаємо розряди
				return result;
			}

			///<summary>Машинний епсилон(ноль) типу</summary>

			/// <summary>
			/// Машинний епсилон(ноль) типу
			/// </summary>
			/// <typeparam name="T">Дійсний тип</typeparam>
			template<typename T>
			constexpr T Epsilon()
			{
				// ReSharper disable once CppStaticAssertFailure
				static_assert(false, "Invalid type");
				return 0;
			};
			///<summary>Машинний епсилон(ноль) для Float</summary>
			template<>
			constexpr Float Epsilon()
			{
				return 1.192092896e-07F;
			};
			///<summary>Машинний епсилон(ноль) для Double</summary>
			template<>
			constexpr Double Epsilon()
			{
				return 2.2204460492503131e-016;
			};

			///<value>Безкінечність</value>
			constexpr Double Infinity = 1e+300 * 1e+300;
			///<summary>Невизначене значення</summary>
			constexpr Double NaN = Infinity * 0.0;
			
			/// <summary>
			/// Заокруглення до верхнього значення при діленні
			/// </summary>
			/// <param name="What">Вхідне число</param>
			/// <param name="By">Дільник</param>
			/// <returns>Ділене заокруглення до верхньої сторони</returns>
			/// <remarks>
			/// Така функція потрібна при обчислені кількості кластерів які будуть зайняті файлом чи даними,
			/// наприклад: розмір кластер 512 байт, розмір даних 1000 байт, кластер може бути або зайнятим або ні,
			/// якщо дані не повністю займають кластер все одно вважається що він зайнятий,
			/// відповідно 1000 / 512 буде 1 але ж зрозуміло що другий кластер теж буде зайнятий
			/// тому і треба заокруглити до більшого тобто до 2
			/// </remarks>
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
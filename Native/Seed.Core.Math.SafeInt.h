#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.ConstString.h"

namespace Seed {
	inline namespace Core {
		inline namespace Exceptions {
			class IntegerOverflowException;
			class DivideByZero;
			class InternalError;
		}

		namespace Math {
#pragma region SafeInt
			///<summary>Класс чисел з безпечними операціями</summary>
			///<param name = "T">Тип який буде базою, може бути лиш unsigned int будь-яклї розрядності</param>
			template<typename T>
			class SeedKernelCall SafeInt final {
			private:
				///Тип числа
				using Int = TypeTraits::Unsigned<T>;
				///<summary>Значення</summary>
				Int value;
			public:
				///<summary>Стандартний конструктор</summary>
				constexpr SafeInt() : value(0) {}

				///<summary>Конструктор який приймає значення</summary>
				constexpr SafeInt(Int value) : value(value) {}

				///<summary>Додавання двох чисел</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
				constexpr __forceinline SafeInt operator+(SafeInt second) {
					if (static_cast<Int>(this->value + second.value) >= this->value)
						return static_cast<Int>(this->value + second.value);
					else
						throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
				}

				///<summary>Віднімання двох чисел</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
				constexpr __forceinline SafeInt operator-(SafeInt second) {
					if(this->value >= second.value)
						return static_cast<Int>(this->value - second.value);
					else
						throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
				}

#pragma warning(push)
#pragma warning(disable : 4164)
				///<summary>Множення двох чисел</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
#pragma intrinsic(_umul128)
				constexpr __forceinline SafeInt operator*(SafeInt second) {
					if constexpr (TypeTraits::IsSameV<Int, UInt8>) {
						UInt16 res = this->value * second.value;
						if (res > Math::Max<Int>())
							throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return static_cast<Int>(res);
					} else
					if constexpr (TypeTraits::IsSameV<Int, UInt16>) {
						UInt32 res = this->value * second.value;
						if (res > Math::Max<Int>())
							throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return static_cast<Int>(res);
					} else
					if constexpr (TypeTraits::IsSameV<Int, UInt32>) {
						UInt64 res = this->value * second.value;
						if (res > Math::Max<Int>())
							throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return static_cast<Int>(res);
					} else
					if constexpr (TypeTraits::IsSameV<Int, UInt64>) {
						UInt64 res[2];
						_umul128(this->value, second.value, res);
						UInt32 res = this->value * second.value;
						if (res[1] > 0)
							throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return res;
					}
					//Unreacheable code
					throw InternalError(_FILE_, __FUNCSIG__, __LINE__);
				}
#pragma warning(pop)

				///<summary>Ділення двох чисел</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
				constexpr __forceinline SafeInt operator/(SafeInt second) {
					if (second.value == 0)
						throw DivideByZero(__FILE__, __FUNCSIG__, __LINE__);
					else
						return this->value / second.value;
				}

				///<summary>Додавання до даного числа</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
				constexpr __forceinline SafeInt operator+=(SafeInt second) {
					if (static_cast<Int>(this->value + second.value) >= this->value)
						return this->value = static_cast<Int>(this->value + second.value);
					else
						throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
				}

				///<summary>Віднімання від поточного числа</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
				constexpr __forceinline SafeInt operator-=(SafeInt second) {
					if (this->value >= second.value)
						return this->value = static_cast<Int>(this->value - second.value);
					else
						throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
				}

#pragma warning(push)
#pragma warning(disable : 4164)
				//We check overflow so this warning not needed
#pragma warning(disable : 26451)
				///<summary>Домножування до поточного значення</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
				constexpr __forceinline SafeInt operator*=(SafeInt second) {
					if constexpr (TypeTraits::IsSameV<Int, UInt8>) {
						UInt16 res = this->value * second.value;
						if (res > Math::Max<Int>())
							throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return this->value = static_cast<Int>(res);
					} else
					if constexpr (TypeTraits::IsSameV<Int, UInt16>) {
						UInt32 res = this->value * second.value;
						if (res > Math::Max<Int>())
							throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return this->value = static_cast<Int>(res);
					} else
					if constexpr (TypeTraits::IsSameV<Int, UInt32>) {
						UInt64 res = static_cast<UInt64>(this->value) * static_cast<UInt64>(second.value);
						if (res > Math::Max<Int>())
							throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return this->value = static_cast<Int>(res);
					} else
					if constexpr (TypeTraits::IsSameV<Int, UInt64>) {
						UInt64 res;
						res = this->value * second.value;
						if (__umulh(this->value, second.value) > 0)
							throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return this->value = res;
					}
					//Unreacheable code
					throw InternalError(__FILE__, __FUNCSIG__, __LINE__);
				}
#pragma warning(pop)

				constexpr __forceinline bool operator<(SafeInt second) {
					return this->value < second.value;
				}

				constexpr __forceinline bool operator>(SafeInt second) {
					return this->value > second.value;
				}
				constexpr __forceinline bool operator<=(SafeInt second) {
					return this->value <= second.value;
				}
				constexpr __forceinline bool operator>=(SafeInt second) {
					return this->value >= second.value;
				}
				constexpr __forceinline bool operator==(SafeInt second) {
					return this->value == second.value;
				}
				constexpr __forceinline bool operator!=(SafeInt second) {
					return this->value != second.value;
				}

				constexpr __forceinline Int GetValue() { return value; }
				Int Property(GetValue) Value;
			};
			
			///<summary>Безпечний UInt8</summary>
			namespace { using SafeInt8 = SafeInt<UInt8>; }
			///<summary>Безпечний UInt16</summary>
			namespace { using SafeInt16 = SafeInt<UInt16>; }
			///<summary>Безпечний UInt32</summary>
			namespace { using SafeInt32 = SafeInt<UInt32>; }
			///<summary>Безпечний UInt64</summary>
			namespace { using SafeInt64 = SafeInt<UInt64>; }
			
#pragma endregion

#pragma region ConvertUnsignedToSigned
			template<typename T>
			TypeTraits::Signed<T> ConvertUnsignedToSigned(TypeTraits::Unsigned<T> From) {
				if (From > static_cast<TypeTraits::Unsigned<T>>(Max<TypeTraits::Signed<T>>()))
					throw IntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
				return static_cast<TypeTraits::Signed<T>>(From);
			}
#pragma endregion
		}
	}
}
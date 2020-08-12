#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.ConstString.h"

namespace Seed {
	inline namespace Core {
		namespace Math
		{
#pragma region SafeInt
			namespace Internal
			{
				class SafeIntBase {
				protected:
					inline SafeIntBase() = default;
					inline SafeIntBase(const SafeIntBase&) = default;
					inline SafeIntBase(SafeIntBase&&) = default;

					static unsigned __int64 _umul128Impl(
					   unsigned __int64 Multiplier,
					   unsigned __int64 Multiplicand,
					   unsigned __int64 *HighProduct
					);

					~SafeIntBase() {}
				};
			}

			///<summary>Класс чисел з безпечними операціями</summary>
			///<param name = "T">Тип який буде базою, може бути лиш unsigned int будь-яклї розрядності</param>
			template<typename T> requires TypeTraits::IsSigned<T> || TypeTraits::IsUnsigned<T>
			class SafeInt final : Internal::SafeIntBase {
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
				constexpr ForceInline SafeInt operator+(SafeInt second) {
					if (static_cast<Int>(this->value + second.value) >= this->value)
						return static_cast<Int>(this->value + second.value);
					else
						Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
				}

				///<summary>Віднімання двох чисел</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
				constexpr ForceInline SafeInt operator-(SafeInt second) {
					if (this->value >= second.value)
						return static_cast<Int>(this->value - second.value);
					else
						Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
				}

#pragma warning(push)
#pragma warning(disable : 4164)
				///<summary>Множення двох чисел</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
#pragma intrinsic(_umul128)
				constexpr ForceInline SafeInt operator*(SafeInt second) {
					if constexpr (TypeTraits::IsSameV<Int, UInt8>)
					{
						UInt16 res = static_cast<UInt16>(this->value) * second.value;
						if (res > Math::Max<Int>())
							Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return static_cast<Int>(res);
					}
					else if constexpr (TypeTraits::IsSameV<Int, UInt16>)
					{
						UInt32 res = static_cast<UInt32>(this->value) * second.value;
						if (res > Math::Max<Int>())
							Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return static_cast<Int>(res);
					}
					else if constexpr (TypeTraits::IsSameV<Int, UInt32>)
					{
						UInt64 res = static_cast<UInt64>(this->value) * second.value;
						if (res > Math::Max<Int>())
							Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return static_cast<Int>(res);
					}
					else if constexpr (TypeTraits::IsSameV<Int, UInt64>)
					{
						UInt64 res[2];
						res[0] = _umul128Impl(this->value, second.value, &res[1]);
						UInt32 res = this->value * second.value;
						if (res[1] > 0)
							Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return res;
					}
					//Unreacheable code
					Seed::Core::Internals::Exceptions::ThrowInternalError(__FILE__, __FUNCSIG__, __LINE__);
				}
#pragma warning(pop)

				///<summary>Ділення двох чисел</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
				constexpr ForceInline SafeInt operator/(SafeInt second) {
					if (second.value == 0)
						Seed::Core::Internals::Exceptions::ThrowDivideByZero(__FILE__, __FUNCSIG__, __LINE__);
					else
						return this->value / second.value;
				}

				///<summary>Додавання до даного числа</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
				constexpr ForceInline SafeInt operator+=(SafeInt second) {
					if (static_cast<Int>(this->value + second.value) >= this->value)
						return this->value = static_cast<Int>(this->value + second.value);
					else
						Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
				}

				///<summary>Віднімання від поточного числа</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
				constexpr ForceInline SafeInt operator-=(SafeInt second) {
					if (this->value >= second.value)
						return this->value = static_cast<Int>(this->value - second.value);
					else
						Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
				}

#pragma warning(push)
#pragma warning(disable : 4164)
				//We check overflow so this warning not needed
#pragma warning(disable : 26451)
				///<summary>Домножування до поточного значення</summary>
				///<param name = "second">Лівий операнд</param>
				///<exception cref="IntegerOverflowException">Якщо відбулось переповнення</exception>
				///<returns>Результат операції</returns>
				constexpr ForceInline SafeInt operator*=(SafeInt second) {
					if constexpr (TypeTraits::IsSameV<Int, UInt8>)
					{
						UInt16 res = this->value * second.value;
						if (res > Math::Max<Int>())
							Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return this->value = static_cast<Int>(res);
					}
					else if constexpr (TypeTraits::IsSameV<Int, UInt16>)
					{
						UInt32 res = this->value * second.value;
						if (res > Math::Max<Int>())
							Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return this->value = static_cast<Int>(res);
					}
					else if constexpr (TypeTraits::IsSameV<Int, UInt32>)
					{
						UInt64 res = static_cast<UInt64>(this->value) * static_cast<UInt64>(second.value);
						if (res > Math::Max<Int>())
							Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return this->value = static_cast<Int>(res);
					}
					else if constexpr (TypeTraits::IsSameV<Int, UInt64>)
					{
						UInt64 res[2];
						_umul128Impl(this->value, second.value, res);
						if (res[1] > 0)
							Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
						else
							return this->value = res[0];
					}
					//Unreacheable code
					Seed::Core::Internals::Exceptions::ThrowInternalError(__FILE__, __FUNCSIG__, __LINE__);
				}
#pragma warning(pop)
				/// <summary>
				/// Оператор універсального порівняння
				/// </summary>
				/// <param name="second">Друге число</param>
				/// <returns>Результат порівняння</returns>
				constexpr ForceInline auto operator<=>(SafeInt second) {
					return this->value <=> second.value;
				}
				/// <summary>
				/// Вертає значення числа
				/// </summary>
				/// <returns>Значення числа</returns>
				constexpr ForceInline Int GetValue() { return value; }
				/// <summary>
				/// Значення числа
				/// </summary>
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
			/// <summary>
			/// Перетворює в знаковий тип числа з перевіркою на переповнення
			/// </summary>
			template<typename T>
			TypeTraits::Signed<T> ConvertUnsignedToSigned(TypeTraits::Unsigned<T> From) {
				if (From > static_cast<TypeTraits::Unsigned<T>>(Max<TypeTraits::Signed<T>>()))
					Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
				return static_cast<TypeTraits::Signed<T>>(From);
			}
#pragma endregion
		}
	}
}
#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Math.Internal.h"
#include "Seed.Core.Math.SafeInt.h"

namespace Seed {
	inline namespace Core {
		namespace Math {
			/// <summary>
			/// Значення константи PI
			/// </summary>
			inline constexpr Double Pi = Internal::Pi();
			/// <summary>
			/// Значення константи E
			/// </summary>
			inline constexpr Double E = Internal::E();
#pragma region Abs
			/// <summary>
			/// Обчислює модуль числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>Модуль числа</returns>
			ForceInline constexpr Int8 Abs(Int8 Val)
			{
				return Internal::ConstexprAbs(Val);
			}
			/// <summary>
			/// Обчислює модуль числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>Модуль числа</returns>
			ForceInline constexpr Int16 Abs(Int16 Val)
			{
				return Internal::ConstexprAbs(Val);
			}
			/// <summary>
			/// Обчислює модуль числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>Модуль числа</returns>
			ForceInline constexpr Int32 Abs(Int32 Val)
			{
				return Internal::ConstexprAbs(Val);
			}
			/// <summary>
			/// Обчислює модуль числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>Модуль числа</returns>
			ForceInline constexpr Int64 Abs(Int64 Val)
			{
				return Internal::ConstexprAbs(Val);
			}
			/// <summary>
			/// Обчислює модуль числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>Модуль числа</returns>
			ForceInline constexpr Float Abs(Float Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprAbs(Val)))
					return Internal::ConstexprAbs(Val);
				else
					return Internal::RuntimeAbs(Val);
			}
			/// <summary>
			/// Обчислює модуль числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>Модуль числа</returns>
			ForceInline constexpr Double Abs(Double Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprAbs(Val)))
					return Internal::ConstexprAbs(Val);
				else
					return Internal::RuntimeAbs(Val);
			}
#pragma endregion
#pragma region Pow
			/// <summary>
			/// Приведення числа до степені x^y
			/// </summary>
			/// <param name="X">Число яке приводиться до степені</param>
			/// <param name="Y">Число степеней</param>
			/// <returns>Результат</returns>
			ForceInline constexpr UInt64 Pow(UInt64 X, UInt8 Y)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprPow(X, Y)))
					return Internal::ConstexprPow(X, Y);
				else
					return Internal::RuntimePow(X, Y);
			}
			/// <summary>
			/// Приведення числа до степені x^y
			/// </summary>
			/// <param name="X">Число яке приводиться до степені</param>
			/// <param name="Y">Число степеней</param>
			/// <returns>Результат</returns>
			ForceInline constexpr Double Pow(Double X, Int32 Y)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprPow(X, Y)))
					return Internal::ConstexprPow(X, Y);
				else
					return Internal::RuntimePow(X, Y);
			}
			/// <summary>
			/// Приведення числа до степені x^y
			/// </summary>
			/// <param name="X">Число яке приводиться до степені</param>
			/// <param name="Y">Число степеней</param>
			/// <returns>Результат</returns>
			ForceInline constexpr Double Pow(Double X, Double Y)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprPow(X, Y)))
					return Internal::ConstexprPow(X, Y);
				else
					return Internal::RuntimePow(X, Y);
			}
#pragma endregion
#pragma region Log
			/// <summary>
			/// Обчислює натуральний логарифм з числа
			/// </summary>
			/// <param name="X">Вхідне значення</param>
			/// <returns>Натуральний логарифм</returns>
			ForceInline constexpr Double Ln(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprLn(X)))
					return Internal::ConstexprLn(X);
				else
					return Internal::RuntimeLn(X);
			}
			/// <summary>
			/// Обчислює натуральний логарифм з числа
			/// </summary>
			/// <param name="X">Вхідне значення</param>
			/// <returns>Натуральний логарифм</returns>
			ForceInline constexpr Double Lg(Double X)
			{
				return Ln(X);
			}
			/// <summary>
			/// Обчислює логарифм за основою 2 з числа
			/// </summary>
			/// <param name="X">Вхідне значення</param>
			/// <returns>Логарифм за основою 2</returns>
			ForceInline constexpr Double Log2(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprLog2(X)))
					return Internal::ConstexprLog2(X);
				else
					return Internal::RuntimeLog2(X);
			}
#pragma endregion
#pragma region Sqrt
			/// <summary>
			/// Обчислює корінь квадратний з числа
			/// </summary>
			/// <param name="X">Вхідне значення</param>
			/// <returns>Корінь квадратний з числа</returns>
			ForceInline constexpr Double Sqrt(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSqrt(X)))
					return Internal::ConstexprSqrt(X);
				else
					return Internal::RuntimeSqrt(X);
			}
#pragma endregion
#pragma region Sin
			/// <summary>
			/// Обчислює синус з числа
			/// </summary>
			/// <param name="X">Значення в радіанах</param>
			/// <returns>Синус числа</returns>
			ForceInline constexpr Double Sin(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSin(X)))
					return Internal::ConstexprSin(X);
				else
					return Internal::RuntimeSin(X);
			}
#pragma endregion
#pragma region Cos
			/// <summary>
			/// Обчислює косинус з числа
			/// </summary>
			/// <param name="X">Значення в радіанах</param>
			/// <returns>Косинус числа</returns>
			ForceInline constexpr Double Cos(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprCos(X)))
					return Internal::ConstexprCos(X);
				else
					return Internal::RuntimeCos(X);
			}
#pragma endregion
#pragma region Tan
			/// <summary>
			/// Обчислює тангенс з числа
			/// </summary>
			/// <param name="X">Значення в радіанах</param>
			/// <returns>Тангенс числа</returns>
			ForceInline constexpr Double Tan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprTan(X)))
					return Internal::ConstexprTan(X);
				else
					return Internal::RuntimeTan(X);
			}
#pragma endregion
#pragma region CTan
			/// <summary>
			/// Обчислює котангенс з числа
			/// </summary>
			/// <param name="X">Значення в радіанах</param>
			/// <returns>Котангенс числа</returns>
			ForceInline constexpr Double CTan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprCtan(X)))
					return Internal::ConstexprCtan(X);
				else
					return Internal::RuntimeCtan(X);
			}
#pragma endregion
#pragma region ASin
			/// <summary>
			/// Обчислює кут в радіанах за синусом кута
			/// </summary>
			/// <param name="X">Синус кута</param>
			/// <returns>Кут в радіанах</returns>
			ForceInline constexpr Double ASin(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprASin(X)))
					return Internal::ConstexprASin(X);
				else
					return Internal::RuntimeASin(X);
			}
#pragma endregion
#pragma region ACos
			/// <summary>
			/// Обчислює кут в радіанах за косинусом кута
			/// </summary>
			/// <param name="X">Косинус кута</param>
			/// <returns>Кут в радіанах</returns>
			ForceInline constexpr Double ACos(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprACos(X)))
					return Internal::ConstexprACos(X);
				else
					return Internal::RuntimeACos(X);
			}
#pragma endregion
#pragma region ATan
			/// <summary>
			/// Обчислює кут в радіанах за тангенсом кута
			/// </summary>
			/// <param name="X">Тангенс кута</param>
			/// <returns>Кут в радіанах</returns>
			ForceInline constexpr Double ATan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprATan(X)))
					return Internal::ConstexprATan(X);
				else
					return Internal::RuntimeATan(X);
			}
#pragma endregion
#pragma region ACtan
			/// <summary>
			/// Обчислює кут в радіанах за котангенсом кута
			/// </summary>
			/// <param name="X">Котангенс кута</param>
			/// <returns>Кут в радіанах</returns>
			ForceInline constexpr Double ACtan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprACtan(X)))
					return Internal::ConstexprACtan(X);
				else
					return Internal::RuntimeACtan(X);
			}
#pragma endregion
#pragma region Sign
			/// <summary>
			/// Визначає знак числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>true якщо додатнє</returns>
			ForceInline constexpr Boolean Sign(Int8 Val)
			{
				return ((Val) & ((Seed::Int8)1 << 7)); //-V2533
			}
			/// <summary>
			/// Визначає знак числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>true якщо додатнє</returns>
			ForceInline constexpr Boolean Sign(Int16 Val)
			{
				return ((Val) & ((Seed::Int16)1 << 15)); //-V2533
			}
			/// <summary>
			/// Визначає знак числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>true якщо додатнє</returns>
			ForceInline constexpr Boolean Sign(Int32 Val)
			{
				return ((Val) & ((Seed::Int32)1 << 31)); //-V2533
			}
			/// <summary>
			/// Визначає знак числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>true якщо додатнє</returns>
			ForceInline constexpr Boolean Sign(Int64 Val)
			{
				return ((Val) & ((Seed::Int64)1 << 63)); //-V2533
			}
			/// <summary>
			/// Визначає знак числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>true якщо додатнє</returns>
			ForceInline constexpr Boolean Sign(Float Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSign(Val)))
					return Internal::ConstexprSign(Val);
				else
					return Internal::RuntimeAbs(Val);
			}
			/// <summary>
			/// Визначає знак числа
			/// </summary>
			/// <param name="Val">Вхідне число</param>
			/// <returns>true якщо додатнє</returns>
			ForceInline constexpr Boolean Sign(Double Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSign(Val)))
					return Internal::ConstexprSign(Val);
				else
					return Internal::RuntimeAbs(Val);
			}
			/// <summary>
			/// Вертає число за позицією в десятковому вигляді
			/// </summary>
			/// <param name="Number">Вхідне число</param>
			/// <param name="Pos">Позиція від нуля з права на ліво</param>
			/// <returns>Значення за відповідною позицією</returns>
			ForceInline constexpr UInt8 GetNumberByPos(const UInt64 Number, const UInt8 Pos)
			{
				return static_cast<UInt8>(
					Number % Pow(10ULL, static_cast<UInt8>(Pos + 1ui8)) / Pow(10ULL, static_cast<UInt8>(Pos + 1ui8)) //-V747 //-V2517
					);
			}
#pragma endregion
			/// <summary>
			/// Експонента з числа: e^x
			/// </summary>
			/// <param name="X">Вхідне число</param>
			/// <returns>Результат функції</returns>
			ForceInline constexpr Double Exp(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprExp(X)))
					return Internal::ConstexprExp(X);
				else
					return Internal::RuntimeExp(X);
			}
			/// <summary>
			/// Розділяє число на цілу і дійсну частину
			/// </summary>
			/// <param name="X">Вхідне число</param>
			/// <param name="Y">Вихідна ціла частина числа</param>
			/// <returns>Дійсна частина числа</returns>
			ForceInline Double ModD(Double X, Double& Y) {
				return Internal::ModD(X, Y);
			}
			/// <summary>
			/// Розділяє число на цілу і дійсну частину
			/// </summary>
			/// <param name="X">Вхідне число</param>
			/// <param name="Y">Вихідна ціла частина числа</param>
			/// <returns>Дійсна частина числа</returns>
			ForceInline Float ModD(Float X, Float& Y) {
				return Internal::ModD(X, Y);
			}
			/// <summary>
			/// Порівняння дійсних чисел з урахування погрішності
			/// </summary>
			/// <param name="first">Перше число</param>
			/// <param name="second">Друге число</param>
			/// <returns>true якщо числа рівні з урахування погрішності</returns>
			ForceInline constexpr bool Equals(Float first, Float second){
				return Abs(first - second) <= Epsilon<Double>();
			}
			/// <summary>
			/// Порівняння дійсних чисел з урахування погрішності
			/// </summary>
			/// <param name="first">Перше число</param>
			/// <param name="second">Друге число</param>
			/// <returns>true якщо числа рівні з урахування погрішності</returns>
			ForceInline constexpr bool Equals(Double first, Double second){
				return Abs(first - second) <= Epsilon<Double>();
			}
		}
	}
}
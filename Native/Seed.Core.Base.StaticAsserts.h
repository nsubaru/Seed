#pragma once
#include "Seed.Core.Base.Types.h"

namespace Seed {
	inline namespace Core {
		///<summary>Простір імен статичних провірок</summary>
		namespace StaticAsserts {
			///<summary>Шаблон логічної константи, використовується для логічних суджень в compile-time</summary>
			template<Boolean value>
			struct LogicConst {
				__forceinline constexpr operator Boolean() { return value; }
				static constexpr Boolean State = value;
			};
			///<summary>Логічна істина для статичних провірок</summary>
			inline namespace { typedef LogicConst<True> LogicTrue; }
			///<summary>Логічна хиба для статичних провірок</summary>
			inline namespace { typedef LogicConst<False> LogicFalse; }

			///<summary>Логічне суддження чи тип T1 == T2 виконується в compile-time</summary>
			template<typename T1, typename T2>
			struct IsSame :public LogicFalse {};

			///<summary>Логічне суддження чи тип T1 == T2 виконується в compile-time</summary>
			template<typename T>
			struct IsSame<T, T> :public LogicTrue {};

			///<summary>Compile-time логічне суддження, якщо Cond == True, вертає тип IfTrue, інакше IfFalse</summary>
			template<Boolean Cond, class IfTrue, class IfFalse>
			struct ConditionT { typedef IfTrue Result; };

			///<summary>Compile-time логічне суддження, якщо Cond == True, вертає тип IfTrue, інакше IfFalse</summary>
			template<class IfTrue, class IfFalse>
			struct ConditionT<False, IfTrue, IfFalse> { typedef IfFalse Result; };


			/////<summary>Compile-time логічне суддження, якщо T є об'єднанням то вертає True інакше False</summary>
			/////<remarks>Спирається на реалізацію в компіляторі</remarks>
			//template<typename T>
			//struct IsUnion :LogicConst<CRT::is_union<T>::value> {}; // Implementatiom defined

			/////<summary>Compile-time логічне суддження, якщо T є классом то вертає True інакше False</summary>
			/////<remarks>Спирається на реалізацію в компіляторі</remarks>
			//template<typename T>
			//struct IsClass :LogicConst<CRT::is_class<T>::value> {}; //Implementation defined


			/////<summary>Compile-time логічне суддження, 
			/////<para>якщо клас Derivde наслідує клас Base то вертає True інакше False</para></summary>
			/////<remarks>Спирається на реалізацію в компіляторі</remarks>
			//template<typename Base, typename Derived>
			//struct IsBasedOn :LogicConst<CRT::is_base_of<Base, Derived>::value> {}; //Implementation defined


			/////<summary>Compile-time логічне суддження, 
			/////<para>вертає true якщо даний тип можна скопіювати простим побітовим копіюванням</para>
			/////</summary>
			/////<remarks>Спирається на реалізацію в компіляторі</remarks>
			//template<typename T>
			//constexpr bool IsMemCopySupported() { return CRT::is_trivially_copyable<T>(); }

		}
	}
}
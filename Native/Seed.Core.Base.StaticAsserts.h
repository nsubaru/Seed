#pragma once
#include "Seed.Core.Base.Types.h"

namespace Seed {
	inline namespace Core {
		///<summary>Деталі реалізації</summary>
		namespace Details {
			template <class T>
			constexpr void IsConstexprHelper(T&& t) {}
		}

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
			template<Boolean Cond, typename IfTrue, typename IfFalse>
			struct ConditionT { using Result = IfFalse; };

			///<summary>Compile-time логічне суддження, якщо Cond == True, вертає тип IfTrue, інакше IfFalse</summary>
			template<typename IfTrue, typename IfFalse>
			struct ConditionT<true, IfTrue, IfFalse> { using Result = IfTrue; };


			/////<summary>Compile-time логічне суддження, якщо T є об'єднанням то вертає True інакше False</summary>
			/////<remarks>Спирається на реалізацію в компіляторі</remarks>
			//template<typename T>
			//struct IsUnion :LogicConst<CRT::is_union<T>::value> {}; // Implementatiom defined

			/////<summary>Compile-time логічне суддження, якщо T є классом то вертає True інакше False</summary>
			/////<remarks>Спирається на реалізацію в компіляторі</remarks>
			//template<typename T>
			//struct IsClass :LogicConst<CRT::is_class<T>::value> {}; //Implementation defined


			///<summary>Compile-time логічне суддження, 
			///<para>якщо клас Derivde наслідує клас Base то вертає True інакше False</para></summary>
			///<remarks>Спирається на реалізацію в компіляторі</remarks>
			template<typename Base, typename Derived>
			struct IsBasedOn :LogicConst<__is_base_of(Base, Derived)> {}; //Implementation defined


			/////<summary>Compile-time логічне суддження, 
			/////<para>вертає true якщо даний тип можна скопіювати простим побітовим копіюванням</para>
			/////</summary>
			/////<remarks>Спирається на реалізацію в компіляторі</remarks>
			//template<typename T>
			//constexpr bool IsMemCopySupported() { return CRT::is_trivially_copyable<T>(); }

			///<summary>Знімає силку з типу</summary>
			template<typename T>
			struct RemoveRef { using Type = T; };

			///<summary>Знімає силку з типу</summary>
			template<typename T>
			struct RemoveRef<T&> { using Type = T; };

			///<summary>Знімає силку з типу</summary>
			template<typename T>
			struct RemoveRef<T&&> { using Type = T; };

			///<summary>Тип без силки</summary>
			template<typename T>
			using RemoveRefT = typename RemoveRef<T>::Type;


			///<summary>primary template</summary>
			template<typename>
			struct IsFunction : LogicFalse { };

			///<summary>specialization for regular functions</summary>
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...)> : LogicTrue {};

			///<summary>specialization for variadic functions such as std::printf</summary>
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...)> : LogicTrue {};

			///<summary>specialization for function types that have cv-qualifiers</summary>
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) volatile> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const volatile> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) volatile> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const volatile> : LogicTrue {};

			///<summary>specialization for function types that have ref-qualifiers</summary>
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) &> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const &> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) volatile &> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const volatile &> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) &> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const &> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) volatile &> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const volatile &> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) && > : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const &&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) volatile &&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const volatile &&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) && > : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const &&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) volatile &&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const volatile &&> : LogicTrue {};

			///<summary>specializations for noexcept versions of all the above (C++17 and later)</summary>
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) volatile noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const volatile noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) volatile noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const volatile noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) & noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const & noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) volatile & noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const volatile & noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) & noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const & noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) volatile & noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const volatile & noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) && noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const && noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) volatile && noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const volatile && noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) && noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const && noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) volatile && noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const volatile && noexcept> : LogicTrue {};

			template<class... _Types>
			using VoidT = void;

			// STRUCT TEMPLATE _Add_reference
			template<class Type,
				class = void>
				struct AddReference
			{	// add reference
				using Lvalue = Type;
				using Rvalue = Type;
			};

			template<class Type>
			struct AddReference<Type, VoidT<Type&>>
			{	// add reference
				using Lvalue = Type & ;
				using Rvalue = Type && ;
			};

			template<class Type>
			using AddRvalueReferenceT = typename AddReference<Type>::Rvalue;

			template<class Type>
			using AddLvalueReferenceT = typename AddReference<Type>::Lvalue;

			template<class Type>
			AddRvalueReferenceT<Type> DeclVal() noexcept {};

			template<typename Type>
			struct IsLValueRef : LogicFalse {};

			template<typename Type>
			struct IsLValueRef<Type&> : LogicTrue {};
		}
	}
}

#define IS_CONSTEXPR(...)  !noexcept(Seed::Details::IsConstexprHelper(__VA_ARGS__))
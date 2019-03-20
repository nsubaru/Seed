#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Utils.h"

namespace Seed {
	inline namespace Core {
		///<summary>Деталі реалізації</summary>
		namespace Details {
			template <class T>
			constexpr void IsConstexprHelper(T&& t) {}

			template< class T, bool IsFunctionType = false >
			struct AddPointer
			{
				using Type = typename StaticAsserts::RemoveRef<T>::Type*;
			};

			template< class T >
			struct AddPointer<T, true>
			{
				using Type = T;
			};

			template< class T, class... Args >
			struct AddPointer<T(Args...), true>
			{
				using Type = T(*)(Args...);
			};

			template< class T, class... Args >
			struct AddPointer<T(Args..., ...), true>
			{
				using Type = T(*)(Args..., ...);
			};
		}

		///<summary>Простір імен статичних провірок</summary>
		namespace StaticAsserts {


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

			template<Boolean Cond, typename IfTrue, typename IfFalse>
			using Condition = typename ConditionT<Cond, IfTrue, IfFalse>::Result;

			template<bool B, class T = void>
			struct EnableIf {};

			template<class T>
			struct EnableIf<true, T> { typedef T type; };

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

			template<class T>
			struct IsCArray : LogicFalse {};

			template<class T>
			struct IsCArray<T[]> : LogicTrue {};

			template<class T, SizeT N>
			struct IsCArray<T[N]> : LogicTrue {};


			template<class T>
			struct RemoveSizeFromCArray { typedef T type; };

			template<class T>
			struct RemoveSizeFromCArray<T[]> { typedef T type; };

			template<class T, SizeT N>
			struct RemoveSizeFromCArray<T[N]> { typedef T type; };

			template<typename T>
			struct RemoveConstStruct
			{
				using Type = T;
			};

			template<typename T>
			struct RemoveConstStruct<const T>
			{
				using Type = T;
			};

			template<typename T>
			using RemoveConst = typename RemoveConstStruct<T>::Type;

			template<typename T>
			struct RemoveVolatileStruct
			{
				using Type = T;
			};

			template<typename T>
			struct RemoveVolatileStruct<volatile T>
			{
				using Type = T;
			};

			template<typename T>
			using RemoveVolatile = typename RemoveVolatileStruct<T>::Type;

			template< class T >
			struct RemoveCVStruct
			{
				typedef typename RemoveVolatileStruct<typename RemoveConstStruct<T>::Type>::Type Type;
			};

			template<typename T>
			using RemoveCV = typename RemoveCVStruct<T>::Type;

#pragma region Is Function static assert
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
#pragma endregion

			template<typename T >
			struct AddPointerStruct : Details::AddPointer<T, IsFunction<T>::State> {};

			template<typename T>
			using AddPointer = typename AddPointerStruct<T>::Type;

#pragma region Signed type converter helpers
			template<typename>
			struct MakeSigned {};

			template<>
			struct MakeSigned<unsigned __int8> {
				using Type = signed __int8;
			};

			template<>
			struct MakeSigned<unsigned __int16> {
				using Type = signed __int16;
			};

			template<>
			struct MakeSigned<unsigned __int32> {
				using Type = signed __int32;
			};

			template<>
			struct MakeSigned<unsigned __int64> {
				using Type = signed __int64;
			};

			template<>
			struct MakeSigned<signed __int8> {
				using Type = signed __int8;
			};

			template<>
			struct MakeSigned<signed __int16> {
				using Type = signed __int16;
			};

			template<>
			struct MakeSigned<signed __int32> {
				using Type = signed __int32;
			};

			template<>
			struct MakeSigned<signed __int64> {
				using Type = signed __int64;
			};
#pragma endregion

			template<typename Int>
			using Signed = typename MakeSigned<Int>::Type;

#pragma region Unsigned type converter helpers
			template<typename>
			struct MakeUnsigned {};

			template<>
			struct MakeUnsigned<signed __int8> {
				using Type = unsigned __int8;
			};

			template<>
			struct MakeUnsigned<signed __int16> {
				using Type = unsigned __int16;
			};

			template<>
			struct MakeUnsigned<signed __int32> {
				using Type = unsigned __int32;
			};

			template<>
			struct MakeUnsigned<signed __int64> {
				using Type = unsigned __int64;
			};

			template<>
			struct MakeUnsigned<unsigned __int8> {
				using Type = unsigned __int8;
			};

			template<>
			struct MakeUnsigned<unsigned __int16> {
				using Type = unsigned __int16;
			};

			template<>
			struct MakeUnsigned<unsigned __int32> {
				using Type = unsigned __int32;
			};

			template<>
			struct MakeUnsigned<unsigned __int64> {
				using Type = unsigned __int64;
			};
#pragma endregion

			template<typename Int>
			using Unsigned = typename MakeUnsigned<Int>::Type;

			template< class T >
			struct DecayStruct
			{
			private:
				typedef typename RemoveRef<T>::Type U;
			public:
				typedef typename Condition<
					IsCArray<U>::State,
					typename RemoveSizeFromCArray<U>::type*,
					typename Condition<
					IsFunction<U>::State,
					typename AddPointer<U>,
					typename RemoveCV<U>
					>
				> Type;
			};

			template<class T>
			using Decay = typename DecayStruct<T>::Type;
		}
	}
}

#define IS_CONSTEXPR(...)  !noexcept(Seed::Details::IsConstexprHelper(__VA_ARGS__))
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
				using Type = typename TypeTraits::RemoveRefT<T>::Type*;
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
		namespace TypeTraits {
			///<summary>Логічне суддження чи тип T1 == T2 виконується в compile-time</summary>
			template<typename T1, typename T2>
			struct IsSameT :public LogicFalse {};

			///<summary>Логічне суддження чи тип T1 == T2 виконується в compile-time</summary>
			template<typename T>
			struct IsSameT<T, T> :public LogicTrue {};

			template<typename T1, typename T2>
			inline constexpr bool IsSameV = IsSameT<T1, T2>::State;

			///<summary>Compile-time логічне суддження, якщо Cond == True, вертає тип IfTrue, інакше IfFalse</summary>
			template<Boolean Cond, typename IfTrue, typename IfFalse>
			struct ConditionT { using Result = IfFalse; };

			///<summary>Compile-time логічне суддження, якщо Cond == True, вертає тип IfTrue, інакше IfFalse</summary>
			template<typename IfTrue, typename IfFalse>
			struct ConditionT<true, IfTrue, IfFalse> { using Result = IfTrue; };

			///<summary>Compile-time результат логічного судження</summary>
			template<Boolean Cond, typename IfTrue, typename IfFalse>
			using ConditionV = typename ConditionT<Cond, IfTrue, IfFalse>::Result;

			template<bool B, class T = void>
			struct EnableIf {};

			template<class T>
			struct EnableIf<true, T> { typedef T Type; };

			template<typename FalseT, class T = void>
			struct EnableIfT {};

			template<class T>
			struct EnableIfT<LogicTrue, T> { typedef T Type; };

			template<bool B, class T>
			using EnableIfV = typename EnableIf<B, T>::Type;

			template<typename B, typename T>
			using EnableIfTV = typename EnableIfT<B, T>::Type;

			/*///<summary>Compile-time логічне суддження, якщо T є об'єднанням то вертає True інакше False</summary>*/
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

			template<typename Base, typename Derived>
			constexpr Boolean IsBasedOnV = __is_base_of(Base, Derived);

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
			///<summary>Compile-time перевірка чи даний тип є функцією</summary>
			template<typename>
			struct IsFunction : LogicFalse { };

			///<summary>Compile-time перевірка чи даний тип є функцією</summary>
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...)> : LogicTrue {};

			///<summary>Compile-time перевірка чи даний тип є функцією</summary>
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...)> : LogicTrue {};

			///<summary>Compile-time перевірка чи даний тип є функцією</summary>
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
			struct IsFunction<Ret(Args...)&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) volatile&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const volatile&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...)&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) volatile&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const volatile&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...)&& > : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const&&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) volatile&&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const volatile&&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...)&& > : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const&&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) volatile&&> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const volatile&&> : LogicTrue {};

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
			struct IsFunction<Ret(Args...) const& noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) volatile& noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const volatile& noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) & noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const& noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) volatile& noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const volatile& noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) && noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const&& noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) volatile&& noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args...) const volatile&& noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) && noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const&& noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) volatile&& noexcept> : LogicTrue {};
			template<typename Ret, typename... Args>
			struct IsFunction<Ret(Args..., ...) const volatile&& noexcept> : LogicTrue {};
#pragma endregion

			template<typename T >
			struct AddPointerStruct : Details::AddPointer<T, IsFunction<T>::State> {};

			template<typename T>
			using AddPointer = typename AddPointerStruct<T>::Type;

#pragma region Signed type converter helpers
			template<typename>
			struct MakeSigned : LogicFalse {};

			template<>
			struct MakeSigned<unsigned __int8> : LogicTrue {
				using Type = signed __int8;
			};

			template<>
			struct MakeSigned<unsigned __int16> : LogicTrue {
				using Type = signed __int16;
			};

			template<>
			struct MakeSigned<unsigned __int32> : LogicTrue {
				using Type = signed __int32;
			};

			template<>
			struct MakeSigned<unsigned __int64> : LogicTrue {
				using Type = signed __int64;
			};

			template<>
			struct MakeSigned<signed __int8> : LogicTrue {
				using Type = signed __int8;
			};

			template<>
			struct MakeSigned<signed __int16> : LogicTrue {
				using Type = signed __int16;
			};

			template<>
			struct MakeSigned<signed __int32> : LogicTrue {
				using Type = signed __int32;
			};

			template<>
			struct MakeSigned<signed __int64> : LogicTrue {
				using Type = signed __int64;
			};
#pragma endregion

			template<typename Int>
			using Signed = typename MakeSigned<Int>::Type;

			template<typename Int>
			constexpr auto IsSigned = MakeSigned<Int>::State;

#pragma region Unsigned type converter helpers
			template<typename>
			struct MakeUnsigned : LogicFalse {};

			template<>
			struct MakeUnsigned<signed __int8> : LogicTrue {
				using Type = unsigned __int8;
			};

			template<>
			struct MakeUnsigned<signed __int16> : LogicTrue {
				using Type = unsigned __int16;
			};

			template<>
			struct MakeUnsigned<signed __int32> : LogicTrue {
				using Type = unsigned __int32;
			};

			template<>
			struct MakeUnsigned<signed __int64> : LogicTrue {
				using Type = unsigned __int64;
			};

			template<>
			struct MakeUnsigned<unsigned __int8> : LogicTrue {
				using Type = unsigned __int8;
			};

			template<>
			struct MakeUnsigned<unsigned __int16> : LogicTrue {
				using Type = unsigned __int16;
			};

			template<>
			struct MakeUnsigned<unsigned __int32> : LogicTrue {
				using Type = unsigned __int32;
			};

			template<>
			struct MakeUnsigned<unsigned __int64> : LogicTrue {
				using Type = unsigned __int64;
			};
#pragma endregion

			template<typename Int>
			using Unsigned = typename MakeUnsigned<Int>::Type;

			template<typename Int>
			constexpr auto IsUnsigned = MakeUnsigned<Int>::State;

			template< class T >
			struct DecayStruct
			{
			private:
				typedef RemoveRefV<T> U;
			public:
				typedef typename ConditionV <
					IsCArray<U>::State,
					typename RemoveSizeFromCArray<U>::type*,
					typename ConditionV<
					IsFunction<U>::State,
					typename AddPointer<U>,
					typename RemoveCV<U>
					>
				> Type;
			};

			template<class T>
			using Decay = typename DecayStruct<T>::Type;

			// STRUCT TEMPLATE AndT
			template <bool _First_value, class _First,
				class... _Rest>
				struct AndTImpl { // handle false trait or last trait
				using Result = _First;
			};

			template <class _True, class _Next, class... _Rest>
			struct AndTImpl<true, _True, _Next, _Rest...> { // the first trait is true, try the next one
				using Result = typename AndTImpl<_Next::State, _Next, _Rest...>::Result;
			};

			template <class... _Traits>
			struct AndT : LogicTrue { // If _Traits is empty, LogicTrue
			};

			template <class _First, class... _Rest>
			struct AndT<_First, _Rest...>
				: LogicConstT<typename AndTImpl<_First::State, _First, _Rest...>::Result> { // Otherwise, if any of _Traits are false, the first false
																		// trait Otherwise, the last trait in _Traits
			};

			template <class... _Traits>
			constexpr bool And = AndT<_Traits...>::State;

			template<class _Ty,
				class... _Args>
				struct IsConstructibleT
				: LogicConst<__is_constructible(_Ty, _Args...)>
			{	// determine whether _Ty(_Args...) is constructible
			};

			template<class _Ty,
				class... _Args>
				inline constexpr bool IsConstructible = __is_constructible(_Ty, _Args...);

			template<class _From,
				class _To>
				struct IsConvertibleT
				: LogicConst<__is_convertible_to(_From, _To)>
			{	// determine whether _From is convertible to _To
			};

			template<class _From,
				class _To>
				inline constexpr bool IsConvertible = __is_convertible_to(_From, _To);

			// STRUCT TEMPLATE negation
			template<class _Trait>
			struct NegationT
				: LogicConst<!static_cast<bool>(_Trait::State)>
			{	// The negated result of _Trait
			};

			template<class _Trait>
			inline constexpr bool Negation = NegationT<_Trait>::State;

			// NOTHROW TRAITS
			// STRUCT TEMPLATE is_nothrow_constructible
			template<class _Ty,
				class... _Args>
				struct IsNothrowConstructibleT
				: LogicConst<__is_nothrow_constructible(_Ty, _Args...)>
			{	// determine whether _Ty(_Args...) is nothrow constructible
			};

			template<class _Ty,
				class... _Args>
				inline constexpr bool IsNothrowConstructible = IsNothrowConstructibleT(_Ty, _Args...);

			// STRUCT TEMPLATE is_assignable
			template<class _To,
				class _From>
				struct IsAssignableT
				: LogicConst<__is_assignable(_To, _From)>
			{	// determine whether _From can be assigned to _To
			};

			template<class _To,
				class _From>
				inline constexpr bool IsAssignable = IsAssignableT(_To, _From);

			// STRUCT TEMPLATE is_nothrow_assignable
			template<class _To,
				class _From>
				struct IsNothrowAssignableT
				: LogicConst<__is_nothrow_assignable(_To, _From)>
			{	// determine whether _From can be assigned to _To, nothrow
			};

			template<class _To,
				class _From>
				inline constexpr bool IsNothrowAssignable = __is_nothrow_assignable(_To, _From);

			// STRUCT TEMPLATE disjunction
			template<bool _First_value,
				class _First,
				class... _Rest>
				struct _Disjunction
			{	// handle true trait or last trait
				using type = _First;
			};

			template<class _False,
				class _Next,
				class... _Rest>
				struct _Disjunction<false, _False, _Next, _Rest...>
			{	// first trait is false, try the next trait
				using type = typename _Disjunction<_Next::State, _Next, _Rest...>::type;
			};

			template<class... _Traits>
			struct OrT
				: LogicFalse
			{	// If _Traits is empty, false_type
			};

			template<class _First,
				class... _Rest>
				struct OrT<_First, _Rest...>
				: _Disjunction<_First::State, _First, _Rest...>::type
			{	// Otherwise, if any of _Traits are true, the first true trait
				// Otherwise, the last trait in _Traits
			};

			template<class... _Traits>
			inline constexpr bool Or = OrT<_Traits...>::State;

			// STRUCT TEMPLATE is_default_constructible
			template<class _Ty>
			struct IsDefaultConstructibleT
				: LogicConst<__is_constructible(_Ty)>
			{	// determine whether _Ty has a default constructor
			};

			template<class _Ty>
			inline constexpr bool IsDefaultConstructible = __is_constructible(_Ty);

			// STRUCT TEMPLATE _Is_implicitly_default_constructible
			template<class _Ty,
				class = void>
				struct IsImplicitlyDefaultConstructibleT
				: LogicFalse
			{	// determine whether _Ty is implicitly default constructible
			};

			template<class _Ty>
			void _Implicitly_default_construct(const _Ty&);

			template<class _Ty>
			struct IsImplicitlyDefaultConstructibleT < _Ty, VoidT<decltype(_Implicitly_default_construct<_Ty>({})) >>
				: LogicTrue
			{	// determine whether _Ty is implicitly default constructible
			};

			// STRUCT TEMPLATE is_nothrow_default_constructible
			template<class _Ty>
			struct IsNothrowDefaultConstructibleT
				: LogicConst<__is_nothrow_constructible(_Ty)>
			{	// determine whether _Ty has a nothrow default constructor
			};

			template<class _Ty>
			inline constexpr bool IsNothrowDefaultConstructible = __is_nothrow_constructible(_Ty);

			// STRUCT TEMPLATE is_move_assignable
			template<class _Ty>
			struct IsMoveAssignableT
				: LogicConst<__is_assignable(AddLvalueReferenceT<_Ty>, _Ty)>
			{	// determine whether _Ty has a move assignment operator
			};

			template<class _Ty>
			inline constexpr bool IsMoveAssignable = __is_assignable(AddLvalueReferenceT<_Ty>, _Ty);

			// STRUCT TEMPLATE is_nothrow_move_assignable
			template<class _Ty>
			struct IsNothrowMoveAssignableT
				: LogicConst<__is_nothrow_assignable(AddLvalueReferenceT<_Ty>, _Ty)>
			{	// determine whether _Ty has a nothrow move assignment operator
			};

			template<class _Ty>
			inline constexpr bool IsNothrowMoveAssignable = __is_nothrow_assignable(AddLvalueReferenceT<_Ty>, _Ty);

			// FUNCTION TEMPLATE declval
			template<class _Ty>
			AddRvalueReferenceT<_Ty> declval() noexcept;

			// STRUCT TEMPLATE _Swappable_with_helper
			template<class _Ty1,
				class _Ty2,
				class = void>
				struct _Swappable_with_helper
				: LogicFalse
			{	// swap(declval<_Ty1>(), declval<_Ty2>()) is not valid
			};

			template<class _Ty1,
				class _Ty2>
				struct _Swappable_with_helper<_Ty1, _Ty2, VoidT<decltype(Swap(declval<_Ty1>(), declval<_Ty2>()))>>
				: LogicTrue
			{	// swap(declval<_Ty1>(), declval<_Ty2>()) is valid
			};

			// STRUCT TEMPLATE _Is_swappable_with
			template<class _Ty1,
				class _Ty2>
				struct _Is_swappable_with
				: LogicConst<And<
				_Swappable_with_helper<_Ty1, _Ty2>,
				_Swappable_with_helper<_Ty2, _Ty1>>>
			{	// Determine if expressions with type and value category _Ty1 and _Ty2
				// can be swapped (and vice versa)
			};

			// STRUCT TEMPLATE _Is_swappable
			template<class _Ty>
			struct IsSwappable
				: _Is_swappable_with<
				AddLvalueReferenceT<_Ty>,
				AddLvalueReferenceT<_Ty>>::type
			{	// Determine if _Ty lvalues satisfy is_swappable_with
			};

			// STRUCT TEMPLATE is_void
			template<class _Ty>
			struct IsVoidT
				: LogicFalse
			{	// determine whether _Ty is void
			};
			//#define _EMPTY_ARGUMENT
			//#define _CLASS_DEFINE_CV(CLASS) \
			//	CLASS(_EMPTY_ARGUMENT) \
			//	CLASS(const) \
			//	CLASS(volatile) \
			//	CLASS(const volatile)
			//
			//#define _IS_VOID(CV_OPT) \
			//template<> \
			//	struct IsVoidT<CV_OPT void> \
			//		: LogicTrue \
			//	{	/* determine whether _Ty is void */ \
			//	};
			//
			//			_CLASS_DEFINE_CV(_IS_VOID)
			//#undef _IS_VOID
			//#undef _CLASS_DEFINE_CV
			//#undef _EMPTY_ARGUMENT
			template<>
			struct IsVoidT<void>
				: LogicTrue
			{	/* determine whether _Ty is void */
			};

			template<>
			struct IsVoidT<const void>
				: LogicTrue
			{	/* determine whether _Ty is void */
			};

			template<>
			struct IsVoidT<volatile void>
				: LogicTrue
			{	/* determine whether _Ty is void */
			};

			template<>
			struct IsVoidT<const volatile void>
				: LogicTrue
			{	/* determine whether _Ty is void */
			};

			//Вертає чи даний тип є void
			template<class _Ty>
			inline constexpr bool IsVoidV = IsVoidT<_Ty>::State;

			// Type modifiers
			// STRUCT TEMPLATE add_const
			template <class _Ty>
			struct AddConstT { // add top-level const qualifier
				using Type = const _Ty;
			};

			template <class _Ty>
			using AddConstV = typename AddConstT<_Ty>::Type;

			// STRUCT TEMPLATE add_volatile
			template <class _Ty>
			struct AddVolatileT { // add top-level volatile qualifier
				using Type = volatile _Ty;
			};

			template <class _Ty>
			using AddVolatileV = typename AddVolatileT<_Ty>::Type;

			// STRUCT TEMPLATE add_cv
			template <class _Ty>
			struct AddCvT { // add top-level const and volatile qualifiers
				using Type = const volatile _Ty;
			};

			template <class _Ty>
			using AddCvV = typename AddCvT<_Ty>::Type;

			// STRUCT TEMPLATE is_copy_assignable
			template <class _Ty>
			struct IsCopyAssignableT
				: LogicConst<__is_assignable(AddLvalueReferenceT<_Ty>, AddLvalueReferenceT<const _Ty>)> {
				// determine whether an lvalue const _Ty can be assigned to an lvalue _Ty
			};

			template <class _Ty>
			inline constexpr bool IsCopyAssignableV = __is_assignable(
				AddLvalueReferenceT<_Ty>, AddLvalueReferenceT<const _Ty>);

			// STRUCT TEMPLATE _Identity
			template <class _Ty>
			struct IdentityT {
				using type = _Ty;
			};
			template <class _Ty>
			using IdentitV = typename IdentityT<_Ty>::type;

			// STRUCT TEMPLATE is_empty
			template <class _Ty>
			struct IsEmptyT : LogicConst<__is_empty(_Ty)> {}; // determine whether _Ty is an empty class

			template <class _Ty>
			inline constexpr bool IsEmptyV = __is_empty(_Ty);

			// STRUCT TEMPLATE is_final
			template <class _Ty>
			struct IsFinalT : LogicConst<__is_final(_Ty)> {}; // determine whether _Ty is a final class

			template <class _Ty>
			inline constexpr bool IsFinalV = __is_final(_Ty);

			// STRUCT TEMPLATE is_lvalue_reference
			template <class>
			inline constexpr bool IsLValueReferenceV = false; // determine whether type argument is an lvalue reference

			template <class _Ty>
			inline constexpr bool IsLValueReferenceV<_Ty&> = true;

			template <class _Ty>
			struct IsLValueReferenceT : LogicConst<IsLValueReferenceV<_Ty>> {};

			// STRUCT TEMPLATE is_rvalue_reference
			template <class>
			inline constexpr bool IsRValueReferenceV = false; // determine whether type argument is an rvalue reference

			template <class _Ty>
			inline constexpr bool IsRValueReferenceV<_Ty&&> = true;

			template <class _Ty>
			struct IsRValueReferenceT : LogicConst<IsRValueReferenceV<_Ty>> {};

			// STRUCT TEMPLATE is_reference
			template <class>
			inline constexpr bool IsReferenceV = false; // determine whether type argument is a reference

			template <class _Ty>
			inline constexpr bool IsReferenceV<_Ty&> = true;

			template <class _Ty>
			inline constexpr bool IsReferenceV<_Ty&&> = true;

			template <class _Ty>
			struct IsReferenceT : LogicConst<IsReferenceV<_Ty>> {};

			//Пакет типів. Спеціальна кострукція для перелічування пакету параметрів
			template<typename ... T>
			struct PackItems {};

			template<SizeT I, typename T>
			struct PackItem {
				static inline constexpr SizeT Index = I;
				using Type = T;
			};

			template<SizeT I>
			struct PackItem<I, PackItems<>> {
				static_assert(I != I, "Invalid index"); //-V501
			};

			template<typename T, typename ...Types>
			struct PackItem<0, PackItems<T, Types...>> {
				typedef T Type;
			};

			template<SizeT I, typename T, typename ... Types>
			struct PackItem<I, PackItems<T, Types...>> {
				typedef typename PackItem<I - 1, Types...>::Type Type;
			};

			//Indexator

			template<SizeT ...>
			struct Indexator {};

			template<SizeT Start, typename IndexatorT, SizeT End>
			struct IndexerImpl;

			template<SizeT End, SizeT ... I>
			struct IndexerImpl<End, Indexator<I...>, End> {
				typedef typename Indexator<I...> Type;
			};

			template<SizeT Start, SizeT ... I, SizeT End>
			struct IndexerImpl<Start, Indexator<I...>, End> {
				typedef typename IndexerImpl<Start + 1, Indexator<Start, I...>, End>::Type Type;
			};

			template<SizeT End, SizeT Start = 0>
			struct Indexer {
				typedef typename IndexerImpl<Start, Indexator<>, End>::Type Type;
			};

			template<typename Indexator, typename ... Types>
			struct PackIndexator;

			template<SizeT ...I, typename ...Types>
			struct PackIndexator<Indexator<I...>, Types...> : PackItem<I, Types>... {};

			template<SizeT Count, typename ... Types>
			struct PackImpl {
				typedef PackIndexator<typename Indexer<Count>::Type, Types...> Items;
			};

			template<typename ... Types>
			struct Pack
			{
				typedef typename PackImpl<sizeof...(Types), Types...>::Items Items;
				static inline constexpr SizeT Count = sizeof...(Types);

				template<typename ...Args>
				struct IsConvertibleFromTypePackT :
					ConditionT<
					sizeof...(Types) == sizeof...(Args),
					LogicConst<AndT<IsConvertibleT<Types, Args>...>::State>,
					LogicFalse
					>::Result
				{};

				template<typename ...Args>
				static inline constexpr Boolean IsConvertibleFromTypePackV = IsConvertibleFromTypePackT<Args...>::State;
			};
		}

		namespace Details {
			template<class Ret, class Cls, class IsMutable, class... LambdaArgs>
			struct Lambda
			{
				using isMutable = IsMutable;

				enum { Arity = sizeof...(LambdaArgs) };

				using ReturnType = TypeTraits::ConditionV<TypeTraits::IsVoidV<Ret>, TypeTraits::VoidT<void>, Ret>;

				using Args = typename TypeTraits::Pack<LambdaArgs...>;

				template<typename ExpectedRet>
				using IsRetTypeConvertibleTo = typename
					TypeTraits::ConditionT<
					TypeTraits::IsVoidV<ReturnType>,
					TypeTraits::IsVoidT<ExpectedRet>,
					TypeTraits::IsConvertibleT<ReturnType, ExpectedRet>
					>::Result;

				//template<typename ... Params>
				//using CanCallWith = typename StaticAsserts::Pack<LambdaArgs...>::IsConvertibleFromTypePackT<Params...>;
				/*template<typename Expected, typename ... Params>
				using IsConvertible = StaticAsserts::AndT<IsRetTypeConvertibleTo<Expected>, CanCallWith<Params...>>;*/
			};
		}

		namespace TypeTraits {
			template<class Ld>
			struct LambdaType
				: LambdaType<decltype(&Ld::operator())>
			{};

			template<class Ret, class Cls, class... Args>
			struct LambdaType<Ret(Cls::*)(Args...)>
				: Details::Lambda<Ret, Cls, LogicTrue, Args...>
			{};

			template<class Ret, class Cls, class... Args>
			struct LambdaType<Ret(Cls::*)(Args...) const>
				: Details::Lambda<Ret, Cls, LogicFalse, Args...>
			{};

			template<class Ld, typename Ret, typename ... Params>
			struct IsLambdaConvertible
				:LogicConst<
				AndT<
				IsSameT<typename LambdaType<Ld>::ReturnType, Ret>,
				IsSameT<typename LambdaType<Ld>::Args, Pack<Params...>>
				>::State
				> {};

			template<class Ld, typename Ret, typename ... Params>
			constexpr bool IsLambdaConvertibleV = IsLambdaConvertible<Ld, Ret, Params...>::State;

			template<typename T>
			struct IsPodT : LogicConst<__is_pod(T)> {};

			template<typename T>
			constexpr bool IsPodV = IsPodT<T>::State;

			template<typename T>
			constexpr bool IsNumeric =
				TypeTraits::IsSigned<T> ||
				TypeTraits::IsUnsigned<T> ||
				TypeTraits::IsSameT<T, Float>::State ||
				TypeTraits::IsSameT<T, Double>::State;

#pragma warning(push)
			//VS bug char and wchar_t trait as equal
#pragma warning(disable: 6287)
			template<typename T>
			constexpr bool IsCharacter = TypeTraits::IsSameT<T, AChar>() || TypeTraits::IsSameT<T, UChar>();
#pragma warning(pop)

			template<typename T>
			constexpr bool IsBoolean = TypeTraits::IsSameT<T, bool>();

			template<typename T>
			struct IsConstT : LogicFalse
			{};

			template<typename T>
			struct IsConstT<const T> : LogicTrue
			{};

			template<typename T>
			constexpr Boolean IsConst = IsConstT<T>::State;


			template<typename T>
			struct IsVolatileT : LogicFalse
			{};

			template<typename T>
			struct IsVolatileT<volatile T> : LogicTrue
			{};

			template<typename T>
			constexpr Boolean IsVolatile = IsVolatileT<T>::State;
		}
	}
}

#define IS_CONSTEXPR(...)  !noexcept(Seed::Details::IsConstexprHelper(__VA_ARGS__))
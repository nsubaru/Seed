#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Utils.h"

namespace Seed {
    inline namespace Core {
        ///<summary>Простір імен статичних перевірок</summary>
        namespace TypeTraits {
            ///<summary>Логічне судження чи тип T1 == T2 виконується в compile-time</summary>
            template<typename T1, typename T2>
            struct IsSameT : LogicFalse {};

            ///<summary>Логічне судження чи тип T1 == T2 виконується в compile-time</summary>
            template<typename T>
            struct IsSameT<T, T> : LogicTrue {};

            /// <summary>
            /// Вертає true якщо типи ідентичні
            /// </summary>
            template<typename T1, typename T2>
            constexpr bool IsSameV = IsSameT<T1, T2>::State;

            ///<summary>Compile-time логічне судження, якщо Cond == True, вертає тип IfTrue, інакше IfFalse</summary>
            template<Boolean TCond, typename TIfTrue, typename TIfFalse>
            struct ConditionT { using Result = TIfFalse; };

            ///<summary>Compile-time логічне судження, якщо Cond == True, вертає тип IfTrue, інакше IfFalse</summary>
            template<typename TIfTrue, typename TIfFalse>
            struct ConditionT<true, TIfTrue, TIfFalse> { using Result = TIfTrue; };

            ///<summary>Compile-time результат логічного судження</summary>
            template<Boolean TCond, typename TIfTrue, typename TIfFalse>
            using ConditionV = typename ConditionT<TCond, TIfTrue, TIfFalse>::Result;

            /// <summary>
            /// Генерує SFINAE помилку якщо TB false, і вертає тип якщо true
            /// </summary>
            template<bool TB, class T = void>
            struct EnableIf {};

            /// <summary>
            /// Генерує SFINAE помилку якщо TB false, і вертає тип якщо true
            /// </summary>
            template<class T>
            struct EnableIf<true, T> { typedef T Type; };

            /// <summary>
            /// Генерує SFINAE помилку якщо TB false, і вертає тип якщо true
            /// </summary>
            template<typename TFalseT, class T = void>
            struct EnableIfT {};

            /// <summary>
            /// Генерує SFINAE помилку якщо TB false, і вертає тип якщо true
            /// </summary>
            template<class T>
            struct EnableIfT<LogicTrue, T> { typedef T Type; };

            /// <summary>
            /// Генерує SFINAE помилку якщо TB false, і вертає тип якщо true
            /// </summary>
            template<bool TB, class T>
            using EnableIfV = typename EnableIf<TB, T>::Type;

            /// <summary>
            /// Генерує SFINAE помилку якщо TB false, і вертає тип якщо true
            /// </summary>
            template<typename B, typename T>
            using EnableIfTV = typename EnableIfT<B, T>::Type;

            ///<summary>Compile-time логічне судження, якщо клас Derivde наслідує клас Base то вертає True інакше False</summary>
            template<typename TBase, typename TDerived>
            struct IsBasedOn :LogicConst<__is_base_of(TBase, TDerived)> {}; //Implementation defined

            ///<summary>Compile-time логічне судження, якщо клас Derivde наслідує клас Base то вертає True інакше False</summary>
            template<typename TBase, typename TDerived>
            constexpr Boolean IsBasedOnV = __is_base_of(TBase, TDerived);

            /// <summary>
            /// Вертає true якщо тип є статичним масивом
            /// </summary>
            template<class T>
            struct IsCArray : LogicFalse {};

            /// <summary>
            /// Вертає true якщо тип є статичним масивом
            /// </summary>
            template<class T>
            struct IsCArray<T[]> : LogicTrue {};

            /// <summary>
            /// Вертає true якщо тип є статичним масивом
            /// </summary>
            template<class T, SizeT N>
            struct IsCArray<T[N]> : LogicTrue {};

            /// <summary>
            /// Вертає тип елементам масиву
            /// </summary>
            template<class T>
            struct RemoveSizeFromCArray { typedef T type; };

            /// <summary>
            /// Вертає тип елементам масиву
            /// </summary>
            template<class T>
            struct RemoveSizeFromCArray<T[]> { typedef T type; };

            /// <summary>
            /// Вертає тип елементам масиву
            /// </summary>
            template<class T, SizeT N>
            struct RemoveSizeFromCArray<T[N]> { typedef T type; };

            /// <summary>
            /// Вертає тип безе const модифікатора
            /// </summary>
            template<typename T>
            struct RemoveConstStruct
            {
                using Type = T;
            };

            /// <summary>
            /// Вертає тип безе const модифікатора
            /// </summary>
            template<typename T>
            struct RemoveConstStruct<const T>
            {
                using Type = T;
            };

            /// <summary>
            /// Вертає тип безе const модифікатора
            /// </summary>
            template<typename T>
            using RemoveConst = typename RemoveConstStruct<T>::Type;

            /// <summary>
            /// Вертає тип безе volatile модифікатора
            /// </summary>
            template<typename T>
            struct RemoveVolatileStruct
            {
                using Type = T;
            };

            /// <summary>
            /// Вертає тип безе volatile модифікатора
            /// </summary>
            template<typename T>
            struct RemoveVolatileStruct<volatile T>
            {
                using Type = T;
            };

            /// <summary>
            /// Вертає тип безе volatile модифікатора
            /// </summary>
            template<typename T>
            using RemoveVolatile = typename RemoveVolatileStruct<T>::Type;

            /// <summary>
            /// Вертає тип безе cv модифікаторів
            /// </summary>
            template< class T >
            struct RemoveCVStruct
            {
                typedef typename RemoveVolatileStruct<typename RemoveConstStruct<T>::Type>::Type Type;
            };

            /// <summary>
            /// Вертає тип безе cv модифікаторів
            /// </summary>
            template<typename T>
            using RemoveCV = typename RemoveCVStruct<T>::Type;

#pragma region Is Function static assert
            ///<summary>Compile-time перевірка чи даний тип є функцією</summary>
            template<typename>
            struct IsFunction : LogicFalse { };

            ///<summary>Compile-time перевірка чи даний тип є функцією</summary>
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...)> : LogicTrue {};

            ///<summary>Compile-time перевірка чи даний тип є функцією</summary>
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...)> : LogicTrue {};

            ///<summary>Compile-time перевірка чи даний тип є функцією</summary>
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) volatile> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const volatile> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) volatile> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const volatile> : LogicTrue {};

            ///<summary>specialization for function types that have ref-qualifiers</summary>
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...)&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) volatile&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const volatile&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...)&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) volatile&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const volatile&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...)&& > : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const&&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) volatile&&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const volatile&&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...)&& > : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const&&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) volatile&&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const volatile&&> : LogicTrue {};

            ///<summary>specializations for noexcept versions of all the above (C++17 and later)</summary>
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) const noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) volatile noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) const volatile noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) const noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) volatile noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) const volatile noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) & noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) const& noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) volatile& noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) const volatile& noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) & noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) const& noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) volatile& noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) const volatile& noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) && noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) const&& noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) volatile&& noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs...) const volatile&& noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) && noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) const&& noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) volatile&& noexcept> : LogicTrue {};
            //template<typename TRet, typename... TArgs>
            //struct IsFunction<TRet(TArgs..., ...) const volatile&& noexcept> : LogicTrue {};
#pragma endregion

            ///<summary>Деталі реалізації</summary>
            namespace Details
            {
                /// <summary>
                /// Помічник для визначення чи функція при компіляції тайм виконається
                /// </summary>
                template <class T>
                constexpr void IsConstexprHelper(T&& t) {}

                /// <summary>
                /// Реалізація додавання покажчика до типу
                /// </summary>
                template< class T, bool TIsFunctionType = false >
                struct AddPointer {
                    using Type = typename TypeTraits::RemoveRefT<T>::Type*;
                };

                /// <summary>
                /// Реалізація додавання покажчика до типу
                /// </summary>
                template< class T >
                struct AddPointer<T, true> {
                    using Type = T;
                };

                /// <summary>
                /// Реалізація додавання покажчика до типу
                /// </summary>
                template< class T, class... TArgs >
                struct AddPointer<T(TArgs...), true> {
                    using Type = T(*)(TArgs...);
                };

                /// <summary>
                /// Реалізація додавання покажчика до типу
                /// </summary>
                template< class T, class... TArgs >
                struct AddPointer<T(TArgs..., ...), true> {
                    using Type = T(*)(TArgs..., ...);
                };
            }

            /// <summary>
            /// Додавання покажчика до типу
            /// </summary>
            template<typename T >
            struct AddPointerStruct : Details::AddPointer<T, IsFunction<T>::State> {};

            /// <summary>
            /// Додавання покажчика до типу
            /// </summary>
            template<typename T>
            using AddPointer = typename AddPointerStruct<T>::Type;

#pragma region Signed type converter helpers
            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<typename>
            struct MakeSigned : LogicFalse {};

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<unsigned __int8> : LogicFalse {
                using Type = signed __int8;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<unsigned __int16> : LogicFalse {
                using Type = signed __int16;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<unsigned __int32> : LogicFalse {
                using Type = signed __int32;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<unsigned __int64> : LogicFalse {
                using Type = signed __int64;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<signed __int8> : LogicTrue {
                using Type = signed __int8;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<signed __int16> : LogicTrue {
                using Type = signed __int16;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<signed __int32> : LogicTrue {
                using Type = signed __int32;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<signed __int64> : LogicTrue {
                using Type = signed __int64;
            };
#pragma endregion
            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<typename TInt>
            using Signed = typename MakeSigned<TInt>::Type;

            /// <summary>
            /// Вертає чи тип знаковий
            /// </summary>
            template<typename TInt>
            constexpr auto IsSigned = MakeSigned<TInt>::State;

#pragma region Unsigned type converter helpers
            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<typename>
            struct MakeUnsigned : LogicFalse {};

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<signed __int8> : LogicFalse {
                using Type = unsigned __int8;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<signed __int16> : LogicFalse {
                using Type = unsigned __int16;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<signed __int32> : LogicFalse {
                using Type = unsigned __int32;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<signed __int64> : LogicFalse {
                using Type = unsigned __int64;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<unsigned __int8> : LogicTrue {
                using Type = unsigned __int8;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<unsigned __int16> : LogicTrue {
                using Type = unsigned __int16;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<unsigned __int32> : LogicTrue {
                using Type = unsigned __int32;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<unsigned __int64> : LogicTrue {
                using Type = unsigned __int64;
            };
#pragma endregion

            template<typename TInt>
            using Unsigned = typename MakeUnsigned<TInt>::Type;

            template<typename TInt>
            constexpr auto IsUnsigned = MakeUnsigned<TInt>::State;

            template< class T >
            struct DecayStruct
            {
            private:
                typedef RemoveRefV<T> U;
            public:
                typedef ConditionV <
                    IsCArray<U>::State,
                    typename RemoveSizeFromCArray<U>::type*,
                    ConditionV<
                    IsFunction<U>::State,
                    AddPointer<U>,
                    RemoveCV<U>
                    >
                > Type;
            };

            template<class T>
            using DecayV = typename DecayStruct<T>::Type;

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

            template<class TTy,
                class... _Args>
                struct IsConstructibleT
                : LogicConst<__is_constructible(TTy, _Args...)>
            {	// determine whether TTy(_Args...) is constructible
            };

            template<class TTy,
                class... _Args>
                inline constexpr bool IsConstructible = __is_constructible(TTy, _Args...);

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
            template<class TTy,
                class... _Args>
                struct IsNothrowConstructibleT
                : LogicConst<__is_nothrow_constructible(TTy, _Args...)>
            {	// determine whether TTy(_Args...) is nothrow constructible
            };

            template<class TTy,
                class... _Args>
                inline constexpr bool IsNothrowConstructible = IsNothrowConstructibleT<TTy, _Args...>::State;

            // STRUCT TEMPLATE is_assignable
            template<class _To,
                class _From>
                struct IsAssignableT
                : LogicConst<__is_assignable(_To, _From)>
            {	// determine whether _From can be assigned to _To
            };

            template<class _To,
                class _From>
                inline constexpr bool IsAssignable = IsAssignableT<_To, _From>::State;

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
            {	// If _Traits is empty, falseTType
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
            template<class TTy>
            struct IsDefaultConstructibleT
                : LogicConst<__is_constructible(TTy)>
            {	// determine whether TTy has a default constructor
            };

            template<class TTy>
            inline constexpr bool IsDefaultConstructible = __is_constructible(TTy);

            // STRUCT TEMPLATE _Is_implicitly_default_constructible
            template<class TTy,
                class = void>
                struct IsImplicitlyDefaultConstructibleT
                : LogicFalse
            {	// determine whether TTy is implicitly default constructible
            };

            template<class TTy>
            void ImplicitlyDefaultConstruct(const TTy&) = delete;

            template<class TTy>
            struct IsImplicitlyDefaultConstructibleT < TTy, VoidT<decltype(ImplicitlyDefaultConstruct<TTy>({})) >>
                : LogicTrue
            {	// determine whether TTy is implicitly default constructible
            };

            // STRUCT TEMPLATE is_nothrow_default_constructible
            template<class TTy>
            struct IsNothrowDefaultConstructibleT
                : LogicConst<__is_nothrow_constructible(TTy)>
            {	// determine whether TTy has a nothrow default constructor
            };

            template<class TTy>
            inline constexpr bool IsNothrowDefaultConstructible = __is_nothrow_constructible(TTy);

            // STRUCT TEMPLATE is_move_assignable
            template<class TTy>
            struct IsMoveAssignableT
                : LogicConst<__is_assignable(AddLvalueReferenceT<TTy>, TTy)>
            {	// determine whether TTy has a move assignment operator
            };

            template<class TTy>
            inline constexpr bool IsMoveAssignable = __is_assignable(AddLvalueReferenceT<TTy>, TTy);

            // STRUCT TEMPLATE is_nothrow_move_assignable
            template<class TTy>
            struct IsNothrowMoveAssignableT
                : LogicConst<__is_nothrow_assignable(AddLvalueReferenceT<TTy>, TTy)>
            {	// determine whether TTy has a nothrow move assignment operator
            };

            template<class TTy>
            inline constexpr bool IsNothrowMoveAssignable = __is_nothrow_assignable(AddLvalueReferenceT<TTy>, TTy);

            // FUNCTION TEMPLATE declval
            template<class TTy>
            AddRvalueReferenceT<TTy> Declval() noexcept { return {}; }

            // STRUCT TEMPLATE _Swappable_with_helper
            template<class TTy1,
                class TTy2,
                class = void>
                struct SwappableWithHelper
                : LogicFalse
            {	// swap(declval<TTy1>(), declval<TTy2>()) is not valid
            };

            template<class TTy1,
                class TTy2>
                struct SwappableWithHelper<TTy1, TTy2, VoidT<decltype(Swap(Declval<TTy1>(), Declval<TTy2>()))>>
                : LogicTrue
            {	// swap(declval<TTy1>(), declval<TTy2>()) is valid
            };

            // STRUCT TEMPLATE _Is_swappable_with
            template<class TTy1,
                class TTy2>
                struct IsSwappableWith
                : LogicConst<And<
                SwappableWithHelper<TTy1, TTy2>,
                SwappableWithHelper<TTy2, TTy1>>>
            {	// Determine if expressions with type and value category TTy1 and TTy2
                // can be swapped (and vice versa)
            };

            // STRUCT TEMPLATE _Is_swappable
            template<class TTy>
            struct IsSwappable
                : IsSwappableWith<
                AddLvalueReferenceT<TTy>,
                AddLvalueReferenceT<TTy>>::type
            {	// Determine if TTy lvalues satisfy is_swappable_with
            };

            /// <summary>
            /// Вертає чи даний тип є void
            /// </summary>
            template<class TTy>
            struct IsVoidT
                : LogicFalse
            {	
            };

            /// <summary>
            /// Вертає чи даний тип є void
            /// </summary>
            template<>
            struct IsVoidT<void>
                : LogicTrue
            {	
            };

            /// <summary>
            /// Вертає чи даний тип є void
            /// </summary>
            template<>
            struct IsVoidT<const void>
                : LogicTrue
            {	
            };

            /// <summary>
            /// Вертає чи даний тип є void
            /// </summary>
            template<>
            struct IsVoidT<volatile void>
                : LogicTrue
            {	
            };

            /// <summary>
            /// Вертає чи даний тип є void
            /// </summary>
            template<>
            struct IsVoidT<const volatile void>
                : LogicTrue
            {	
            };

            /// <summary>
            /// Вертає чи даний тип є void
            /// </summary>
            template<class TTy>
            inline constexpr bool IsVoidV = IsVoidT<TTy>::State;

            /// <summary>
            /// Додає const модифікатор до типу
            /// </summary>
            template <class TTy>
            struct AddConstT { 
                using Type = const TTy;
            };

            /// <summary>
            /// Додає const модифікатор до типу
            /// </summary>
            template <class TTy>
            using AddConstV = typename AddConstT<TTy>::Type;

            /// <summary>
            /// Додає volatile модифікатор до типу
            /// </summary>
            template <class TTy>
            struct AddVolatileT {
                using Type = volatile TTy;
            };

            /// <summary>
            /// Додає volatile модифікатор до типу
            /// </summary>
            template <class TTy>
            using AddVolatileV = typename AddVolatileT<TTy>::Type;

            /// <summary>
            /// Додає cv модифікатори до типу
            /// </summary>
            template <class TTy>
            struct AddCvT {
                using Type = const volatile TTy;
            };

            /// <summary>
            /// Додає cv модифікатори до типу
            /// </summary>
            template <class TTy>
            using AddCvV = typename AddCvT<TTy>::Type;

            /// <summary>
            /// Вертає true якщо змінну типу можна присвоїти копіюванням
            /// </summary>
            template <class TTy>
            struct IsCopyAssignableT
                : LogicConst<__is_assignable(AddLvalueReferenceT<TTy>, AddLvalueReferenceT<const TTy>)> {
            };

            template <class TTy>
            inline constexpr bool IsCopyAssignableV = __is_assignable(
                AddLvalueReferenceT<TTy>, AddLvalueReferenceT<const TTy>);

            /// <summary>
            /// Потрібен для визначення типу в процесі інстанціювання шаблону
            /// </summary>
            template <class TTy>
            struct IdentityT {
                using Type = TTy;
            };
            template <class TTy>
            using IdentitV = typename IdentityT<TTy>::Type;

            /// <summary>
            /// Вертає true якщо тип не містить не статичних полів
            /// </summary>
            template <class TTy>
            struct IsEmptyT : LogicConst<__is_empty(TTy)> {};

            /// <summary>
            /// Вертає true якщо тип не містить не статичних полів
            /// </summary>
            template <class TTy>
            inline constexpr bool IsEmptyV = __is_empty(TTy);

            /// <summary>
            /// Вертає true якщо клас позначений модифікатор final
            /// </summary>
            template <class TTy>
            struct IsFinalT : LogicConst<__is_final(TTy)> {};

            /// <summary>
            /// Вертає true якщо клас позначений модифікатор final
            /// </summary>
            template <class TTy>
            constexpr bool IsFinalV = __is_final(TTy);

            /// <summary>
            /// Вертає true якщо тип LValue
            /// </summary>
            template <class>
            constexpr bool IsLValueReferenceV = false;

            /// <summary>
            /// Вертає true якщо тип LValue
            /// </summary>
            template <class TTy>
            constexpr bool IsLValueReferenceV<TTy&> = true;

            /// <summary>
            /// Вертає true якщо тип LValue
            /// </summary>
            template <class TTy>
            struct IsLValueReferenceT : LogicConst<IsLValueReferenceV<TTy>> {};

            /// <summary>
            /// Вертає true якщо тип RValue
            /// </summary>
            template <class>
            constexpr bool IsRValueReferenceV = false;

            /// <summary>
            /// Вертає true якщо тип RValue
            /// </summary>
            template <class TTy>
            constexpr bool IsRValueReferenceV<TTy&&> = true;

            /// <summary>
            /// Вертає true якщо тип RValue
            /// </summary>
            template <class TTy>
            struct IsRValueReferenceT : LogicConst<IsRValueReferenceV<TTy>> {};

            /// <summary>
            /// Вертає true якщо тип силка
            /// </summary>
            template <class>
            constexpr bool IsReferenceV = false;

            /// <summary>
            /// Вертає true якщо тип силка
            /// </summary>
            template <class TTy>
            constexpr bool IsReferenceV<TTy&> = true;

            /// <summary>
            /// Вертає true якщо тип силка
            /// </summary>
            template <class TTy>
            constexpr bool IsReferenceV<TTy&&> = true;

            /// <summary>
            /// Вертає true якщо тип силка
            /// </summary>
            template <class TTy>
            struct IsReferenceT : LogicConst<IsReferenceV<TTy>> {};

            /// <summary>
            /// Пакет типів. Спеціальна конструкція для перелічування пакету параметрів
            /// </summary>
            template<typename ... T>
            struct PackItems {};

            /// <summary>
            /// Пакет типів. Спеціальна конструкція для перелічування пакету параметрів
            /// </summary>
            template<SizeT I, typename T>
            struct PackItem {
                static inline constexpr SizeT Index = I;
                using Type = T;
            };

            /// <summary>
            /// Пакет типів. Спеціальна конструкція для перелічування пакету параметрів
            /// </summary>
            template<SizeT I>
            struct PackItem<I, PackItems<>>;

            /// <summary>
            /// Пакет типів. Спеціальна конструкція для перелічування пакету параметрів
            /// </summary>
            template<typename T, typename ...TTypes>
            struct PackItem<0, PackItems<T, TTypes...>> {
                typedef T Type;
            };

            /// <summary>
            /// Пакет типів. Спеціальна конструкція для перелічування пакету параметрів
            /// </summary>
            template<SizeT I, typename T, typename ... TTypes>
            struct PackItem<I, PackItems<T, TTypes...>> {
                typedef typename PackItem<I - 1, TTypes...>::Type Type;
            };


            /// <summary>
            /// Індексатор який генерує перелік чисел
            /// </summary>
            template<SizeT ...>
            struct Indexator {};

            /// <summary>
            /// Індексатор який генерує перелік чисел
            /// </summary>
            template<SizeT TStart, typename TIndexatorT, SizeT TEnd>
            struct IndexerImpl;

            /// <summary>
            /// Індексатор який генерує перелік чисел
            /// </summary>
            template<SizeT TEnd, SizeT ... TI>
            struct IndexerImpl<TEnd, Indexator<TI...>, TEnd> {
                typedef Indexator<TI...> Type;
            };

            /// <summary>
            /// Індексатор який генерує перелік чисел
            /// </summary>
            template<SizeT TStart, SizeT ... TI, SizeT TEnd>
            struct IndexerImpl<TStart, Indexator<TI...>, TEnd> {
                typedef typename IndexerImpl<TStart + 1, Indexator<TStart, TI...>, TEnd>::Type Type;
            };

            /// <summary>
            /// Індексатор який генерує перелік чисел
            /// </summary>
            template<SizeT TEnd, SizeT TStart = 0>
            struct Indexer {
                typedef typename IndexerImpl<TStart, Indexator<>, TEnd>::Type Type;
            };

            /// <summary>
            /// Індексатор пакета типів
            /// </summary>
            template<typename TIndexator, typename ... TTypes>
            struct PackIndexator;

            /// <summary>
            /// Індексатор пакета типів
            /// </summary>
            template<SizeT ...TI, typename ...TTypes>
            struct PackIndexator<Indexator<TI...>, TTypes...> : PackItem<TI, TTypes>... {};

            /// <summary>
            /// Реалізація пакета
            /// </summary>
            template<SizeT TCount, typename ... TTypes>
            struct PackImpl {
                typedef PackIndexator<typename Indexer<TCount>::Type, TTypes...> Items;
            };

            /// <summary>
            /// Пакет типів
            /// </summary>
            template<typename ... TTypes>
            struct Pack
            {
                /// <summary>
                /// Кортеж типів
                /// </summary>
                typedef typename PackImpl<sizeof...(TTypes), TTypes...>::Items Items;
                static inline constexpr SizeT Count = sizeof...(TTypes);

                /// <summary>
                /// Чи типи в пакетах перетворюються з кортежу типів
                /// </summary>
                template<typename ...TArgs>
                struct IsConvertibleFromTypePackT :
                    ConditionT<
                    sizeof...(TTypes) == sizeof...(TArgs),
                    LogicConst<AndT<IsConvertibleT<TTypes, TArgs>...>::State>,
                    LogicFalse
                    >::Result
                {};

                /// <summary>
                /// Чи типи в пакетах перетворюються з кортежу типів
                /// </summary>
                template<typename ...TArgs>
                static inline constexpr Boolean IsConvertibleFromTypePackV = IsConvertibleFromTypePackT<TArgs...>::State;
            };
        }

        namespace DetailsLambda {
            /// <summary>
            /// Визначення типів лямбди
            /// </summary>
            /// <typeparam name="TRet">Тип повернення лямбди</typeparam>
            /// <typeparam name="TCls">Тип класу лямбди</typeparam>
            /// <typeparam name="TIsMutable">Чи лямбда mutable</typeparam>
            /// <typeparam name="TLambdaArgs">Аргументи лямбди</typeparam>
            template<class TRet, class TCls, class TIsMutable, class... TLambdaArgs>
            struct Lambda
            {
                /// <summary>
                /// Чи лямбда mutable
                /// </summary>
                using IsMutableT = TIsMutable;

                /// <summary>
                /// Чи лямбда mutable
                /// </summary>
                static constexpr auto IsMutable = TIsMutable::State;

                /// <summary>
                /// Кількість аргументів
                /// </summary>
                static constexpr auto Arity = sizeof...(TLambdaArgs);

                /// <summary>
                /// Тип повернення лямбди
                /// </summary>
                using ReturnType = TypeTraits::ConditionV<TypeTraits::IsVoidV<TRet>, TypeTraits::VoidT<void>, TRet>;

                /// <summary>
                /// Пакет аргументів лямбди
                /// </summary>
                using Args = TypeTraits::Pack<TLambdaArgs...>;

                /// <summary>
                /// LogicTrue якщо результат лямбди можна перетворити в тип TExpectedTRet
                /// </summary>
                template<typename TExpectedTRet>
                using IsTRetTypeConvertibleTo = typename
                    TypeTraits::ConditionT<
                    TypeTraits::IsVoidV<ReturnType>,
                    TypeTraits::IsVoidT<TExpectedTRet>,
                    TypeTraits::IsConvertibleT<ReturnType, TExpectedTRet>
                    >::Result;

                /// <summary>
                /// Чи можливо викликати лямбду з наступним набором типів
                /// </summary>
                template<typename ... TParams>
                using CanCallWith = typename TypeTraits::Pack<TLambdaArgs...>::template IsConvertibleFromTypePackT<TParams...>;

                /// <summary>
                /// Чи можливо перетворити аргумент і тип повернення
                /// </summary>
                template<typename TExpected, typename ... TParams>
                using IsConvertible = TypeTraits::AndT<IsTRetTypeConvertibleTo<TExpected>, CanCallWith<TParams...>>;

                /// <summary>
                /// Сигнатура функтора лямбди
                /// </summary>
                using LambdaFunctionSign = TRet(*)(TLambdaArgs...);
            };
        }

        namespace TypeTraits {
            /// <summary>
            /// Визначення типів лямбди
            /// </summary>
            /// <typeparam name="TLd"></typeparam>
            template<class TLd>
            struct LambdaType
                : LambdaType<decltype(&TLd::operator())>
            {};

            /// <summary>
            /// Визначення типів лямбди
            /// </summary>
            /// <typeparam name="TRet">Тип повернення лямбди</typeparam>
            /// <typeparam name="TCls">Тип класу лямбди</typeparam>
            /// <typeparam name="TArgs">Аргументи лямбди</typeparam>
            template<class TRet, class TCls, class... TArgs>
            struct LambdaType<TRet(TCls::*)(TArgs...)>
                : DetailsLambda::Lambda<TRet, TCls, LogicTrue, TArgs...>
            {};

            /// <summary>
            /// Визначення типів лямбди
            /// </summary>
            /// <typeparam name="TRet">Тип повернення лямбди</typeparam>
            /// <typeparam name="TCls">Тип класу лямбди</typeparam>
            /// <typeparam name="TArgs">Аргументи лямбди</typeparam>
            template<class TRet, class TCls, class... TArgs>
            struct LambdaType<TRet(TCls::*)(TArgs...) const>
                : DetailsLambda::Lambda<TRet, TCls, LogicFalse, TArgs...>
            {};

            /// <summary>
            /// Чи тип повернення і аргументів збігається в лямбди
            /// </summary>
            /// <typeparam name="TLd">Тип лямбди</typeparam>
            /// <typeparam name="TRet">Очікуваний тип повернення</typeparam>
            /// <typeparam name="TParams">Очікуваний типи аргументів</typeparam>
            template<class TLd, typename TRet, typename ... TParams>
            struct IsLambdaConvertibleT
                :LogicConst<
                AndT<
                IsSameT<typename LambdaType<TLd>::ReturnType, TRet>,
                IsSameT<typename LambdaType<TLd>::Args, Pack<TParams...>>
                >::State
                > {};

            /// <summary>
            /// Чи тип повернення і аргументів збігається в лямбди
            /// </summary>
            /// <typeparam name="TLd">Тип лямбди</typeparam>
            /// <typeparam name="TRet">Очікуваний тип повернення</typeparam>
            /// <typeparam name="TParams">Очікуваний типи аргументів</typeparam>
            template<class TLd, typename TRet, typename ... TParams>
            constexpr bool IsLambdaConvertibleV = IsLambdaConvertibleT<TLd, TRet, TParams...>::State;

            /// <summary>
            /// Перевірка чи лямбда має список захоплення
            /// </summary>
            template<typename T>
            static constexpr bool IsLambdaHaveCapture = !(__is_convertible_to(T, typename LambdaType<T>::LambdaFunctionSign));

            /// <summary>
            /// Вертає true якщо тип POD
            /// </summary>
            template<typename T>
            struct IsPodT : LogicConst<__is_pod(T)> {};

            /// <summary>
            /// Вертає true якщо тип POD
            /// </summary>
            template<typename T>
            constexpr bool IsPodV = IsPodT<T>::State;

            /// <summary>
            /// Вертає true якщо тип числовий
            /// </summary>
            template<typename T>
            constexpr bool IsNumeric =
                TypeTraits::IsSigned<T> ||
                TypeTraits::IsUnsigned<T> ||
                TypeTraits::IsSameT<T, Float>::State ||
                TypeTraits::IsSameT<T, Double>::State;

#pragma warning(push)
            //VS bug char and wchar_t trait as equal
#pragma warning(disable: 6287)
            /// <summary>
            /// Вертає чи тип символьний
            /// </summary>
            template<typename T>
            constexpr bool IsCharacter = TypeTraits::IsSameT<T, AChar>() || TypeTraits::IsSameT<T, UChar>();
#pragma warning(pop)

            /// <summary>
            /// Вертає чи тип символьний
            /// </summary>
            template<typename T>
            constexpr bool IsBoolean = TypeTraits::IsSameT<T, bool>();

            /// <summary>
            /// Вертає чи тип має модифікатор const
            /// </summary>
            template<typename T>
            struct IsConstT : LogicFalse
            {};

            /// <summary>
            /// Вертає чи тип має модифікатор const
            /// </summary>
            template<typename T>
            struct IsConstT<const T> : LogicTrue
            {};

            /// <summary>
            /// Вертає чи тип має модифікатор const
            /// </summary>
            template<typename T>
            constexpr Boolean IsConstV = IsConstT<T>::State;

            /// <summary>
            /// Вертає чи тип має модифікатор volatile
            /// </summary>
            template<typename T>
            struct IsVolatileT : LogicFalse
            {};

            /// <summary>
            /// Вертає чи тип має модифікатор volatile
            /// </summary>
            template<typename T>
            struct IsVolatileT<volatile T> : LogicTrue
            {};

            /// <summary>
            /// Вертає чи тип має модифікатор volatile
            /// </summary>
            template<typename T>
            constexpr Boolean IsVolatileV = IsVolatileT<T>::State;

            /// <summary>
            /// Вертає логічне And з змінних
            /// </summary>
            template<typename ... TItems>
            constexpr Boolean AndPack(TItems ... items) {
                return (... && items);
            }

            /// <summary>
            /// Вертає логічне Or з змінних
            /// </summary>
            template<typename ... TItems>
            constexpr Boolean OrPack(TItems ... items) {
                return (... || items);
            }

            /// <summary>
            /// Додаткова структура яка вказує чи даний тип є методом класу або чи існує метод з таким іменем в класу
            /// </summary>
            template<class T>
            struct IsMemberFunctionPointerHelper : LogicFalse {};

            /// <summary>
            /// Додаткова структура яка вказує чи даний тип є методом класу або чи існує метод з таким іменем в класу
            /// </summary>
            template<class T, class TU>
            struct IsMemberFunctionPointerHelper<T TU::*> : IsFunction<T> {};

            /// <summary>
            /// Вказує чи даний тип є покажчиком на метод класу або чи існує метод з таким іменем в класу
            /// </summary>
            template<class T>
            struct IsMemberFunctionPointerT : IsMemberFunctionPointerHelper<RemoveCV<T>> {};

            /// <summary>
            /// Вказує чи даний тип є покажчиком на метод класу або чи існує метод з таким іменем в класу
            /// </summary>
            /// <typeparam name="T">Покажчик на метод</typeparam>
            template<typename T>
            constexpr bool IsMemberFunctionPointerV = IsMemberFunctionPointerT<T>::State;

			/// <summary>
			/// Струтура для тегування функцій та методів по тегу з типу
			/// </summary>
			template<typename T>
			struct TypeTag {
				/// <summary>
				/// Тип по якому тегують
				/// </summary>
				using Type = T;
			};

#pragma region Test comparing
            template<typename T, typename U, typename = VoidT<>>
            struct HasEqualT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasEqualRetT
                : LogicFalse
            {};

            template<typename T, typename U, typename = VoidT<>>
            struct HasNothrowEqualT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasNothrowEqualRetT
                : LogicFalse
            {};

            template<typename T, typename U>
            struct HasEqualT<T, U, VoidT<decltype(DeclVal<T>() == DeclVal<U>())>>
                : LogicTrue
            {};

            template<typename R, typename T, typename U>
            struct HasEqualRetT<R, T, U, VoidT<decltype(DeclVal<T>() == DeclVal<U>())>>
                : IsConvertibleT<decltype(DeclVal<T>() == DeclVal<U>()), R>
            {};

            template<typename T, typename U>
            struct HasNothrowEqualT<T, U, VoidT<decltype(DeclVal<T>() == DeclVal<U>())>>
                : LogicConst<noexcept(DeclVal<T>() == DeclVal<U>())>
            {};

            template<typename R, typename T, typename U>
            struct HasNothrowEqualRetT<R, T, U, VoidT<decltype(DeclVal<T>() == DeclVal<U>())>>
                : LogicConst<(noexcept(DeclVal<T>() == DeclVal<U>()) && IsConvertible<decltype(DeclVal<T>() == DeclVal<U>()), R>)>
            {};

            // -------------------------------------------------------------------------------------------------------------------------------------------------------

            template<typename T, typename U, typename = VoidT<>>
            struct HasNotEqualT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasNotEqualRetT
                : LogicFalse
            {};

            template<typename T, typename U, typename = VoidT<>>
            struct HasNothrowNotEqualT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasNothrowNotEqualRetT
                : LogicFalse
            {};

            template<typename T, typename U>
            struct HasNotEqualT<T, U, VoidT<decltype(DeclVal<T>() != DeclVal<U>())>>
                : LogicTrue
            {};

            template<typename R, typename T, typename U>
            struct HasNotEqualRetT<R, T, U, VoidT<decltype(DeclVal<T>() != DeclVal<U>())>>
                : IsConvertibleT<decltype(DeclVal<T>() != DeclVal<U>()), R>
            {};

            template<typename T, typename U>
            struct HasNothrowNotEqualT<T, U, VoidT<decltype(DeclVal<T>() != DeclVal<U>())>>
                : LogicConst<noexcept(DeclVal<T>() != DeclVal<U>())>
            {};

            template<typename R, typename T, typename U>
            struct HasNothrowNotEqualRetT<R, T, U, VoidT<decltype(DeclVal<T>() != DeclVal<U>())>>
                : LogicConst<(noexcept(DeclVal<T>() != DeclVal<U>()) && IsConvertible<decltype(DeclVal<T>() != DeclVal<U>()), R>)>
            {};

            // -------------------------------------------------------------------------------------------------------------------------------------------------------

            template<typename T, typename U, typename = VoidT<>>
            struct HasLessT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasLessRetT
                : LogicFalse
            {};

            template<typename T, typename U, typename = VoidT<>>
            struct HasNothrowLessT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasNothrowLessRetT
                : LogicFalse
            {};

            template<typename T, typename U>
            struct HasLessT<T, U, VoidT<decltype(DeclVal<T>() < DeclVal<U>())>>
                : LogicTrue
            {};

            template<typename R, typename T, typename U>
            struct HasLessRetT<R, T, U, VoidT<decltype(DeclVal<T>() < DeclVal<U>())>>
                : IsConvertibleT<decltype(DeclVal<T>() < DeclVal<U>()), R>
            {};

            template<typename T, typename U>
            struct HasNothrowLessT<T, U, VoidT<decltype(DeclVal<T>() < DeclVal<U>())>>
                : LogicConst<noexcept(DeclVal<T>() < DeclVal<U>())>
            {};

            template<typename R, typename T, typename U>
            struct HasNothrowLessRetT<R, T, U, VoidT<decltype(DeclVal<T>() < DeclVal<U>())>>
                : LogicConst<(noexcept(DeclVal<T>() < DeclVal<U>()) && IsConvertible<decltype(DeclVal<T>() < DeclVal<U>()), R>)>
            {};    

            // -------------------------------------------------------------------------------------------------------------------------------------------------------

            template<typename T, typename U, typename = VoidT<>>
            struct HasGreaterT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasGreaterRetT
                : LogicFalse
            {};

            template<typename T, typename U, typename = VoidT<>>
            struct HasNothrowGreaterT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasNothrowGreaterRetT
                : LogicFalse
            {};

            template<typename T, typename U>
            struct HasGreaterT<T, U, VoidT<decltype(DeclVal<T>() > DeclVal<U>())>>
                : LogicTrue
            {};

            template<typename R, typename T, typename U>
            struct HasGreaterRetT<R, T, U, VoidT<decltype(DeclVal<T>() > DeclVal<U>())>>
                : IsConvertibleT<decltype(DeclVal<T>() > DeclVal<U>()), R>
            {};

            template<typename T, typename U>
            struct HasNothrowGreaterT<T, U, VoidT<decltype(DeclVal<T>() > DeclVal<U>())>>
                : LogicConst<noexcept(DeclVal<T>() > DeclVal<U>())>
            {};

            template<typename R, typename T, typename U>
            struct HasNothrowGreaterRetT<R, T, U, VoidT<decltype(DeclVal<T>() > DeclVal<U>())>>
                : LogicConst<(noexcept(DeclVal<T>() > DeclVal<U>()) && IsConvertible<decltype(DeclVal<T>() > DeclVal<U>()), R>)>
            {};

            // -------------------------------------------------------------------------------------------------------------------------------------------------------

            template<typename T, typename U, typename = VoidT<>>
            struct HasLessEqualT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasLessEqualRetT
                : LogicFalse
            {};

            template<typename T, typename U, typename = VoidT<>>
            struct HasNothrowLessEqualT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasNothrowLessEqualRetT
                : LogicFalse
            {};    

            template<typename T, typename U>
            struct HasLessEqualT<T, U, VoidT<decltype(DeclVal<T>() <= DeclVal<U>())>>
                : LogicTrue
            {};

            template<typename R, typename T, typename U>
            struct HasLessEqualRetT<R, T, U, VoidT<decltype(DeclVal<T>() <= DeclVal<U>())>>
                : IsConvertibleT<decltype(DeclVal<T>() <= DeclVal<U>()), R>
            {};

            template<typename T, typename U>
            struct HasNothrowLessEqualT<T, U, VoidT<decltype(DeclVal<T>() <= DeclVal<U>())>>
                : LogicConst<noexcept(DeclVal<T>() <= DeclVal<U>())>
            {};

            template<typename R, typename T, typename U>
            struct HasNothrowLessEqualRetT<R, T, U, VoidT<decltype(DeclVal<T>() <= DeclVal<U>())>>
                : LogicConst<(noexcept(DeclVal<T>() <= DeclVal<U>()) && IsConvertible<decltype(DeclVal<T>() <= DeclVal<U>()), R>)>
            {};    

            // -------------------------------------------------------------------------------------------------------------------------------------------------------

            template<typename T, typename U, typename = VoidT<>>
            struct HasGreaterEqualT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasGreaterEqualRetT
                : LogicFalse
            {};

            template<typename T, typename U, typename = VoidT<>>
            struct HasNothrowGreaterEqualT
                : LogicFalse
            {};

            template<typename R, typename T, typename U, typename = VoidT<>>
            struct HasNothrowGreaterEqualRetT
                : LogicFalse
            {};    

            template<typename T, typename U>
            struct HasGreaterEqualT<T, U, VoidT<decltype(DeclVal<T>() >= DeclVal<U>())>>
                : LogicTrue
            {};

            template<typename R, typename T, typename U>
            struct HasGreaterEqualRetT<R, T, U, VoidT<decltype(DeclVal<T>() >= DeclVal<U>())>>
                : IsConvertibleT<decltype(DeclVal<T>() >= DeclVal<U>()), R>
            {};

            template<typename T, typename U>
            struct HasNothrowGreaterEqualT<T, U, VoidT<decltype(DeclVal<T>() >= DeclVal<U>())>>
                : LogicConst<noexcept(DeclVal<T>() >= DeclVal<U>())>
            {};

            template<typename R, typename T, typename U>
            struct HasNothrowGreaterEqualRetT<R, T, U, VoidT<decltype(DeclVal<T>() >= DeclVal<U>())>>
                : LogicConst<(noexcept(DeclVal<T>() >= DeclVal<U>()) && IsConvertible<decltype(DeclVal<T>() >= DeclVal<U>()), R>)>
            {};

            // -------------------------------------------------------------------------------------------------------------------------------------------------------

            template<typename T, typename U>
            inline constexpr auto HasEqualV = HasEqualT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasEqualRetV = HasEqualRetT<R, T, U>::State;

            template<typename T, typename U>
            inline constexpr auto HasNothrowEqualV = HasNothrowEqualT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasNothrowEqualRetV = HasNothrowEqualRetT<R, T, U>::State;

           // -------------------------------------------------------------------------------------------------------------------------------------------------------

            template<typename T, typename U>
            inline constexpr auto HasNotEqualV = HasNotEqualT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasNotEqualRetV = HasNotEqualRetT<R, T, U>::State;

            template<typename T, typename U>
            inline constexpr auto HasNothrowNotEqualV = HasNothrowNotEqualT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasNothrowNotEqualRetV = HasNothrowNotEqualRetT<R, T, U>::State;

            // -------------------------------------------------------------------------------------------------------------------------------------------------------

            template<typename T, typename U>
            inline constexpr auto HasLessv = HasLessT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasLessRetV = HasLessRetT<R, T, U>::State;

            template<typename T, typename U>
            inline constexpr auto HasNothrowLessTv = HasNothrowLessT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasNothrowLessRetV = HasNothrowLessRetT<R, T, U>::State;

            // -------------------------------------------------------------------------------------------------------------------------------------------------------

            template<typename T, typename U>
            inline constexpr auto HasGreaterTv = HasGreaterT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasGreaterRetV = HasGreaterRetT<R, T, U>::State;

            template<typename T, typename U>
            inline constexpr auto HasNothrowGreaterTv = HasNothrowGreaterT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasNothrowGreaterRetV = HasNothrowGreaterRetT<R, T, U>::State;

            // -------------------------------------------------------------------------------------------------------------------------------------------------------

            template<typename T, typename U>
            inline constexpr auto HasLessEqualTV = HasLessEqualT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasLessEqualRetV = HasLessEqualRetT<R, T, U>::State;

            template<typename T, typename U>
            inline constexpr auto HasNothrowLessEqualTV = HasNothrowLessEqualT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasNothrowLessEqualRetV = HasNothrowLessEqualRetT<R, T, U>::State;

            // -------------------------------------------------------------------------------------------------------------------------------------------------------

            template<typename T, typename U>
            inline constexpr auto HasGreaterEqualTV = HasGreaterEqualT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasGreaterEqualRetV = HasGreaterEqualRetT<R, T, U>::State;

            template<typename T, typename U>
            inline constexpr auto HasNothrowGreaterEqualTV = HasNothrowGreaterEqualT<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto HasNothrowGreaterEqualRetV = HasNothrowGreaterEqualRetT<R, T, U>::State;    


        template<typename T, typename U>
            struct IsComparable
                : LogicConst<
                      And<
                          HasEqualT<T, U>,
                          HasNotEqualT<T, U>,
                          HasLessT<T, U>,
                          HasGreaterT<T, U>,
                          HasLessEqualT<T, U>,
                          HasGreaterEqualT<T, U>
                      > // And
                  > // LogicConst
            {}; // IsComparable

            template<typename R, typename T, typename U>
            struct IsComparableRet
                : LogicConst<
                      And<
                          HasEqualRetT<R, T, U>,
                          HasNotEqualRetT<R, T, U>,
                          HasLessRetT<R, T, U>,
                          HasGreaterRetT<R, T, U>,
                          HasLessEqualRetT<R, T, U>,
                          HasGreaterEqualRetT<R, T, U>
                      > // And
                  > // LogicConst
            {}; // IsComparableRet

            template<typename T, typename U>
            struct IsNothrowComparable
                : LogicConst<
                      And<
                          HasNothrowEqualT<T, U>,
                          HasNothrowNotEqualT<T, U>,
                          HasNothrowLessT<T, U>,
                          HasNothrowGreaterT<T, U>,
                          HasNothrowLessEqualT<T, U>,
                          HasNothrowGreaterEqualT<T, U>
                      > // And
                  > // LogicConst
            {}; // IsNothrowComparable

            template<typename R, typename T, typename U>
            struct IsNothrowComparableRet
                : LogicConst<
                      And<
                          HasNothrowEqualRetT<R, T, U>,
                          HasNothrowNotEqualRetT<R, T, U>,
                          HasNothrowLessRetT<R, T, U>,
                          HasNothrowGreaterRetT<R, T, U>,
                          HasNothrowLessEqualRetT<R, T, U>,
                          HasNothrowGreaterEqualRetT<R, T, U>
                      > // And
                  > // LogicConst
            {}; // IsNothrowComparableRet

            template<typename T, typename U>
            inline constexpr auto IsComparableV = IsComparable<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto IsComparableRetV = IsComparableRet<R, T, U>::State;

            template<typename T, typename U>
            inline constexpr auto IsNothrowComparableV = IsNothrowComparable<T, U>::State;

            template<typename R, typename T, typename U>
            inline constexpr auto IsNothrowComparableRetV = IsNothrowComparableRet<R, T, U>::State;    
#pragma endregion
        }
    }
}

#define IS_CONSTEXPR(...)  !noexcept(Seed::Core::TypeTraits::Details::IsConstexprHelper(__VA_ARGS__))  // NOLINT(cppcoreguidelines-macro-usage)
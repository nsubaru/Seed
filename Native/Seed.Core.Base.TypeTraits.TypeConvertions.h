#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"
#include "Seed.Core.Base.TypeTraits.Utils.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
            /// <summary>
            /// Перевіряє чи тип має конструктор з заданими типами
            /// </summary>
			template<class TTy,
                class... TArgs>
                struct IsConstructibleT
                : LogicConst<__is_constructible(TTy, TArgs...)>
            {	// determine whether TTy(_Args...) is constructible
            };

            /// <summary>
            /// Перевіряє чи тип має конструктор з заданими типами
            /// </summary>
            template<class TTy,
                class... TArgs>
                inline constexpr bool IsConstructible = __is_constructible(TTy, TArgs...);

            /// <summary>
            /// Перевіряє чи один тип можна перетворити в другий
            /// </summary>
            template<class TFrom,
                class TTo>
                struct IsConvertibleT
                : LogicConst<__is_convertible_to(TFrom, TTo)>
            {	// determine whether _From is convertible to _To
            };

            /// <summary>
            /// Перевіряє чи один тип можна перетворити в другий
            /// </summary>
            template<class TFrom,
                class TTo>
                inline constexpr bool IsConvertible = __is_convertible_to(TFrom, TTo);

            /// <summary>
            /// Перевіряє чи тип має конструктор з заданими типами без генерації помилок
            /// </summary>
            template<class TTy,
                class... _Args>
                struct IsNothrowConstructibleT
                : LogicConst<__is_nothrow_constructible(TTy, _Args...)>
            {	// determine whether TTy(_Args...) is nothrow constructible
            };

            /// <summary>
            /// Перевіряє чи тип має конструктор з заданими типами без генерації помилок
            /// </summary>
            template<class TTy,
                class... _Args>
                inline constexpr bool IsNothrowConstructible = IsNothrowConstructibleT<TTy, _Args...>::State;

            /// <summary>
            /// Перевіряй чи значення одного типу можна присвоїти значенню іншого
            /// </summary>
            template<class TTo,
                class TFrom>
                struct IsAssignableT
                : LogicConst<__is_assignable(TTo, TFrom)>
            {	// determine whether _From can be assigned to _To
            };

            /// <summary>
            /// Перевіряй чи значення одного типу можна присвоїти значенню іншого
            /// </summary>
            template<class TTo,
                class TFrom>
                inline constexpr bool IsAssignable = IsAssignableT<TTo, TFrom>::State;

            /// <summary>
            /// Перевіряй чи значення одного типу можна присвоїти значенню іншого без генерації помилок
            /// </summary>
            template<class TTo,
                class TFrom>
                struct IsNothrowAssignableT
                : LogicConst<__is_nothrow_assignable(TTo, TFrom)>
            {	// determine whether _From can be assigned to _To, nothrow
            };

            /// <summary>
            /// Перевіряй чи значення одного типу можна присвоїти значенню іншого без генерації помилок
            /// </summary>
            template<class TTo,
                class TFrom>
                inline constexpr bool IsNothrowAssignable = __is_nothrow_assignable(TTo, TFrom);

            
            /// <summary>
            /// Перевіряє чи тип має конструктор без аргументів
            /// </summary>
            template<class TTy>
            struct IsDefaultConstructibleT
                : LogicConst<__is_constructible(TTy)>
            {	// determine whether TTy has a default constructor
            };

            /// <summary>
            /// Перевіряє чи тип має конструктор без аргументів
            /// </summary>
            template<class TTy>
            inline constexpr bool IsDefaultConstructible = __is_constructible(TTy);

            /// <summary>
            /// Перевіряє чи тип має неявний конструктор без аргументів
            /// </summary>
            template<class TTy,
                class = void>
                struct IsImplicitlyDefaultConstructibleT
                : LogicFalse
            {	// determine whether TTy is implicitly default constructible
            };

            namespace Details
            {
                /// <summary>
                /// Допоміжна функція для перевірки неявного створення об'єкта
                /// </summary>
                template<class TTy>
                void ImplicitlyDefaultConstruct(const TTy&) = delete;
            }

            /// <summary>
            /// Перевіряє чи тип має неявний конструктор без аргументів
            /// </summary>
            template<class TTy>
            struct IsImplicitlyDefaultConstructibleT < TTy, VoidT<decltype(Details::ImplicitlyDefaultConstruct<TTy>({})) >>
                : LogicTrue
            {	// determine whether TTy is implicitly default constructible
            };

            /// <summary>
            /// Перевіряє чи тип має неявний конструктор без аргументів та без генерації помилок
            /// </summary>
            template<class TTy>
            struct IsNothrowDefaultConstructibleT
                : LogicConst<__is_nothrow_constructible(TTy)>
            {	// determine whether TTy has a nothrow default constructor
            };

            /// <summary>
            /// Перевіряє чи тип має неявний конструктор без аргументів та без генерації помилок
            /// </summary>
            template<class TTy>
            inline constexpr bool IsNothrowDefaultConstructible = __is_nothrow_constructible(TTy);

            /// <summary>
            /// Перевіряє чи тип має оператор переміщення
            /// </summary>
            template<class TTy>
            struct IsMoveAssignableT
                : LogicConst<__is_assignable(AddLvalueReferenceV<TTy>, TTy)>
            {	// determine whether TTy has a move assignment operator
            };

            template<class TTy>
            inline constexpr bool IsMoveAssignable = __is_assignable(AddLvalueReferenceV<TTy>, TTy);

            /// <summary>
            /// Перевіряє чи тип має оператор переміщення без генерації помилок
            /// </summary>
            template<class TTy>
            struct IsNothrowMoveAssignableT
                : LogicConst<__is_nothrow_assignable(AddLvalueReferenceV<TTy>, TTy)>
            {	// determine whether TTy has a nothrow move assignment operator
            };

            /// <summary>
            /// Перевіряє чи тип має оператор переміщення без генерації помилок
            /// </summary>
            template<class TTy>
            inline constexpr bool IsNothrowMoveAssignable = __is_nothrow_assignable(AddLvalueReferenceV<TTy>, TTy);

            /// <summary>
            /// Перевіряє чи тип має допоміжну функцію переміщення
            /// </summary>
            template<class TTy1,
                class TTy2,
                class = void>
                struct SwappableWithHelper
                : LogicFalse
            {	// swap(declval<TTy1>(), declval<TTy2>()) is not valid
            };

            /// <summary>
            /// Перевіряє чи тип має допоміжну функцію переміщення
            /// </summary>
            template<class TTy1,
                class TTy2>
                struct SwappableWithHelper<TTy1, TTy2, VoidT<decltype(Swap(Declval<TTy1>(), Declval<TTy2>()))>>
                : LogicTrue
            {	// swap(declval<TTy1>(), declval<TTy2>()) is valid
            };

            /// <summary>
            /// Перевіряє чи два типа можна обміняти місцями
            /// </summary>
            template<class TTy1,
                class TTy2>
                struct IsSwappableWith
                : LogicConst<
                SwappableWithHelper<TTy1, TTy2>::State &&
                SwappableWithHelper<TTy2, TTy1>::State>
            {	// Determine if expressions with type and value category TTy1 and TTy2
                // can be swapped (and vice versa)
            };

            /// <summary>
            /// Перевірка чи тип має операцію обміну
            /// </summary>
            template<class TTy>
            struct IsSwappable
                : LogicConst<IsSwappableWith<
                AddLvalueReferenceV<TTy>,
                AddLvalueReferenceV<TTy>>::State>
            {	// Determine if TTy lvalues satisfy is_swappable_with
            };

            /// <summary>
            /// Вертає true якщо змінну типу можна присвоїти копіюванням
            /// </summary>
            template <class TTy>
            struct IsCopyAssignableT
                : LogicConst<__is_assignable(AddLvalueReferenceV<TTy>, AddLvalueReferenceV<const TTy>)> {
            };

            /// <summary>
            /// Вертає true якщо змінну типу можна присвоїти копіюванням
            /// </summary>
            template <class TTy>
            inline constexpr bool IsCopyAssignableV = __is_assignable(
                AddLvalueReferenceV<TTy>, AddLvalueReferenceV<const TTy>);
		}
	}
}
#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
#pragma region Conditional
			///<summary>Compile-time логічне судження, якщо Cond == True, вертає тип IfTrue, інакше IfFalse</summary>
            template<Boolean TCond, typename TIfTrue, typename TIfFalse>
            struct ConditionT { using Result = TIfFalse; };

            ///<summary>Compile-time логічне судження, якщо Cond == True, вертає тип IfTrue, інакше IfFalse</summary>
            template<typename TIfTrue, typename TIfFalse>
            struct ConditionT<true, TIfTrue, TIfFalse> { using Result = TIfTrue; };

            ///<summary>Compile-time результат логічного судження</summary>
            template<Boolean TCond, typename TIfTrue, typename TIfFalse>
            using ConditionV = typename ConditionT<TCond, TIfTrue, TIfFalse>::Result;
#pragma endregion
#pragma region EnableIf
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
#pragma endregion
#pragma region And
            /// <summary>
            /// Реалізація логічного and для мета програмування
            /// </summary>
            template <bool FirstValue, class TFirst,
                class... TRest>
                struct AndTImpl { // handle false trait or last trait
                using Result = TFirst;
            };

            /// <summary>
            /// Реалізація логічного and для мета програмування
            /// </summary>
            template <class TTrue, class TNext, class... TRest>
            struct AndTImpl<true, TTrue, TNext, TRest...> { // the first trait is true, try the next one
                using Result = typename AndTImpl<TNext::State, TNext, TRest...>::Result;
            };

            /// <summary>
            /// Реалізація логічного and для мета програмування
            /// </summary>
            template <class... TTypes>
            struct AndT : LogicTrue { // If _Traits is empty, LogicTrue
            };

            /// <summary>
            /// Реалізація логічного and для мета програмування
            /// </summary>
            template <class TFirst, class... TRest>
            struct AndT<TFirst, TRest...>
                : LogicConstT<typename AndTImpl<TFirst::State, TFirst, TRest...>::Result> { // Otherwise, if any of _Traits are false, the first false
                                                                        // trait Otherwise, the last trait in _Traits
            };
            
            /// <summary>
            /// Реалізація логічного and для мета програмування
            /// </summary>
            template <class... TTypes>
            constexpr bool And = AndT<TTypes...>::State;
#pragma endregion
#pragma region Negation
            /// <summary>
            /// Реалізаця логічного заперечення для мета програмування
            /// </summary>
            template<class TType>
            struct NegationT
                : LogicConst<!static_cast<bool>(TType::State)>
            {	// The negated result of _Trait
            };

            /// <summary>
            /// Реалізаця логічного заперечення для мета програмування
            /// </summary>
            template<class TType>
            inline constexpr bool Negation = NegationT<TType>::State;
#pragma endregion
#pragma region Or
            /// <summary>
            /// Реалізаця логічного "або" для мета програмування
            /// </summary>
            template<bool FirstValue,
                class TFirst,
                class... TRest>
                struct Disjunction
            {	// handle true trait or last trait
                using type = TFirst;
            };

            template<class TFalse,
                class TNext,
                class... TRest>
                struct Disjunction<false, TFalse, TNext, TRest...>
            {	// first trait is false, try the next trait
                using Type = typename Disjunction<TNext::State, TNext, TRest...>::Type;
            };

            template<class... _Traits>
            struct OrT
                : LogicFalse
            {	// If _Traits is empty, falseTType
            };

            template<class TFirst,
                class... TRest>
                struct OrT<TFirst, TRest...>
                : Disjunction<TFirst::State, TFirst, TRest...>::Type
            {	// Otherwise, if any of _Traits are true, the first true trait
                // Otherwise, the last trait in _Traits
            };

            template<class... TTypes>
            inline constexpr bool Or = OrT<TTypes...>::State;
#pragma endregion
#pragma region Logic operations with constexpr variable 
            /// <summary>
            /// Вертає логічне And з змінних
            /// </summary>
            template<typename ... TItems>
            constexpr Boolean AndV(TItems ... items) {
                return (... && items);
            }

            /// <summary>
            /// Вертає логічне Or з змінних
            /// </summary>
            template<typename ... TItems>
            constexpr Boolean OrV(TItems ... items) {
                return (... || items);
            }
#pragma endregion
		}
	}
}
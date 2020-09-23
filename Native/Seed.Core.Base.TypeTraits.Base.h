#pragma once
#include "Seed.Core.Base.Types.h"

namespace Seed {
    inline namespace Core {
        ///<summary>Простір імен статичних перевірок</summary>
        namespace TypeTraits {
            /// <summary>
            /// Абстрактний тип пустоти
            /// </summary>
            template<class... _Types>
			using VoidT = void;

			/// <summary>
			/// Шаблон логічної константи, використовується для логічних суджень в compile-time
			/// </summary>
			template<bool value>
			struct LogicConst
			{
				__forceinline constexpr operator bool() const { return value; }
				static constexpr bool State = value;
			};

			/// <summary>
			/// Шаблон логічної константи, використовується для логічних суджень в compile-time
			/// </summary>
			template<typename value>
			struct LogicConstT
			{
				__forceinline constexpr operator bool() const { return value::State; }
				static constexpr bool State = value::State;
			};

			/// <summary>
			/// Логічна істина для статичних перевірок
			/// </summary>
			inline namespace { typedef LogicConst<true> LogicTrue; }

			/// <summary>
			/// Логічна хиба для статичних перевірок
			/// </summary>
			inline namespace { typedef LogicConst<false> LogicFalse; }

            /// <summary>
            /// Логічне судження чи тип T1 == T2 виконується в compile-time
            /// </summary>
            template<typename T1, typename T2>
            struct IsSameT : LogicFalse {};

            /// <summary>
            /// Логічне судження чи тип T1 == T2 виконується в compile-time
            /// </summary>
            template<typename T>
            struct IsSameT<T, T> : LogicTrue {};

            /// <summary>
            /// Вертає true якщо типи ідентичні
            /// </summary>
            template<typename T1, typename T2>
            constexpr bool IsSameV = IsSameT<T1, T2>::State;

            /// <summary>
            /// Compile-time логічне судження, якщо клас Derivde наслідує клас Base то вертає True інакше False
            /// </summary>
            template<typename TBase, typename TDerived>
            struct IsBasedOn :LogicConst<__is_base_of(TBase, TDerived)> {};

            /// <summary>
            /// Compile-time логічне судження, якщо клас Derivde наслідує клас Base то вертає True інакше False
            /// </summary>
            template<typename TBase, typename TDerived>
            constexpr Boolean IsBasedOnV = __is_base_of(TBase, TDerived);

            ///<summary>Деталі реалізації</summary>
            namespace Details
            {
                /// <summary>
                /// Помічник для визначення чи функція при компіляції тайм виконається
                /// </summary>
                template <class T>
                constexpr void IsConstexprHelper(T&& t) {}
            }

            //Вертає чи даний тип є void
            template<class TTy>
            struct IsVoidT
                : LogicFalse
            {	
            };

            //Вертає чи даний тип є void
            template<>
            struct IsVoidT<void>
                : LogicTrue
            {	
            };

            //Вертає чи даний тип є void
            template<>
            struct IsVoidT<const void>
                : LogicTrue
            {	
            };

            //Вертає чи даний тип є void
            template<>
            struct IsVoidT<volatile void>
                : LogicTrue
            {	
            };

            //Вертає чи даний тип є void
            template<>
            struct IsVoidT<const volatile void>
                : LogicTrue
            {	
            };

            //Вертає чи даний тип є void
            template<class TTy>
            inline constexpr bool IsVoidV = IsVoidT<TTy>::State;

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
			/// Струтура для тегування функцій та методів по тегу з типу
			/// </summary>
			template<typename T>
			struct TypeTag {
				/// <summary>
				/// Тип по якому тегують
				/// </summary>
				using Type = T;
			};
        }
    }
}

#define IS_CONSTEXPR(...)  !noexcept(Seed::Core::TypeTraits::Details::IsConstexprHelper(__VA_ARGS__)) 
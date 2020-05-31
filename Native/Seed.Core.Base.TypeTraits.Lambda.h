#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"
#include "Seed.Core.Base.TypeTraits.Pack.h"
#include "Seed.Core.Base.TypeTraits.LogicOperations.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
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
		}
	}
}
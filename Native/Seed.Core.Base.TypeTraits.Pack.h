#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
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
	}
}
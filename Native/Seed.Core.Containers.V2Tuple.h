#pragma once
#include "Seed.Core.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Containers
		{
			namespace LINQ
			{
				class ITupleGenericBase {};
			}

			namespace Details
			{
				template<SizeT I, typename T, typename ... TRest>
				struct TypeGetter {
					using Type = typename TypeGetter<I - 1, TRest...>::Type;
				};

				template<typename T, typename ... TRest>
				struct TypeGetter<0, T, TRest...> {
					using Type = T;
				};

				template<SizeT Idx, typename T>
				struct V2Leaf {
					T value;

					ForceInline constexpr V2Leaf() requires (Concepts::IsDefaultConstructible<T>) : value() {}

					ForceInline constexpr V2Leaf(T&& value) : value(ForwardReference<T&&>(value)) {}

					ForceInline constexpr V2Leaf(const V2Leaf& leaf) : value(leaf.value) {}
					ForceInline constexpr V2Leaf(V2Leaf&& leaf) : value(ForwardReference<T&&>(leaf.value)) {}
				};

				template<typename TIndexer, typename ...TTypes>
				class V2Leafs;

				template<SizeT ... Idxs, typename ... TTypes>
				struct V2Leafs<typename TypeTraits::Indexator<Idxs...>, TTypes...> : V2Leaf<(sizeof...(TTypes) - 1) - Idxs, TTypes>... {
					static_assert(sizeof...(Idxs) == sizeof...(TTypes));
					static constexpr SizeT Count = sizeof...(TTypes);

					constexpr V2Leafs() requires (Concepts::IsDefaultConstructible<TTypes> && ...) : V2Leaf<(sizeof...(TTypes) - 1) - Idxs, TTypes>()... {}

					constexpr V2Leafs(TTypes&&... data) : V2Leaf<(sizeof...(TTypes) - 1) - Idxs, TTypes>(ForwardReference<TTypes&&>(data))... {}

					constexpr V2Leafs(const V2Leafs& leafs) : V2Leaf<(sizeof...(TTypes) - 1) - Idxs, TTypes>(static_cast<const V2Leaf<(sizeof...(TTypes) - 1) - Idxs, TTypes>&>(leafs)) ... {}
					constexpr V2Leafs(V2Leafs&& leafs) : V2Leaf<(sizeof...(TTypes) - 1) - Idxs, TTypes>(static_cast<V2Leaf<(sizeof...(TTypes) - 1) - Idxs, TTypes>&&>(leafs)) ... {}

					template<SizeT Idx> requires (sizeof...(TTypes) > Idx)
					using TypeByIdx = typename TypeGetter<Idx, TTypes...>::Type;

					template<SizeT Idx> 
						requires (sizeof...(TTypes) > Idx)
					ForceInline constexpr typename TypeByIdx<Idx> GetByIdx() {
						return static_cast<V2Leaf<Idx, typename TypeGetter<Idx, TTypes...>::Type>&>(*this).value;
					}

					template<SizeT Idx> 
						requires (sizeof...(TTypes) > Idx)
					ForceInline constexpr typename TypeByIdx<Idx> GetByIdx() const {
						return static_cast<const V2Leaf<Idx, typename TypeGetter<Idx, TTypes...>::Type>&>(*this).value;
					}

					template<SizeT Idx>
						requires (sizeof...(TTypes) > Idx)
					ForceInline constexpr typename TypeTraits::AddLvalueReferenceV<TypeTraits::AddConstV<TypeByIdx<Idx>>> GetByIdxAsRef() const {
						return static_cast<const V2Leaf<Idx, typename TypeGetter<Idx, TTypes...>::Type>&>(*this).value;
					}

					template<SizeT Idx>
						requires (sizeof...(TTypes) > Idx)
					ForceInline constexpr typename TypeTraits::AddLvalueReferenceV<TypeByIdx<Idx>> GetByIdxAsRef() {
						return static_cast<V2Leaf<Idx, typename TypeGetter<Idx, TTypes...>::Type>&>(*this).value;
					}
				};
			}

			template<typename ... TTypes>
			class V2Tuple;

			namespace Details
			{
				template<typename F, typename TTuple, typename TIndexator>
				struct Executor;

				template<typename F, typename ... TTypes, SizeT ... Idxs>
				struct Executor<F, V2Tuple<TTypes...>, typename TypeTraits::Indexator<Idxs...>> {
					ForceInline constexpr static decltype(auto) Execute(F&& f, const V2Tuple<TTypes...>& tuple) {
						if constexpr (TypeTraits::IsSameV<TypeTraits::InvockableResult<F, TTypes...>, void>)
						{
							f(tuple.GetByIdx<(sizeof...(TTypes) - 1) - Idxs>()...);
						}
						else
						{
							return f(tuple.GetByIdx<(sizeof...(TTypes) - 1) - Idxs>()...);
						}
					}

					ForceInline constexpr static decltype(auto) Execute(F&& f, V2Tuple<TTypes...>& tuple) {
						if constexpr (TypeTraits::IsSameV<TypeTraits::InvockableResult<F, TTypes...>, void>)
						{
							f(tuple.GetByIdx<(sizeof...(TTypes) - 1) - Idxs>()...);
						}
						else
						{
							return f(tuple.GetByIdx<(sizeof...(TTypes) - 1) - Idxs>()...);
						}
					}
				};

				template<typename F, typename TTuple, typename TIndexator>
				struct Convertor;

				template<typename F, typename ... TTypes, SizeT ... Idxs>
				struct Convertor<F, V2Tuple<TTypes...>, typename TypeTraits::Indexator<Idxs...>> {
					ForceInline constexpr static decltype(auto) Convert(F&& f, V2Tuple<TTypes...>& tuple) {
						return V2Tuple<decltype(f(tuple.GetByIdx<Idxs>()))...>(f(tuple.GetByIdx<Idxs>())...);
					}

					ForceInline constexpr static decltype(auto) Convert(F&& f, const V2Tuple<TTypes...>& tuple) {
						return V2Tuple<decltype(f(tuple.GetByIdx<Idxs>()))...>(f(tuple.GetByIdx<Idxs>())...);
					}
				};
			}

			/// <summary>
			/// Кортеж
			/// </summary>
			/// <typeparam name="...TTypes">Типи кортежу</typeparam>
			template<typename ... TTypes>
			class V2Tuple : LINQ::ITupleGenericBase {
			private:
				using TV2Leafs = typename Details::V2Leafs<typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type, TTypes...>;
			private:
				TV2Leafs leafs;
			public:
				/// <summary>
				/// Кількість елементів
				/// </summary>
				static constexpr SizeT Count = TV2Leafs::Count;
			public:
				/// <summary>
				/// Пустий конструктор
				/// </summary>
				ForceInline constexpr V2Tuple() requires (Concepts::IsDefaultConstructible<TTypes> && ...) : leafs() {}
			public:
				/// <summary>
				/// Конструктор з елементів
				/// </summary>
				ForceInline constexpr V2Tuple(TTypes... data) : leafs(ForwardReference<TTypes>(data)...) {}
			public:
				/// <summary>
				/// Копіювальний конструктор
				/// </summary>
				/// <param name="tuple">Оригінал</param>
				ForceInline constexpr V2Tuple(const V2Tuple& tuple) : leafs(tuple.leafs) {}

				/// <summary>
				/// Конструктор переміщення
				/// </summary>
				/// <param name="tuple">Оригінал</param>
				ForceInline constexpr V2Tuple(V2Tuple&& tuple) : leafs(MoveRValue(tuple.leafs)) {}
			public:
				/// <summary>
				/// Оператор копіювання
				/// </summary>
				/// <param name="tuple">Оригінал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				ForceInline constexpr V2Tuple& operator=(const V2Tuple& tuple) {
					this->~V2Tuple();
					this->V2Tuple::V2Tuple(tuple);
					return *this;
				}

				/// <summary>
				/// Оператор переміщення
				/// </summary>
				/// <param name="tuple">Оригінал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				ForceInline constexpr V2Tuple& operator=(V2Tuple&& tuple) {
					this->~V2Tuple();
					this->V2Tuple::V2Tuple(MoveRValue(tuple));
					return *this;
				}
			public:
				/// <summary>
				/// Вертає елемент по індексу
				/// </summary>
				/// <typeparam name="Idx"> Індекс елементу від нуля</typeparam>
				/// <returns> Копію елемента</returns>
				template<SizeT Idx> 
					requires (sizeof...(TTypes) > Idx)
				ForceInline constexpr decltype(auto) GetByIdx() {
					return leafs.GetByIdx<Idx>();
				}

				/// <summary>
				/// Вертає елемент по індексу
				/// </summary>
				/// <typeparam name="Idx"> Індекс елементу від нуля</typeparam>
				/// <returns> Копію елемента</returns>
				template<SizeT Idx>
					requires (sizeof...(TTypes) > Idx)
				ForceInline constexpr decltype(auto) GetByIdx() const {
					return leafs.GetByIdx<Idx>();
				}

				/// <summary>
				/// Вертає посилання на елемент по індексу
				/// </summary>
				/// <typeparam name="Idx"> Індекс елементу від нуля</typeparam>
				/// <returns> Посилання на елемент</returns>
				template<SizeT Idx>
					requires (sizeof...(TTypes) > Idx)
				ForceInline constexpr decltype(auto) GetByIdxAsRef() const {
					return leafs.GetByIdxAsRef<Idx>();
				}

				/// <summary>
				/// Вертає посилання на елемент по індексу
				/// </summary>
				/// <typeparam name="Idx"> Індекс елементу від нуля</typeparam>
				/// <returns> Константне посилання на елемент</returns>
				template<SizeT Idx>
					requires (sizeof...(TTypes) > Idx)
				ForceInline constexpr decltype(auto) GetByIdxAsRef() {
					return leafs.GetByIdxAsRef<Idx>();
				}
			public:
				/// <summary>
				/// Тип елемента по ідексу
				/// </summary>
				template<SizeT Idx>
				using ItemType = typename TV2Leafs::template TypeByIdx<Idx>;
			public:
				/// <summary>
				/// Викликає функтор f з аргументами з кортежу
				/// </summary>
				/// <param name = "f"> Функтор який потрібно викликати </param>
				/// <typeparam name="F"> Тип функтора</typeparam>
				/// <returns> Результат функтора якщо він є </returns>
				template<typename F>
				ForceInline constexpr decltype(auto) CallWithArgsFromTuple(F&& f) const {
					using TExecutor = typename Details::Executor<F, V2Tuple<TTypes...>, typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

					if constexpr (TypeTraits::IsSameV<TypeTraits::InvockableResult<F, TTypes...>, void>)
					{
						TExecutor::Execute(ForwardReference<F&&>(f), *this);
					}
					else
					{
						return TExecutor::Execute(ForwardReference<F&&>(f), *this);
					}
				}

				/// <summary>
				/// Викликає функтор f з аргументами з кортежу
				/// </summary>
				/// <param name = "f"> Функтор який потрібно викликати </param>
				/// <typeparam name="F"> Тип функтора</typeparam>
				/// <returns> Результат функтора якщо він є </returns>
				template<typename F>
				ForceInline constexpr decltype(auto) CallWithArgsFromTuple(F&& f) {
					using TExecutor = typename Details::Executor<
						F, 
						V2Tuple<TTypes...>, 
						typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

					if constexpr (TypeTraits::IsSameV<TypeTraits::InvockableResult<F, TTypes...>, void>)
					{
						TExecutor::Execute(ForwardReference<F&&>(f), *this);
					}
					else
					{
						return TExecutor::Execute(ForwardReference<F&&>(f), *this);
					}
				}
			public:
				/// <summary>
				/// Конвертує кортеж в інший кортеж використовуючи лямбду конвертор
				/// </summary>
				/// <typeparam name="TLambda">Тип лямбди конвертора</typeparam>
				/// <param name="lambda">Лямбда конвертор</param>
				/// <returns>Новий кортеж з копією елементів</returns>
                template<typename TLambda>
                ForceInline constexpr decltype(auto) ConvertTo(TLambda&& lambda) const {
					using Convertor = typename Details::Convertor<
						TLambda,
						V2Tuple<TTypes...>, 
						typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

					return Convertor::Convert(MoveRValue(lambda), *this);
                }

				/// <summary>
				/// Конвертує кортеж в інший кортеж використовуючи лямбду конвертор
				/// </summary>
				/// <typeparam name="TLambda">Тип лямбди конвертора</typeparam>
				/// <param name="lambda">Лямбда конвертор</param>
				/// <returns>Новий кортеж з копією елементів</returns>
				template<typename TLambda>
				ForceInline constexpr decltype(auto) ConvertTo(TLambda&& lambda) {
					using Convertor = typename Details::Convertor<
						TLambda,
						V2Tuple<TTypes...>,
						typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

					return Convertor::Convert(MoveRValue(lambda), *this);
				}
			};

			/// <summary>
			/// Пустий кортеж
			/// </summary>
			template<>
			class V2Tuple<> : LINQ::ITupleGenericBase {
			public:
				/// <summary>
				/// Пустий конструктор
				/// </summary>
				constexpr V2Tuple() {}

				/// <summary>
				/// Конструктор копіювання
				/// </summary>
				/// <param name="orig">Оригінал</param>
				constexpr V2Tuple(const V2Tuple& orig) = default;

				/// <summary>
				/// Конструктор переміщення
				/// </summary>
				/// <param name="orig">Оригінал</param>
				constexpr V2Tuple(V2Tuple&& orig) = default;
			public:
				/// <summary>
				/// Оператор копіювання
				/// </summary>
				/// <param name="orig">Оригінал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				constexpr V2Tuple& operator=(const V2Tuple& orig) = default;

				/// <summary>
				/// Оператор переміщення
				/// </summary>
				/// <param name="orig">Оригінал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				constexpr V2Tuple& operator=(V2Tuple&& orig) = default;
			public:
				/// <summary>
				/// Кількість елементів
				/// </summary>
				static constexpr SizeT Count = 0;
			public:
				/// <summary>
				/// Викликає функтор f з аргументами з кортежу
				/// </summary>
				/// <param name = "f"> Функтор який потрібно викликати </param>
				/// <typeparam name="F"> Тип функтора</typeparam>
				/// <returns> Результат функтора якщо він є </returns>
				template<typename F>
				constexpr decltype(auto) CallWithArgsFromTuple(F&& f) {
					if constexpr (TypeTraits::IsSameV<TypeTraits::InvockableResult<F>, void>)
					{
						f();
					}
					else
					{
						return f();
					}
				}

				/// <summary>
				/// Викликає функтор f з аргументами з кортежу
				/// </summary>
				/// <param name = "f"> Функтор який потрібно викликати </param>
				/// <typeparam name="F"> Тип функтора</typeparam>
				/// <returns> Результат функтора якщо він є </returns>
				template<typename F>
				constexpr decltype(auto) CallWithArgsFromTuple(F&& f) const {
					if constexpr (TypeTraits::IsSameV<TypeTraits::InvockableResult<F>, void>)
					{
						f();
					}
					else
					{
						return f();
					}
				}
			};

			template<typename ... TTypes>
			V2Tuple(TTypes... data)->V2Tuple<TTypes...>;

			/// <summary>
			/// Аліас кортежу
			/// </summary>
			/// <typeparam name="...TTypes">Типи кортежу</typeparam>
			template<typename ... TTypes>
			using Tuple = V2Tuple<TTypes...>;
		}

		namespace TypeTraits
		{
			template<typename T>
			struct IsTupleT : LogicFalse {};

			template<typename ... Types>
			struct IsTupleT<Containers::V2Tuple<Types...>> : LogicTrue {};

			template<typename T>
			constexpr bool IsTupleV = IsTupleT<T>::State;
		}

		namespace Concepts
		{
			template<typename T>
			concept Tuple = TypeTraits::IsTupleV<T>;
		}
	}
}

namespace std
{
	template<typename ... TTypes>
	using V2Tuple = Seed::Core::Containers::V2Tuple<TTypes...>;

#pragma region Get item
	template <size_t TI, class... TType>
	ForceInline constexpr decltype(auto) get(V2Tuple<TType...>& tuple) {
		return tuple.template GetByIdx<TI>();
	}

	template <size_t TI, class... TType>
	ForceInline constexpr decltype(auto) get(const V2Tuple<TType...>& tuple) {
		return tuple.template GetByIdx<TI>();
	}

	template <size_t TI, class... TType>
	ForceInline constexpr decltype(auto) get(V2Tuple<TType...>&& tuple) {
		return tuple.template GetByIdx<TI>();
	}

	template <size_t TI, class... TType>
	ForceInline constexpr decltype(auto) get(const V2Tuple<TType...>&& tuple) {
		return tuple.template GetByIdx<TI>();
	}
#pragma endregion

#pragma region Tuple element
	template <size_t _Index, class _Tuple>
	struct tuple_element;

	template <size_t TI, typename ... TTypes>
	struct tuple_element<TI, V2Tuple<TTypes...>> {
		using ItemType = typename V2Tuple<TTypes...>::template ItemType<TI>;
		using type = ItemType;
	};

	template <size_t TI, typename ... TTypes>
	struct tuple_element<TI, const V2Tuple<TTypes...>> {
		using ItemType = typename V2Tuple<TTypes...>::template ItemType<TI>;
		using ConstItemType = Seed::TypeTraits::AddConstV<ItemType>;
		using type = ConstItemType;
	};

	template <size_t TI, typename ... TTypes>
	struct tuple_element<TI, volatile V2Tuple<TTypes...>> {
		using ItemType = typename V2Tuple<TTypes...>::template ItemType<TI>;
		using VolatileItemType = Seed::TypeTraits::AddVolatileV<ItemType>;
		using type = VolatileItemType;
	};

	template <size_t TI, typename ... TTypes>
	struct tuple_element<TI, const volatile V2Tuple<TTypes...>> {
		using ItemType = typename V2Tuple<TTypes...>::template ItemType<TI>;
		using CVItemType = Seed::TypeTraits::AddCvV<ItemType>;
		using type = CVItemType;
	};
#pragma endregion

#pragma region Tuple size
	template<class T>
	struct tuple_size;

	template<typename ... TTypes>
	struct tuple_size<V2Tuple<TTypes...>> {

		static constexpr size_t value = sizeof...(TTypes);

		ForceInline constexpr operator size_t() const {
			return value;
		}

		ForceInline constexpr size_t operator()() const {
			return value;
		}

		using value_type = size_t;
	};
#pragma endregion
}
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

					constexpr V2Leaf() requires (Concepts::IsDefaultConstructible<T>) : value() {}

					constexpr V2Leaf(T&& value) : value(ForwardReference<T&&>(value)) {}

					constexpr V2Leaf(const V2Leaf& leaf) : value(leaf.value) {}
					constexpr V2Leaf(V2Leaf&& leaf) : value(ForwardReference<T&&>(leaf.value)) {}
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
					constexpr typename TypeByIdx<Idx> GetByIdx() {
						return static_cast<V2Leaf<Idx, typename TypeGetter<Idx, TTypes...>::Type>&>(*this).value;
					}

					template<SizeT Idx> 
						requires (sizeof...(TTypes) > Idx)
					constexpr typename TypeByIdx<Idx> GetByIdx() const {
						return static_cast<const V2Leaf<Idx, typename TypeGetter<Idx, TTypes...>::Type>&>(*this).value;
					}

					template<SizeT Idx>
						requires (sizeof...(TTypes) > Idx)
					constexpr typename TypeTraits::AddLvalueReferenceV<TypeTraits::AddConstV<TypeByIdx<Idx>>> GetByIdxAsRef() const {
						return static_cast<const V2Leaf<Idx, typename TypeGetter<Idx, TTypes...>::Type>&>(*this).value;
					}

					template<SizeT Idx>
						requires (sizeof...(TTypes) > Idx)
					constexpr typename TypeTraits::AddLvalueReferenceV<TypeByIdx<Idx>> GetByIdxAsRef() {
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
					constexpr static decltype(auto) Execute(F&& f, const V2Tuple<TTypes...>& tuple) {
						if constexpr (TypeTraits::IsSameV<TypeTraits::InvockableResult<F, TTypes...>, void>)
						{
							f(tuple.GetByIdx<(sizeof...(TTypes) - 1) - Idxs>()...);
						}
						else
						{
							return f(tuple.GetByIdx<(sizeof...(TTypes) - 1) - Idxs>()...);
						}
					}

					constexpr static decltype(auto) Execute(F&& f, V2Tuple<TTypes...>& tuple) {
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
					constexpr static decltype(auto) Convert(F&& f, V2Tuple<TTypes...>& tuple) {
						return V2Tuple<decltype(f(tuple.GetByIdx<Idxs>()))...>(f(tuple.GetByIdx<Idxs>())...);
					}

					constexpr static decltype(auto) Convert(F&& f, const V2Tuple<TTypes...>& tuple) {
						return V2Tuple<decltype(f(tuple.GetByIdx<Idxs>()))...>(f(tuple.GetByIdx<Idxs>())...);
					}
				};
			}

			template<typename ... TTypes>
			class V2Tuple : LINQ::ITupleGenericBase {
			private:
				using TV2Leafs = typename Details::V2Leafs<typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type, TTypes...>;
			private:
				TV2Leafs leafs;
			public:
				static constexpr SizeT Count = TV2Leafs::Count;
			public:
				constexpr V2Tuple() requires (Concepts::IsDefaultConstructible<TTypes> && ...) : leafs() {}
			public:
				constexpr V2Tuple(TTypes... data) : leafs(ForwardReference<TTypes>(data)...) {}
			public:
				constexpr V2Tuple(const V2Tuple& tuple) : leafs(tuple.leafs) {}
				constexpr V2Tuple(V2Tuple&& tuple) : leafs(MoveRValue(tuple.leafs)) {}
			public:
				constexpr V2Tuple& operator=(const V2Tuple& tuple) {
					this->~V2Tuple();
					this->V2Tuple::V2Tuple(tuple);
					return *this;
				}

				constexpr V2Tuple& operator=(V2Tuple&& tuple) {
					this->~V2Tuple();
					this->V2Tuple::V2Tuple(MoveRValue(tuple));
					return *this;
				}
			public:
				template<SizeT Idx> 
					requires (sizeof...(TTypes) > Idx)
				constexpr decltype(auto) GetByIdx() {
					return leafs.GetByIdx<Idx>();
				}

				template<SizeT Idx>
					requires (sizeof...(TTypes) > Idx)
				constexpr decltype(auto) GetByIdx() const {
					return leafs.GetByIdx<Idx>();
				}

				template<SizeT Idx>
					requires (sizeof...(TTypes) > Idx)
				constexpr decltype(auto) GetByIdxAsRef() const {
					return leafs.GetByIdxAsRef<Idx>();
				}

				template<SizeT Idx>
					requires (sizeof...(TTypes) > Idx)
				constexpr decltype(auto) GetByIdxAsRef() {
					return leafs.GetByIdxAsRef<Idx>();
				}
			public:
				template<SizeT Idx>
				using ItemType = typename TV2Leafs::template TypeByIdx<Idx>;
			public:
				template<typename F>
				constexpr decltype(auto) CallWithArgsFromTuple(F&& f) const {
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

				template<typename F>
				constexpr decltype(auto) CallWithArgsFromTuple(F&& f) {
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
                template<typename TLambda>
                ForceInline constexpr decltype(auto) ConvertTo(TLambda&& lambda) const {
					using Convertor = typename Details::Convertor<
						TLambda,
						V2Tuple<TTypes...>, 
						typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

					return Convertor::Convert(MoveRValue(lambda), *this);
                }

				template<typename TLambda>
				ForceInline constexpr decltype(auto) ConvertTo(TLambda&& lambda) {
					using Convertor = typename Details::Convertor<
						TLambda,
						V2Tuple<TTypes...>,
						typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

					return Convertor::Convert(MoveRValue(lambda), *this);
				}
			};

			template<>
			class V2Tuple<> : LINQ::ITupleGenericBase {
			public:
				constexpr V2Tuple() {}
				constexpr V2Tuple(const V2Tuple&) = default;
				constexpr V2Tuple(V2Tuple&&) = default;
			public:
				constexpr V2Tuple& operator=(const V2Tuple&) = default;
				constexpr V2Tuple& operator=(V2Tuple&&) = default;
			public:
				static constexpr SizeT Count = 0;
			public:
				template<typename F>
				constexpr decltype(auto) CallWithArgsFromTuple(F&& f) {
					return f();
				}

				template<typename F>
				constexpr decltype(auto) CallWithArgsFromTuple(F&& f) const {
					return f();
				}
			};

			template<typename ... TTypes>
			V2Tuple(TTypes... data)->V2Tuple<TTypes...>;

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
#pragma once
#include "Seed.Core.Base.h"

namespace std {
	///<summary>Ñïèñîê ³í³ö³àë³çàö³¿</summary>
	template<class _Elem>
	class initializer_list
	{	// list of pointers to elements
	public:
		typedef _Elem value_type;
		typedef const _Elem& reference;
		typedef const _Elem& const_reference;
		typedef size_t size_type;

		typedef const _Elem* iterator;
		typedef const _Elem* const_iterator;

		constexpr initializer_list() noexcept
			: _First(nullptr), _Last(nullptr)
		{	// empty list
		}

		constexpr initializer_list(const _Elem* _First_arg,
			const _Elem* _Last_arg) noexcept
			: _First(_First_arg), _Last(_Last_arg)
		{	// construct with pointers
		}

		[[nodiscard]] constexpr const _Elem* begin() const noexcept
		{	// get beginning of list
			return (_First);
		}

		[[nodiscard]] constexpr const _Elem* end() const noexcept
		{	// get end of list
			return (_Last);
		}

		[[nodiscard]] constexpr size_t size() const noexcept
		{	// get length of list
			return (static_cast<size_t>(_Last - _First));
		}

	private:
		const _Elem* _First;
		const _Elem* _Last;
	};

	// FUNCTION TEMPLATE begin
	template<class _Elem>
	[[nodiscard]] constexpr const _Elem* begin(initializer_list<_Elem> _Ilist) noexcept
	{	// get beginning of sequence
		return (_Ilist.begin());
	}

	// FUNCTION TEMPLATE end
	template<class _Elem>
	[[nodiscard]] constexpr const _Elem* end(initializer_list<_Elem> _Ilist) noexcept
	{	// get end of sequence
		return (_Ilist.end());
	}
}

namespace Seed {
	inline namespace Core {
		///<summary>Ñïèñîê ³í³ö³àë³çàö³¿</summary>
		namespace {
			template<typename T>
			using InitilizerList = std::initializer_list<T>;
		}

		namespace Internal
		{
			namespace TupleImpl {
				template <SizeT... Indices>
				struct index_sequence
				{
					using type = index_sequence<Indices...>;
				};

				template <SizeT I, typename Sequence>
				struct cat_index_sequence {};

				template <SizeT I, SizeT... Indices>
				struct cat_index_sequence<I, index_sequence<Indices...>>
					: index_sequence<Indices..., I>
				{};

				template <SizeT N>
				struct make_index_sequence
					: cat_index_sequence<N - 1, typename make_index_sequence<N - 1>::type>::type
				{};

				template <>
				struct make_index_sequence<1> : index_sequence<0> {};

				template <SizeT, typename T>
				struct tuple_element
				{
					T value_;

					__forceinline constexpr tuple_element() : value_() {}
					__forceinline constexpr tuple_element(const T& value) : value_(value) {}

					template <typename U>
					__forceinline constexpr explicit tuple_element(U&& value) : value_(ForwardReference<U>(value)) {}
				};

				template <typename Sequence, typename... Types>
				struct tuple_impl {};

				template <typename ... Types>
				struct is_tuple_impl
					: StaticAsserts::LogicFalse
				{};

				template <SizeT... Indices, typename ... Types>
				struct is_tuple_impl<tuple_impl<index_sequence<Indices...>, Types...>>
					: StaticAsserts::LogicTrue
				{};

				template <template <class> typename>
				__forceinline static constexpr bool is_any_of() {
					return false;
				}

				template <template <class> typename Op, typename Head, typename... Tail>
				__forceinline static constexpr bool is_any_of() {
					return Op<Head>::State || is_any_of<Op, Tail...>();
				}

				template <SizeT... Indices, typename ... Types>
				struct tuple_impl<index_sequence<Indices...>, Types...>
					: public tuple_element<Indices, Types>...
				{
					constexpr tuple_impl() = default;
					constexpr tuple_impl(const tuple_impl&) = default;
					constexpr tuple_impl(tuple_impl&&) = default;

					template <typename... OtherTypes,
						typename = typename StaticAsserts::EnableIf<
						!is_any_of<is_tuple_impl, typename StaticAsserts::Decay<OtherTypes>...>()
						>::type
					>
						__forceinline constexpr explicit tuple_impl(OtherTypes&& ... elements)
						: tuple_element<Indices, Types>(ForwardReference<OtherTypes>(elements))...
					{
					}
				};

				template<typename ... Types>
				class tuple
					: public tuple_impl<typename make_index_sequence<sizeof...(Types)>::type, Types...>
				{
				public:
					__forceinline constexpr tuple() = default;
					__forceinline constexpr tuple(tuple const&) = default;
					__forceinline constexpr tuple(tuple&&) = default;
					__forceinline constexpr tuple& operator=(tuple const& rhs) = default;
					__forceinline constexpr tuple& operator=(tuple&&) = default;

					using base_t = tuple_impl<typename make_index_sequence<sizeof...(Types)>::type,
						Types...>;

					template <typename... OtherTypes>
					__forceinline constexpr explicit tuple(tuple<OtherTypes...> const& rhs) : base_t(rhs) {}

					template <typename... OtherTypes>
					__forceinline constexpr explicit tuple(tuple<OtherTypes...>&& rhs) : base_t(rhs) {}

					template <typename... OtherTypes,
						typename = typename StaticAsserts::EnableIf<
						sizeof...(OtherTypes) == sizeof...(Types)>::type>
						__forceinline constexpr explicit tuple(OtherTypes && ... elements)
						: base_t(ForwardReference<OtherTypes>(elements)...) {}
				};

				template <SizeT I, typename Head, typename... Tail>
				struct type_at_index
				{
					using type = typename type_at_index<I - 1, Tail...>::type;
				};

				template <typename Head, typename... Tail>
				struct type_at_index<0, Head, Tail...>
				{
					using type = Head;
				};

				template <SizeT I, typename... Types>
				using type_at_index_t = typename type_at_index<I, Types...>::type;

				template <size_t I, typename... Types>
				__forceinline constexpr type_at_index_t<I, Types...> const& get(tuple<Types...> const& tup)
				{
					const tuple_element<I, type_at_index_t<I, Types...>>& base = tup;
					return base.value_;
				}

				template <size_t I, typename... Types>
				__forceinline constexpr type_at_index_t<I, Types...>& get(tuple<Types...>& tup)
				{
					tuple_element<I, type_at_index_t<I, Types...>>& base = tup;
					return base.value_;
				}

				template <size_t I, typename... Types>
				__forceinline constexpr type_at_index_t<I, Types...>&& get(tuple<Types...>&& tup)
				{
					tuple_element<I, type_at_index_t<I, Types...>>& base = tup;
					return base.value_;
				}

				/*template <SizeT I, typename ... Types>
				__forceinline static constexpr type_at_index_t<I> const& get(tuple<Types...> const& tup) {
					const tuple_element<I, type_at_index_t<I>>& base = tup;
					return base.value_;
				}

				template <SizeT I, typename ... Types>
				__forceinline static type_at_index_t<I>& get(tuple<Types...>& tup) {
					tuple_element<I, type_at_index_t<I>>& base = tup;
					return base.value_;
				}

				template <SizeT I, typename ... Types>
				__forceinline static constexpr StaticAsserts::RemoveRefT<type_at_index_t<I>>&& get(tuple<Types...>&& tup) {
					tuple_element<I, type_at_index_t<I>>& base = tup;
					return ForwardReference(base.value_);
				}*/

				template <typename>
				__forceinline static constexpr int count() {
					return 0;
				}
				template <typename T, typename Head, typename... Tail>
				__forceinline static constexpr int count() {
					return (std::is_same<T, Head>::value ? 1 : 0) + count<T, Tail...>();
				}

				template <typename>
				__forceinline static constexpr int find(int) {
					return -1;
				}
				template <typename T, typename Head, typename... Tail>
				__forceinline static constexpr int find(int current_index = 0) {
					return std::is_same<T, Head>::value
						? current_index
						: find<T, Tail...>(current_index + 1);
				}

				template <typename T, typename ... Types>
				__forceinline static constexpr T& get(tuple<Types...> & tup) {
					static_assert(count<T, Types...>() == 1, "T must appear exactly once in ...Types");
					get<find<T, Types...>()>(tup);
				}
				template<typename ... Types>
				__forceinline static constexpr tuple<Types...> MakeTuple(const Types & ...  variables) {
					return tuple<Types...>(variables...);
				}
				template<typename ... Types>
				__forceinline static constexpr tuple<Types...> MakeTuple(Types && ...  variables) {
					return tuple<Types...>(ForwardReference<Types>(variables)...);
				}
			}
		}

		template<typename... Types>
		class Tuple
		{
		private:
			Internal::TupleImpl::tuple<Types...> data;
		public:
			__forceinline constexpr Tuple() : data() {}
			__forceinline constexpr Tuple(const Types& ... values)
				: data(Internal::TupleImpl::MakeTuple<Types...>(values...)) {}
			__forceinline constexpr Tuple(Types&& ... values)
				: data(Internal::TupleImpl::MakeTuple(ForwardReference<Types>(values)...)) {}
			__forceinline constexpr Tuple(const Tuple& orig)
				: data(orig.data) {}
			__forceinline constexpr Tuple(Tuple&& orig)
				: data(ForwardReference<Internal::TupleImpl::tuple<Types...>>(orig.data)) {}

			__forceinline constexpr Tuple& operator=(const Tuple& orig) {
				this->Tuple::~Tuple();
				this->Tuple::Tuple(orig);
			}

			__forceinline constexpr Tuple& operator=(Tuple&& orig) {
				this->Tuple::~Tuple();
				this->Tuple::Tuple(ForwardReference<Tuple<Types...>>(orig));
			}

			template<SizeT Index>
			__forceinline constexpr auto& Get() const {
				return Internal::TupleImpl::get<Index>(data);
			}

			template<SizeT Index>
			__forceinline auto& Get() {
				return Internal::TupleImpl::get<Index>(data);
			}

			__forceinline constexpr static SizeT Count() { return sizeof...(Types); }
		};
	}
}
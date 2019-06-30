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

	template< class T >
	class tuple_size;
}

namespace Seed {
	inline namespace Core {
		///<summary>Ñïèñîê ³í³ö³àë³çàö³¿</summary>
		namespace {
			template<typename T>
			using InitilizerList = std::initializer_list<T>;
		}

		//namespace Internal
		//{
		//	namespace TupleImpl {
		//		template <SizeT... Indices>
		//		struct index_sequence
		//		{
		//			using type = index_sequence<Indices...>;
		//		};

		//		template <SizeT I, typename Sequence>
		//		struct cat_index_sequence {};

		//		template <SizeT I, SizeT... Indices>
		//		struct cat_index_sequence<I, index_sequence<Indices...>>
		//			: index_sequence<Indices..., I>
		//		{};

		//		template <SizeT N>
		//		struct make_index_sequence
		//			: cat_index_sequence<N - 1, typename make_index_sequence<N - 1>::type>::type
		//		{};

		//		template <>
		//		struct make_index_sequence<1> : index_sequence<0> {};

		//		template <>
		//		struct make_index_sequence<0> {
		//			using type = void;
		//		};

		//		template <SizeT, typename T>
		//		struct tuple_element
		//		{
		//			T value_;

		//			__forceinline constexpr tuple_element() : value_() {}
		//			__forceinline constexpr tuple_element(const T& value) : value_(value) {}

		//			template <typename U>
		//			__forceinline constexpr explicit tuple_element(U&& value) : value_(ForwardReference<U>(value)) {}
		//		};

		//		template <typename Sequence, typename... Types>
		//		struct tuple_impl {};

		//		template <typename ... Types>
		//		struct is_tuple_impl
		//			: StaticAsserts::LogicFalse
		//		{};

		//		template <SizeT... Indices, typename ... Types>
		//		struct is_tuple_impl<tuple_impl<index_sequence<Indices...>, Types...>>
		//			: StaticAsserts::LogicTrue
		//		{};

		//		template <template <class> typename>
		//		__forceinline static constexpr bool is_any_of() {
		//			return false;
		//		}

		//		template <template <class> typename Op, typename Head, typename... Tail>
		//		__forceinline static constexpr bool is_any_of() {
		//			return Op<Head>::State || is_any_of<Op, Tail...>();
		//		}

		//		template <SizeT... Indices, typename ... Types>
		//		struct tuple_impl<index_sequence<Indices...>, Types...>
		//			: public tuple_element<Indices, Types>...
		//		{
		//			constexpr tuple_impl() = default;
		//			constexpr tuple_impl(const tuple_impl&) = default;
		//			constexpr tuple_impl(tuple_impl&&) = default;

		//			template <typename... OtherTypes,
		//				typename = typename StaticAsserts::EnableIf<
		//				!is_any_of<is_tuple_impl, typename StaticAsserts::Decay<OtherTypes>...>()
		//				>::type
		//			>
		//				__forceinline constexpr explicit tuple_impl(OtherTypes&& ... elements)
		//				: tuple_element<Indices, Types>(ForwardReference<OtherTypes>(elements))...
		//			{
		//			}
		//		};

		//		template<typename ... Types>
		//		class tuple
		//			: public tuple_impl<typename make_index_sequence<sizeof...(Types)>::type, Types...>
		//		{
		//		public:
		//			__forceinline constexpr tuple() = default;
		//			__forceinline constexpr tuple(tuple const&) = default;
		//			__forceinline constexpr tuple(tuple&&) = default;
		//			__forceinline constexpr tuple& operator=(tuple const& rhs) = default;
		//			__forceinline constexpr tuple& operator=(tuple&&) = default;

		//			using base_t = tuple_impl<typename make_index_sequence<sizeof...(Types)>::type,
		//				Types...>;

		//			template <typename... OtherTypes>
		//			__forceinline constexpr explicit tuple(tuple<OtherTypes...> const& rhs) : base_t(rhs) {}

		//			template <typename... OtherTypes>
		//			__forceinline constexpr explicit tuple(tuple<OtherTypes...>&& rhs) : base_t(rhs) {}

		//			template <typename... OtherTypes,
		//				typename = typename StaticAsserts::EnableIf<
		//				sizeof...(OtherTypes) == sizeof...(Types)>::type>
		//				__forceinline constexpr explicit tuple(OtherTypes && ... elements)
		//				: base_t(ForwardReference<OtherTypes>(elements)...) {}
		//		};

		//		template <SizeT I, typename Head, typename... Tail>
		//		struct type_at_index
		//		{
		//			using type = typename type_at_index<I - 1, Tail...>::type;
		//		};

		//		template <typename Head, typename... Tail>
		//		struct type_at_index<0, Head, Tail...>
		//		{
		//			using type = Head;
		//		};

		//		template <SizeT I, typename... Types>
		//		using type_at_index_t = typename type_at_index<I, Types...>::type;

		//		template <size_t I, typename... Types>
		//		__forceinline constexpr type_at_index_t<I, Types...> const& get(tuple<Types...> const& tup)
		//		{
		//			const tuple_element<I, type_at_index_t<I, Types...>>& base = tup;
		//			return base.value_;
		//		}

		//		template <size_t I, typename... Types>
		//		__forceinline constexpr type_at_index_t<I, Types...>& get(tuple<Types...>& tup)
		//		{
		//			tuple_element<I, type_at_index_t<I, Types...>>& base = tup;
		//			return base.value_;
		//		}

		//		template <size_t I, typename... Types>
		//		__forceinline constexpr type_at_index_t<I, Types...>&& get(tuple<Types...>&& tup)
		//		{
		//			tuple_element<I, type_at_index_t<I, Types...>>& base = tup;
		//			return base.value_;
		//		}

		//		template <size_t I, typename... Types>
		//		using tuple_element_t = typename tuple_element<I, Types...>::type;

		//		/*template <SizeT I, typename ... Types>
		//		__forceinline static constexpr type_at_index_t<I> const& get(tuple<Types...> const& tup) {
		//			const tuple_element<I, type_at_index_t<I>>& base = tup;
		//			return base.value_;
		//		}

		//		template <SizeT I, typename ... Types>
		//		__forceinline static type_at_index_t<I>& get(tuple<Types...>& tup) {
		//			tuple_element<I, type_at_index_t<I>>& base = tup;
		//			return base.value_;
		//		}

		//		template <SizeT I, typename ... Types>
		//		__forceinline static constexpr StaticAsserts::RemoveRefT<type_at_index_t<I>>&& get(tuple<Types...>&& tup) {
		//			tuple_element<I, type_at_index_t<I>>& base = tup;
		//			return ForwardReference(base.value_);
		//		}*/

		//		template <typename>
		//		__forceinline static constexpr int count() {
		//			return 0;
		//		}
		//		template <typename T, typename Head, typename... Tail>
		//		__forceinline static constexpr int count() {
		//			return (std::is_same<T, Head>::value ? 1 : 0) + count<T, Tail...>();
		//		}

		//		template <typename>
		//		__forceinline static constexpr int find(int) {
		//			return -1;
		//		}
		//		template <typename T, typename Head, typename... Tail>
		//		__forceinline static constexpr int find(int current_index = 0) {
		//			return std::is_same<T, Head>::value
		//				? current_index
		//				: find<T, Tail...>(current_index + 1);
		//		}

		//		template <typename T, typename ... Types>
		//		__forceinline static constexpr T& get(tuple<Types...> & tup) {
		//			static_assert(count<T, Types...>() == 1, "T must appear exactly once in ...Types");
		//			get<find<T, Types...>()>(tup);
		//		}
		//		template<typename ... Types>
		//		__forceinline static constexpr tuple<Types...> MakeTuple(const Types & ...  variables) {
		//			return tuple<Types...>(variables...);
		//		}
		//		template<typename ... Types>
		//		__forceinline static constexpr tuple<Types...> MakeTuple(Types && ...  variables) {
		//			return tuple<Types...>(ForwardReference<Types>(variables)...);
		//		}
		//	}

		//	template<typename ...Types>
		//	struct VariadicTypedef {};

		//	template <template <typename ... Args> typename T, typename ... Args>
		//	struct TypeWithList
		//	{
		//		typedef T<Args...> type;
		//	};

		//	template <template <typename ... Args> typename T, typename ... Args>
		//	struct TypeWithList<T, VariadicTypedef<Args...>>
		//	{
		//		typedef typename TypeWithList<T, Args...>::type type;
		//	};
		//}

		/*template<typename... Types>
		class Tuple
		{
		private:
			Internal::TupleImpl::tuple<Types...> data;
		private:
			

			
		public:
			__forceinline constexpr Tuple() : data() {}
			__forceinline constexpr Tuple(const Types& ... values)
				: data(Internal::TupleImpl::MakeTuple<const Types&...>(values...)) {}
			__forceinline constexpr Tuple(Types&& ... values)
				: data(Internal::TupleImpl::MakeTuple(ForwardReference<Types>(values)...)) {}
			__forceinline constexpr Tuple(const Tuple& orig)
				: data(orig.data) {}
			__forceinline constexpr Tuple(Tuple&& orig)
				: data(ForwardReference<Internal::TupleImpl::tuple<Types&&...>>(orig.data)) {}

			__forceinline constexpr Tuple& operator=(const Tuple& orig) {
				this->Tuple::~Tuple();
				this->Tuple::Tuple(orig);
				return *this;
			}

			__forceinline constexpr Tuple& operator=(Tuple&& orig) {
				this->Tuple::~Tuple();
				this->Tuple::Tuple(ForwardReference<Tuple<Types...>>(orig));
				return *this;
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
		};*/

		//template<>
		//class Tuple<>
		//{
		//private:
		//	Internal::TupleImpl::tuple<> data;
		//public:
		//	__forceinline constexpr Tuple() : data() {}
		//	__forceinline constexpr Tuple(const Tuple& orig)
		//		: data(orig.data) {}

		//	__forceinline constexpr Tuple& operator=(const Tuple& orig) {
		//		this->Tuple::~Tuple();
		//		this->Tuple::Tuple(orig);
		//		return *this;
		//	}

		//	template<SizeT Index>
		//	__forceinline constexpr auto& Get() const {
		//		return Internal::TupleImpl::get<Index>(data);
		//	}

		//	template<SizeT Index>
		//	__forceinline auto& Get() {
		//		return Internal::TupleImpl::get<Index>(data);
		//	}

		//	template<size_t _Index,
		//		class... _Types>
		//		 constexpr const Internal::TupleImpl::tuple_element_t<_Index, Tuple<_Types...>>&&
		//		get(const Tuple<_Types...>&& _Tuple) noexcept
		//	{	// get const rvalue reference to _Index element of Tuple
		//		typedef typename Internal::TupleImpl::tuple_element<_Index, Tuple<_Types...>>::type _Ttype;
		//		typedef const Internal::TupleImpl::tuple_element_t<_Index, Tuple<_Types...>>&& _RRtype;
		//		return (ForwardReference<_RRtype>(((_Ttype&)_Tuple)._Myfirst._Val));
		//	}

		//	// FUNCTION TEMPLATE get (by type)
		//	template<class _Ty,
		//		class... _Types>
		//		 constexpr _Ty& get(Tuple<_Types...>& _Tuple) noexcept
		//	{	// get reference to _Ty element of Tuple
		//		typedef typename Internal::TupleImpl::tuple_element<_Ty, Tuple<_Types...>>::type _Ttype;
		//		return (((_Ttype&)_Tuple)._Myfirst._Val);
		//	}

		//	template<class _Ty,
		//		class... _Types>
		//		 constexpr const _Ty& get(const Tuple<_Types...>& _Tuple) noexcept
		//	{	// get const reference to _Ty element of Tuple
		//		typedef typename Internal::TupleImpl::tuple_element<_Ty, Tuple<_Types...>>::type _Ttype;
		//		return (((_Ttype&)_Tuple)._Myfirst._Val);
		//	}

		//	template<class _Ty,
		//		class... _Types>
		//		 constexpr _Ty&& get(Tuple<_Types...>&& _Tuple) noexcept
		//	{	// get rvalue reference to _Ty element of Tuple
		//		typedef typename Internal::TupleImpl::tuple_element<_Ty, Tuple<_Types...>>::type _Ttype;
		//		return (ForwardReference<_Ty&&>(((_Ttype&)_Tuple)._Myfirst._Val));
		//	}

		//	template<class _Ty,
		//		class... _Types>
		//		 constexpr const _Ty&& get(const Tuple<_Types...>&& _Tuple) noexcept
		//	{	// get const rvalue reference to _Ty element of Tuple
		//		typedef typename Internal::TupleImpl::tuple_element<_Ty, Tuple<_Types...>>::type _Ttype;
		//		return (ForwardReference<const _Ty&&>(((_Ttype&)_Tuple)._Myfirst._Val));
		//	}


		//	__forceinline constexpr static SizeT Count() { return 0; }
		//};
	}
}

namespace std {
	////template< class... Types >
	////class tuple_size<Seed::Tuple<Types...>>
	////{
	////public:
	////	static constexpr Seed::SizeT value = Seed::Tuple<Types...>::Count();
	////	typedef Seed::SizeT value_type;

	////	constexpr operator value_type() const noexcept { return value; }
	////	constexpr value_type operator()() const noexcept { return value; }
	////};

	//template<class _Ty,
	//	_Ty _Val>
	//	struct integral_constant
	//{	// convenient template for integral constant types
	//	static constexpr _Ty value = _Val;

	//	using value_type = _Ty;
	//	using type = integral_constant;

	//	constexpr operator value_type() const noexcept
	//	{	// return stored value
	//		return (value);
	//	}

	//	[[nodiscard]] constexpr value_type operator()() const noexcept
	//	{	// return stored value
	//		return (value);
	//	}
	//};

	//template<class>
	//// false value attached to a dependent name (for static_assert)
	//inline constexpr bool _Always_false = false;

	//template<size_t _Index,
	//	class _Tuple>
	//	struct tuple_element;

	//template<size_t _Index,
	//	class _Tuple>
	//	using tuple_element_t = typename tuple_element<_Index, _Tuple>::type;

	//template<class... _Types>
	//struct tuple_size<Seed::Tuple<_Types...>>
	//	: integral_constant<size_t, sizeof...(_Types)>
	//{	// size of tuple
	//};

	//template<size_t _Index>
	//struct tuple_element<_Index, Seed::Tuple<>>
	//{	// enforce bounds checking
	//	static_assert(_Always_false<integral_constant<size_t, _Index>>,
	//		"tuple index out of bounds");
	//};

	//template<class _This,
	//	class... _Rest>
	//	struct tuple_element<0, Seed::Tuple<_This, _Rest...>>
	//{	// select first element
	//	using type = _This;
	//	using _Ttype = Seed::Tuple<_This, _Rest...>;
	//};

	//template<size_t _Index,
	//	class _This,
	//	class... _Rest>
	//	struct tuple_element<_Index, Seed::Tuple<_This, _Rest...>>
	//	: public tuple_element<_Index - 1, Seed::Tuple<_Rest...>>
	//{	// recursive tuple_element definition
	//};
}
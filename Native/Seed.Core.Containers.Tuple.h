////#pragma once
////#include "Seed.Core.Base.h"
////
////
////namespace std {
////
////	using size_t = Seed::SizeT;
////	// STRUCT TEMPLATE integral_constant
////	template<class _Ty,
////		_Ty _Val>
////		struct integral_constant
////	{	// convenient template for integral constant types
////		static constexpr _Ty value = _Val;
////
////		using value_type = _Ty;
////		using type = integral_constant;
////
////		constexpr operator value_type() const noexcept
////		{	// return stored value
////			return (value);
////		}
////
////		[[nodiscard]] constexpr value_type operator()() const noexcept
////		{	// return stored value
////			return (value);
////		}
////	};
////
////	// STRUCT TEMPLATE integer_sequence
////	template<class _Ty,
////		_Ty... _Vals>
////		struct integer_sequence
////	{	// sequence of integer parameters
////		static_assert(is_integral_v<_Ty>,
////			"integer_sequence<T, I...> requires T to be an integral type.");
////
////		using value_type = _Ty;
////
////		[[nodiscard]] static constexpr size_t size() noexcept
////		{	// get length of parameter list
////			return (sizeof...(_Vals));
////		}
////	};
////
////	// ALIAS TEMPLATE make_integer_sequence
////	template<class _Ty,
////		_Ty _Size>
////		using make_integer_sequence = __make_integer_seq<integer_sequence, _Ty, _Size>;
////
////	template<size_t... _Vals>
////	using index_sequence = integer_sequence<size_t, _Vals...>;
////
////	template<size_t _Size>
////	using make_index_sequence = make_integer_sequence<size_t, _Size>;
////
////	template<class... _Types>
////	using index_sequence_for = make_index_sequence<sizeof...(_Types)>;
////
////	// ALIAS TEMPLATE bool_constant
////	template<bool _Val>
////	using bool_constant = integral_constant<bool, _Val>;
////
////	using true_type = bool_constant<true>;
////	using false_type = bool_constant<false>;
////
////	template<typename ... Types>
////	class tuple;
////
////	template< class T >
////	class tuple_size; /*undefined*/
////
////	template< class... Types >
////	class tuple_size< std::tuple<Types...> >
////		: public std::integral_constant<std::size_t, sizeof...(Types)> { };
////
////	template< class T >
////	class tuple_size<const T>
////		: public std::integral_constant<std::size_t, tuple_size<T>::value> { };
////
////	template< class T >
////	class tuple_size< volatile T >
////		: public std::integral_constant<std::size_t, tuple_size<T>::value> { };
////
////	template< class T >
////	class tuple_size< const volatile T >
////		: public std::integral_constant<std::size_t, tuple_size<T>::value> { };
////
////	template<class _Ty>
////	inline constexpr size_t tuple_size_v = tuple_size<_Ty>::value;
////
////	// STRUCTS FOR STRUCTURED BINDINGS tuple_size AND tuple_element
////
////	template<size_t _Index,
////		class _Tuple>
////		struct tuple_element;
////
////	template <size_t _Index>
////	struct tuple_element<_Index, tuple<>> { // enforce bounds checking
////		static_assert(_Always_false<integral_constant<size_t, _Index>>, "tuple index out of bounds");
////	};
////
////	template <class _This, class... _Rest>
////	struct tuple_element<0, tuple<_This, _Rest...>> { // select first element
////		using type   = _This;
////		using _Ttype = tuple<_This, _Rest...>;
////	};
////
////	template <size_t _Index, class _This, class... _Rest>
////	struct tuple_element<_Index, tuple<_This, _Rest...>>
////		: public tuple_element<_Index - 1, tuple<_Rest...>> { // recursive tuple_element definition
////	};
////
////	template<size_t _Index,
////		class _Tuple>
////		struct tuple_element<_Index, const _Tuple>
////		: public tuple_element<_Index, _Tuple>
////	{	// tuple_element for const
////		using _Mybase = tuple_element<_Index, _Tuple>;
////		using type = Seed::StaticAsserts::AddConstV<typename _Mybase::type>;
////	};
////
////	template<size_t _Index,
////		class _Tuple>
////		struct tuple_element<_Index, volatile _Tuple>
////		: public tuple_element<_Index, _Tuple>
////	{	// tuple_element for volatile
////		using _Mybase = tuple_element<_Index, _Tuple>;
////		using type = Seed::StaticAsserts::AddVolatileV<typename _Mybase::type>;
////	};
////
////	template<size_t _Index,
////		class _Tuple>
////		struct tuple_element<_Index, const volatile _Tuple>
////		: public tuple_element<_Index, _Tuple>
////	{	// tuple_element for const volatile
////		using _Mybase = tuple_element<_Index, _Tuple>;
////		using type = Seed::StaticAsserts::AddCvV<typename _Mybase::type>;
////	};
////
////	template<size_t _Index,
////		class _Tuple>
////		using tuple_element_t = typename tuple_element<_Index, _Tuple>::type;
////
////	// STRUCT TEMPLATE _Tuple_implicit_val
////	template<bool _Same,
////	class _Dest,
////	class... _Srcs>
////	struct _Tuple_implicit_val0
////		:Seed::Core::StaticAsserts::LogicFalse
////	{	// Constrain tuple's implicit constructors
////	};
////
////	template<class... _Dests,
////		class... _Srcs>
////		struct _Tuple_implicit_val0<true, tuple<_Dests...>, _Srcs...>
////		: Seed::Core::StaticAsserts::LogicConstT<Seed::Core::StaticAsserts::AndT<
////		Seed::Core::StaticAsserts::IsConstructibleT<_Dests, _Srcs>...,
////		Seed::Core::StaticAsserts::IsConvertibleT<_Srcs, _Dests>...
////		>>
////	{	// Constrain tuple's implicit constructors
////	};
////
////	template<class _Dest,
////		class... _Srcs>
////		struct _Tuple_implicit_val
////		: Seed::Core::StaticAsserts::LogicConstT<_Tuple_implicit_val0<tuple_size_v<_Dest> == sizeof...(_Srcs), _Dest, _Srcs...>>
////	{	// Constrain tuple's implicit constructors
////	};
////
////	// STRUCT TEMPLATE _Tuple_explicit_val
////	template<bool _Same,
////		class _Dest,
////		class... _Srcs>
////		struct _Tuple_explicit_val0
////		: Seed::Core::StaticAsserts::LogicFalse
////	{	// Constrain tuple's explicit constructors
////	};
////
////	template<class... _Dests,
////		class... _Srcs>
////		struct _Tuple_explicit_val0<true, tuple<_Dests...>, _Srcs...>
////		: Seed::StaticAsserts::LogicConstT<Seed::Core::StaticAsserts::AndT<
////		Seed::Core::StaticAsserts::IsConstructibleT<_Dests, _Srcs>...,
////		Seed::Core::StaticAsserts::NegationT<Seed::Core::StaticAsserts::AndT<Seed::Core::StaticAsserts::IsConvertibleT<_Srcs, _Dests>...>>
////		>>
////	{	// Constrain tuple's explicit constructors
////	};
////
////	template<class _Dest,
////		class... _Srcs>
////		struct _Tuple_explicit_val
////		: Seed::StaticAsserts::LogicConstT<_Tuple_explicit_val0<tuple_size_v<_Dest> == sizeof...(_Srcs), _Dest, _Srcs...>>
////	{	// Constrain tuple's explicit constructors
////	};
////
////	// VARIABLE TEMPLATE _Tuple_constructible_v
////	template<bool _Same,
////		class _Dest,
////		class... _Srcs>
////		struct _Tuple_constructible_val0
////		: Seed::Core::StaticAsserts::LogicFalse
////	{
////	};
////
////	template<class... _Dests,
////		class... _Srcs>
////		struct _Tuple_constructible_val0<true, tuple<_Dests...>, _Srcs...>
////		: Seed::StaticAsserts::LogicConstT<Seed::Core::StaticAsserts::AndT<Seed::Core::StaticAsserts::IsConstructibleT<_Dests, _Srcs>...>>
////	{
////	};
////
////	template<class _Dest,
////		class... _Srcs>
////		inline constexpr bool _Tuple_constructible_v =
////		_Tuple_constructible_val0<tuple_size_v<_Dest> == sizeof...(_Srcs), _Dest, _Srcs...>::State;
////
////	// VARIABLE TEMPLATE _Tuple_nothrow_constructible_v
////	template<bool _Same,
////		class _Dest,
////		class... _Srcs>
////		struct _Tuple_nothrow_constructible_val0
////		: Seed::Core::StaticAsserts::LogicFalse
////	{
////	};
////
////	template<class... _Dests,
////		class... _Srcs>
////		struct _Tuple_nothrow_constructible_val0<true, tuple<_Dests...>, _Srcs...>
////		: Seed::StaticAsserts::LogicConstT<Seed::Core::StaticAsserts::AndT<Seed::Core::StaticAsserts::IsNothrowAssignableT<_Dests, _Srcs>...>>
////	{
////	};
////
////	template<class _Dest,
////		class... _Srcs>
////		inline constexpr bool _Tuple_nothrow_constructible_v =
////		_Tuple_nothrow_constructible_val0<tuple_size_v<_Dest> == sizeof...(_Srcs), _Dest, _Srcs...>::State;
////
////	// VARIABLE TEMPLATE _Tuple_assignable_v
////	template<bool _Same,
////		class _Dest,
////		class... _Srcs>
////		struct _Tuple_assignable_val0
////		: Seed::Core::StaticAsserts::LogicFalse
////	{
////	};
////
////	template<class... _Dests,
////		class... _Srcs>
////		struct _Tuple_assignable_val0<true, tuple<_Dests...>, _Srcs...>
////		: Seed::StaticAsserts::LogicConstT<Seed::Core::StaticAsserts::AndT<Seed::Core::StaticAsserts::IsAssignableT<_Dests&, _Srcs>...>>	// note _Dests& instead of _Dests
////	{
////	};
////
////	template<class _Dest,
////		class... _Srcs>
////		inline constexpr bool _Tuple_assignable_v =
////		_Tuple_assignable_val0<tuple_size_v<_Dest> == sizeof...(_Srcs), _Dest, _Srcs...>::State;
////
////	template<class _Dest,
////		class... _Srcs>
////		struct _Tuple_assignable_val
////		: bool_constant<_Tuple_assignable_v<_Dest, _Srcs...>>
////	{
////	};
////
////	// VARIABLE TEMPLATE _Tuple_nothrow_assignable_v
////	template<bool _Same,
////		class _Dest,
////		class... _Srcs>
////		struct _Tuple_nothrow_assignable_val0
////		: Seed::Core::StaticAsserts::LogicFalse
////	{
////	};
////
////	template<class... _Dests,
////		class... _Srcs>
////		struct _Tuple_nothrow_assignable_val0<true, tuple<_Dests...>, _Srcs...>
////		: Seed::Core::StaticAsserts::LogicConstT<Seed::Core::StaticAsserts::AndT<Seed::Core::StaticAsserts::IsNothrowAssignableT<_Dests&, _Srcs>...>>	// note _Dests& instead of _Dests
////	{
////	};
////
////	template<class _Dest,
////		class... _Srcs>
////		inline constexpr bool _Tuple_nothrow_assignable_v =
////		_Tuple_nothrow_assignable_val0<tuple_size_v<_Dest> == sizeof...(_Srcs), _Dest, _Srcs...>::State;
////
////	// STRUCT TEMPLATE _Tuple_convert_copy_val
////	template<class _Myself,
////		class... _Other>
////		struct _Tuple_convert_copy_val
////		: Seed::StaticAsserts::LogicTrue
////	{	// Constrain tuple's converting copy constructor (LWG 2549)
////	};
////
////	template<class _This,
////		class _Uty>
////		struct _Tuple_convert_copy_val<tuple<_This>, _Uty>
////		: Seed::StaticAsserts::NegationT<Seed::Core::StaticAsserts::DisjunctionT<
////		Seed::Core::StaticAsserts::IsSameT<_This, _Uty>,
////		Seed::Core::StaticAsserts::IsConstructibleT<_This, const tuple<_Uty>&>,
////		Seed::Core::StaticAsserts::IsConvertibleT<const tuple<_Uty>&, _This>>>
////	{	// Constrain tuple's converting copy constructor (LWG 2549)
////	};
////
////	// STRUCT TEMPLATE _Tuple_convert_move_val
////	template<class _Myself,
////		class... _Other>
////		struct _Tuple_convert_move_val
////		: Seed::Core::StaticAsserts::LogicTrue
////	{	// Constrain tuple's converting move constructor (LWG 2549)
////	};
////
////	template<class _This,
////		class _Uty>
////		struct _Tuple_convert_move_val<tuple<_This>, _Uty>
////		: Seed::Core::StaticAsserts::NegationT<Seed::Core::StaticAsserts::DisjunctionT<
////		Seed::Core::StaticAsserts::IsSameT<_This, _Uty>,
////		Seed::Core::StaticAsserts::IsConstructibleT<_This, tuple<_Uty>>,
////		Seed::Core::StaticAsserts::IsConvertibleT<tuple<_Uty>, _This>>>
////	{	// Constrain tuple's converting move constructor (LWG 2549)
////	};
////
////	// STRUCT TEMPLATE _Tuple_perfect_val
////	template<class _Myself,
////		class _This2,
////		class... _Rest2>
////		struct _Tuple_perfect_val
////		: Seed::StaticAsserts::LogicTrue
////	{	// Constrain tuple's perfect forwarding constructor (LWG issue not yet filed)
////	};
////
////	template<class _Myself,
////		class _This2>
////		struct _Tuple_perfect_val<_Myself, _This2>
////		: Seed::Core::StaticAsserts::NegationT<Seed::Core::StaticAsserts::IsSameT<_Myself, Seed::Core::StaticAsserts::RemoveConst<Seed::Core::StaticAsserts::RemoveRefT<_This2>>>>
////	{	// Constrain tuple's perfect forwarding constructor (LWG issue not yet filed)
////	};
////
////	// STRUCT _Ignore
////	struct _Ignore
////	{	// struct that ignores assignments
////		template<class _Ty>
////		constexpr const _Ignore& operator=(const _Ty&) const noexcept	// strengthened
////		{	// do nothing
////			return (*this);
////		}
////	};
////
////	inline constexpr _Ignore ignore{};
////
////	// STRUCT TEMPLATE _Tuple_val
////	template<class _Ty>
////	struct _Tuple_val
////	{	// stores each value in a tuple
////		constexpr _Tuple_val()
////			: _Val()
////		{	// default construct
////		}
////
////		template<class _Other>
////		constexpr _Tuple_val(_Other&& _Arg)
////			: _Val(Seed::Core::ForwardReference<_Other>(_Arg))
////		{	// construct with argument
////		}
////
////		_Ty _Val;
////	};
////
////	// CLASS TEMPLATE tuple
////	struct _Exact_args_t
////	{	// tag type to disambiguate construction (from one arg per element)
////	};
////
////	struct _Unpack_tuple_t
////	{	// tag type to disambiguate construction (from unpacking a tuple/pair)
////	};
////
////	struct _Alloc_exact_args_t
////	{	// tag type to disambiguate construction (from an allocator and one arg per element)
////	};
////
////	struct _Alloc_unpack_tuple_t
////	{	// tag type to disambiguate construction (from an allocator and unpacking a tuple/pair)
////	};
////
////	template<class... _Types>
////	class tuple;
////
////	template<>
////	class tuple<>
////	{	// empty tuple
////	public:
////		constexpr tuple() noexcept
////		{	// default construct
////		}
////
////		constexpr tuple(const tuple&) noexcept	// TRANSITION, for binary compatibility
////		{	// copy construct
////		}
////
////
////		template<class _Tag,
////			typename Seed::Core::StaticAsserts::EnableIf<Seed::Core::StaticAsserts::IsSameT<_Tag, _Exact_args_t>::State, int>::Type>
////			constexpr tuple(_Tag) noexcept
////		{	// construct from one arg per element
////		}
////
////		void swap(tuple&) noexcept
////		{	// swap elements
////		}
////
////		constexpr bool _Equals(const tuple&) const noexcept
////		{	// test if *this == _Right
////			return (true);
////		}
////
////		constexpr bool _Less(const tuple&) const noexcept
////		{	// test if *this < _Right
////			return (false);
////		}
////	};
////
////	template<class _This,
////		class... _Rest>
////		class tuple<_This, _Rest...>
////		: private tuple<_Rest...>
////	{	// recursive tuple definition
////	public:
////		typedef _This _This_type;
////		typedef tuple<_Rest...> _Mybase;
////
////		template<class _Tag,
////			class _This2,
////			class... _Rest2,
////			Seed::Core::StaticAsserts::EnableIfV<Seed::Core::StaticAsserts::IsSameT<_Tag, _Exact_args_t>::State, int> = 0>
////			constexpr tuple(_Tag, _This2 && _This_arg, _Rest2 && ... _Rest_arg)
////			: _Mybase(_Exact_args_t{}, Seed::Core::ForwardReference<_Rest2>(_Rest_arg)...),
////			_Myfirst(Seed::Core::ForwardReference<_This2>(_This_arg))
////		{	// construct from one arg per element
////		}
////
////		template<class _Tag,
////			class _Tpl,
////			size_t... _Indices,
////			typename Seed::Core::StaticAsserts::EnableIfV<Seed::Core::StaticAsserts::IsSameT<_Tag, _Unpack_tuple_t>::State, int> = 0>
////			constexpr tuple(_Tag, _Tpl && _Right, index_sequence<_Indices...>);
////
////		template<class _Tag,
////			class _Tpl,
////			typename Seed::Core::StaticAsserts::EnableIfV<Seed::Core::StaticAsserts::IsSameT<_Tag, _Unpack_tuple_t>::State, int>>
////			constexpr tuple(_Tag, _Tpl && _Right)
////			: tuple(_Unpack_tuple_t{}, Seed::Core::ForwardReference<_Tpl>(_Right),
////				make_index_sequence<tuple_size_v<remove_reference_t<_Tpl>>>{})
////		{	// construct from unpacking a tuple/pair
////		}
////
////
////		template<class _This2 = _This, typename
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			Seed::Core::StaticAsserts::IsDefaultConstructibleT<_This2>,
////			Seed::Core::StaticAsserts::IsDefaultConstructibleT<_Rest>...,
////			Seed::Core::StaticAsserts::IsImplicitlyDefaultConstructibleT<_This2>,
////			Seed::Core::StaticAsserts::IsImplicitlyDefaultConstructibleT<_Rest>...
////		>, int>::Type>
////			constexpr tuple()
////			noexcept(Seed::Core::StaticAsserts::And<
////				Seed::Core::StaticAsserts::IsNothrowDefaultConstructibleT<_This2>,
////				Seed::Core::StaticAsserts::IsNothrowDefaultConstructibleT<_Rest>...
////			>)	// strengthened
////			: _Mybase(), _Myfirst()
////		{
////		}
////
////		template<class _This2 = _This,
////			typename Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			Seed::Core::StaticAsserts::IsDefaultConstructibleT<_This2>,
////			Seed::Core::StaticAsserts::IsDefaultConstructibleT<_Rest>...,
////			Seed::Core::StaticAsserts::NegationT<Seed::Core::StaticAsserts::AndT<
////			Seed::Core::StaticAsserts::IsImplicitlyDefaultConstructibleT<_This2>,
////			Seed::Core::StaticAsserts::IsImplicitlyDefaultConstructibleT<_Rest>...
////			>>>, int>>
////			constexpr explicit tuple()
////			noexcept(Seed::Core::StaticAsserts::AndT<
////				Seed::Core::StaticAsserts::IsNothrowDefaultConstructibleT<_This2>,
////				Seed::Core::StaticAsserts::IsNothrowDefaultConstructibleT<_Rest>...
////			>::State)	// strengthened
////			: _Mybase(), _Myfirst()
////		{
////		}
////
////		template<class _This2 = _This,
////			Seed::Core::StaticAsserts::EnableIfV<_Tuple_implicit_val<tuple, const _This2&, const _Rest& ...>::value, int>>
////			constexpr tuple(const _This & _This_arg, const _Rest & ... _Rest_arg)
////			noexcept(Seed::Core::StaticAsserts::AndT<
////				Seed::Core::StaticAsserts::IsNothrowDefaultConstructibleT<_This2>,
////				Seed::Core::StaticAsserts::IsNothrowDefaultConstructibleT<_Rest>...
////			>::State)	// strengthened
////			: tuple(_Exact_args_t{}, _This_arg, _Rest_arg...)
////		{
////		}
////
////		template<class _This2 = _This,
////			Seed::Core::StaticAsserts::EnableIfV<_Tuple_explicit_val<tuple, const _This2&, const _Rest& ...>::value, int>>
////			constexpr explicit tuple(const _This & _This_arg, const _Rest & ... _Rest_arg)
////			noexcept(Seed::Core::StaticAsserts::AndT<
////				Seed::Core::StaticAsserts::IsNothrowDefaultConstructibleT<_This2>,
////				Seed::Core::StaticAsserts::IsNothrowDefaultConstructibleT<_Rest>...
////			>)	// strengthened
////			: tuple(_Exact_args_t{}, _This_arg, _Rest_arg...)
////		{
////		}
////
////		template<class _This2,
////			class... _Rest2,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			_Tuple_perfect_val<tuple, _This2, _Rest2...>,
////			_Tuple_implicit_val<tuple, _This2, _Rest2...>
////		>, int> = 0>
////			constexpr tuple(_This2 && _This_arg, _Rest2 && ... _Rest_arg)
////			noexcept(_Tuple_nothrow_constructible_v<tuple, _This2, _Rest2...>)	// strengthened
////			: tuple(_Exact_args_t{}, Seed::Core::ForwardReference<_This2>(_This_arg), Seed::Core::ForwardReference<_Rest2>(_Rest_arg)...)
////		{
////		}
////
////		template<class _This2,
////			class... _Rest2,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			_Tuple_perfect_val<tuple, _This2, _Rest2...>,
////			_Tuple_explicit_val<tuple, _This2, _Rest2...>
////		>, int> = 0>
////			constexpr explicit tuple(_This2 && _This_arg, _Rest2 && ... _Rest_arg)
////			noexcept(_Tuple_nothrow_constructible_v<tuple, _This2, _Rest2...>)	// strengthened
////			: tuple(_Exact_args_t{}, Seed::Core::ForwardReference<_This2>(_This_arg), Seed::Core::ForwardReference<_Rest2>(_Rest_arg)...)
////		{
////		}
////
////		tuple(const tuple&) = default;
////		tuple(tuple&&) = default;
////
////		template<class... _Other,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::StaticAsserts::AndT<
////			_Tuple_implicit_val<tuple, const _Other& ...>,
////			_Tuple_convert_copy_val<tuple, _Other...>
////		>, int> = 0>
////			constexpr tuple(const tuple<_Other...> & _Right)
////			noexcept(_Tuple_nothrow_constructible_v<tuple, const _Other & ...>)	// strengthened
////			: tuple(_Unpack_tuple_t{}, _Right)
////		{
////		}
////
////		template<class... _Other,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			_Tuple_explicit_val<tuple, const _Other& ...>,
////			_Tuple_convert_copy_val<tuple, _Other...>
////		>, int> = 0>
////			constexpr explicit tuple(const tuple<_Other...> & _Right)
////			noexcept(_Tuple_nothrow_constructible_v<tuple, const _Other & ...>)	// strengthened
////			: tuple(_Unpack_tuple_t{}, _Right)
////		{
////		}
////
////		template<class... _Other,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			_Tuple_implicit_val<tuple, _Other...>,
////			_Tuple_convert_move_val<tuple, _Other...>
////		>, int> = 0>
////			constexpr tuple(tuple<_Other...> && _Right)
////			noexcept(_Tuple_nothrow_constructible_v<tuple, _Other...>)	// strengthened
////			: tuple(_Unpack_tuple_t{}, _STD move(_Right))
////		{
////		}
////
////		template<class... _Other,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			_Tuple_explicit_val<tuple, _Other...>,
////			_Tuple_convert_move_val<tuple, _Other...>
////		>, int> = 0>
////			constexpr explicit tuple(tuple<_Other...> && _Right)
////			noexcept(_Tuple_nothrow_constructible_v<tuple, _Other...>)	// strengthened
////			: tuple(_Unpack_tuple_t{}, _STD move(_Right))
////		{
////		}
////
////		//template<class _First,
////		//	class _Second,
////		//	Seed::Core::StaticAsserts::EnableIfV<_Tuple_implicit_val<tuple, const _First&, const _Second&>::value, int> = 0>
////		//	constexpr tuple(const pair<_First, _Second> & _Right)
////		//	noexcept(_Tuple_nothrow_constructible_v<tuple, const _First&, const _Second&>)	// strengthened
////		//	: tuple(_Unpack_tuple_t{}, _Right)
////		//{
////		//}
////
////		//template<class _First,
////		//	class _Second,
////		//	Seed::Core::StaticAsserts::EnableIfV<_Tuple_explicit_val<tuple, const _First&, const _Second&>::value, int> = 0>
////		//	constexpr explicit tuple(const pair<_First, _Second> & _Right)
////		//	noexcept(_Tuple_nothrow_constructible_v<tuple, const _First&, const _Second&>)	// strengthened
////		//	: tuple(_Unpack_tuple_t{}, _Right)
////		//{
////		//}
////
////		//template<class _First,
////		//	class _Second,
////		//	Seed::Core::StaticAsserts::EnableIfV<_Tuple_implicit_val<tuple, _First, _Second>::value, int> = 0>
////		//	constexpr tuple(pair<_First, _Second> && _Right)
////		//	noexcept(_Tuple_nothrow_constructible_v<tuple, _First, _Second>)	// strengthened
////		//	: tuple(_Unpack_tuple_t{}, _STD move(_Right))
////		//{
////		//}
////
////		//template<class _First,
////		//	class _Second,
////		//	Seed::Core::StaticAsserts::EnableIfV<_Tuple_explicit_val<tuple, _First, _Second>::value, int> = 0>
////		//	constexpr explicit tuple(pair<_First, _Second> && _Right)
////		//	noexcept(_Tuple_nothrow_constructible_v<tuple, _First, _Second>)	// strengthened
////		//	: tuple(_Unpack_tuple_t{}, _STD move(_Right))
////		//{
////		//}
////
////		/*template<class _Alloc,
////			class _This2 = _This,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			Seed::Core::StaticAsserts::IsDefaultConstructibleT<_This2>,
////			Seed::Core::StaticAsserts::IsDefaultConstructibleT<_Rest>...
////		>, int> = 0>
////			tuple(allocator_arg_t, const _Alloc & _Al)
////			: _Mybase(allocator_arg, _Al), _Myfirst(_Al, allocator_arg)
////		{
////		}
////
////		template<class _Alloc,
////			class _This2 = _This,
////			Seed::Core::StaticAsserts::EnableIfV<_Tuple_implicit_val<tuple, const _This2&, const _Rest& ...>::value, int> = 0>
////			tuple(allocator_arg_t, const _Alloc & _Al,
////				const _This & _This_arg, const _Rest & ... _Rest_arg)
////			: tuple(_Alloc_exact_args_t{}, _Al, _This_arg, _Rest_arg...)
////		{
////		}
////
////		template<class _Alloc,
////			class _This2 = _This,
////			Seed::Core::StaticAsserts::EnableIfV<_Tuple_explicit_val<tuple, const _This2&, const _Rest& ...>::value, int> = 0>
////			explicit tuple(allocator_arg_t, const _Alloc & _Al,
////				const _This & _This_arg, const _Rest & ... _Rest_arg)
////			: tuple(_Alloc_exact_args_t{}, _Al, _This_arg, _Rest_arg...)
////		{
////		}
////
////		template<class _Alloc,
////			class _This2,
////			class... _Rest2,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			_Tuple_perfect_val<tuple, _This2, _Rest2...>,
////			_Tuple_explicit_val<tuple, _This2, _Rest2...>
////		>, int> = 0>
////			explicit tuple(allocator_arg_t, const _Alloc & _Al,
////				_This2 && _This_arg, _Rest2 && ... _Rest_arg)
////			: tuple(_Alloc_exact_args_t{}, _Al, Seed::Core::ForwardReference<_This2>(_This_arg), Seed::Core::ForwardReference<_Rest2>(_Rest_arg)...)
////		{
////		}
////
////		template<class _Alloc,
////			class _This2 = _This,
////			Seed::Core::StaticAsserts::EnableIfTV<_Tuple_constructible_v<tuple, const _This2&, const _Rest& ...>, int> = 0>
////			tuple(allocator_arg_t, const _Alloc & _Al,
////				const tuple & _Right)
////			: tuple(_Alloc_unpack_tuple_t{}, _Al, _Right)
////		{
////		}
////
////		template<class _Alloc,
////			class _This2 = _This,
////			Seed::Core::StaticAsserts::EnableIfTV<_Tuple_constructible_v<tuple, _This2, _Rest...>, int> = 0>
////			tuple(allocator_arg_t, const _Alloc & _Al,
////				tuple && _Right)
////			: tuple(_Alloc_unpack_tuple_t{}, _Al, Seed::MoveRValue(_Right))
////		{
////		}
////
////		template<class _Alloc,
////			class... _Other,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			_Tuple_implicit_val<tuple, const _Other& ...>,
////			_Tuple_convert_copy_val<tuple, _Other...>
////		>, int> = 0>
////			tuple(allocator_arg_t, const _Alloc & _Al,
////				const tuple<_Other...> & _Right)
////			: tuple(_Alloc_unpack_tuple_t{}, _Al, _Right)
////		{
////		}
////
////		template<class _Alloc,
////			class... _Other,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::And<
////			_Tuple_explicit_val<tuple, const _Other& ...>,
////			_Tuple_convert_copy_val<tuple, _Other...>
////		>, int> = 0>
////			explicit tuple(allocator_arg_t, const _Alloc & _Al,
////				const tuple<_Other...> & _Right)
////			: tuple(_Alloc_unpack_tuple_t{}, _Al, _Right)
////		{
////		}
////
////		template<class _Alloc,
////			class... _Other,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::And<
////			_Tuple_implicit_val<tuple, _Other...>,
////			_Tuple_convert_move_val<tuple, _Other...>
////		>, int> = 0>
////			tuple(allocator_arg_t, const _Alloc & _Al,
////				tuple<_Other...> && _Right)
////			: tuple(_Alloc_unpack_tuple_t{}, _Al, Seed::MoveRValue(_Right))
////		{
////		}
////
////		template<class _Alloc,
////			class... _Other,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::And<
////			_Tuple_explicit_val<tuple, _Other...>,
////			_Tuple_convert_move_val<tuple, _Other...>
////		>, int> = 0>
////			explicit tuple(allocator_arg_t, const _Alloc & _Al,
////				tuple<_Other...> && _Right)
////			: tuple(_Alloc_unpack_tuple_t{}, _Al, Seed::MoveRValue(_Right))
////		{
////		}
////
////		template<class _Alloc,
////			class _First,
////			class _Second,
////			Seed::Core::StaticAsserts::EnableIfV<_Tuple_implicit_val<tuple, const _First&, const _Second&>::value, int> = 0>
////			tuple(allocator_arg_t, const _Alloc & _Al,
////				const pair<_First, _Second> & _Right)
////			: tuple(_Alloc_unpack_tuple_t{}, _Al, _Right)
////		{
////		}
////
////		template<class _Alloc,
////			class _First,
////			class _Second,
////			Seed::Core::StaticAsserts::EnableIfV<_Tuple_explicit_val<tuple, const _First&, const _Second&>::value, int> = 0>
////			explicit tuple(allocator_arg_t, const _Alloc & _Al,
////				const pair<_First, _Second> & _Right)
////			: tuple(_Alloc_unpack_tuple_t{}, _Al, _Right)
////		{
////		}
////
////		template<class _Alloc,
////			class _First,
////			class _Second,
////			Seed::Core::StaticAsserts::EnableIfV<_Tuple_implicit_val<tuple, _First, _Second>::value, int> = 0>
////			tuple(allocator_arg_t, const _Alloc & _Al,
////				pair<_First, _Second> && _Right)
////			: tuple(_Alloc_unpack_tuple_t{}, _Al, Seed::MoveRValue(_Right))
////		{
////		}
////
////		template<class _Alloc,
////			class _First,
////			class _Second,
////			Seed::Core::StaticAsserts::EnableIfV<_Tuple_explicit_val<tuple, _First, _Second>::value, int> = 0>
////			explicit tuple(allocator_arg_t, const _Alloc & _Al,
////				pair<_First, _Second> && _Right)
////			: tuple(_Alloc_unpack_tuple_t{}, _Al, Seed::MoveRValue(_Right))
////		{
////		}*/
////
////		//template<class _Alloc,
////		//	class _This2,
////		//	class... _Rest2,
////		//	Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////		//	_Tuple_perfect_val<tuple, _This2, _Rest2...>,
////		//	_Tuple_implicit_val<tuple, _This2, _Rest2...>
////		//>, int> = 0>
////		//tuple(allocator_arg_t, const _Alloc & _Al,
////		//	_This2 && _This_arg, _Rest2 && ... _Rest_arg)
////		//	: tuple(_Alloc_exact_args_t{}, _Al, Seed::Core::ForwardReference<_This2>(_This_arg), Seed::Core::ForwardReference<_Rest2>(_Rest_arg)...)
////		//{
////		//}
////
////		tuple& operator=(const volatile tuple&) = delete;
////
////		template<class _Myself = tuple,
////			class _This2 = _This,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			Seed::Core::StaticAsserts::IsMoveAssignableT<_This2>,
////			Seed::Core::StaticAsserts::IsCopyAssignableT<_Rest>...
////		>, int> = 0>
////			tuple & operator=(Seed::Core::StaticAsserts::IdentitV<const _Myself&> _Right)
////			noexcept(Seed::Core::StaticAsserts::And<
////				Seed::Core::StaticAsserts::IsNothrowAssignableT<_This2>,
////				Seed::Core::StaticAsserts::IsNothrowAssignableT<_Rest>...
////			>)	// strengthened
////		{
////			_Myfirst._Val = _Right._Myfirst._Val;
////			_Get_rest() = _Right._Get_rest();
////			return (*this);
////		}
////
////		template<class _Myself = tuple,
////			class _This2 = _This,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			Seed::Core::StaticAsserts::IsMoveAssignableT<_This2>,
////			Seed::Core::StaticAsserts::IsMoveAssignableT<_Rest>...
////		>, int> = 0>
////			tuple & operator=(Seed::Core::StaticAsserts::IdentitV<_Myself&&> _Right)
////			noexcept(Seed::Core::StaticAsserts::And<
////				Seed::Core::StaticAsserts::IsNothrowMoveAssignable<_This2>,
////				Seed::Core::StaticAsserts::IsNothrowMoveAssignable<_Rest>...
////			>)
////		{
////			_Myfirst._Val = Seed::Core::ForwardReference<_This>(_Right._Myfirst._Val);
////			_Get_rest() = Seed::Core::ForwardReference<_Mybase>(_Right._Get_rest());
////			return (*this);
////		}
////
////		template<class... _Other,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			Seed::Core::StaticAsserts::NegationT<Seed::Core::StaticAsserts::IsSameT<tuple, tuple<_Other...>>>,
////			_Tuple_assignable_val<tuple, const _Other& ...>
////		>, int> = 0>
////			tuple & operator=(const tuple<_Other...> & _Right)
////			noexcept(_Tuple_nothrow_assignable_v<tuple, const _Other & ...>)	// strengthened
////		{
////			_Myfirst._Val = _Right._Myfirst._Val;
////			_Get_rest() = _Right._Get_rest();
////			return (*this);
////		}
////
////		template<class... _Other,
////			Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::AndT<
////			Seed::Core::StaticAsserts::NegationT<Seed::Core::StaticAsserts::IsSameT<tuple, tuple<_Other...>>>,
////			_Tuple_assignable_val<tuple, _Other...>
////		>, int> = 0>
////			tuple & operator=(tuple<_Other...> && _Right)
////			noexcept(_Tuple_nothrow_assignable_v<tuple, _Other...>)	// strengthened
////		{
////			_Myfirst._Val = Seed::Core::ForwardReference<typename tuple<_Other...>::_This_type>(_Right._Myfirst._Val);
////			_Get_rest() = Seed::Core::ForwardReference<typename tuple<_Other...>::_Mybase>(_Right._Get_rest());
////			return (*this);
////		}
////
////		//template<class _First,
////		//	class _Second,
////		//	Seed::Core::StaticAsserts::EnableIfTV<_Tuple_assignable_v<tuple, const _First&, const _Second&>, int> = 0>
////		//	tuple & operator=(const pair<_First, _Second> & _Right)
////		//	noexcept(_Tuple_nothrow_assignable_v<tuple, const _First&, const _Second&>)	// strengthened
////		//{
////		//	_Myfirst._Val = _Right.first;
////		//	_Get_rest()._Myfirst._Val = _Right.second;
////		//	return (*this);
////		//}
////
////		//template<class _First,
////		//	class _Second,
////		//	Seed::Core::StaticAsserts::EnableIfTV<_Tuple_assignable_v<tuple, _First, _Second>, int> = 0>
////		//	tuple & operator=(pair<_First, _Second> && _Right)
////		//	noexcept(_Tuple_nothrow_assignable_v<tuple, _First, _Second>)	// strengthened
////		//{
////		//	_Myfirst._Val = Seed::Core::ForwardReference<_First>(_Right.first);
////		//	_Get_rest()._Myfirst._Val = Seed::Core::ForwardReference<_Second>(_Right.second);
////		//	return (*this);
////		//}
////
////		void swap(tuple& _Right)
////			noexcept(Seed::Core::StaticAsserts::And<_Is_nothrow_swappable<_This>,
////				_Is_nothrow_swappable<_Rest>...>)
////		{
////			_Swap_adl(_Myfirst._Val, _Right._Myfirst._Val);
////			_Mybase::swap(_Right._Get_rest());
////		}
////
////		_Mybase& _Get_rest() noexcept
////		{	// get reference to rest of elements
////			return (*this);
////		}
////
////		constexpr const _Mybase& _Get_rest() const noexcept
////		{	// get const reference to rest of elements
////			return (*this);
////		}
////
////		template<class... _Other>
////		constexpr bool _Equals(const tuple<_Other...>& _Right) const
////		{	// test if *this == _Right
////			return (_Myfirst._Val == _Right._Myfirst._Val
////				&& _Mybase::_Equals(_Right._Get_rest()));
////		}
////
////		template<class... _Other>
////		constexpr bool _Less(const tuple<_Other...> & _Right) const
////		{	// test if *this < _Right
////			return (_Myfirst._Val < _Right._Myfirst._Val
////				|| (!(_Right._Myfirst._Val < _Myfirst._Val)
////					&& _Mybase::_Less(_Right._Get_rest())));
////		}
////
////		_Tuple_val<_This> _Myfirst;	// the stored element
////	};
////
////	template<class... _Types>
////	tuple(_Types...)->tuple<_Types...>;
////
////
////	// OPERATORS FOR tuple
////	template<class... _Types1,
////		class... _Types2>
////		[[nodiscard]] constexpr bool operator==(const tuple<_Types1...> & _Left, const tuple<_Types2...> & _Right)
////	{	// test if _Left == _Right
////		static_assert(sizeof...(_Types1) == sizeof...(_Types2), "cannot compare tuples of different sizes");
////		return (_Left._Equals(_Right));
////	}
////
////	template<class... _Types1,
////		class... _Types2>
////		[[nodiscard]] constexpr bool operator!=(const tuple<_Types1...> & _Left, const tuple<_Types2...> & _Right)
////	{	// test if _Left != _Right
////		return (!(_Left == _Right));
////	}
////
////	template<class... _Types1,
////		class... _Types2>
////		[[nodiscard]] constexpr bool operator<(const tuple<_Types1...> & _Left, const tuple<_Types2...> & _Right)
////	{	// test if _Left < _Right
////		static_assert(sizeof...(_Types1) == sizeof...(_Types2), "cannot compare tuples of different sizes");
////		return (_Left._Less(_Right));
////	}
////
////	template<class... _Types1,
////		class... _Types2>
////		[[nodiscard]] constexpr bool operator>=(const tuple<_Types1...> & _Left, const tuple<_Types2...> & _Right)
////	{	// test if _Left >= _Right
////		return (!(_Left < _Right));
////	}
////
////	template<class... _Types1,
////		class... _Types2>
////		[[nodiscard]] constexpr bool operator>(const tuple<_Types1...> & _Left, const tuple<_Types2...> & _Right)
////	{	// test if _Left > _Right
////		return (_Right < _Left);
////	}
////
////	template<class... _Types1,
////		class... _Types2>
////		[[nodiscard]] constexpr bool operator<=(const tuple<_Types1...> & _Left, const tuple<_Types2...> & _Right)
////	{	// test if _Left <= _Right
////		return (!(_Right < _Left));
////	}
////
////	template<class... _Types,
////		Seed::Core::StaticAsserts::EnableIfV<Seed::Core::StaticAsserts::And<Seed::Core::StaticAsserts::IsSwappable<_Types>...>, int> = 0> inline
////		void swap(tuple<_Types...> & _Left, tuple<_Types...> & _Right)
////		noexcept(noexcept(_Left.swap(_Right)))
////	{	// swap _Left and _Right
////		return (_Left.swap(_Right));
////	}
////
////
////	// CLASS _Tuple_element (find element by type)
////	template<class _Ty,
////		class _Tuple>
////		struct _Tuple_element;
////
////	template<class _This,
////		class... _Rest>
////		struct _Tuple_element<_This, tuple<_This, _Rest...>>
////	{	// select first element
////		typedef int _Check_type;
////		static_assert(Seed::Core::StaticAsserts::IsVoidV<typename _Tuple_element<_This, tuple<_Rest...>>::_Check_type>,
////			"duplicate type T in get<T>(tuple)");
////
////		typedef _This type;
////		typedef tuple<_This, _Rest...> _Ttype;
////	};
////
////	template<class _Ty,
////		class _This,
////		class... _Rest>
////		struct _Tuple_element<_Ty, tuple<_This, _Rest...>>
////		: _Tuple_element<_Ty, tuple<_Rest...>>
////	{	// recursive _Tuple_element definition
////	};
////
////	template<class _Ty>
////	struct _Tuple_element<_Ty, tuple<>>
////	{	// backstop _Tuple_element definition
////		typedef void _Check_type;	// proof that no duplicate type exists
////	};
////
////	// FUNCTION TEMPLATE get (by index)
////	template<size_t _Index,
////		class... _Types>
////		[[nodiscard]] constexpr tuple_element_t<_Index, tuple<_Types...>>&
////		get(tuple<_Types...> & _Tuple) noexcept
////	{	// get reference to _Index element of tuple
////		typedef typename tuple_element<_Index, tuple<_Types...>>::_Ttype _Ttype;
////		return (((_Ttype&)_Tuple)._Myfirst._Val);
////	}
////
////	template<size_t _Index,
////		class... _Types>
////		[[nodiscard]] constexpr const tuple_element_t<_Index, tuple<_Types...>>&
////		get(const tuple<_Types...> & _Tuple) noexcept
////	{	// get const reference to _Index element of tuple
////		typedef typename tuple_element<_Index, tuple<_Types...>>::_Ttype _Ttype;
////		return (((_Ttype&)_Tuple)._Myfirst._Val);
////	}
////
////	template<size_t _Index,
////		class... _Types>
////		[[nodiscard]] constexpr tuple_element_t<_Index, tuple<_Types...>>&&
////		get(tuple<_Types...> && _Tuple) noexcept
////	{	// get rvalue reference to _Index element of tuple
////		typedef typename tuple_element<_Index, tuple<_Types...>>::_Ttype _Ttype;
////		typedef tuple_element_t<_Index, tuple<_Types...>>&& _RRtype;
////		return (Seed::Core::ForwardReference<_RRtype>(((_Ttype&)_Tuple)._Myfirst._Val));
////	}
////
////	template<size_t _Index,
////		class... _Types>
////		[[nodiscard]] constexpr const tuple_element_t<_Index, tuple<_Types...>>&&
////		get(const tuple<_Types...> && _Tuple) noexcept
////	{	// get const rvalue reference to _Index element of tuple
////		typedef typename tuple_element<_Index, tuple<_Types...>>::_Ttype _Ttype;
////		typedef const tuple_element_t<_Index, tuple<_Types...>>&& _RRtype;
////		return (Seed::Core::ForwardReference<_RRtype>(((_Ttype&)_Tuple)._Myfirst._Val));
////	}
////
////	// FUNCTION TEMPLATE get (by type)
////	template<class _Ty,
////		class... _Types>
////		[[nodiscard]] constexpr _Ty& get(tuple<_Types...> & _Tuple) noexcept
////	{	// get reference to _Ty element of tuple
////		typedef typename _Tuple_element<_Ty, tuple<_Types...>>::_Ttype _Ttype;
////		return (((_Ttype&)_Tuple)._Myfirst._Val);
////	}
////
////	template<class _Ty,
////		class... _Types>
////		[[nodiscard]] constexpr const _Ty& get(const tuple<_Types...> & _Tuple) noexcept
////	{	// get const reference to _Ty element of tuple
////		typedef typename _Tuple_element<_Ty, tuple<_Types...>>::_Ttype _Ttype;
////		return (((_Ttype&)_Tuple)._Myfirst._Val);
////	}
////
////	template<class _Ty,
////		class... _Types>
////		[[nodiscard]] constexpr _Ty&& get(tuple<_Types...> && _Tuple) noexcept
////	{	// get rvalue reference to _Ty element of tuple
////		typedef typename _Tuple_element<_Ty, tuple<_Types...>>::_Ttype _Ttype;
////		return (Seed::Core::ForwardReference<_Ty&&>(((_Ttype&)_Tuple)._Myfirst._Val));
////	}
////
////	template<class _Ty,
////		class... _Types>
////		[[nodiscard]] constexpr const _Ty&& get(const tuple<_Types...> && _Tuple) noexcept
////	{	// get const rvalue reference to _Ty element of tuple
////		typedef typename _Tuple_element<_Ty, tuple<_Types...>>::_Ttype _Ttype;
////		return (Seed::Core::ForwardReference<const _Ty&&>(((_Ttype&)_Tuple)._Myfirst._Val));
////	}
////
////	//// CONSTRUCTOR TEMPLATES FOR tuple
////	//template<class _This,
////	//	class... _Rest>
////	//	template<class _Tag,
////	//	class _Tpl,
////	//	size_t... _Indices,
////	//	Seed::Core::StaticAsserts::EnableIfTV<Seed::Core::StaticAsserts::IsSameT<_Tag, _Unpack_tuple_t>(), int> = 0>
////	//	constexpr tuple<_This, _Rest...>::tuple(_Tag, _Tpl && _Right, index_sequence<_Indices...>)
////	//	: tuple(_Exact_args_t{}, std::get<_Indices>(Seed::Core::ForwardReference<_Tpl>(_Right))...)
////	//{	// construct from unpacking a tuple/pair, using get<I>()
////	//}
////
////
////	//// FUNCTION TEMPLATE make_tuple
////	////template<class... _Types>
////	////[[nodiscard]] constexpr tuple<_Unrefwrap_t<_Types>...>
////	////	make_tuple(_Types && ... _Args)
////	////{	// make tuple from elements
////	////	typedef tuple<_Unrefwrap_t<_Types>...> _Ttype;
////	////	return (_Ttype(Seed::Core::ForwardReference<_Types>(_Args)...));
////	////}
////
////	// FUNCTION TEMPLATE tie
////	template<class... _Types>
////	[[nodiscard]] constexpr tuple<_Types& ...>
////		tie(_Types & ... _Args) noexcept
////	{	// make tuple from elements
////		typedef tuple<_Types& ...> _Ttype;
////		return (_Ttype(_Args...));
////	}
////
////	// FUNCTION TEMPLATE forward_as_tuple
////	template<class... _Types>
////	[[nodiscard]] constexpr tuple<_Types&& ...>
////		forward_as_tuple(_Types && ... _Args) noexcept
////	{	// forward arguments in a tuple
////		return (tuple<_Types && ...>(Seed::Core::ForwardReference<_Types>(_Args)...));
////	}
////
////	// STRUCT TEMPLATE _Cat_sequences
////	template<class _Seq_type1,
////		class _Seq_type2>
////		struct _Cat_sequences;
////
////	template<size_t... _Indexes1,
////		size_t... _Indexes2>
////		struct _Cat_sequences<index_sequence<_Indexes1...>,
////		index_sequence<_Indexes2...>>
////	{	// concatenates two index_sequence types
////		typedef index_sequence<_Indexes1..., _Indexes2...> type;
////	};
////
////	// FORWARD DECLARATIONS
////	template<class _Ty,
////		size_t _Size>
////		class array;
////
////	template<size_t _Idx,
////		class _Ty,
////		size_t _Size>
////		[[nodiscard]] constexpr _Ty& get(array<_Ty, _Size> & _Arr) noexcept;
////
////	template<size_t _Idx,
////		class _Ty,
////		size_t _Size>
////		[[nodiscard]] constexpr const _Ty& get(const array<_Ty, _Size> & _Arr) noexcept;
////
////	template<size_t _Idx,
////		class _Ty,
////		size_t _Size>
////		[[nodiscard]] constexpr _Ty&& get(array<_Ty, _Size> && _Arr) noexcept;
////
////	template<size_t _Idx,
////		class _Ty,
////		size_t _Size>
////		[[nodiscard]] constexpr const _Ty&& get(const array<_Ty, _Size> && _Arr) noexcept;
////
////	// STRUCT TEMPLATE _View_as_tuple
////	template<class _Ty,
////		class... _For_array>
////		struct _View_as_tuple
////	{	// tuple_cat() supports only tuples, pairs, and arrays
////		static_assert(_Always_false<_Ty>,
////			"Unsupported tuple_cat arguments.");
////	};
////
////	template<class... _Types>
////	struct _View_as_tuple<tuple<_Types...>>
////	{	// view a tuple as a tuple
////		typedef tuple<_Types...> type;
////	};
////
////
////	// STRUCT TEMPLATE _Repeat_for
////	template<size_t _Nx,
////		class _Ty>
////		struct _Repeat_for
////		: integral_constant<size_t, _Nx>
////	{	// repeats _Nx for each _Ty in a parameter pack
////	};
////
////	// FUNCTION TEMPLATE tuple_cat
////	template<class _Ret,
////		class _Kx_arg,
////		class _Ix_arg,
////		size_t _Ix_next,
////		class... _Tuples>
////		struct _Tuple_cat2
////	{	// determine tuple_cat's return type and _Kx/_Ix indices
////		static_assert(sizeof...(_Tuples) == 0,
////			"Unsupported tuple_cat arguments.");
////		typedef _Ret type;
////		typedef _Kx_arg _Kx_arg_seq;
////		typedef _Ix_arg _Ix_arg_seq;
////	};
////
////	template<class... _Types1,
////		class _Kx_arg,
////		size_t... _Ix,
////		size_t _Ix_next,
////		class... _Types2,
////		class... _Rest>
////		struct _Tuple_cat2<tuple<_Types1...>, _Kx_arg,
////		index_sequence<_Ix...>, _Ix_next,
////		tuple<_Types2...>, _Rest...>
////		: _Tuple_cat2<
////		tuple<_Types1..., _Types2...>,
////		typename _Cat_sequences<_Kx_arg, index_sequence_for<_Types2...>>::type,
////		index_sequence<_Ix..., _Repeat_for<_Ix_next, _Types2>::value...>,
////		_Ix_next + 1,
////		_Rest...>
////	{	// determine tuple_cat's return type and _Kx/_Ix indices
////	};
////
////	template<class... _Tuples>
////	struct _Tuple_cat1
////		: _Tuple_cat2<tuple<>, index_sequence<>,
////		index_sequence<>, 0,
////		typename _View_as_tuple<Seed::Core::StaticAsserts::Decay<_Tuples>>...>
////	{	// prepare to determine tuple_cat's return type and _Kx/_Ix indices
////	};
////
////#pragma warning(push)
////#pragma warning(disable: 4100)	// TRANSITION, VSO#181496, unreferenced formal parameter
////	template<class _Ret,
////		size_t... _Kx,
////		size_t... _Ix,
////		class _Ty>
////		constexpr _Ret _Tuple_cat(index_sequence<_Kx...>,
////			index_sequence<_Ix...>, _Ty&& _Arg)
////	{	// concatenate tuples
////		return (_Ret(std::get<_Kx>(std::get<_Ix>(Seed::Core::ForwardReference<_Ty>(_Arg)))...));
////	}
////#pragma warning(pop)
////
////	template<class... _Tuples>
////	[[nodiscard]] constexpr typename _Tuple_cat1<_Tuples...>::type
////		tuple_cat(_Tuples && ... _Tpls)
////	{	// concatenate tuples
////		typedef _Tuple_cat1<_Tuples...> _Cat1;
////		return (_Tuple_cat<typename _Cat1::type>(
////			typename _Cat1::_Kx_arg_seq(), typename _Cat1::_Ix_arg_seq(),
////			forward_as_tuple(Seed::Core::ForwardReference<_Tuples>(_Tpls)...)));
////	}
////
////	// FUNCTION TEMPLATE apply
////#pragma warning(push)
////#pragma warning(disable: 4100)	// TRANSITION, VSO#181496, unreferenced formal parameter
////	template<class _Callable,
////		class _Tuple,
////		size_t... _Indices>
////		constexpr decltype(auto) _Apply_impl(_Callable && _Obj, _Tuple && _Tpl, index_sequence<_Indices...>)
////	{	// invoke _Obj with the elements of _Tpl
////		return (_C_invoke(Seed::Core::ForwardReference<_Callable>(_Obj), std::get<_Indices>(Seed::Core::ForwardReference<_Tuple>(_Tpl))...));
////	}
////#pragma warning(pop)
////
////	template<class _Callable,
////		class _Tuple>
////		constexpr decltype(auto) apply(_Callable && _Obj, _Tuple && _Tpl)
////	{	// invoke _Obj with the elements of _Tpl
////		return (_Apply_impl(Seed::Core::ForwardReference<_Callable>(_Obj), Seed::Core::ForwardReference<_Tuple>(_Tpl),
////			make_index_sequence<tuple_size_v<Seed::Core::StaticAsserts::RemoveRefT<_Tuple>>>{}));
////	}
////
////	// FUNCTION TEMPLATE make_from_tuple
////#pragma warning(push)
////#pragma warning(disable: 4100)	// TRANSITION, VSO#181496, unreferenced formal parameter
////	template<class _Ty,
////		class _Tuple,
////		size_t... _Indices>
////		constexpr _Ty _Make_from_tuple_impl(_Tuple&& _Tpl, index_sequence<_Indices...>)
////	{	// construct _Ty from the elements of _Tpl
////		return (_Ty(std::get<_Indices>(Seed::Core::ForwardReference<_Tuple>(_Tpl))...));
////	}
////#pragma warning(pop)
////
////	template<class _Ty,
////		class _Tuple>
////		[[nodiscard]] constexpr _Ty make_from_tuple(_Tuple&& _Tpl)
////	{	// construct _Ty from the elements of _Tpl
////		return (_Make_from_tuple_impl<_Ty>(Seed::Core::ForwardReference<_Tuple>(_Tpl),
////			make_index_sequence<tuple_size_v<Seed::Core::StaticAsserts::RemoveRefT<_Tuple>>>{}));
////	}
////
////
////	// FUNCTION TEMPLATE _For_each_tuple_element
////	template<class _Tpl,
////		class _Fx,
////		size_t... _Indices> inline
////		void _For_each_tuple_element_impl(_Tpl&& _Tuple,
////			_Fx _Func, index_sequence<_Indices...>)
////	{	// call _Func() on the _Indices elements of _Tuple
////		int _Ignored[] = { (static_cast<void>(_Func(
////			std::get<_Indices>(Seed::Core::ForwardReference<_Tpl>(_Tuple))
////			)), 0)... };
////		(void)_Ignored;
////	}
////
////	template<class _Tpl,
////		class _Fx> inline
////		void _For_each_tuple_element(_Tpl&& _Tuple, _Fx _Func)
////	{	// call _Func() on each element in _Tuple
////		_For_each_tuple_element_impl(
////			Seed::Core::ForwardReference<_Tpl>(_Tuple),
////			_Func,
////			make_index_sequence<tuple_size_v<Seed::Core::StaticAsserts::RemoveRefT<_Tpl>>>()
////		);
////	}
////}
////
//////namespace Seed {
//////	inline namespace Core {
//////		template<typename ... Types>
//////		class Tuple {
//////		private:
//////			CRT::tuple<Types...> data;
//////		public:
//////
//////		};
//////	}
//////}
////
////namespace Seed {
////	inline namespace Core {
////		template<typename ... Types>
////		using Tuple = ::std::tuple<Types...>;
////	}
////}
//
//#include "Seed.Core.Base.h"
//
//namespace Seed {
//	inline namespace Core {
//		template<typename ... Types>
//		class Tuple;
//
//		namespace TupleImpl {
//			template <SizeT I_, typename ValueType_,
//				bool IsEmpty_ = StaticAsserts::IsEmptyV<ValueType_> && StaticAsserts::IsFinalV<ValueType_>
//			>
//				class TupleLeaf;
//
//			template <size_t I, typename ValueType, bool IsEmpty>
//			inline void Swap(TupleLeaf<I, ValueType, IsEmpty>& a,
//				TupleLeaf<I, ValueType, IsEmpty>& b) {
//				Swap(a.get(), b.get());
//			}
//
//			template <size_t I_, typename ValueType_, bool IsEmpty_>
//			class TupleLeaf {
//				ValueType_ value_;
//
//				TupleLeaf& operator=(const TupleLeaf&) = delete;
//			public:
//				TupleLeaf()
//					: value_() {
//					static_assert(!StaticAsserts::IsReferenceV<ValueType_>,
//						"Attempt to default construct a reference element in a tuple");
//				}
//
//				TupleLeaf(const TupleLeaf& t)
//					: value_(t.get()) {
//					static_assert(!StaticAsserts::IsRValueReferenceV<ValueType_>,
//						"Can not copy a tuple with rvalue reference member");
//				}
//
//				template <typename T, typename = typename
//					StaticAsserts::EnableIfTV<StaticAsserts::IsConstructibleT<ValueType_, T>>
//					explicit TupleLeaf(T&& t)
//					: value_(ForwardReference<T>(t)) {
//					//static_assert(!StaticAsserts::IsReferenceV<ValueType_> ||
//					//	(StaticAsserts::IsLValueRefenceV<ValueType_> &&
//					//		StaticAsserts::IsLValueRefenceV<T> ||
//					//		(StaticAsserts::IsRValueReferenceV<ValueType_> &&
//					//			!StaticAsserts::IsLValueRefence<T>)),
//					//	"Attempted to construct a reference element in a tuple with an rvalue");
//				}
//
//				template <typename T>
//				explicit TupleLeaf(const TupleLeaf<I_, T>& t)
//					: value_(t.get()) {
//				}
//
//				template <typename T>
//				TupleLeaf& operator=(T&& t) {
//					value_ = ForwardReference<T>(t);
//					return *this;
//				}
//
//				int Swap(TupleLeaf& t) {
//					Swap(*this, t);
//					return 0;
//				}
//
//				ValueType_& get() {
//					return value_;
//				}
//
//				const ValueType_& get() const {
//					return value_;
//				}
//			};
//
//			// TupleLeaf specialization in case we can use Empty Base Class Optimization:
//
//			template <size_t I_, typename ValueType_>
//			class TupleLeaf<I_, ValueType_, true>
//				: private ValueType_ {
//
//				TupleLeaf& operator=(const TupleLeaf&) = delete;
//			public:
//				TupleLeaf() {
//				}
//
//				TupleLeaf(const TupleLeaf& t)
//					: ValueType_(t.get()) {
//					static_assert(!StaticAsserts::IsRValueReferenceV<ValueType_>,
//						"Can not copy a tuple with rvalue reference member");
//				}
//
//				template <typename T, typename = typename StaticAsserts::EnableIf<
//					StaticAsserts::IsConstructible<ValueType_, T>>::Type>
//					explicit TupleLeaf(T&& t)
//					: ValueType_(ForwardReference<T>(t)) {
//				}
//
//				template <typename T>
//				explicit TupleLeaf(const TupleLeaf<I_, T>& t)
//					: ValueType_(t.get()) {
//				}
//
//				template <typename T>
//				TupleLeaf& operator=(T&& t) {
//					ValueType_::operator=(ForwardReference<T>(t));
//					return *this;
//				}
//
//				int Swap(TupleLeaf& t) {
//					Swap(*this, t);
//					return 0;
//				}
//
//				ValueType_& get() {
//					return static_cast<ValueType_&>(*this);
//				}
//
//				const ValueType_& get() const {
//					return static_cast<const ValueType_&>(*this);
//				}
//			};
//
//			template <SizeT... Is_>
//			struct TupleIndexes {
//			};
//
//			template <SizeT Start_, typename TupleIndexes_, SizeT End_>
//			struct MakeTupleIndexesImpl;
//
//			template <SizeT Start_, SizeT... Indexes_, SizeT End_>
//			struct MakeTupleIndexesImpl<Start_, TupleIndexes<Indexes_...>, End_> {
//				typedef typename MakeTupleIndexesImpl<Start_ + 1,
//					TupleIndexes<Indexes_..., Start_>, End_>::Type Type;
//			};
//
//			template <SizeT End_, SizeT... Indexes_>
//			struct MakeTupleIndexesImpl<End_, TupleIndexes<Indexes_...>, End_> {
//				typedef TupleIndexes<Indexes_...> Type;
//			};
//
//			template <SizeT End_, SizeT Start_ = 0>
//			struct MakeTupleIndexes {
//				static_assert(Start_ <= End_, "MakeTupleIndexes: invalid parameters");
//				typedef typename MakeTupleIndexesImpl<Start_, TupleIndexes<>, End_>::Type Type;
//			};
//
//			// TupleTypes
//
//			template <typename... Ts_>
//			struct TupleTypes {
//			};
//
//			// TupleSize
//
//			template <typename T_>
//			struct TupleSize
//				: public tuple_size<T_> {
//			};
//
//			template <typename T_>
//			struct TupleSize<const T_>
//				: public TupleSize<T_> {
//			};
//
//			template <typename T_>
//			struct TupleSize<volatile T_>
//				: public TupleSize<T_> {
//			};
//
//			template <typename T_>
//			struct TupleSize<const volatile T_>
//				: public TupleSize<T_> {
//			};
//
//			template <typename... Ts_>
//			struct TupleSize<TupleTypes<Ts_...>> {
//				using Type = SizeT;
//				constexpr SizeT Value = sizeof...(Ts_);
//
//				inline constexpr operator() { return Value; }
//			};
//
//			// specialization for Tuple
//
//			template <typename... Ts_>
//			struct TupleSize<Tuple<Ts_...>> {
//				using Type = SizeT;
//				constexpr SizeT Value = sizeof...(Ts_);
//
//				inline constexpr operator() { return Value; }
//			};
//
//			// TupleElement
//
//			template <size_t I_, typename T_>
//			class TupleElement : public tuple_element<I_, T_> {
//			};
//
//			template <size_t I_>
//			class TupleElement<I_, TupleTypes<>> {
//			public:
//				static_assert(I_ != I_, "tuple_element index out of range");
//			};
//
//			template <typename H_, typename... Ts_>
//			class TupleElement<0, TupleTypes<H_, Ts_...>> {
//			public:
//				typedef H_ type;
//			};
//
//			template <size_t I_, typename H_, typename... Ts_>
//			class TupleElement<I_, TupleTypes<H_, Ts_...>> {
//			public:
//				typedef typename TupleElement<I_ - 1, TupleTypes<Ts_...>>::type type;
//			};
//
//			// specialization for Tuple
//
//			template <size_t I_, typename... Ts_>
//			class TupleElement<I_, Tuple<Ts_...>> {
//			public:
//				typedef typename TupleElement<I_, TupleTypes<Ts_...>>::Type Type;
//			};
//
//			template <size_t I_, typename... Ts_>
//			class TupleElement<I_, const Tuple<Ts_...>> {
//			public:
//				typedef typename StaticAsserts::AddConstV<typename TupleElement<I_, TupleTypes<Ts_...>>::Type> Type;
//			};
//
//			template <size_t I_, typename... Ts_>
//			class TupleElement<I_, volatile Tuple<Ts_...>> {
//			public:
//				typedef typename StaticAsserts::AddVolatileV<typename TupleElement<I_, TupleTypes<Ts_...>>::Type> Type;
//			};
//
//			template <size_t I_, typename... Ts_>
//			class TupleElement<I_, const volatile Tuple<Ts_...>> {
//			public:
//				typedef typename StaticAsserts::AddCvV<typename TupleElement<I_, TupleTypes<Ts_...>>::Type> Type;
//			};
//
//			// MakeTupleTypes
//
//			template <typename TupleTypes_, typename Tuple_, SizeT Start_, SizeT End_>
//			struct MakeTupleTypesImpl;
//
//			template <typename... Types_, typename Tuple_, SizeT Start_, SizeT End_>
//			struct MakeTupleTypesImpl<TupleTypes<Types_...>, Tuple_, Start_, End_> {
//				typedef StaticAsserts::RemoveRefV<Tuple_> TupleType;
//				typedef typename MakeTupleTypesImpl<TupleTypes<Types_...,
//					typename StaticAsserts::ConditionT<StaticAsserts::IsLValueReferenceV<Tuple_>,
//					// append ref if Tuple_ is ref
//					typename TupleElement<Start_, TupleType>::Type&,
//					// append non-ref otherwise
//					typename TupleElement<Start_, TupleType>::Type>::Type>,
//					Tuple_, Start_ + 1, End_>::Type  Type;
//			};
//
//			template <typename... Types_, typename Tuple_, size_t End_>
//			struct MakeTupleTypesImpl<TupleTypes<Types_...>, Tuple_, End_, End_> {
//				typedef TupleTypes<Types_...> Type;
//			};
//
//			template <typename Tuple_,
//				size_t End_ = TupleSize<typename StaticAsserts::RemoveRefV<Tuple_>>::Value,
//				size_t Start_ = 0>
//				struct MakeTupleTypes {
//				static_assert(Start_ <= End_, "MakeTupleTypes: invalid parameters");
//				typedef typename MakeTupleTypesImpl<TupleTypes<>, Tuple_, Start_, End_>::Type Type;
//			};
//
//			// TupleImpl
//
//			template <typename... Ts>
//			void swallow(Ts&& ...) {
//			}
//
//			template <typename TupleIndexes_, typename... Ts_>
//			struct TupleImpl;
//
//			template <SizeT... Indexes_, typename... Ts_>
//			struct TupleImpl<TupleIndexes<Indexes_...>, Ts_...>
//				: public TupleLeaf<Indexes_, Ts_>... {
//				template <SizeT... FirstIndexes, typename... FirstTypes,
//					SizeT... LastIndexes, typename... LastTypes,
//					typename... ValueTypes>
//					explicit TupleImpl(
//						TupleIndexes<FirstIndexes...>, TupleTypes<FirstTypes...>,
//						TupleIndexes<LastIndexes...>, TupleTypes<LastTypes...>,
//						ValueTypes&& ... values)
//					: TupleLeaf<FirstIndexes,
//					FirstTypes>(forward<ValueTypes>(values))...,
//					TupleLeaf<LastIndexes, LastTypes>()... {
//				}
//
//				template <typename OtherTuple>
//				TupleImpl(OtherTuple&& t)
//					: TupleLeaf<Indexes_, Ts_>(ForwardReference<
//						typename TupleElement<Indexes_,
//						typename MakeTupleTypes<OtherTuple>::Type>::Type
//					>(get<Indexes_>(t)))... {
//				}
//
//				template <typename OtherTuple>
//				TupleImpl& operator=(OtherTuple&& t) {
//					swallow(TupleLeaf<Indexes_, Ts_>::operator=(forward<
//						typename TupleElement<Indexes_,
//						typename MakeTupleTypes<OtherTuple>::Type>::Type
//					>(get<Indexes_>(t)))...);
//					return *this;
//				}
//
//				TupleImpl& operator=(const TupleImpl& t) {
//					swallow(TupleLeaf<Indexes_, Ts_>::operator=(static_cast<
//						const TupleLeaf<Indexes_, Ts_>&>(t).get())...);
//					return *this;
//				}
//
//				void Swap(TupleImpl& t) {
//					swallow(TupleLeaf<Indexes_, Ts_>::Swap(
//						static_cast<TupleLeaf<Indexes_, Ts_>&>(t))...);
//				}
//			};
//
//			// TupleLike
//
//			template <typename T_>
//			struct TupleLike
//				: public StaticAsserts::LogicFalse {
//			};
//
//			template <typename T_>
//			struct TupleLike<const T_>
//				: public StaticAsserts::TupleLike<T_> {
//			};
//
//			template <typename T_>
//			struct TupleLike<volatile T_>
//				: public StaticAsserts::TupleLike<T_> {
//			};
//
//			template <typename T_>
//			struct TupleLike<const volatile T_>
//				: public StaticAsserts::TupleLike<T_> {
//			};
//
//			template <typename... Ts_>
//			struct TupleLike<Tuple<Ts_...>>
//				: public StaticAsserts::LogicTrue {
//			};
//
//			template <typename... Ts_>
//			struct TupleLike<TupleTypes<Ts_...>>
//				: public StaticAsserts::LogicTrue {
//			};
//
//			// TupleConvertible
//
//			template <bool IsSameSize_, typename From_, typename To_>
//			struct TupleConvertibleImpl
//				: public LogicFalse {
//			};
//
//			template <typename From0_, typename... FromRest_,
//				typename To0_, typename... ToRest_>
//			struct TupleConvertibleImpl<true, TupleTypes<From0_, FromRest_...>,
//				TupleTypes<To0_, ToRest_...>>
//				: public LogicConst<
//				IsConvertible<From0_, To0_>::State &&
//				TupleConvertibleImpl<true, TupleImpl::TupleTypes<FromRest_...>,
//				TupleTypes<ToRest_...>>::State> {
//			};
//
//			template <>
//			struct TupleConvertibleImpl<true, TupleTypes<>, TupleTypes<>>
//				: public LogicTrue {
//			};
//
//			template <typename From_, typename To_,
//				bool = TupleLike<StaticAsserts::RemoveRefV<From_>>::State,
//				bool = TupleLike<StaticAsserts::RemoveRefV<To_>>::State
//			>
//				struct TupleConvertible
//				: public LogicFalse {
//			};
//
//			template<typename From_, typename To_>
//			struct TupleConvertible<From_, To_, true, true>
//				: public TupleConvertibleImpl<
//				TupleSize<RemoveRefV<From_>>::State
//				== TupleSize<RemoveRefV<To_>>::State,
//				typename MakeTupleTypes<From_>::Type,
//				typename MakeTupleTypes<To_>::Type> {
//			};
//
//			// TupleAssignable
//
//			template <bool IsSameSize_, typename Target_, typename From_>
//			struct TupleAssignableImpl
//				: public LogicFalse {
//			};
//
//			template <typename Target0_, typename... TargetRest_,
//				typename From0_, typename... FromRest_>
//				struct TupleAssignableImpl<true, TupleTypes<Target0_, TargetRest_...>,
//				TupleTypes<From0_, FromRest_...>>
//				: public integral_constant<bool,
//				IsAssignableT<Target0_, From0_>::State&&
//				TupleAssignableImpl<true, TupleTypes<TargetRest_...>,
//				TupleTypes<FromRest_...>>::State> {
//			};
//
//			template <>
//			struct TupleAssignableImpl<true, TupleTypes<>, TupleTypes<>>
//				: public LogicTrue {
//			};
//
//			template <typename Target_, typename From_,
//				bool = TupleLike<StaticAsserts::RemoveRefV<Target_>>::State,
//				bool = TupleLike<StaticAsserts::RemoveRefV<From_>>::State>
//				struct TupleAssignable
//				: public LogicFalse {
//			};
//
//			template<typename Target_, typename From_>
//			struct TupleAssignable<Target_, From_, true, true>
//				: public TupleAssignableImpl<
//				TupleLike<RemoveRefV<Target_>>::State
//				== TupleLike<RemoveRefV<From_>>::State,
//				typename TupleImpl::MakeTupleTypes<Target_>::Type,
//				typename TupleImpl::MakeTupleTypes<From_>::Type> {
//			};
//
//			// TupleEqueal
//
//			template <size_t I_>
//			struct TupleEqual {
//				template <typename Tuple1_, typename Tuple2_>
//				bool operator()(const Tuple1_& t1, const Tuple2_& t2) {
//					return TupleEqual<I_ - 1>()(t1, t2) &&
//						get<I_ - 1>(t1) == get<I_ - 1>(t2);
//				}
//			};
//
//			template<>
//			struct TupleEqual<0> {
//				template <typename Tuple1_, typename Tuple2_>
//				bool operator()(const Tuple1_&, const Tuple2_&) {
//					return true;
//				}
//			};
//
//			// TupleLess
//
//			template <size_t I_>
//			struct TupleLess {
//				template <typename Tuple1_, typename Tuple2_>
//				bool operator()(const Tuple1_& t1, const Tuple2_& t2) {
//					return TupleLess<I_ - 1>()(t1, t2) ||
//						(!TupleLess<I_ - 1>()(t2, t1) && get<I_ - 1>(t1) < get<I_ - 1>(t2));
//				}
//			};
//
//			template<>
//			struct TupleLess<0> {
//				template <typename Tuple1_, typename Tuple2_>
//				bool operator()(const Tuple1_&, const Tuple2_&) {
//					return false;
//				}
//			};
//		}
//
//		// tuple_size
//
//		template <typename Tuple_>
//		class tuple_size : public TupleImpl::TupleSize<Tuple_> {
//		};
//
//		// tuple_element
//
//		template <size_t I_, typename Tuple_>
//		class tuple_element : public TupleImpl::TupleElement<I_, Tuple_> {
//		};
//
//		// Tuple
//
//		template <typename... Ts_>
//		class Tuple {
//			typedef TupleImpl::TupleImpl<typename TupleImpl::MakeTupleIndexes<sizeof...(Ts_)>::Type, Ts_...> Impl;
//			Impl impl_;
//
//			template <SizeT I, typename... Ts>
//			friend
//				typename tuple_element<I, Tuple<Ts...>>::Type&
//				get(Tuple<Ts...>& t);
//
//			template <size_t I, typename... Ts>
//			friend
//				const typename tuple_element<I, Tuple<Ts...>>::Type&
//				get(const Tuple<Ts...>& t);
//
//			template <size_t I, typename... Ts>
//			friend
//				typename tuple_element<I, Tuple<Ts...>>::Type&&
//				get(Tuple<Ts...>&& t);
//
//		public:
//			explicit Tuple(const Ts_& ... t)
//				: impl_(typename TupleImpl::MakeTupleIndexes<sizeof...(Ts_)>::type(),
//					typename TupleImpl::MakeTupleTypes<Tuple, sizeof...(Ts_)>::type(),
//					typename TupleImpl::MakeTupleIndexes<0>::Type(),
//					typename TupleImpl::MakeTupleTypes<Tuple, 0>::Type(),
//					t...) {
//			}
//
//			template <typename... Us, typename StaticAsserts::EnableIfV<
//				sizeof...(Us) <= sizeof...(Ts_) &&
//				TupleImpl::TupleConvertible<
//				typename TupleImpl::MakeTupleTypes<Tuple<Us...>>::Type,
//				typename TupleImpl::MakeTupleTypes<Tuple,
//				sizeof...(Us) < sizeof...(Ts_)
//				? sizeof...(Us)
//				: sizeof...(Ts_) > ::Type>::State,
//				bool>::Type = false>
//				explicit Tuple(Us && ... u)
//				: impl_(typename detail::MakeTupleIndexes<sizeof...(Us)>::Type(),
//					typename TupleImpl::MakeTupleTypes<Tuple, sizeof...(Us)>::Type(),
//					typename TupleImpl::MakeTupleIndexes<sizeof...(Ts_), sizeof...(Us)>::Type(),
//					typename TupleImpl::MakeTupleTypes<Tuple, sizeof...(Ts_), sizeof...(Us)>::Type(),
//					ForwardReference(u)...) {
//			}
//
//
//			template <typename OtherTuple, typename StaticAsserts::EnableIfT<
//				TupleImpl::TupleConvertible<OtherTuple, Tuple>::State,
//				bool>::Type = false>
//				Tuple(OtherTuple && t)
//				: impl_(ForwardReference<OtherTuple>(t)) {
//			}
//
//			template <typename OtherTuple, typename StaticAsserts::EnableIfT<
//				TupleImpl::TupleAssignable<Tuple, OtherTuple>::State,
//				bool>::Type = false>
//				Tuple & operator=(OtherTuple && t) {
//				impl_.operator=(ForwardReference<OtherTuple>(t));
//				return *this;
//			}
//
//			void Swap(Tuple& t) {
//				impl_.Swap(t.impl_);
//			}
//		};
//
//		// Empty Tuple
//
//		template <>
//		class Tuple<> {
//		public:
//			Tuple() {
//			}
//
//			template <typename OtherTuple, typename StaticAsserts::EnableIfV<
//				TupleImpl::TupleConvertible<OtherTuple, Tuple<>>::State,
//				bool>::Type = false>
//				Tuple(OtherTuple&&) {
//			}
//
//			template <typename OtherTuple, typename StaticAsserts::EnableIfV<
//				TupleImpl::TupleAssignable<Tuple<>, OtherTuple>::State,
//				bool>::Type = false>
//				Tuple & operator=(OtherTuple&&) {
//				return *this;
//			}
//
//			void Swap(Tuple<>&) {
//			}
//		};
//
//		// get
//
//		template <SizeT I, typename... Ts>
//		inline
//			typename tuple_element<I, Tuple<Ts...>>::Type&
//			get(Tuple<Ts...>& t) {
//			typedef typename tuple_element<I, Tuple<Ts...>>::Type Type;
//			return static_cast<TupleImpl::TupleLeaf<I, Type>&>(t.impl_).get();
//		}
//
//		template <SizeT I, typename... Ts>
//		inline
//			const typename tuple_element<I, Tuple<Ts...>>::Type&
//			get(const Tuple<Ts...>& t) {
//			typedef typename tuple_element<I, Tuple<Ts...>>::Type Type;
//			return static_cast<const TupleImpl::TupleLeaf<I, Type>&>(t.impl_).get();
//		}
//
//		template <size_t I, typename... Ts>
//		inline
//			typename tuple_element<I, Tuple<Ts...>>::Type&&
//			get(Tuple<Ts...>&& t) {
//			typedef typename tuple_element<I, Tuple<Ts...>>::Type Type;
//			return static_cast<Type&&>(static_cast<TupleImpl::TupleLeaf<I, Type>&&>(t.impl_).get());
//		}
//
//		// swap
//
//		template <typename... Ts>
//		inline
//			void Swap(Tuple<Ts...>& a, Tuple<Ts...>& b) {
//			a.Swap(b);
//		}
//
//		// relational operators
//
//		template <typename... T1s_, typename... T2s_>
//		inline bool operator==(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2) {
//			return TupleImpl::TupleEqual<sizeof...(T1s_)>()(t1, t2);
//		}
//
//		template <typename... T1s_, typename... T2s_>
//		inline bool operator!=(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2) {
//			return !(t1 == t2);
//		}
//
//		template <typename... T1s_, typename... T2s_>
//		inline bool operator<(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2) {
//			return TupleImpl::TupleLess<sizeof...(T1s_)>()(t1, t2);
//		}
//
//		template <typename... T1s_, typename... T2s_>
//		inline bool operator>(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2) {
//			return t2 < t1;
//		}
//
//		template <typename... T1s_, typename... T2s_>
//		inline bool operator<=(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2) {
//			return !(t2 < t1);
//		}
//
//		template <typename... T1s_, typename... T2s_>
//		inline bool operator>=(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2) {
//			return !(t1 < t2);
//		}
//	}
//}
#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers
		{
			template <typename... Ts_>
			class Tuple;

			namespace TupleImpl
			{
				using namespace TypeTraits;

#pragma region TupleLeaf
				// TupleLeaf

				template <size_t I_, typename ValueType_,
					bool IsEmpty_ = IsEmptyV<ValueType_>
					&& !IsFinalV<ValueType_>
				>
					class TupleLeaf;

				template <SizeT I, typename ValueType, bool IsEmpty>
				constexpr inline void Swap(TupleLeaf<I, ValueType, IsEmpty>& a,
										   TupleLeaf<I, ValueType, IsEmpty>& b)
				{
					Swap(a.get(), b.get());
				}

				template <SizeT I_, typename ValueType_, bool IsEmpty_>
				class TupleLeaf
				{
					ValueType_ value_;

					TupleLeaf& operator=(const TupleLeaf&) = delete;
				public:
					constexpr TupleLeaf()
						: value_()
					{
						static_assert(!IsReferenceV<ValueType_>,
									  "Attempt to default construct a reference element in a tuple");
					}

					constexpr TupleLeaf(const TupleLeaf& t)
						: value_(t.get())
					{
						static_assert(!IsRValueReferenceV<ValueType_>,
									  "Can not copy a tuple with rvalue reference member");
					}

					template <typename T, typename = typename EnableIfT<
						IsConstructibleT<ValueType_, T>>>
						constexpr explicit TupleLeaf(T&& t)
						: value_(ForwardReference<T>(t))
					{
						static_assert(!IsReferenceV<ValueType_> ||
									  IsLValueReferenceV<ValueType_> ||
									  (IsRValueReferenceV<ValueType_> &&
									   !IsLValueReferenceV<T>),
									  "Attempted to construct a reference element in a tuple with an rvalue");
					}

					template <typename T>
					constexpr explicit TupleLeaf(const TupleLeaf<I_, T>& t)
						: value_(t.get())
					{}

					template <typename T>
					constexpr TupleLeaf& operator=(T&& t)
					{
						value_ = ForwardReference<T>(t);
						return *this;
					}

					constexpr int Swap(TupleLeaf& t)
					{
						Swap(*this, t);
						return 0;
					}

					ValueType_& get()
					{
						return value_;
					}

					constexpr const ValueType_& get() const
					{
						return value_;
					}
				};
#pragma endregion

#pragma region TupleLeaf with EmptyBaseOptimization
				// TupleLeaf specialization in case we can use Empty Base Class Optimization:

				template <SizeT I_, typename ValueType_>
				class TupleLeaf<I_, ValueType_, true>
					: private ValueType_
				{
					TupleLeaf& operator=(const TupleLeaf&) = delete;
				public:
					constexpr TupleLeaf()
					{}

					constexpr TupleLeaf(const TupleLeaf& t)
						: ValueType_(t.get())
					{
						static_assert(!IsRValueReferenceV<ValueType_>,
									  "Can not copy a tuple with rvalue reference member");
					}

					template <typename T, typename = typename EnableIfT<
						IsConstructibleT<ValueType_, T>>>
						constexpr explicit TupleLeaf(T&& t)
						: ValueType_(ForwardReference<T>(t))
					{}

					template <typename T>
					constexpr explicit TupleLeaf(const TupleLeaf<I_, T>& t)
						: ValueType_(t.get())
					{}

					template <typename T>
					constexpr TupleLeaf& operator=(T&& t)
					{
						ValueType_::operator=(ForwardReference<T>(t));
						return *this;
					}

					constexpr int Swap(TupleLeaf& t)
					{
						Swap(*this, t);
						return 0;
					}

					ValueType_& get()
					{
						return static_cast<ValueType_&>(*this);
					}

					constexpr const ValueType_& get() const
					{
						return static_cast<const ValueType_&>(*this);
					}
				};
#pragma endregion
#pragma region TupleIndexes
				// TupleIndexes

				template <SizeT... Is_>
				struct TupleIndexes
				{};

				template <SizeT Start_, typename TupleIndexes_, SizeT End_>
				struct MakeTupleIndexesImpl;

				template <SizeT Start_, SizeT... Indexes_, SizeT End_>
				struct MakeTupleIndexesImpl<Start_, TupleIndexes<Indexes_...>, End_>
				{
					typedef typename MakeTupleIndexesImpl<Start_ + 1,
						TupleIndexes<Indexes_..., Start_>, End_>::Type Type;
				};

				template <SizeT End_, SizeT... Indexes_>
				struct MakeTupleIndexesImpl<End_, TupleIndexes<Indexes_...>, End_>
				{
					typedef TupleIndexes<Indexes_...> Type;
				};

				template <SizeT End_, SizeT Start_ = 0>
				struct MakeTupleIndexes
				{
					static_assert(Start_ <= End_, "MakeTupleIndexes: invalid parameters");
					typedef typename MakeTupleIndexesImpl<Start_, TupleIndexes<>, End_>::Type Type;
				};
#pragma endregion
#pragma region Type Holder
				// TupleTypes

				template <typename... Ts_>
				struct TupleTypes
				{};

				// TupleSize

				template <typename T_>
				struct TupleSize;

				template <typename T_>
				struct TupleSize<const T_>
					: public TupleSize<T_>
				{};

				template <typename T_>
				struct TupleSize<volatile T_>
					: public TupleSize<T_>
				{};

				template <typename T_>
				struct TupleSize<const volatile T_>
					: public TupleSize<T_>
				{};

				template <typename... Ts_>
				struct TupleSize<TupleTypes<Ts_...>>
				{
					static constexpr size_t Value = sizeof...(Ts_);
					typedef SizeT ValueType;
					typedef TupleSize Type; // using injected-class-name
					constexpr operator ValueType() const noexcept
					{
						return Value;
					}
					constexpr ValueType operator()() const noexcept
					{
						return Value;
					} //since c++14
				};

				template <typename TupleIndexes_, typename... Ts_>
				struct TupleImpl;

				// specialization for Tuple

				template <typename... Ts_>
				struct TupleSize<Tuple<Ts_...>>
				{
					static constexpr size_t Value = sizeof...(Ts_);
					typedef SizeT ValueType;
					typedef TupleSize Type; // using injected-class-name
					constexpr operator ValueType() const noexcept
					{
						return Value;
					}
					constexpr ValueType operator()() const noexcept
					{
						return Value;
					} //since c++14
				};
#pragma endregion
#pragma region Tuple Element
				// TupleElement

				template <SizeT I_, typename T_>
				class TupleElement /*: public tuple_element<I_, T_> {
				}*/;

				template <SizeT I_>
				class TupleElement<I_, TupleTypes<>>
				{
				public:
					static_assert(I_ != I_, "tuple_element index out of range"); //-V501
				};

				template <typename H_, typename... Ts_>
				class TupleElement<0, TupleTypes<H_, Ts_...>>
				{
				public:
					typedef H_ Type;
				};

				template <SizeT I_, typename H_, typename... Ts_>
				class TupleElement<I_, TupleTypes<H_, Ts_...>>
				{
				public:
					typedef typename TupleElement<I_ - 1, TupleTypes<Ts_...>>::Type Type;
				};

				// specialization for Tuple

				template <size_t I_, typename... Ts_>
				class TupleElement<I_, Tuple<Ts_...>>
				{
				public:
					typedef typename TupleElement<I_, TupleTypes<Ts_...>>::Type Type;
				};

				template <size_t I_, typename... Ts_>
				class TupleElement<I_, const Tuple<Ts_...>>
				{
				public:
					typedef typename AddConstT<typename TupleElement<I_, TupleTypes<Ts_...>>::type>::Type Type;
				};

				template <size_t I_, typename... Ts_>
				class TupleElement<I_, volatile Tuple<Ts_...>>
				{
				public:
					typedef typename AddVolatileT<typename TupleElement<I_, TupleTypes<Ts_...>>::type>::Type Type;
				};

				template <size_t I_, typename... Ts_>
				class TupleElement<I_, const volatile Tuple<Ts_...>>
				{
				public:
					typedef typename AddCvT<typename TupleElement<I_, TupleTypes<Ts_...>>::type>::Type Type;
				};
#pragma endregion
#pragma region Make Tuple helper
				// MakeTupleTypes

				template <typename TupleTypes_, typename Tuple_, size_t Start_, size_t End_>
				struct MakeTupleTypesImpl;

				template <typename... Types_, typename Tuple_, size_t Start_, size_t End_>
				struct MakeTupleTypesImpl<TupleTypes<Types_...>, Tuple_, Start_, End_>
				{
					typedef typename RemoveRefT<Tuple_>::Type TupleType;
					typedef typename MakeTupleTypesImpl<TupleTypes<Types_...,
						typename ConditionT<IsLValueReferenceT<Tuple_>::State,
						// append ref if Tuple_ is ref
						typename TupleElement<Start_, TupleType>::Type&,
						// append non-ref otherwise
						typename TupleElement<Start_, TupleType>::Type>::Result>,
						Tuple_, Start_ + 1, End_>::Type  Type;
				};

				template <typename... Types_, typename Tuple_, SizeT End_>
				struct MakeTupleTypesImpl<TupleTypes<Types_...>, Tuple_, End_, End_>
				{
					typedef TupleTypes<Types_...> Type;
				};

				template <typename Tuple_,
					size_t End_ = TupleSize<typename RemoveRefT<Tuple_>::Type>::Value,
					size_t Start_ = 0>
					struct MakeTupleTypes
				{
					static_assert(Start_ <= End_, "MakeTupleTypes: invalid parameters");
					typedef typename MakeTupleTypesImpl<TupleTypes<>, Tuple_, Start_, End_>::Type Type;
				};
#pragma endregion
#pragma region TupleImpl and swallow
				// TupleImpl

				template <typename... Ts>
				void swallow(Ts&& ...)
				{}

				template <SizeT... Indexes_, typename... Ts_>
				struct TupleImpl<TupleIndexes<Indexes_...>, Ts_...>
					: public TupleLeaf<Indexes_, Ts_>... {
					template <SizeT... FirstIndexes, typename... FirstTypes,
						SizeT... LastIndexes, typename... LastTypes,
						typename... ValueTypes>
						constexpr explicit TupleImpl(
							TupleIndexes<FirstIndexes...>, TupleTypes<FirstTypes...>,
							TupleIndexes<LastIndexes...>, TupleTypes<LastTypes...>,
							ValueTypes&& ... values)
						: TupleLeaf<FirstIndexes,
						FirstTypes>(ForwardReference<ValueTypes>(values))...,
						TupleLeaf<LastIndexes, LastTypes>()... {
					}

					template <typename OtherTuple>
					constexpr TupleImpl(OtherTuple&& t)
						: TupleLeaf<Indexes_, Ts_>(ForwardReference<
												   typename TupleElement<Indexes_, typename MakeTupleTypes<OtherTuple>::Type>::Type
						>(t.Get<Indexes_>()))... {
					}

					template <typename OtherTuple>
					constexpr TupleImpl& operator=(OtherTuple&& t)
					{
						//swallow(TupleLeaf<Indexes_, Ts_>::operator=(ForwardReference<decltype(t.Get<Indexes_>())>(t.Get<Indexes_>()))...);
						this->~TupleImpl();
						this->TupleImpl::TupleImpl(MoveRValue(t));
						return *this;
					}

					constexpr TupleImpl& operator=(const TupleImpl& t)
					{
						swallow(TupleLeaf<Indexes_, Ts_>::operator=(static_cast<
																	const TupleLeaf<Indexes_, Ts_>&>(t).get())...);
						return *this;
					}

					constexpr void Swap(TupleImpl& t)
					{
						swallow(TupleLeaf<Indexes_, Ts_>::Swap(
							static_cast<TupleLeaf<Indexes_, Ts_>&>(t))...);
					}
				};
#pragma endregion
#pragma region SFINAE helpers
#pragma region Tuple Like
				// TupleLike

				template <typename T_>
				struct TupleLike
					: public LogicFalse
				{};

				template <typename T_>
				struct TupleLike<const T_>
					: public TupleLike<T_>
				{};

				template <typename T_>
				struct TupleLike<volatile T_>
					: public TupleLike<T_>
				{};

				template <typename T_>
				struct TupleLike<const volatile T_>
					: public TupleLike<T_>
				{};

				template <typename... Ts_>
				struct TupleLike<Tuple<Ts_...>>
					: public LogicTrue
				{};

				template <typename... Ts_>
				struct TupleLike<TupleTypes<Ts_...>>
					: public LogicTrue
				{};
#pragma endregion
#pragma region Tuple Convertible
				// TupleConvertible

				template <bool IsSameSize_, typename From_, typename To_>
				struct TupleConvertibleImpl
					: public LogicFalse
				{};

				template <typename From0_, typename... FromRest_,
					typename To0_, typename... ToRest_>
					struct TupleConvertibleImpl<true, TupleTypes<From0_, FromRest_...>,
					TupleTypes<To0_, ToRest_...>>
					: public LogicConst<
					IsConvertibleT<From0_, To0_>::State &&
					TupleConvertibleImpl<true, TupleTypes<FromRest_...>,
					TupleTypes<ToRest_...>>::State> {
				};

				template <>
				struct TupleConvertibleImpl<true, TupleTypes<>, TupleTypes<>>
					: public LogicTrue
				{};

				template <typename From_, typename To_,
					bool = TupleLike<typename RemoveRefT<From_>::Type>::State,
					bool = TupleLike<typename RemoveRefT<To_>::Type>::State>
					struct TupleConvertible
					: public LogicFalse
				{};

				template<typename From_, typename To_>
				struct TupleConvertible<From_, To_, true, true>
					: public TupleConvertibleImpl<
					TupleSize<typename RemoveRefT<From_>::Type>::Value
					== TupleSize<typename RemoveRefT<To_>::Type>::Value,
					typename MakeTupleTypes<From_>::Type,
					typename MakeTupleTypes<To_>::Type>
				{};
#pragma endregion
#pragma region Tuple Assignable
				// TupleAssignable

				template <bool IsSameSize_, typename Target_, typename From_>
				struct TupleAssignableImpl
					: public LogicFalse
				{};

				template <typename Target0_, typename... TargetRest_,
					typename From0_, typename... FromRest_>
					struct TupleAssignableImpl<true, TupleTypes<Target0_, TargetRest_...>,
					TupleTypes<From0_, FromRest_...>>
					: public LogicConst<
					IsAssignableT<Target0_, From0_>::State &&
					TupleAssignableImpl<true, TupleTypes<TargetRest_...>,
					TupleTypes<FromRest_...>>::State> {
				};

				template <>
				struct TupleAssignableImpl<true, TupleTypes<>, TupleTypes<>>
					: public LogicTrue
				{};

				template <typename Target_, typename From_,
					bool = TupleLike<typename RemoveRefT<Target_>::Type>::State,
					bool = TupleLike<typename RemoveRefT<From_>::Type>::State>
					struct TupleAssignable
					: public LogicFalse
				{};

				template<typename Target_, typename From_>
				struct TupleAssignable<Target_, From_, true, true>
					: public TupleAssignableImpl<
					TupleSize<typename RemoveRefT<Target_>::Type>::Value
					== TupleSize<typename RemoveRefT<From_>::Type>::Value,
					typename MakeTupleTypes<Target_>::Type,
					typename MakeTupleTypes<From_>::Type>
				{};
#pragma endregion
#pragma region Tuple Order helpers
				// TupleEqueal

				template <SizeT I_>
				struct TupleEqual
				{
					template <typename Tuple1_, typename Tuple2_>
					bool operator()(const Tuple1_& t1, const Tuple2_& t2)
					{
						return TupleEqual<I_ - 1>()(t1, t2) &&
							get<I_ - 1>(t1) == get<I_ - 1>(t2);
					}
				};

				template<>
				struct TupleEqual<0>
				{
					template <typename Tuple1_, typename Tuple2_>
					bool operator()(const Tuple1_&, const Tuple2_&)
					{
						return true;
					}
				};

				// TupleLess

				template <SizeT I_>
				struct TupleLess
				{
					template <typename Tuple1_, typename Tuple2_>
					bool operator()(const Tuple1_& t1, const Tuple2_& t2)
					{
						return TupleLess<I_ - 1>()(t1, t2) ||
							(!TupleLess<I_ - 1>()(t2, t1) && get<I_ - 1>(t1) < get<I_ - 1>(t2));
					}
				};

				template<>
				struct TupleLess<0>
				{
					template <typename Tuple1_, typename Tuple2_>
					bool operator()(const Tuple1_&, const Tuple2_&)
					{
						return false;
					}
				};
#pragma endregion
#pragma endregion
			}
#pragma region Tuple
			// tuple_size

			template <typename Tuple_>
			class tuple_size : public TupleImpl::TupleSize<Tuple_>
			{};

			// tuple_element

			template <size_t I_, typename Tuple_>
			class tuple_element : public TupleImpl::TupleElement<I_, Tuple_>
			{};

			// Tuple

			template <typename... Ts_>
			class Tuple
			{
				typedef TupleImpl::TupleImpl<typename TupleImpl::MakeTupleIndexes<sizeof...(Ts_)>::Type, Ts_...> Impl;
				Impl impl_;

				template <SizeT I, typename... Ts>
				friend
					typename tuple_element<I, Tuple<Ts...>>::Type&
					get(Tuple<Ts...>& t);

				template <size_t I, typename... Ts>
				friend
					constexpr const typename tuple_element<I, Tuple<Ts...>>::Type&
					get(const Tuple<Ts...>& t);

				template <size_t I, typename... Ts>
				friend
					typename tuple_element<I, Tuple<Ts...>>::Type&&
					get(Tuple<Ts...>&& t);

			public:
				constexpr Tuple(const Ts_& ... t)
					: impl_(typename TupleImpl::MakeTupleIndexes<sizeof...(Ts_)>::Type(),
							typename TupleImpl::MakeTupleTypes<Tuple, sizeof...(Ts_)>::Type(),
							typename TupleImpl::MakeTupleIndexes<0>::Type(),
							typename TupleImpl::MakeTupleTypes<Tuple, 0>::Type(),
							t...)
				{}

				template <typename... Us, typename TypeTraits::EnableIf<
					sizeof...(Us) <= sizeof...(Ts_) &&
					TupleImpl::TupleConvertible<
					typename TupleImpl::MakeTupleTypes<Tuple<Us...>>::Type,
					typename TupleImpl::MakeTupleTypes<Tuple,
					sizeof...(Us) < sizeof...(Ts_)
					? sizeof...(Us)
					: sizeof...(Ts_) > ::Type>::State,
					bool>::Type = false>
					constexpr explicit Tuple(Us && ... u)
					: impl_(typename TupleImpl::MakeTupleIndexes<sizeof...(Us)>::Type(),
							typename TupleImpl::MakeTupleTypes<Tuple, sizeof...(Us)>::Type(),
							typename TupleImpl::MakeTupleIndexes<sizeof...(Ts_), sizeof...(Us)>::Type(),
							typename TupleImpl::MakeTupleTypes<Tuple, sizeof...(Ts_), sizeof...(Us)>::Type(),
							ForwardReference<Us>(u)...)
				{}

				template <typename OtherTuple, typename TypeTraits::EnableIf<
					TupleImpl::TupleConvertible<OtherTuple, Tuple>::State,
					bool>::Type = false>
					constexpr Tuple(OtherTuple&& t)
					: impl_(ForwardReference<OtherTuple>(t))
				{}

				template <typename OtherTuple, typename TypeTraits::EnableIf<
					TupleImpl::TupleAssignable<Tuple, OtherTuple>::State,
					bool>::Type = false>
					constexpr Tuple& operator=(OtherTuple&& t)
				{
					impl_.operator=(ForwardReference<OtherTuple>(t));
					return *this;
				}

				constexpr void Swap(Tuple& t)
				{
					impl_.Swap(t.impl_);
				}

				//template<SizeT Index>
				//constexpr auto& Get() {
				//	return get<Index>(*this);
				//}

				template<SizeT Index>
				constexpr auto& Get()
				{
					return get<Index>(*this);
				}

				template<SizeT Index>
				using Item = typename TupleImpl::TupleElement<Index, Tuple>::Type;

				constexpr static SizeT Count()
				{
					return sizeof...(Ts_);
				}
			};

			// Empty Tuple

			template <>
			class Tuple<>
			{
			public:
				constexpr Tuple()
				{}

				template <typename OtherTuple, typename TypeTraits::EnableIf<
					TupleImpl::TupleConvertible<OtherTuple, Tuple>::State,
					bool>::Type = false>
					constexpr Tuple(OtherTuple&&)
				{}

				template <typename OtherTuple, typename TypeTraits::EnableIf<
					TupleImpl::TupleAssignable<Tuple, OtherTuple>::State,
					bool>::Type = false>
					constexpr Tuple& operator=(OtherTuple&&)
				{
					return *this;
				}

				constexpr void Swap(Tuple&)
				{}

				constexpr static SizeT Count()
				{
					return 0;
				}
			};

			// get

			template <SizeT I, typename... Ts>
			inline
				typename tuple_element<I, Tuple<Ts...>>::Type&
				get(Tuple<Ts...>& t)
			{
				typedef typename tuple_element<I, Tuple<Ts...>>::Type Type;
				return static_cast<TupleImpl::TupleLeaf<I, Type>&>(t.impl_).get();
			}

			template <size_t I, typename... Ts>
			inline
				constexpr const typename tuple_element<I, Tuple<Ts...>>::Type&
				get(const Tuple<Ts...>& t)
			{
				typedef typename tuple_element<I, Tuple<Ts...>>::Type Type;
				return static_cast<const TupleImpl::TupleLeaf<I, Type>&>(t.impl_).get();
			}

			template <SizeT I, typename... Ts>
			inline
				typename tuple_element<I, Tuple<Ts...>>::Type&&
				get(Tuple<Ts...>&& t)
			{
				typedef typename tuple_element<I, Tuple<Ts...>>::Type Type;
				return static_cast<Type&&>(static_cast<TupleImpl::TupleLeaf<I, Type>&&>(t.impl_).get());
			}

			// swap

			template <typename... Ts>
			constexpr inline
				void swap(Tuple<Ts...>& a, Tuple<Ts...>& b)
			{
				a.swap(b);
			}

			// relational operators

			template <typename... T1s_, typename... T2s_>
			inline bool operator==(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2)
			{
				return TupleImpl::TupleEqual<sizeof...(T1s_)>()(t1, t2);
			}

			template <typename... T1s_, typename... T2s_>
			inline bool operator!=(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2)
			{
				return !(t1 == t2);
			}

			template <typename... T1s_, typename... T2s_>
			inline bool operator<(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2)
			{
				return TupleImpl::TupleLess<sizeof...(T1s_)>()(t1, t2);
			}

			template <typename... T1s_, typename... T2s_>
			inline bool operator>(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2)
			{
				return t2 < t1;
			}

			template <typename... T1s_, typename... T2s_>
			inline bool operator<=(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2)
			{
				return !(t2 < t1);
			}

			template <typename... T1s_, typename... T2s_>
			inline bool operator>=(const Tuple<T1s_...>& t1, const Tuple<T2s_...>& t2)
			{
				return !(t1 < t2);
			}
		}
#pragma endregion
	}
}

namespace std {
	using Seed::SizeT;
	// tuple_size

	template <typename... Ts_>
	class tuple_size<Seed::Core::Containers::Tuple<Ts_...>>
		: public Seed::Core::Containers::tuple_size<Seed::Core::Containers::Tuple<Ts_...>> {
	public:
		static constexpr SizeT value = Value;
		typedef SizeT value_type;
		typedef tuple_size type; // using injected-class-name
		constexpr operator value_type() const noexcept { return value; }
		constexpr value_type operator()() const noexcept { return value; } //since c++14
	};

	template <typename... Ts_>
	class tuple_size<const Seed::Core::Containers::Tuple<Ts_...>>
		: public Seed::Core::Containers::tuple_size<const Seed::Core::Containers::Tuple<Ts_...>> {
	public:
		static constexpr SizeT value = Value;
		typedef SizeT value_type;
		typedef tuple_size type; // using injected-class-name
		constexpr operator value_type() const noexcept { return value; }
		constexpr value_type operator()() const noexcept { return value; } //since c++14
	};

	template <typename... Ts_>
	class tuple_size<volatile Seed::Core::Containers::Tuple<Ts_...>>
		: public Seed::Core::Containers::tuple_size<volatile Seed::Core::Containers::Tuple<Ts_...>> {
	public:
		static constexpr SizeT value = Value;
		typedef SizeT value_type;
		typedef tuple_size type; // using injected-class-name
		constexpr operator value_type() const noexcept { return value; }
		constexpr value_type operator()() const noexcept { return value; } //since c++14
	};

	template <typename... Ts_>
	class tuple_size<const volatile Seed::Core::Containers::Tuple<Ts_...>>
		: public Seed::Core::Containers::tuple_size<const volatile Seed::Core::Containers::Tuple<Ts_...>> {
	public:
		static constexpr SizeT value = Value;
		typedef SizeT value_type;
		typedef tuple_size type; // using injected-class-name
		constexpr operator value_type() const noexcept { return value; }
		constexpr value_type operator()() const noexcept { return value; } //since c++14
	};

	// tuple_element

	template <SizeT I_, typename... Ts_>
	class tuple_element;

	template <SizeT I_, typename... Ts_>
	class tuple_element<I_, Seed::Core::Containers::Tuple<Ts_...>>
		: public Seed::Core::Containers::tuple_element<I_, Seed::Core::Containers::Tuple<Ts_...>> {
		using type = typename Seed::Core::Containers::tuple_element<I_, Seed::Core::Containers::Tuple<Ts_...>>::Type;
	};

	template <SizeT I_, typename... Ts_>
	class tuple_element<I_, const Seed::Core::Containers::Tuple<Ts_...>>
		: public Seed::Core::Containers::tuple_element<I_, const Seed::Core::Containers::Tuple<Ts_...>> {
		using type = typename Seed::Core::Containers::tuple_element<I_, Seed::Core::Containers::Tuple<Ts_...>>::Type;
	};

	template <SizeT I_, typename... Ts_>
	class tuple_element<I_, volatile Seed::Core::Containers::Tuple<Ts_...>>
		: public Seed::Core::Containers::tuple_element<I_, volatile Seed::Core::Containers::Tuple<Ts_...>> {
		using type = typename Seed::Core::Containers::tuple_element<I_, Seed::Core::Containers::Tuple<Ts_...>>::Type;
	};

	template <SizeT I_, typename... Ts_>
	class tuple_element<I_, const volatile Seed::Core::Containers::Tuple<Ts_...>>
		: public Seed::Core::Containers::tuple_element<I_, const volatile Seed::Core::Containers::Tuple<Ts_...>> {
		using type = typename Seed::Core::Containers::tuple_element<I_, Seed::Core::Containers::Tuple<Ts_...>>::Type;
	};

	using Seed::Core::Containers::get;
} // namespace std
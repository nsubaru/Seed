#pragma once
#include "Seed.Core.Containers.Tuple.h"

namespace Seed::Core::Containers
{
	template<typename ... TTypes>
	template<typename T, SizeT...Idxs>
	ForceInline constexpr Tuple<TTypes..., T> 
	Tuple<TTypes...>::AddImpl(T&& newItem, UnusedData TypeTraits::TypeTag<TypeTraits::Indexator<Idxs...>>)&&
	{
		return { ((TTypes) this->MoveByIdx<(sizeof...(TTypes) - 1) - Idxs>())..., newItem };
	}

	template<typename ... TTypes>
	template<SizeT...Idxs>
	ForceInline Tuple<TTypes...>::DiscardedTuple<Idxs...> constexpr 
	Tuple<TTypes...>::DiscardImpl(UnusedData TypeTraits::TypeTag<TypeTraits::Indexator<Idxs...>>)&&
	{
		using TTuple = DiscardedTuple<Idxs...>;
		constexpr SizeT N = (sizeof...(TTypes)) - 2;
		return TTuple{ this->MoveByIdx<N - Idxs>()... };
	}

	template<typename ... TTypes>
	ForceInline constexpr Tuple<TTypes...>& Tuple<TTypes...>::operator=(const Tuple& tuple)
	{
		this->~Tuple();
		this->Tuple::Tuple(tuple);
		return *this;
	}

	template<typename ... TTypes>
	ForceInline constexpr Tuple<TTypes...>& Tuple<TTypes...>::operator=(Tuple&& tuple)
	{
		this->~Tuple();
		this->Tuple::Tuple(MoveRValue(tuple));
		return *this;
	}

	template<typename ... TTypes>
	template<SizeT Idx>
	requires (sizeof...(TTypes) > Idx)
	constexpr decltype(auto) Tuple<TTypes...>::GetByIdx()
	{
		return leaves.GetByIdx<Idx>();
	}

	template<typename ... TTypes>
	template<SizeT Idx>
	requires (sizeof...(TTypes) > Idx)
	ForceInline constexpr decltype(auto) Tuple<TTypes...>::MoveByIdx()
	{
		return leaves.MoveByIdx<Idx>();
	}

	template<typename ... TTypes>
	template<SizeT Idx>
	requires (sizeof...(TTypes) > Idx)
	ForceInline constexpr decltype(auto) Tuple<TTypes...>::GetByIdx() const
	{
		return leaves.GetByIdx<Idx>();
	}

	template<typename ... TTypes>
	template<SizeT Idx>
	requires (sizeof...(TTypes) > Idx)
	ForceInline constexpr decltype(auto) Tuple<TTypes...>::GetByIdxAsRef() const
	{
		return leaves.GetByIdxAsRef<Idx>();
	}

	template<typename ... TTypes>
	template<SizeT Idx>
	requires (sizeof...(TTypes) > Idx)
	ForceInline constexpr decltype(auto) Tuple<TTypes...>::GetByIdxAsRef()
	{
		return leaves.GetByIdxAsRef<Idx>();
	}

	template<typename ... TTypes>
	template<typename F>
	ForceInline constexpr decltype(auto) Tuple<TTypes...>::CallWithArgsFromTuple(F&& f) const
	{
		using TExecutor = typename Details::Executor<F, Tuple<TTypes...>, typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		if constexpr (TypeTraits::IsSameV<TypeTraits::InvocableResult<F, TTypes...>, void>)
		{
			TExecutor::Execute(ForwardReference<F>(f), *this);
		}
		else
		{
			return TExecutor::Execute(ForwardReference<F>(f), *this);
		}
	}

	template<typename ... TTypes>
	template<typename F>
	ForceInline constexpr decltype(auto) Tuple<TTypes...>::CallWithArgsFromTuple(F&& f)
	{
		using TExecutor = typename Details::Executor<
			F,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		if constexpr (TypeTraits::IsSameV<TypeTraits::InvocableResult<F, TTypes...>, void>)
		{
			TExecutor::Execute(ForwardReference<F>(f), *this);
		}
		else
		{
			return TExecutor::Execute(ForwardReference<F>(f), *this);
		}
	}

	template<typename ... TTypes>
	template<typename T, typename F>
	constexpr decltype(auto) Tuple<TTypes...>::CallMethodWithArgsFromTuple(T& obj, F&& f)
	{
		using TExecutor = typename Details::MethodExecutor<
			T,
			F,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		if constexpr (TypeTraits::IsSameV<decltype(TExecutor::Execute(obj, ForwardReference<F&&>(f), *this)), void>)
		{
			TExecutor::Execute(obj, ForwardReference<F&&>(f), *this);
		}
		else
		{
			return TExecutor::Execute(obj, ForwardReference<F&&>(f), *this);
		}
	}

	template<typename ... TTypes>
	template<typename TLambda>
	ForceInline constexpr decltype(auto) Tuple<TTypes...>::ConvertTo(TLambda&& lambda) const
	{
		using Convertor = typename Details::Convertor<
			TLambda,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		return Convertor::Convert(MoveRValue(lambda), *this);
	}

	template<typename ... TTypes>
	template<typename TLambda>
	ForceInline constexpr decltype(auto) Tuple<TTypes...>::ConvertTo(TLambda&& lambda)
	{
		using Convertor = typename Details::Convertor<
			TLambda,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		return Convertor::Convert(MoveRValue(lambda), *this);
	}

	template<typename ... TTypes>
	template<typename TLambda>
	ForceInline constexpr const Tuple<TTypes...>& Tuple<TTypes...>::ForEach(TLambda&& lambda) const&
	{
		using ForEach = typename Details::ForEachImpl<
			TLambda,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		ForEach::ForEach(MoveRValue(lambda), *this);

		return *this;
	}

	template<typename ... TTypes>
	template<typename TLambda>
	ForceInline constexpr Tuple<TTypes...>& Tuple<TTypes...>::ForEach(TLambda&& lambda)&
	{
		using ForEach = typename Details::ForEachImpl<
			TLambda,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		ForEach::ForEach(MoveRValue(lambda), *this);

		return *this;
	}

	template<typename ... TTypes>
	template<typename TLambda>
	ForceInline constexpr Tuple<TTypes...>&& Tuple<TTypes...>::ForEach(TLambda&& lambda)&&
	{
		using ForEach = typename Details::ForEachImpl<
			TLambda,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		ForEach::ForEach(MoveRValue(lambda), *this);

		return *this;
	}

	template<typename ... TTypes>
	template<typename TLambda>
	ForceInline constexpr decltype(auto) Tuple<TTypes...>::Sum(TLambda&& lambda) const
	{
		using Sum = typename Details::SumImpl<
			TLambda,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		return Sum::Sum(MoveRValue(lambda), *this);
	}

	template<typename ... TTypes>
	template<typename TLambda>
	ForceInline constexpr static decltype(auto) Tuple<TTypes...>::SumType(TLambda&& lambda)
	{
		using Sum = typename Details::SumImpl<
			TLambda,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		return Sum::Sum(MoveRValue(lambda));
	}

	template<typename ... TTypes>
	template<typename TLambda>
	ForceInline static constexpr void Tuple<TTypes...>::ForEachType(TLambda&& lambda)
	{
		using ForEachType = typename Details::ForEachTypeImpl<
			TLambda,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		ForEachType::ForEach(MoveRValue(lambda));
	}

	template<typename ... TTypes>
	template<typename TLambda>
	ForceInline constexpr static SizeT Tuple<TTypes...>::FirstType(TLambda lambda)
	{
		using FirstImpl = typename Details::FirstImpl<
			TLambda,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		return FirstImpl::First(MoveRValue(lambda));
	}

	template<typename ... TTypes>
	template<typename TLambda>
	ForceInline constexpr static SizeT Tuple<TTypes...>::FirstType()
	{
		using FirstImpl = typename Details::FirstImpl<
			TLambda,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		return FirstImpl::First();
	}

	template<typename ... TTypes>
	template<typename TLambda>
	ForceInline constexpr SizeT Tuple<TTypes...>::FirstItem(TLambda&& lambda) const
	{
		using FirstImpl = typename Details::FirstImpl<
			TLambda,
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		return FirstImpl::First(MoveRValue(lambda), *this);
	}

	template<typename ... TTypes>
	ForceInline constexpr auto Tuple<TTypes...>::Reverse() const
	{
		using ReverseImpl = typename Details::ReverseImpl<
			Tuple<TTypes...>,
			typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type>;

		return ReverseImpl::Reverse(*this);
	}

	template<typename ... TTypes>
	template<typename T>
	ForceInline constexpr Tuple<TTypes..., T> Tuple<TTypes...>::Add(const T& newItem) const&
	{
		using Indexator = typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type;
		auto tuple = *this;
		return MoveRValue(tuple).AddImpl<T>(MoveRValue(T(newItem)), TypeTraits::TypeTag<Indexator>{});
	}

	template<typename ... TTypes>
	template<typename T>
	ForceInline constexpr Tuple<TTypes..., T> Tuple<TTypes...>::Add(T&& newItem) const&
	{
		using Indexator = typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type;
		auto tuple = *this;
		return MoveRValue(tuple).AddImpl<T>(MoveRValue(newItem), TypeTraits::TypeTag<Indexator>{});
	}

	template<typename ... TTypes>
	template<typename T>
	ForceInline constexpr Tuple<TTypes..., T> Tuple<TTypes...>::Add(const T& newItem)&&
	{
		using Indexator = typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type;
		return MoveRValue(*this).AddImpl<T>(MoveRValue(T(newItem)), TypeTraits::TypeTag<Indexator>{});
	}

	template<typename ... TTypes>
	template<typename T>
	ForceInline constexpr Tuple<TTypes..., T> Tuple<TTypes...>::Add(T&& newItem)&&
	{
		using Indexator = typename TypeTraits::Indexer<sizeof...(TTypes)>::template Type;
		return MoveRValue(*this).AddImpl<T>(MoveRValue(newItem), TypeTraits::TypeTag<Indexator>{});
	}

	template<typename ... TTypes>
	template<typename T>
	constexpr auto Tuple<TTypes...>::operator+(T& item) const&
	{
		return Add<T>(T(item));
	}

	template<typename ... TTypes>
	template<typename T>
	constexpr auto Tuple<TTypes...>::operator+(const T& item) const&
	{
		return Add<T>(T(item));
	}

	template<typename ... TTypes>
	template<typename T>
	constexpr auto Tuple<TTypes...>::operator+(T&& item) const&
	{
		return Add<T>(MoveRValue(item));
	}

	template<typename ... TTypes>
	template<typename T>
	constexpr auto Tuple<TTypes...>::operator+(T& item)&&
	{
		return MoveRValue(*this).Add<T>(T(item));
	}

	template<typename ... TTypes>
	template<typename T>
	constexpr auto Tuple<TTypes...>::operator+(const T& item)&&
	{
		return MoveRValue(*this).Add<T>(item);
	}

	template<typename ... TTypes>
	template<typename T>
	constexpr auto Tuple<TTypes...>::operator+(T&& item)&&
	{
		return MoveRValue(*this).Add<T>(MoveRValue(item));
	}

	template<typename ... TTypes>
	constexpr auto Tuple<TTypes...>::DiscardLastItem()&
	{
		return Tuple(*this).DiscardLastItem();
	}

	template<typename ... TTypes>
	constexpr auto Tuple<TTypes...>::DiscardLastItem() const&
	{
		return Tuple(*this).DiscardLastItem();
	}

	template<typename ... TTypes>
	constexpr auto Tuple<TTypes...>::DiscardLastItem()&&
	{
		using Indexator = typename TypeTraits::Indexer<(sizeof...(TTypes)) - 1>::Type;
		return MoveRValue(*this).DiscardImpl(TypeTraits::TypeTag<Indexator>{});
	}

	template<typename ... TTypes>
	constexpr auto Tuple<TTypes...>::DiscardLastItem() const&&
	{
		return Tuple(*this).DiscardLastItem();
	}

	template<typename F>
	ForceInline constexpr decltype(auto) Tuple<>::CallWithArgsFromTuple(F&& f)
	{
		if constexpr (TypeTraits::IsSameV<TypeTraits::InvocableResult<F>, void>)
		{
			f();
		}
		else
		{
			return f();
		}
	}

	template<typename F>
	ForceInline constexpr decltype(auto) Tuple<>::CallWithArgsFromTuple(F&& f) const
	{
		if constexpr (TypeTraits::IsSameV<TypeTraits::InvocableResult<F>, void>)
		{
			f();
		}
		else
		{
			return f();
		}
	}

	template<typename TLambda>
	ForceInline static constexpr void Tuple<>::ForEachType(TLambda&& lambda)
	{
	}

	template<typename TLambda>
	ForceInline constexpr const Tuple<>& Tuple<>::ForEach(TLambda&& lambda) const&
	{
		return *this;
	}

	template<typename TLambda>
	ForceInline constexpr Tuple<>& Tuple<>::ForEach(TLambda&& lambda)&
	{
		return *this;
	}

	template<typename TLambda>
	ForceInline constexpr Tuple<>&& Tuple<>::ForEach(TLambda&& lambda)&&
	{
		return MoveRValue(*this);
	}
}
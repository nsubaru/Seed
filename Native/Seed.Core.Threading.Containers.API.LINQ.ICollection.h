#pragma once
#include "Seed.Core.Threading.Containers.API.LINQ.Base.h"


namespace Seed::Core::Containers::LINQ
{
	template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySupport, Boolean TIsReadonly, Boolean TIsCollectionCanReturnRef>
	template<
		template <typename> typename NewCollection,
		Concepts::AsyncCopyFunction<T> TLambda,
		typename U
	>
	requires (
		Concepts::IsBasedOn<ICollectionGenericBase, NewCollection<U>> &&
		!NewCollection<U>::IsReadonly
	)
	inline Threading::BaseTask<NewCollection<U>, void> ICollection<TCollection, THeap, T, TIsDynamic, TIsConcurencySupport, TIsReadonly, TIsCollectionCanReturnRef>::CopyToAsync(TLambda&& lambda)
	{
		decltype(auto) thisCollectionRef = static_cast<const ThisCollection&>(*this);

		if constexpr (NewCollection<U>::IsDynamic == false)
		{
			NewCollection<U> instance = NewCollection<U>(thisCollectionRef.Count);

			co_await thisCollectionRef.ForEachAsync([&](const T& item, SizeT index) -> Threading::Task<void> InlineLambda{
				if constexpr (Concepts::CopyFunctionWithIdx<TLambda, T>)
				{
					instance.UnsafeSet(co_await lambda(item, index), index);
				}
				else
				{
					instance.UnsafeSet(co_await lambda(item), index);
				}
			});

			co_return instance;
		}
		else
		{
			NewCollection<U> instance = NewCollection<U>();

			if constexpr (Concepts::CopyFunctionWithIdx<TLambda, T>)
			{
				co_await thisCollectionRef.ForEach([&](const T& item, SizeT index) -> Threading::Task<void> InlineLambda
				{
					instance.UnsafeAdd(co_await lambda(item, index));
				});
			}
			else
			{
				co_await thisCollectionRef.ForEach([&](const T& item) InlineLambda
				{
					instance.UnsafeAdd(co_await lambda(item));
				});
			}
			co_return instance;
		}
	}

	template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySupport, Boolean TIsReadonly, Boolean TIsCollectionCanReturnRef>
	template<
		template <typename> typename NewCollection,
		Concepts::AsyncCopyFunction<T> TLambda,
		typename U
	>
	requires (
		Concepts::IsBasedOn<ICollectionGenericBase, NewCollection<U>> &&
		!NewCollection<U>::IsReadonly
	)
	inline Threading::BaseTask<NewCollection<U>, void> ICollection<TCollection, THeap, T, TIsDynamic, TIsConcurencySupport, TIsReadonly, TIsCollectionCanReturnRef>::CopyToAsync(TLambda&& lambda) const
	{
		decltype(auto) thisCollectionRef = static_cast<const ThisCollection&>(*this);

		if constexpr (NewCollection<U>::IsDynamic == false)
		{
			NewCollection<U> instance = NewCollection<U>(thisCollectionRef.Count);

			co_await thisCollectionRef.ForEachAsync([&] (const T& item, SizeT index) -> Threading::Task<void> InlineLambda {
				if constexpr (Concepts::CopyFunctionWithoutIdx<TLambda, T>)
				{
					instance.UnsafeSet(co_await lambda(item, index), index);
				}
				else
				{
					instance.UnsafeSet(co_await lambda(item), index);
				}
			});

			co_return instance;
		}
		else
		{
			NewCollection<U> instance = NewCollection<U>();

			if constexpr (Concepts::CopyFunctionWithoutIdx<TLambda, T>)
			{
				co_await thisCollectionRef.ForEach([&] (const T& item, SizeT index) -> Threading::Task<void> InlineLambda {
					instance.UnsafeAdd(co_await lambda(item, index));
				});
			}
			else
			{
				co_await thisCollectionRef.ForEach([&](const T& item) -> Threading::Task<void> InlineLambda
				{
					instance.UnsafeAdd(co_await lambda(item));
				});
			}

			co_return instance;
		}
	}

	template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySupport, Boolean TIsReadonly, Boolean TIsCollectionCanReturnRef>
	template<template <typename> typename NewCollection,
		Concepts::AsyncMoveFunction<T> TLambda,
		typename U>
	requires (
		Concepts::IsBasedOn<ICollectionGenericBase, NewCollection<T>> &&
		!NewCollection<U>::IsReadonly &&
		!TIsReadonly)
	inline Threading::BaseTask<NewCollection<U>, void> ICollection<TCollection, THeap, T, TIsDynamic, TIsConcurencySupport, TIsReadonly, TIsCollectionCanReturnRef>::MoveToAsync(TLambda&& lambda)
	{
		decltype(auto) thisCollectionRef = static_cast<ThisCollection&>(*this);

		if constexpr (NewCollection<U>::IsDynamic == false)
		{
			NewCollection<U> instance = NewCollection<U>(thisCollectionRef.Count);

			SizeT index = 0;
			co_await thisCollectionRef.RemoveAllAsync([&](T&& item, SizeT index) -> Threading::Task<void> InlineLambda {
				if constexpr (Concepts::AsyncMoveFunctionWithIdx<TLambda, T>)
				{
					instance.UnsafeSet(co_await lambda(MoveRValue(item), index), index);
				}
				else
				{
					instance.UnsafeSet(co_await lambda(MoveRValue(item)), index);
				}
				index++;
			});

			co_return instance;
		}
		else
		{
			NewCollection<U> instance = NewCollection<U>();

			if constexpr (Concepts::AsyncMoveFunctionWithIdx<TLambda, T>)
			{
				co_await thisCollectionRef.RemoveAllAsync([&](T&& item, SizeT index) -> Threading::Task<void> InlineLambda{
					instance.UnsafeAdd(co_await lambda(MoveRValue(item), index));
				});
			}
			else
			{
				co_await thisCollectionRef.RemoveAllAsync([&](T&& item) -> Threading::Task<void> InlineLambda
				{
					instance.UnsafeAdd(co_await lambda(MoveRValue(item)));
				});
			}

			co_return instance;
		}
	}

	template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySupport, Boolean TIsReadonly, Boolean TIsCollectionCanReturnRef>
	template<template <typename> typename NewCollection, typename TLambda, typename U>
	requires (Concepts::AsyncCopyFunction<TLambda, T> && !NewCollection<U>::IsReadonly)
	Threading::BaseTask<NewCollection<U>, void> ICollection<TCollection, THeap, T, TIsDynamic, TIsConcurencySupport, TIsReadonly, TIsCollectionCanReturnRef>::SelectAsync(TLambda&& lambda) const&
	{
		return CopyToAsync<NewCollection>(MoveRValue(lambda));
	}

	template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySupport, Boolean TIsReadonly, Boolean TIsCollectionCanReturnRef>
	template<template <typename> typename NewCollection, typename TLambda, typename U>
		requires (Concepts::AsyncCopyFunction<TLambda, T> && !NewCollection<U>::IsReadonly)
	Threading::BaseTask<NewCollection<U>, void> ICollection<TCollection, THeap, T, TIsDynamic, TIsConcurencySupport, TIsReadonly, TIsCollectionCanReturnRef>::SelectAsync(TLambda&& lambda) const&&
	{
		return CopyToAsync<NewCollection>(MoveRValue(lambda));
	}

	template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySupport, Boolean TIsReadonly, Boolean TIsCollectionCanReturnRef>
	template<template <typename> typename NewCollection, typename TLambda, typename U>
		requires (Concepts::AsyncCopyFunction<TLambda, T> && !NewCollection<U>::IsReadonly)
	Threading::BaseTask<NewCollection<U>, void> ICollection<TCollection, THeap, T, TIsDynamic, TIsConcurencySupport, TIsReadonly, TIsCollectionCanReturnRef>::SelectAsync(TLambda&& lambda) &
	{
		return CopyToAsync<NewCollection>(MoveRValue(lambda));
	}

	template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySupport, Boolean TIsReadonly, Boolean TIsCollectionCanReturnRef>
	template<template <typename> typename NewCollection, typename TLambda, typename U>
		requires (Concepts::AsyncMoveFunction<TLambda, T&&> && !NewCollection<U>::IsReadonly && !TIsReadonly)
	Threading::BaseTask<NewCollection<U>, void> ICollection<TCollection, THeap, T, TIsDynamic, TIsConcurencySupport, TIsReadonly, TIsCollectionCanReturnRef>::SelectAsync(TLambda&& lambda)&&
	{
		return MoveToAsync<NewCollection>(MoveRValue(lambda));
	}
}


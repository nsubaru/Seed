#pragma once
#include "Seed.Core.Threading.Containers.Concurrency.Base.h"

namespace Seed::Core
{
#pragma region ForEach
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurrencyFixedArray<THeap, TSize, T>::ForEachAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> || Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			auto _ = Threading::SharedScopedLock(lock);

			for (SizeT i = 0; i < TSize; i++)
			{
				co_await lambda(this->array[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> || Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			auto _ = Threading::SharedScopedLock(lock);

			for (SizeT i = 0; i < TSize; i++)
			{
				co_await lambda(this->array[i], i);
			}

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, T&> || Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, T&, SizeT> || Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>);
			__assume(0);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurrencyFixedArray<THeap, TSize, T>::ForEachAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>) 
		{
			auto _ = Threading::SharedScopedLock(lock);

			for (SizeT i = 0; i < TSize; i++)
			{
				co_await lambda(this->array[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>) 
		{
			auto _ = Threading::SharedScopedLock(lock);

			for (SizeT i = 0; i < TSize; i++)
			{
				co_await lambda(this->array[i], i);
			}

			co_return;
		}
		else 
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>);
			__assume(0);
		}
	}
#pragma endregion

#pragma region RemoveAll
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurrencyFixedArray<THeap, TSize, T>::RemoveAllAsyncImpl(TLambda&& lambda)
	{
		auto _ = Threading::ExclusiveScopedLock(lock);

		for (SizeT i = 0; i < TSize; i++)
		{
			co_await lambda(MoveRValue(this->array[i]));
		}

		co_return;
	}
#pragma endregion
}
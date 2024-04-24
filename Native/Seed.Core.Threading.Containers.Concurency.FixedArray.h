#pragma once
#include "Seed.Core.Threading.Containers.Concurency.Base.h"

namespace Seed::Core
{
#pragma region ForEach
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurencyFixedArray<THeap, TSize, T>::ForEachAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> || Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			auto _ = Threading::SharedScoppedLock(lock);

			for (SizeT i = 0; i < TSize; i++)
			{
				co_await lambda(this->array[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> || Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			auto _ = Threading::SharedScoppedLock(lock);

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
	inline Threading::BaseTask<void, void> Containers::BaseConcurencyFixedArray<THeap, TSize, T>::ForEachAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>) 
		{
			auto _ = Threading::SharedScoppedLock(lock);

			for (SizeT i = 0; i < TSize; i++)
			{
				co_await lambda(this->array[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>) 
		{
			auto _ = Threading::SharedScoppedLock(lock);

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
	inline Threading::BaseTask<void, void> Containers::BaseConcurencyFixedArray<THeap, TSize, T>::RemoveAllAsyncImpl(TLambda&& lambda)
	{
		auto _ = Threading::ExclusiveScoppedLock(lock);

		for (SizeT i = 0; i < TSize; i++)
		{
			co_await lambda(MoveRValue(this->array[i]));
		}

		co_return;
	}
#pragma endregion
}
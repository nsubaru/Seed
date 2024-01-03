#pragma once
#include "Seed.Core.Threading.Containers.NonConcurency.Base.h"

namespace Seed::Core
{
#pragma region ForEach
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseFixedQueue<THeap, TSize, T>::ForEachAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(GetByIdx(i));
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
			Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(GetByIdx(i), i);
			}

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseFixedQueue<THeap, TSize, T>::ForEachAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(GetByIdx(i));
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(GetByIdx(i), i);
			}

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>);
		}
	}
#pragma endregion

#pragma region RemoveAll
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseFixedQueue<THeap, TSize, T>::RemoveAllAsyncImpl(TLambda&& lambda)
	{
		if (count == 0)
			co_return;

		for (SizeT i = 0; i < count; i++)
		{
			co_await lambda(MoveRValue(GetByIdx(i)));
		}

		this->count = 0;
		co_return;
	}
#pragma endregion
}
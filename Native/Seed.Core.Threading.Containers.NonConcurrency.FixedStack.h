#pragma once
#include "Seed.Core.Threading.Containers.NonConcurrency.Base.h"

namespace Seed::Core
{
#pragma region ForEach
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseFixedStack<THeap, TSize, T>::ForEachAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> || Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			if (count == 0)
				co_return;

			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(stack[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> || Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			if (count == 0)
				co_return;

			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(stack[i], i);
			}

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, T&> || Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, T&, SizeT> || Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseFixedStack<THeap, TSize, T>::ForEachAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			if (count == 0)
				co_return;

			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(stack[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			if (count == 0)
				co_return;

			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(stack[i], i);
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
	inline Threading::BaseTask<void, void> Containers::BaseFixedStack<THeap, TSize, T>::RemoveAllAsyncImpl(TLambda&& lambda)
	{
		for (SizeT i = 0; i < count; i++)
		{
			co_await lambda(MoveRValue(stack[i])); 
			stack[i].~T();
		}
		this->count = 0;
		co_return;
	}
#pragma endregion
}
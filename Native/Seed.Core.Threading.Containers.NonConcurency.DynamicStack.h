#pragma once
#include "Seed.Core.Threading.Containers.NonConcurency.Base.h"

namespace Seed::Core
{
	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDynamicStack<THeap, T>::ForEachAsyncImpl(TLambda&& lambda)
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

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDynamicStack<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT itemsCount)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> || Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			if (count == 0)
				co_return;

			SizeT end = (itemsCount > count) ? count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				co_await lambda(stack[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> || Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			if (count == 0)
				co_return;

			SizeT end = (itemsCount > count) ? count : itemsCount;

			for (SizeT i = 0; i < end; i++)
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

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDynamicStack<THeap, T>::ForEachAsyncImpl(TLambda&& lambda) const
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

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDynamicStack<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT itemsCount) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			if (count == 0)
				co_return;

			SizeT end = (itemsCount > count) ? count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				co_await lambda(stack[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			if (count == 0)
				co_return;

			SizeT end = (itemsCount > count) ? count : itemsCount;

			for (SizeT i = 0; i < end; i++)
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
	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDynamicStack<THeap, T>::RemoveAllAsyncImpl(TLambda&& lambda)
	{
		for (SizeT i = 0; i < count; i++)
		{
			co_await lambda(MoveRValue(stack[i]));
		}

		delete[] stack; stack = nullptr;
		count = bufferSize = 0;
		this->count = 0;

		co_return;
	}
#pragma endregion

#pragma region GetCountIf
	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<SizeT, void> Containers::BaseDynamicStack<THeap, T>::GetCountIfAsyncImpl(TLambda&& lambda) const
	{
		SizeT selectedCount = 0;

		for (SizeT i = 0; i < count; i++)
		{
			if (co_await lambda(stack[i]))
				selectedCount++;
		}

		co_return selectedCount;
	}
#pragma endregion
}
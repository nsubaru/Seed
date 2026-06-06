#pragma once
#include "Seed.Core.Threading.Containers.Concurrency.Base.h"

namespace Seed::Core
{
#pragma region ForEach
	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurrencyDynamicStack<THeap, T>::ForEachAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> || Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			auto _ = Threading::SharedScopedLock(lock);

			if (this->count == 0)
				co_return;

			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(this->stack[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> || Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			auto _ = Threading::SharedScopedLock(lock);

			if (this->count == 0)
				co_return;

			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(this->stack[i], i);
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

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurrencyDynamicStack<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT itemsCount)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> || Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			auto _ = Threading::SharedScopedLock(lock);

			if (this->count == 0)
				co_return;

			SizeT end = (itemsCount > this->Count) ? this->Count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				co_await lambda(this->stack[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> || Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			auto _ = Threading::SharedScopedLock(lock);

			if (this->count == 0)
				co_return;

			SizeT end = (itemsCount > this->Count) ? this->Count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				co_await lambda(this->stack[i], i);
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

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurrencyDynamicStack<THeap, T>::ForEachAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			auto _ = Threading::SharedScopedLock(lock);

			if (this->count == 0)
				co_return;

			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(this->stack[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			auto _ = Threading::SharedScopedLock(lock);

			if (this->count == 0)
				co_return;

			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(this->stack[i], i);
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

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurrencyDynamicStack<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT itemsCount) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			auto _ = Threading::SharedScopedLock(lock);

			if (this->count == 0)
				co_return;

			SizeT end = (itemsCount > this->Count) ? this->Count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				co_await lambda(this->stack[i]);
			}

			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			auto _ = Threading::SharedScopedLock(lock);

			if (this->count == 0)
				co_return;

			SizeT end = (itemsCount > this->Count) ? this->Count : itemsCount;

			for (SizeT i = 0; i < end; i++)
			{
				co_await lambda(this->stack[i], i);
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
	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurrencyDynamicStack<THeap, T>::RemoveAllAsyncImpl(TLambda&& lambda)
	{
		auto _ = Threading::ExclusiveScopedLock(lock);

		for (SizeT i = 0; i < this->count; i++)
		{
			co_await lambda(MoveRValue(this->stack[i]));
		}

		delete[] this->stack;
		this->stack = nullptr;
		this->count = this->bufferSize = 0;

		co_return;
	}
#pragma endregion

#pragma region GetCountIf
	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<SizeT, void> Containers::BaseConcurrencyDynamicStack<THeap, T>::GetCountIfAsyncImpl(TLambda&& lambda) const
	{
		auto _ = Threading::SharedScopedLock(lock);

		SizeT selectedCount = 0;

		for (SizeT i = 0; i < count; i++)
		{
			if (co_await lambda(this->stack[i]))
				selectedCount++;
		}

		co_return selectedCount;
	}
#pragma endregion
}
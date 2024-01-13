#pragma once
#include "Seed.Core.Threading.Containers.NonConcurency.Base.h"

namespace Seed::Core
{
#pragma region ForEach
	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDynamicQueue<THeap, T>::ForEachAsyncImpl(TLambda&& lambda)
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
			Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{

			SizeT index = 0;

			for (SizeT i = 0; i < count; i++, index++)
			{
				co_await lambda(GetByIdx(i), index);
			}

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>);
			__assume(false);
			co_return;
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDynamicQueue<THeap, T>::ForEachAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < this->count; i++)
			{
				co_await lambda(GetByIdx(i));
			}
			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			SizeT index = 0;

			for (SizeT i = 0; i < this->count; i++, index++)
			{
				co_await lambda(GetByIdx(i), index);
			}

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>);
			__assume(false);
			co_return;
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDynamicQueue<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT requiredCount)
	{
		if (requiredCount > this->count)
			requiredCount = this->count;

		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> ||
			Concepts::IsAsyncInvocable<TLambda, const T&>)
		{

			for (SizeT i = 0; i < requiredCount; i++)
			{
				co_await lambda(GetByIdx(i));
			}
			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
			Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{

			SizeT index = 0;

			for (SizeT i = 0; i < requiredCount; i++, index++)
			{
				co_await lambda(GetByIdx(i), index);
			}

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>);
			__assume(false);
			co_return;
		}
	}

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDynamicQueue<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT requiredCount) const
	{
		if (requiredCount > this->count)
			requiredCount = this->count;

		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			for (SizeT i = 0; i < requiredCount; i++)
			{
				co_await lambda(GetByIdx(i));
			}
			co_return;
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			SizeT index = 0;

			for (SizeT i = 0; i < requiredCount; i++, index++)
			{
				co_await lambda(GetByIdx(i), index);
			}

			co_return;
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>);
			__assume(false);
			co_return;
		}
	}
#pragma endregion

#pragma region RemoveAll
	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseDynamicQueue<THeap, T>::RemoveAllAsyncImpl(TLambda&& lambda)
	{
		if (count == 0)
			co_return;

		for (SizeT i = 0; i < count; i++)
		{
			co_await lambda(MoveRValue(GetByIdx(i)));
		}

		delete[] queue;
		front = rear = count = bufferSize = 0;
		queue = nullptr;
		this->count = 0;
		
		co_return;
	}
#pragma endregion

#pragma region GetCountIf
	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<SizeT, void> Containers::BaseDynamicQueue<THeap, T>::GetCountIfAsyncImpl(TLambda&& lambda) const
	{
		SizeT selectedCount = 0;

		for (SizeT i = 0; i < count; i++)
		{
			if (co_await lambda(GetByIdx(i)))
				selectedCount++;
		}

		co_return selectedCount;
	}
#pragma endregion
}
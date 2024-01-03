#pragma once
#include "Seed.Core.Threading.Containers.NonConcurency.Base.h"

namespace Seed::Core
{
#pragma region ForEach
	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurencyFixedQueue<THeap, TSize, T>::ForEachAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					co_await lambda(this->GetByIdx(i));
				}

				co_return;
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
			Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					co_await lambda(this->GetByIdx(i), i);
				}

				co_return;
			});
		}
		else
		{
			static_assert(Concepts::IsAsyncInvocable<TLambda, T&> ||
				Concepts::IsAsyncInvocable<TLambda, const T&> ||
				Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
				Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>);
			__assume(0);
		}
	}

	template<typename THeap, SizeT TSize, typename T>
	requires (TSize > 0)
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurencyFixedQueue<THeap, TSize, T>::ForEachAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					co_await lambda(this->GetByIdx(i));
				}
				co_return;
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					co_await lambda(this->GetByIdx(i), i);
				}

				co_return;
			});
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
	Threading::BaseTask<void, void> Containers::BaseConcurencyFixedQueue<THeap, TSize, T>::RemoveAllImpl(TLambda&& lambda)
	{
		return this->flowController.SharedLock([&, lambda = MoveRValue(lambda)]()->Threading::BaseTask<void, void> InlineLambda{
			if (this->count == 0)
				co_return;

			for (SizeT i = 0; i < this->count; i++)
			{
				co_await lambda(MoveRValue(this->GetByIdx(i)));
			}

			this->count = 0;

			co_return;
		});
	}
#pragma endregion
}
#pragma once
#include "Seed.Core.Threading.Containers.Concurency.Base.h"

namespace Seed::Core
{
#pragma region ForEach
	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurencyDynamicQueue<THeap, T>::ForEachAsyncImpl(TLambda&& lambda)
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> ||
			Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					co_await lambda(GetByIdx(i));
				}

				co_return;
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
			Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				SizeT index = 0;

				for (SizeT i = 0; i < this->count; i++, index++)
				{
					co_await lambda(GetByIdx(i), index);
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

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurencyDynamicQueue<THeap, T>::ForEachAsyncImpl(TLambda&& lambda) const
	{
		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				for (SizeT i = 0; i < this->count; i++)
				{
					co_await lambda(GetByIdx(i));
				}

				co_return;
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				SizeT Index = 0;

				for (SizeT i = 0; i < this->count; i++, Index++)
				{
					co_await lambda(GetByIdx(i), Index);
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

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurencyDynamicQueue<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT requiredCount)
	{
		if (requiredCount > this->Count)
			requiredCount = this->Count;

		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> ||
			Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				for (SizeT i = 0; i < requiredCount; i++)
				{
					co_await lambda(GetByIdx(i));
				}

				co_return;
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
			Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				SizeT index = 0;

				for (SizeT i = 0; i < requiredCount; i++, index++)
				{
					co_await lambda(GetByIdx(i), index);
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

	template<typename THeap, typename T>
	template<typename TLambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurencyDynamicQueue<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT requiredCount) const
	{
		if (requiredCount > this->Count)
			requiredCount = this->Count;

		if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				for (SizeT i = 0; i < requiredCount; i++)
				{
					co_await lambda(GetByIdx(i));
				}

				co_return;
			});
		}
		else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
		{
			return this->flowController.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
				SizeT Index = 0;

				for (SizeT i = 0; i < requiredCount; i++, Index++)
				{
					co_await lambda(GetByIdx(i), Index);
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
	template<typename THeap, typename T>
	template<typename Lambda>
	inline Threading::BaseTask<void, void> Containers::BaseConcurencyDynamicQueue<THeap, T>::RemoveAllAsyncImpl(Lambda&& lambda)
	{
		return this->flowController.SharedLock([&, lambda = MoveRValue(lambda)]()->BaseConcurencyDynamicQueue & InlineLambda{
			if (this->count == 0)
				return *this;
			for (SizeT i = 0; i < this->count; i++)
			{
				co_await lambda(MoveRValue(GetByIdx(i)));
			}

			delete[] this->queue;
			this->front = this->rear = this->count = this->bufferSize = 0;
			this->queue = nullptr;

			co_return;
		});
	}
#pragma endregion

#pragma region GetCountIf
	template<typename THeap, typename T>
	template<typename Lambda>
	inline Threading::BaseTask<SizeT, void> Containers::BaseConcurencyDynamicQueue<THeap, T>::GetCountIfAsyncImpl(Lambda&& lambda) const
	{
		return this->flowController.SharedLock([&]() InlineLambda{
			SizeT selectedCount = 0;

			for (SizeT i = 0; i < this->count; i++)
			{
				if (co_await lambda(GetByIdx(i)))
					selectedCount++;
			}

			co_return selectedCount;
		});
	}
#pragma endregion
}
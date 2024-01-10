#pragma once
#include "Seed.Core.Threading.Containers.NonConcurency.Base.h"

namespace Seed::Core
{
#pragma region RemoveAll
    template<typename THeap, typename T>
    template<typename TLambda>
    Threading::BaseTask<void, void> Containers::BaseArray<THeap, T>::RemoveAllAsyncImpl(TLambda&& lambda)
    {
		if constexpr (Concepts::IsAsyncInvocable<TLambda, T&&>)
		{
			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(MoveRValue(array[i]));
			}
			delete[] this->array;
			this->array = nullptr;
			this->count = 0;
		}
		else
		{
			for (SizeT i = 0; i < count; i++)
			{
				co_await lambda(MoveRValue(array[i]), i);
			}
			delete[] this->array;
			this->array = nullptr;
			this->count = 0;
		}
    }
#pragma endregion
}
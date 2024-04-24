#pragma once
#include "Seed.Core.Threading.Containers.Concurency.Base.h"

namespace Seed::Core
{
#pragma region ForEach
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    inline Threading::BaseTask<void, void> Containers::BaseConcurencyFixedStack<THeap, TSize, T>::ForEachAsyncImpl(Lambda&& lambda)
    {
        if constexpr (Concepts::IsAsyncInvocable<Lambda, T&> ||
                Concepts::IsAsyncInvocable<Lambda, const T&>)
        {
            auto _ = Threading::SharedScoppedLock(lock);

            if (this->count == 0)
                co_return;

            for (SizeT i = this->count; i-- > 0;)
            {
                co_await lambda(this->stack[i]);
            }

            co_return;
        }
        else if constexpr (Concepts::IsAsyncInvocable<Lambda, T&, SizeT> ||
            Concepts::IsAsyncInvocable<Lambda, const T&, SizeT>)
        {
            auto _ = Threading::SharedScoppedLock(lock);

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
            static_assert(Concepts::IsAsyncInvocable<Lambda, T&> ||
                Concepts::IsAsyncInvocable<Lambda, const T&> ||
                Concepts::IsAsyncInvocable<Lambda, T&, SizeT> ||
                Concepts::IsAsyncInvocable<Lambda, const T&, SizeT>);
            __assume(0);
        }
    }

    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    inline Threading::BaseTask<void, void> Containers::BaseConcurencyFixedStack<THeap, TSize, T>::ForEachAsyncImpl(Lambda&& lambda) const
    {
        if constexpr (Concepts::IsAsyncInvocable<Lambda, const T&>)
        {
            auto _ = Threading::SharedScoppedLock(lock);

            if (this->count == 0)
                co_return;

            for (SizeT i = 0; i < count; i++)
            {
                co_await lambda(this->stack[i]);
            }

            co_return;
        }
        else if constexpr (Concepts::IsAsyncInvocable<Lambda, const T&, SizeT>)
        {
            auto _ = Threading::SharedScoppedLock(lock);

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
            static_assert(Concepts::IsAsyncInvocable<Lambda, const T&> ||
                Concepts::IsAsyncInvocable<Lambda, const T&, SizeT>);
            __assume(0);
        }
    }
#pragma endregion

#pragma region RemoveAll
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    inline Threading::BaseTask<void, void> Containers::BaseConcurencyFixedStack<THeap, TSize, T>::RemoveAllAsyncImpl(Lambda&& lambda)
    {
        auto _ = Threading::ExclusiveScoppedLock(lock);

        for (SizeT i = 0; i < this->count; i++)
        {
            co_await lambda(MoveRValue(this->stack[i]));
            this->stack[i].~T();
        }

        this->count = 0;
        co_return;
    }
#pragma endregion
}
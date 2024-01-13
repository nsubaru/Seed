#pragma once
#include "Seed.Core.Threading.Containers.Concurency.Base.h"

namespace Seed::Core
{
#pragma region ForEach
    template<typename THeap, typename T>
    template<typename TLambda>
    inline Threading::BaseTask<void,void> Containers::BaseConcurencyArray<THeap, T>::ForEachAsyncImpl(TLambda&& lambda)
    {
        if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> ||
                        Concepts::IsAsyncInvocable<TLambda, const T&>)
        {
            return lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
                for (SizeT i = 0; i < count; i++)
                {
                    co_await lambda(array[i]);
                }

                co_return;
            });
        }
        else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
                            Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
        {
            return lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
                for (SizeT i = 0; i < count; i++)
                {
                    co_await lambda(array[i], i);
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
    inline Threading::BaseTask<void, void> Containers::BaseConcurencyArray<THeap, T>::ForEachAsyncImpl(TLambda&& lambda) const
    {
        if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
        {
            return this->lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
                for (SizeT i = 0; i < count; i++)
                {
                    co_await lambda(array[i]);
                }

                co_return;
            });
        }
        else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
        {
            return this->lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
                for (SizeT i = 0; i < count; i++)
                {
                    co_await lambda(array[i], i);
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
    inline Threading::BaseTask<void, void> Containers::BaseConcurencyArray<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT reqCount)
    {
        if (reqCount > this->Count)
            reqCount = this->Count;

        if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> ||
                        Concepts::IsAsyncInvocable<TLambda, const T&>)
        {
            return lock.SharedLock([&] ()->Threading::BaseTask<void, void> {
                for (SizeT i = 0; i < reqCount; i++)
                {
                    co_await lambda(array[i]);
                }

                co_return;
            });
        }
        else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
                            Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
        {
            return lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
                for (SizeT i = 0; i < reqCount; i++)
                {
                    co_await lambda(array[i], i);
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
    inline Threading::BaseTask<void, void> Containers::BaseConcurencyArray<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT reqCount) const
    {
        if (reqCount > this->Count)
            reqCount = this->Count;

        if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
        {
            return this->lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
                for (SizeT i = 0; i < reqCount; i++)
                {
                    co_await lambda(array[i]);
                }

                co_return;
            });
        }
        else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
        {
            return this->lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
                for (SizeT i = 0; i < reqCount; i++)
                {
                    co_await lambda(array[i], i);
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
    inline Threading::BaseTask<void, void> Containers::BaseConcurencyArray<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT startIndex, SizeT reqCount)
    {
        if (startIndex > this->Count)
            Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, startIndex, this->Count);

        if (reqCount == Math::Max<SizeT>())
            reqCount = this->count - startIndex;

        if (startIndex + reqCount > this->Count)
            reqCount = this->Count - startIndex;

        if constexpr (Concepts::IsAsyncInvocable<TLambda, T&> ||
                        Concepts::IsAsyncInvocable<TLambda, const T&>)
        {
            return lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
                for (SizeT i = 0; i < reqCount; i++)
                {
                    co_await lambda(array[startIndex + i]);
                }
                
                co_return;
            });
        }
        else if constexpr (Concepts::IsAsyncInvocable<TLambda, T&, SizeT> ||
                            Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
        {
            return lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
                for (SizeT i = 0; i < reqCount; i++)
                {
                    co_await lambda(array[startIndex + i], startIndex + i);
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
    inline Threading::BaseTask<void, void> Containers::BaseConcurencyArray<THeap, T>::ForEachAsyncImpl(TLambda&& lambda, SizeT startIndex, SizeT reqCount) const
    {
        if (startIndex > this->Count)
            Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, startIndex, this->Count);

        if (reqCount == Math::Max<SizeT>())
            reqCount = this->count - startIndex;

        if (startIndex + reqCount > this->Count)
            reqCount = this->Count - startIndex;

        if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&>)
        {
            return this->lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
                for (SizeT i = 0; i < reqCount; i++)
                {
                    co_await lambda(array[startIndex + i]);
                }

                co_return;
            });
        }
        else if constexpr (Concepts::IsAsyncInvocable<TLambda, const T&, SizeT>)
        {
            return this->lock.SharedLock([&]()->Threading::BaseTask<void, void> InlineLambda{
                for (SizeT i = 0; i < reqCount; i++)
                {
                    co_await lambda(array[startIndex + i], startIndex + i);
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
    template<typename TLambda>
    inline Threading::BaseTask<void, void> Containers::BaseConcurencyArray<THeap, T>::RemoveAllAsyncImpl(TLambda&& lambda)
    {
        return lock.ExclusiveLock([&]()->Threading::BaseTask<void, void> InlineLambda {
            for (SizeT i = 0; i < count; i++) {
                co_await lambda(MoveRValue(array[i]));
            }
                    	
            delete[] this->array;
            this->array = nullptr;
            this->count = 0;

            co_return;
        });
    }
#pragma endregion

#pragma region GetCountIf
    template<typename THeap, typename T>
    template<typename TLambda>
    inline Threading::BaseTask<SizeT, void> Containers::BaseConcurencyArray<THeap, T>::GetCountIfAsyncImpl(TLambda&& lambda) const
    {
        return lock.SharedLock([&]()-> Threading::BaseTask<SizeT, void> InlineLambda{
            SizeT selectedCount = 0;
            for (SizeT i = 0; i < count; i++)
            {
                if (co_await lambda(array[i]))
                    selectedCount++;
            }

            co_return selectedCount;
        });
    }
#pragma endregion
}
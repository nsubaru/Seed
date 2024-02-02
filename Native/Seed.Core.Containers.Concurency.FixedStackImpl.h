#pragma once
#include "Seed.Core.Containers.Concurency.FixedStack.h"

#pragma warning(push)
#pragma warning(disable: 4172)
#pragma warning(disable: 4996)

namespace Seed::Core::Containers
{
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    BaseConcurencyFixedStack<THeap, TSize, T>::~BaseConcurencyFixedStack()
    {
        if constexpr (TypeTraits::IsSameV<THeap, Memory::SensitiveDataHeap> &&
            TypeTraits::IsStandardLayout<T>)
        {
            Memory::SecureZeroMemory(stack);
        }
    };
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    BaseConcurencyFixedStack<THeap, TSize, T>::BaseConcurencyFixedStack(InitilizerList<T>&& list): count(0), lock(), stack()
    {
        if (list.size() != 0)
        {
            SizeT actuallyCopied = (TSize < list.size()) ? TSize : list.size();
            auto it = list.begin();

            for (SizeT i = 0; i < actuallyCopied; i++)
            {
                this->stack[i] = MoveRValue(*it); ++it;
            }

            this->count = actuallyCopied;
        }
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    BaseConcurencyFixedStack<THeap, TSize, T>::BaseConcurencyFixedStack(const BaseConcurencyFixedStack& original): lock()
    {
        original.lock.SharedLock([&] () {
            this->count = original.count;

            if constexpr (TypeTraits::IsStandardLayout<T>)
            {
                Memory::Copy(original.stack, this->stack);
            }
            else
            {
                for (SizeT i = 0; i < TSize; i++)
                {
                    this->stack[i] = original.stack[i];
                }
            }
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    BaseConcurencyFixedStack<THeap, TSize, T>::BaseConcurencyFixedStack(BaseConcurencyFixedStack&& original): lock()
    {
        original.lock.SharedLock([&] () {
            this->count = original.count;
            original.count = 0;

            if constexpr (TypeTraits::IsStandardLayout<T>)
            {
                Memory::Copy(original.stack, this->stack);
            }
            else
            {
                for (SizeT i = 0; i < TSize; i++)
                {
                    this->stack[i] = MoveRValue(original.stack[i]);
                }
            }
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    BaseConcurencyFixedStack<THeap, TSize, T>& BaseConcurencyFixedStack<THeap, TSize, T>::operator=(const BaseConcurencyFixedStack& original)
    {
        this->~BaseConcurencyFixedStack();
        this->BaseConcurencyFixedStack::BaseConcurencyFixedStack(original);
        return *this;
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    BaseConcurencyFixedStack<THeap, TSize, T>& BaseConcurencyFixedStack<THeap, TSize, T>::operator=(BaseConcurencyFixedStack&& original)
    {
        this->~BaseConcurencyFixedStack();
        this->BaseConcurencyFixedStack::BaseConcurencyFixedStack(MoveRValue(original));
        return *this;
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    void BaseConcurencyFixedStack<THeap, TSize, T>::Push(const T& item)
    {
        this->lock.ExclusiveLock([&]() mutable InlineLambda{
            if (this->count == TSize)
                Throw(FullStackException(SourcePosition, TSize));

            this->stack[this->count] = item;
            ++this->count;
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    void BaseConcurencyFixedStack<THeap, TSize, T>::Push(T&& item)
    {
        this->lock.ExclusiveLock([&]() mutable InlineLambda{
            if (this->count == TSize)
                Throw(FullStackException(SourcePosition, TSize));

            this->stack[this->count] = MoveRValue(item);
            ++this->count;
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    T BaseConcurencyFixedStack<THeap, TSize, T>::Pop()
    {
        return MoveRValue(this->lock.ExclusiveLock([&]() mutable InlineLambda{
            if (this->count == 0)
                Throw(EmptyStackException(SourcePosition));

            --this->count;
            return MoveRValue(this->stack[this->count]);
                                                   }));
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    T BaseConcurencyFixedStack<THeap, TSize, T>::Pop(T&& item)
    {
        return this->lock.ExclusiveLock([&]() mutable InlineLambda{
            if (this->count == 0)
                return MoveRValue(item);

            --this->count;
            return MoveRValue(this->stack[this->count]);
            });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    T BaseConcurencyFixedStack<THeap, TSize, T>::Pop(const T& item)
    {
        return this->lock.ExclusiveLock([&]() mutable InlineLambda{
            if (this->count == 0)
                return MoveRValue(item);

            --this->count;
            return MoveRValue(this->stack[this->count]);
            });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename TLambda> requires Concepts::IsInvocableWithResult<TLambda, T>
    T BaseConcurencyFixedStack<THeap, TSize, T>::Pop(TLambda&& lambda)
    {
        return this->lock.ExclusiveLock([&]() mutable InlineLambda{
            if (this->count == 0)
                return lambda();

            --this->count;
            return MoveRValue(this->stack[this->count]);
            });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    const T& BaseConcurencyFixedStack<THeap, TSize, T>::Top()
    {
        return this->lock.ExclusiveLock([&]() mutable -> const T & InlineLambda{
            if (this->count == 0)
                Throw(EmptyStackException(SourcePosition));

            return this->stack[this->count - 1];
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    const T& BaseConcurencyFixedStack<THeap, TSize, T>::Top() const
    {
        return this->lock.ExclusiveLock([&]() mutable -> const T & InlineLambda{
            if (this->count == 0)
                Throw(EmptyStackException(SourcePosition));

            return this->stack[this->count - 1];
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    ForceInline SizeT BaseConcurencyFixedStack<THeap, TSize, T>::GetCount() const
    {
        return this->count;
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename> requires (Concepts::IsClonable<T>)
    BaseConcurencyFixedStack<THeap, TSize, T> BaseConcurencyFixedStack<THeap, TSize, T>::Clone() const {
            return this->lock.SharedLock([&]() InlineLambda {
                BaseConcurencyFixedStack result;
                result.count = this->count;

                if constexpr (TypeTraits::IsStandardLayout<T>) 
                {
                    Memory::Copy(this->stack, result.stack);
                }
                else {
                    for (SizeT i = 0; i < this->count; i++)
                    {
                        result.stack[i] = this->stack[i];
                    }
                }
                return result;
            });
        }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
    BaseConcurencyFixedStack<THeap, TSize, T> BaseConcurencyFixedStack<THeap, TSize, T>::Clone(TLambda&& lambda) const {
        return this->lock.SharedLock([&]() InlineLambda {
            BaseConcurencyFixedStack result;
            result.count = this->count;

            for (SizeT i = 0; i < this->count; i++)
            {
                result.stack[i] = lambda(this->stack[i]);
            }

            return result;
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    BaseConcurencyFixedStack<THeap, TSize, T>& BaseConcurencyFixedStack<THeap, TSize, T>::ForEachImpl(Lambda&& lambda)
    {
        if constexpr (TypeTraits::IsInvocable<Lambda, T&> ||
            TypeTraits::IsInvocable<Lambda, const T&>)
        {
            return this->lock.SharedLock([&]()->BaseConcurencyFixedStack & InlineLambda{
                if (this->count == 0)
                    return *this;

                for (SizeT i = this->count; i-- > 0;)
                {
                    lambda(this->stack[i]);
                }

                return *this;
            });
        }
        else if constexpr (TypeTraits::IsInvocable<Lambda, T&, SizeT> ||
            TypeTraits::IsInvocable<Lambda, const T&, SizeT>)
        {
            return this->lock.SharedLock([&]()->BaseConcurencyFixedStack & InlineLambda{
                if (this->count == 0)
                    return *this;

                for (SizeT i = 0; i < count; i++)
                {
                    lambda(this->stack[i], i);
                }

                return *this;
            });
        }
        else
        {
            static_assert(TypeTraits::IsInvocable<Lambda, T&> ||
                TypeTraits::IsInvocable<Lambda, const T&> ||
                TypeTraits::IsInvocable<Lambda, T&, SizeT> ||
                TypeTraits::IsInvocable<Lambda, const T&, SizeT>);
            __assume(0);
        }
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    const BaseConcurencyFixedStack<THeap, TSize, T>& BaseConcurencyFixedStack<THeap, TSize, T>::ForEachImpl(Lambda&& lambda) const
    {
        if constexpr (TypeTraits::IsInvocable<Lambda, const T&>)
        {
            return this->lock.SharedLock([&]() -> const BaseConcurencyFixedStack & InlineLambda{
                if (this->count == 0)
                    return *this;

                for (SizeT i = 0; i < count; i++)
                {
                    lambda(this->stack[i]);
                }

                return *this;
            });
        }
        else if constexpr (TypeTraits::IsInvocable<Lambda, const T&, SizeT>)
        {
            return this->lock.SharedLock([&]() -> const BaseConcurencyFixedStack & InlineLambda{
                if (this->count == 0)
                    return *this;

                for (SizeT i = 0; i < count; i++)
                {
                    lambda(this->stack[i], i);
                }

                return *this;
            });
        }
        else
        {
            static_assert(TypeTraits::IsInvocable<Lambda, const T&> ||
                TypeTraits::IsInvocable<Lambda, const T&, SizeT>);
            __assume(0);
        }
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename U, typename TLambda>
    U BaseConcurencyFixedStack<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, const U& value) const
    {
        return MoveRValue(this->lock.SharedLock([&]() mutable InlineLambda{
            U result = value;
            U & data = result;

            if (this->count == 0)
                return result;

            for (SizeT i = 0; i < count; i++)
            {
                if constexpr (
                    Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
                    Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
                {
                    data = lambda(data, this->stack[i]);
                }
                else
                {
                    lambda(data, this->stack[i]);
                }
            }

            return MoveRValue(result);
                                                }));
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename U, typename TLambda>
    U BaseConcurencyFixedStack<THeap, TSize, T>::AggregateImpl(TLambda&& lambda, U&& value) const
    {
        return MoveRValue(this->lock.SharedLock([&, value = MoveRValue(value)]() mutable InlineLambda{
            if (this->count == 0)
                return value;

            for (SizeT i = 0; i < count; i++)
            {
                if constexpr (
                    Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
                    Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
                {
                    value = lambda(value, this->stack[i]);
                }
                else
                {
                    lambda(value, this->stack[i]);
                }
            }

            return MoveRValue(value);
                                                }));
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    ForceInline bool BaseConcurencyFixedStack<THeap, TSize, T>::AnyImpl() const
    {
        return this->count > 0;
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    bool BaseConcurencyFixedStack<THeap, TSize, T>::AnyImpl(Lambda&& lambda) const
    {
        return MoveRValue(this->lock.SharedLock([&]() InlineLambda{
            if (this->count == 0)
                return false;

            for (SizeT i = 0; i < count; i++)
            {
                if (lambda(this->stack[i]))
                    return true;
            }

            return false;
        }));
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    bool BaseConcurencyFixedStack<THeap, TSize, T>::AllImpl(Lambda&& lambda) const
    {
        return MoveRValue(this->lock.SharedLock([&]() InlineLambda{
            if (this->count == 0)
                return false;

            for (SizeT i = 0; i < count; i++)
            {
                if (!lambda(this->stack[i]))
                    return false;
            }

            return true;
        }));
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    T& BaseConcurencyFixedStack<THeap, TSize, T>::FirstImpl(Lambda&& lambda)
    {
        return this->lock.SharedLock([&]()->T & InlineLambda{
            if (this->count == 0)
                Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

            for (SizeT i = 0; i < count; i++)
            {
                if (lambda(this->stack[i]))
                    return static_cast<T&>(this->stack[i]);
            }

            Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    T& BaseConcurencyFixedStack<THeap, TSize, T>::LastImpl(Lambda&& lambda)
    {
        return this->lock.SharedLock([&]()->T & InlineLambda{
            if (this->count == 0)
                Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

            SizeT index = -1;

            for (SizeT i = 0; i < count; i++)
            {
                if (lambda(this->stack[i]))
                    index = i;
            }

            if (index != -1)
                return static_cast<T&>(this->stack[index]);

            Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    const T& BaseConcurencyFixedStack<THeap, TSize, T>::FirstImpl(Lambda&& lambda) const
    {
        return this->lock.SharedLock([&]() -> const T & InlineLambda{
            if (this->count == 0)
                Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

            for (SizeT i = 0; i < count; i++)
            {
                if (lambda(this->stack[i]))
                    return static_cast<const T&>(this->stack[i]);
            }

            Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    const T& BaseConcurencyFixedStack<THeap, TSize, T>::LastImpl(Lambda&& lambda) const
    {
        return this->lock.SharedLock([&]() -> const T & InlineLambda{
            if (this->count == 0)
                Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);

            SizeT index = -1;

            for (SizeT i = 0; i < count; i++)
            {
                if (lambda(this->stack[i]))
                    index = i;
            }

            if (index != -1)
                return static_cast<const T&>(this->stack[index]);

            Internals::Exceptions::ThrowNullPointException(SourcePosition, 0, 0);
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename TLambda>
    T BaseConcurencyFixedStack<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, T&& item) const
    {
        return this->lock.SharedLock([&, item = MoveRValue(item)]() InlineLambda{
            if (count == 0)
                return MoveRValue(item);

            for (SizeT i = 0; i < count; i++)
            {
                if (lambda(this->stack[i]))
                    return this->stack[i];
            }

            return MoveRValue(item);
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename TLambda>
    T BaseConcurencyFixedStack<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, T&& item) const
    {
        return this->lock.SharedLock([&, item = MoveRValue(item)]() InlineLambda{
            if (count == 0)
                return MoveRValue(item);

            SizeT index = -1;

            for (SizeT i = 0; i < count; i++)
            {
                if (lambda(this->stack[i]))
                    index = i;
            }

            if (index != -1)
                return this->stack[index];

            return MoveRValue(item);
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename TLambda>
    T BaseConcurencyFixedStack<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, const T& item) const
    {
        return this->lock.SharedLock([&]() InlineLambda{
            if (count == 0)
                return item;

            for (SizeT i = 0; i < count; i++)
            {
                if (lambda(this->stack[i]))
                    return this->stack[i];
            }

            return item;
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename TLambda>
    T BaseConcurencyFixedStack<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, const T& item) const
    {
        return this->lock.SharedLock([&]() InlineLambda{
            if (count == 0)
                return item;

            SizeT index = -1;

            for (SizeT i = 0; i < count; i++)
            {
                if (lambda(this->stack[i]))
                    index = i;
            }

            if (index != -1)
                return this->stack[index];

            return item;
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename TLambda, typename TGetterLambda>
    T BaseConcurencyFixedStack<THeap, TSize, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
    {
        return this->lock.SharedLock([&]() InlineLambda{
            if (count == 0)
                return otherLambda();

            for (SizeT i = 0; i < count; i++)
            {
                if (lambda(this->stack[i]))
                    return this->stack[i];
            }

            return otherLambda();
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename TLambda, typename TGetterLambda>
    T BaseConcurencyFixedStack<THeap, TSize, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
    {
        return this->lock.SharedLock([&]() InlineLambda{
            if (count == 0)
                return otherLambda();

            SizeT index = -1;

            for (SizeT i = 0; i < count; i++)
            {
                if (lambda(this->stack[i]))
                    index = i;
            }

            if (index != -1)
                return this->stack[index];

            return otherLambda();
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    BaseConcurencyFixedStack<THeap, TSize, T>& BaseConcurencyFixedStack<THeap, TSize, T>::ReverseImpl() {
        return this->lock.ExclusiveLock([&]() -> BaseConcurencyFixedStack& InlineLambda {
            for (SizeT i = 0; i < this->count / 2; i++) {
                Seed::Core::Swap(this->stack[i], this->stack[(this->count - 1) - i]);
            }
            return *this;
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    ForceInline void BaseConcurencyFixedStack<THeap, TSize, T>::UnsafeSet(T&& value, SizeT index)
    {
        this->stack[index] = MoveRValue(value);
        this->count = index + 1;
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    ForceInline void BaseConcurencyFixedStack<THeap, TSize, T>::UnsafeSet(const T& value, SizeT index)
    {
        this->stack[index] = MoveRValue(value);
        this->count = index + 1;
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    BaseConcurencyFixedStack<THeap, TSize, T>& BaseConcurencyFixedStack<THeap, TSize, T>::RemoveAllImpl() {
        return this->lock.ExclusiveLock([&]() -> BaseConcurencyFixedStack& InlineLambda {
            for (SizeT i = 0; i < this->count; i++) {
                this->stack[i].~T();
            }
            this->count = 0;
            return *this;
        });
    }
    
    template<typename THeap, SizeT TSize, typename T>
    requires (TSize > 0)
    template<typename Lambda>
    BaseConcurencyFixedStack<THeap, TSize, T>& BaseConcurencyFixedStack<THeap, TSize, T>::RemoveAllImpl(Lambda&& lambda) {
        return this->lock.ExclusiveLock([&]() -> BaseConcurencyFixedStack& InlineLambda {
            for (SizeT i = 0; i < this->count; i++) {
                lambda(MoveRValue(this->stack[i])); this->stack[i].~T();
            }
            this->count = 0;
            return *this;
        });
    }











}

#pragma warning(pop)
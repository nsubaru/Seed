#pragma once
#include "Seed.Core.Containers.Concurency.Array.h"

#pragma warning(push)
#pragma warning(disable: 4172)
#pragma warning(disable: 6386)
#pragma warning(disable: 4996)
#pragma warning(disable: 4172)

namespace Seed::Core::Containers
{
    template<typename THeap, typename T>
    template<typename TLambda>
    inline static void BaseConcurencyArray<THeap, T>::QuickSort(T arr[], Int64 low, Int64 high, TLambda& lambda)
    {
        if (low < high)
        {
            Int64 pIndex = QuickSortPartition(arr, low, high, lambda);

            QuickSort(arr, low, pIndex - 1, lambda);
            QuickSort(arr, pIndex + 1, high, lambda);
        }
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    inline static SizeT BaseConcurencyArray<THeap, T>::QuickSortPartition(T arr[], Int64 low, Int64 high, TLambda& lambda)
    {
        Int64 pIndex = low;
        T pivot = arr[high];

        for (Int64 i = low; i < high; i++)
        {
            if (!lambda(arr[i], pivot))
            {
                Seed::Core::Swap((T&) arr[pIndex], (T&) arr[i]);
                pIndex++;
            }
        }

        Seed::Core::Swap((T&) arr[pIndex], (T&) arr[high]);

        return pIndex;
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::ForEachImpl(TLambda&& lambda)
    {
        if constexpr (TypeTraits::IsInvocable<TLambda, T&> ||
                        TypeTraits::IsInvocable<TLambda, const T&>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < count; i++)
            {
                lambda(array[i]);
            }

            return *this;
        }
        else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
                            TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < count; i++)
            {
                lambda(array[i], i);
            }

            return *this;
        }
        else
        {
            static_assert(TypeTraits::IsInvocable<TLambda, T&> ||
                TypeTraits::IsInvocable<TLambda, const T&> ||
                TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
                TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
            __assume(0);
        }
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    const BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::ForEachImpl(TLambda&& lambda) const
    {
        if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < count; i++)
            {
                lambda(array[i]);
            }

            return *this;
        }
        else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < count; i++)
            {
                lambda(array[i], i);
            }

            return *this;
        }
        else
        {
            static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
                TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
            __assume(0);
        }
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT reqCount)
    {
        if (reqCount > this->Count)
            reqCount = this->Count;

        if constexpr (TypeTraits::IsInvocable<TLambda, T&> ||
                        TypeTraits::IsInvocable<TLambda, const T&>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < reqCount; i++)
            {
                lambda(array[i]);
            }

            return *this;
        }
        else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
                            TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < reqCount; i++)
            {
                lambda(array[i], i);
            }

            return *this;
        }
        else
        {
            static_assert(TypeTraits::IsInvocable<TLambda, T&> ||
                TypeTraits::IsInvocable<TLambda, const T&> ||
                TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
                TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
            __assume(0);
        }
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    const BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT reqCount) const
    {
        if (reqCount > this->Count)
            reqCount = this->Count;

        if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < reqCount; i++)
            {
                lambda(array[i]);
            }

            return *this;
        }
        else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < reqCount; i++)
            {
                lambda(array[i], i);
            }

            return *this;
        }
        else
        {
            static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
                TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
            __assume(0);
        }
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT startIndex, SizeT reqCount)
    {
        if (startIndex > this->Count)
            Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, startIndex, this->Count);

        if (reqCount == Math::Max<SizeT>())
            reqCount = this->count - startIndex;

        if (startIndex + reqCount > this->Count)
            reqCount = this->Count - startIndex;

        if constexpr (TypeTraits::IsInvocable<TLambda, T&> ||
                        TypeTraits::IsInvocable<TLambda, const T&>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < reqCount; i++)
            {
                lambda(array[startIndex + i]);
            }

            return *this;
        }
        else if constexpr (TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
                            TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < reqCount; i++)
            {
                lambda(array[startIndex + i], startIndex + i);
            }

            return *this;
        }
        else
        {
            static_assert(TypeTraits::IsInvocable<TLambda, T&> ||
                TypeTraits::IsInvocable<TLambda, const T&> ||
                TypeTraits::IsInvocable<TLambda, T&, SizeT> ||
                TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
            __assume(0);
        }
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    const BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::ForEachImpl(TLambda&& lambda, SizeT startIndex, SizeT reqCount) const
    {
        if (startIndex > this->Count)
            Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, startIndex, this->Count);

        if (reqCount == Math::Max<SizeT>())
            reqCount = this->count - startIndex;

        if (startIndex + reqCount > this->Count)
            reqCount = this->Count - startIndex;

        if constexpr (TypeTraits::IsInvocable<TLambda, const T&>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < reqCount; i++)
            {
                lambda(array[startIndex + i]);
            }

            return *this;
        }
        else if constexpr (TypeTraits::IsInvocable<TLambda, const T&, SizeT>)
        {
            auto _ = Threading::SharedScoppedLock(this->lock);

            for (SizeT i = 0; i < reqCount; i++)
            {
                lambda(array[startIndex + i], startIndex + i);
            }

            return *this;
        }
        else
        {
            static_assert(TypeTraits::IsInvocable<TLambda, const T&> ||
                TypeTraits::IsInvocable<TLambda, const T&, SizeT>);
            __assume(0);
        }
    }

    template<typename THeap, typename T>
    template<typename U, typename TLambda>
    U BaseConcurencyArray<THeap, T>::AggregateImpl(TLambda&& lambda, const U& Value) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        U result = Value;
        for (SizeT i = 0; i < count; i++)
        {
            if constexpr (Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
                Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
            {
                result = lambda(result, array[i]);
            }
            else
            {
                lambda(result, array[i]);
            }
        }

        return MoveRValue(result);
    }

    template<typename THeap, typename T>
    template<typename U, typename TLambda>
    U BaseConcurencyArray<THeap, T>::AggregateImpl(TLambda&& lambda, U&& result) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        for (SizeT i = 0; i < count; i++)
        {
            if constexpr (
                Concepts::IsInvocableWithResult<TLambda, U&, U&, const T&> ||
                Concepts::IsInvocableWithResult<TLambda, U, U&, const T&>)
            {
                result = lambda(result, array[i]);
            }
            else
            {
                lambda(result, array[i]);
            }
        }

        return MoveRValue(result);
    }

    template<typename THeap, typename T>
    ForceInline bool BaseConcurencyArray<THeap, T>::AnyImpl() const
    {
        return count > 0;
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    bool BaseConcurencyArray<THeap, T>::AnyImpl(TLambda&& lambda) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        for (SizeT i = 0; i < count; i++)
        {
            if (lambda(array[i]))
                return true;
        }

        return false;
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    bool BaseConcurencyArray<THeap, T>::AllImpl(TLambda&& lambda) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        for (SizeT i = 0; i < count; i++)
        {
            if (!lambda(array[i]))
                return false;
        }

        return true;
    }

    template<typename THeap, typename T>
    BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::SortImpl(bool IsDescending)
    {
        auto _ = Threading::ExclusiveScoppedLock(this->lock);

        if (!IsDescending)
        {
            auto l = [](const T& V1, const T& V2) InlineLambda{ return V1 < V2; };
            QuickSort(array, 0, count - 1, l);
        }
        else
        {
            auto l = [](const T& V1, const T& V2) InlineLambda{ return V1 > V2; };
            QuickSort(array, 0, count - 1, l);
        }

        return *this;
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::SortImpl(TLambda&& lambda)
    {
        auto _ = Threading::ExclusiveScoppedLock(this->lock);

        if (count == 0)
            return *this;

        auto l = [lambda = MoveRValue(lambda)] (const T& v1, const T& v2) { return !lambda(v1, v2); };
        QuickSort(array, 0, count - 1, l);

        return *this;
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::SortByLambdaImpl(TLambda&& lambda, bool IsDescending)
    {
        auto _ = Threading::ExclusiveScoppedLock(this->lock);

        if (count == 0)
            return *this;

        if (IsDescending)
        {
            auto l = [lambda = MoveRValue(lambda)](const T& l, const T& r) mutable InlineLambda{ return lambda(l) < lambda(r); };
            QuickSort(array, 0, count - 1, l);
        }
        else
        {
            auto l = [lambda = MoveRValue(lambda)](const T& l, const T& r) mutable InlineLambda{ return !(lambda(l) < lambda(r)); };
            QuickSort(array, 0, count - 1, l);
        }

        return *this;
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    T& BaseConcurencyArray<THeap, T>::FirstImpl(TLambda&& lambda)
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        for (SizeT i = 0; i < count; i++)
            if (lambda(array[i]))
                return static_cast<T&>(array[i]);

        Internals::Exceptions::ThrowNotFound(SourcePosition);
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    T& BaseConcurencyArray<THeap, T>::LastImpl(TLambda&& lambda)
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        SizeT Index = -1;
        for (SizeT i = 0; i < count; i++)
            if (lambda(array[i]))
                Index = i;

        if (Index != -1)
            return static_cast<T&>(array[Index]);

        Internals::Exceptions::ThrowNotFound(SourcePosition);
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    const T& BaseConcurencyArray<THeap, T>::FirstImpl(TLambda&& lambda) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        for (SizeT i = 0; i < count; i++)
            if (lambda(array[i]))
                return static_cast<T&>(array[i]);

        Internals::Exceptions::ThrowNotFound(SourcePosition);
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    const T& BaseConcurencyArray<THeap, T>::LastImpl(TLambda&& lambda) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        SizeT Index = -1;
        for (SizeT i = 0; i < count; i++)
            if (lambda(array[i]))
                Index = i;

        if (Index != -1)
            return static_cast<T&>(array[Index]);

        Internals::Exceptions::ThrowNotFound(SourcePosition);
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    T BaseConcurencyArray<THeap, T>::FirstOrImpl(TLambda&& lambda, T&& item) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        if (count == 0)
            return MoveRValue(item);

        for (SizeT i = 0; i < count; i++)
            if (lambda(array[i]))
                return array[i];

        return MoveRValue(item);
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    T BaseConcurencyArray<THeap, T>::LastOrImpl(TLambda&& lambda, T&& item) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        if (count == 0)
            return MoveRValue(item);

        SizeT Index = -1;
        for (SizeT i = 0; i < count; i++)
            if (lambda(array[i]))
                Index = i;

        if (Index != -1)
            return array[Index];

        return MoveRValue(item);
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    T BaseConcurencyArray<THeap, T>::FirstOrImpl(TLambda&& lambda, const T& item) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        if (count == 0)
            return item;

        for (SizeT i = 0; i < count; i++)
            if (lambda(array[i]))
                return array[i];

        return item;
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    T BaseConcurencyArray<THeap, T>::LastOrImpl(TLambda&& lambda, const T& item) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        if (count == 0)
            return item;

        SizeT Index = -1;
        for (SizeT i = 0; i < count; i++)
            if (lambda(array[i]))
                Index = i;

        if (Index != -1)
            return array[Index];

        return item;
    }

    template<typename THeap, typename T>
    template<typename TLambda, typename TGetterLambda>
    T BaseConcurencyArray<THeap, T>::FirstOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        if (count == 0)
            return otherLambda();

        for (SizeT i = 0; i < count; i++)
            if (lambda(array[i]))
                return array[i];

        return otherLambda();
    }

    template<typename THeap, typename T>
    template<typename TLambda, typename TGetterLambda>
    T BaseConcurencyArray<THeap, T>::LastOrImpl(TLambda&& lambda, TGetterLambda&& otherLambda) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        if (count == 0)
            return otherLambda();

        SizeT Index = -1;
        for (SizeT i = 0; i < count; i++)
            if (lambda(array[i]))
                Index = i;

        if (Index != -1)
            return array[Index];

        return otherLambda();
    }

    template<typename THeap, typename T>
    BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::ReverseImpl()&
    {
        auto _ = Threading::ExclusiveScoppedLock(this->lock);

        for (SizeT i = 0; i < count / 2; i++)
        {
            Seed::Core::Swap<T>(array[i], array[(count - 1) - i]);
        }

        return *this;
    }

    template<typename THeap, typename T>
    BaseConcurencyArray<THeap, T> BaseConcurencyArray<THeap, T>::ReverseImpl()&&
    {
        {
            auto _ = Threading::ExclusiveScoppedLock(this->lock);

            for (SizeT i = 0; i < count / 2; i++)
            {
                Seed::Core::Swap<T>(array[i], array[(count - 1) - i]);
            }
        }

        return MoveRValue(*this);
    }

    template<typename THeap, typename T>
    ForceInline void BaseConcurencyArray<THeap, T>::UnsafeSet(const T& Value, SizeT Index)
    {
        array[Index] = Value;
    }

    template<typename THeap, typename T>
    ForceInline void BaseConcurencyArray<THeap, T>::UnsafeSet(T&& Value, SizeT Index)
    {
        array[Index] = MoveRValue(Value);
    }

    template<typename THeap, typename T>
    ForceInline BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::RemoveAllImpl()
    {
        auto _ = Threading::ExclusiveScoppedLock(this->lock);

        delete[] this->array;
        this->array = nullptr;
        this->count = 0;

        return *this;
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::RemoveAllImpl(TLambda&& lambda)
    {
        auto _ = Threading::ExclusiveScoppedLock(this->lock);

        if constexpr (Concepts::IsInvocable<TLambda, T&&>)
        {
            for (SizeT i = 0; i < count; i++)
            {
                lambda(MoveRValue(array[i]));
            }
        }
        else
        {
            for (SizeT i = 0; i < count; i++)
            {
                lambda(MoveRValue(array[i]), i);
            }
        }

        delete[] this->array;
        this->array = nullptr;
        this->count = 0;

        return *this;
    }

    template<typename THeap, typename T>
    template<typename TLambda>
    SizeT BaseConcurencyArray<THeap, T>::GetCountIfImpl(TLambda&& lambda) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        SizeT selectedCount = 0;

        for (SizeT i = 0; i < count; i++)
        {
            if (lambda(array[i]))
                selectedCount++;
        }

        return selectedCount;
    }

    template<typename THeap, typename T>
    template<typename TLambda> requires (Concepts::IsInvocable<TLambda, const T&>)
        SizeT BaseConcurencyArray<THeap, T>::IndexOfImpl(TLambda&& lambda) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        for (SizeT i = 0; i < this->count; i++)
        {
            if (lambda(this->array[i]))
                return i;
        }

        return Math::Max<SizeT>();
    }

    template<typename THeap, typename T>
    BaseConcurencyArray<THeap, T>::BaseConcurencyArray(InitilizerList<T>&& list): count(0), array(nullptr), lock()
    {
        if (list.size() != 0)
        {
            count = list.size();
            this->array = new T[count];
            auto it = list.begin();
            for (SizeT i = 0; i < count; i++)
            {
                this->array[i] = MoveRValue(*it); ++it;
            }
        }
    }

    template<typename THeap, typename T>
    BaseConcurencyArray<THeap, T>::BaseConcurencyArray(const BaseConcurencyArray& array): count(0), array(nullptr), lock()
    {
        auto _ = Threading::SharedScoppedLock(array.lock);

        this->array = new T[array.count];
        this->count = array.count;
        if constexpr (TypeTraits::IsStandardLayout<T>)
        {
            Memory::Copy(array.array, this->array, sizeof(T) * this->count);
        }
        else
        {
            for (SizeT i = 0; i < this->count; i++)
            {
                this->array[i] = array.array[i];
            }
        }
    }

    template<typename THeap, typename T>
    BaseConcurencyArray<THeap, T>::BaseConcurencyArray(BaseConcurencyArray&& array): count(0), array(nullptr), lock()
    {
        auto _ = Threading::ExclusiveScoppedLock(array.lock);

        this->count = array.count;
        array.count = 0;

        this->array = array.array;
        array.array = nullptr;
    }

    template<typename THeap, typename T>
    ForceInline BaseConcurencyArray<THeap, T>::BaseConcurencyArray(SizeT startCount): count(0), array(nullptr), lock()
    {
        if (startCount != 0)
        {
            array = new T[startCount];
            count = startCount;
        }
    }

    template<typename THeap, typename T>
    InternalApiCall BaseConcurencyArray<THeap, T>::BaseConcurencyArray(const T*& array, SizeT startCount): count(0), array(nullptr), lock()
    {
        if (startCount != 0)
        {
            this->array = new T[startCount];
            count = startCount;
            if constexpr (TypeTraits::IsStandardLayout<T>)
            {
                Memory::Copy(array, this->array, startCount * sizeof(T));
            }
            else
            {
                for (SizeT i = 0; i < startCount; i++)
                {
                    this->array[i] = array[i];
                }
            }
        }
    }

    template<typename THeap, typename T>
    template<SizeT TN>
    BaseConcurencyArray<THeap, T>::BaseConcurencyArray(const T(&array)[TN]):
        count(TN),
        array(nullptr)
    {
        this->array = new T[TN];

        if constexpr (TypeTraits::IsStandardLayout<T>)
        {
            Memory::Copy(array, this->array, TN * sizeof(T));
        }
        else
        {
            for (SizeT i = 0; i < TN; i++)
            {
                this->array[i] = array[i];
            }
        }
    }

    template<typename THeap, typename T>
    BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::operator=(const BaseConcurencyArray& original)
    {
        this->~BaseConcurencyArray();
        this->BaseConcurencyArray::BaseConcurencyArray(original);
        return *this;
    }

    template<typename THeap, typename T>
    BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::operator=(BaseConcurencyArray&& original)
    {
        this->~BaseConcurencyArray();
        this->BaseConcurencyArray::BaseConcurencyArray(MoveRValue(original));
        return *this;
    }

    template<typename THeap, typename T>
    ForceInline InternalApiCall BaseConcurencyArray<THeap, T>::operator const T* const() const
    {
        return array;
    }

    template<typename THeap, typename T>
    ForceInline T& BaseConcurencyArray<THeap, T>::operator[](SizeT index)&
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        if (index < count)
        {
            return array[index];
        }
        else
        {
            Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, index, count);
        }
    }

    template<typename THeap, typename T>
    ForceInline const T& BaseConcurencyArray<THeap, T>::operator[](SizeT index) const&
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        if (index < count)
        {
            return array[index];
        }
        else
        {
            Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, index, count);
        }
    }

    template<typename THeap, typename T>
    ForceInline T BaseConcurencyArray<THeap, T>::operator[](SizeT index)&&
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        if (index < count)
        {
            return MoveRValue(array[index]);
        }
        else
        {
            Internals::Exceptions::ThrowIndexOutOfRange(SourcePosition, index, count);
        }
    }

    template<typename THeap, typename T>
    BaseConcurencyArray<THeap, T>& BaseConcurencyArray<THeap, T>::Resize(SizeT newSize)
    {
        auto _ = Threading::ExclusiveScoppedLock(this->lock);

        if (array == nullptr || count == 0)
        {
            array = new T[newSize];
            count = newSize;

            return *this;
        }

        if (newSize == 0)
        {
            delete[] array;
            count = 0;

            return *this;
        }

        T* buf = new T[newSize];
        const SizeT copySize = (newSize > count) ? count : newSize;

        if constexpr (TypeTraits::IsStandardLayout<T>)
        {
            Memory::Copy(array, buf, copySize * sizeof(T));
        }
        else
        {
            for (SizeT i = 0; i < copySize; i++)
            {
                buf[i] = MoveRValue(array[i]);
            }
        }

        count = newSize;
        delete[] array;
        array = buf;

        return *this;

    }

    template<typename THeap, typename T>
    template<typename> requires (Concepts::IsClonable<T>)
        BaseConcurencyArray<THeap, T> BaseConcurencyArray<THeap, T>::Clone() const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        if constexpr (TypeTraits::IsStandardLayout<T>)
        {
            return BaseConcurencyArray((const T*&) (array), count);
        }
        else
        {
            T* array = nullptr;

            try
            {
                array = new T[count];

                for (SizeT i = 0; i < count; ++i)
                {
                    array[i] = Core::Clone(this->array[i]);
                }

                return BaseConcurencyArray(MoveRValue(array), count);
            }
            catch (...)
            {
                delete[] array;
                Internals::Exceptions::__ReThrow();
            }
        }
    }

    template<typename THeap, typename T>
    template<typename TLambda> requires (Concepts::IsInvocableWithResult<TLambda, T, const T&>)
        BaseConcurencyArray<THeap, T> BaseConcurencyArray<THeap, T>::Clone(TLambda&& lambda) const
    {
        auto _ = Threading::SharedScoppedLock(this->lock);

        T* newArray = nullptr;
        try
        {
            newArray = new T[count];
            for (auto i = 0ui64; i < count; i++)
            {
                newArray[i] = lambda(array[i]);
            }

            return BaseConcurencyArray(MoveRValue(newArray), count);
        }
        catch (...)
        {
            delete[] newArray;
            Internals::Exceptions::__ReThrow();
        }
    }

    template<typename THeap, typename T>
    ForceInline SizeT BaseConcurencyArray<THeap, T>::GetCount() const
    {
        return count;
    }

    template<typename THeap, typename T>
    ForceInline const T* const BaseConcurencyArray<THeap, T>::GetRaw() const
    {
        return array;
    }

    template<typename THeap, typename T>
    bool BaseConcurencyArray<THeap, T>::operator==(const BaseConcurencyArray& other) const requires Core::Concepts::HaveEqual<T, T>
    {
        if (other.array == this->array)
            return true;

        if (other.count != this->count)
            return false;

        auto _ = Threading::SharedScoppedLock(this->lock);

        {
            auto _ = Threading::SharedScoppedLock(other.lock);

            for (SizeT i = 0; i < this->count; i++)
            {
                if (!(other.array[i] == this->array[i]))
                    return false;
            }

            return true;
        }
    }

    template<typename THeap, typename T>
    bool BaseConcurencyArray<THeap, T>::operator!=(const BaseConcurencyArray& other) const requires Core::Concepts::HaveEqual<T, T>
    {
        return !(*this == other);
    }

    template<typename THeap, typename T>
    BaseConcurencyArray<THeap, T>::~BaseConcurencyArray()
    {
        delete[] array;
        array = nullptr;
    }
}

#pragma warning(pop)
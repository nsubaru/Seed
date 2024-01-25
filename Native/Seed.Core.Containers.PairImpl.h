#pragma once
#include "Seed.Core.Containers.Pair.h"

namespace Seed::Core::Containers
{
    template<typename THeap, typename TKey, typename TValue>
    ForceInline BasePair<THeap, TKey, TValue>& BasePair<THeap, TKey, TValue>::operator=(const BasePair& other)
    {
        this->~BasePair();
        this->BasePair::BasePair(other);
        return  *this;
    }

    template<typename THeap, typename TKey, typename TValue>
    ForceInline BasePair<THeap, TKey, TValue>& BasePair<THeap, TKey, TValue>::operator=(BasePair&& other)
    {
        this->~BasePair();
        this->BasePair::BasePair(MoveRValue(other));
        return *this;
    }

    template<typename THeap, typename TKey, typename TValue>
    template<typename UHeap>
    ForceInline bool BasePair<THeap, TKey, TValue>::operator==(const BasePair<UHeap, TKey, TValue>& other) const
    {
        return this->key == other.key && this->value == other.value;
    }

    template<typename THeap, typename TKey, typename TValue>
    template<typename UHeap>
    ForceInline bool BasePair<THeap, TKey, TValue>::operator!=(const BasePair<UHeap, TKey, TValue>& other) const
    {
        return this->key != other.key || this->value != other.value;
    }

    template<typename THeap, typename TKey, typename TValue>
    ForceInline const TKey& BasePair<THeap, TKey, TValue>::GetKey() const&
    {
        return key;
    }

    template<typename THeap, typename TKey, typename TValue>
    ForceInline TKey BasePair<THeap, TKey, TValue>::GetKey()&&
    {
        return MoveRValue(key);
    } //-V659

    template<typename THeap, typename TKey, typename TValue>
    ForceInline TValue& BasePair<THeap, TKey, TValue>::GetValue()&
    {
        return value;
    }

    template<typename THeap, typename TKey, typename TValue>
    ForceInline const TValue& BasePair<THeap, TKey, TValue>::GetValue() const&
    {
        return value;
    }

    template<typename THeap, typename TKey, typename TValue>
    ForceInline TValue BasePair<THeap, TKey, TValue>::GetValue()&&
    {
        return MoveRValue(value);
    } //-V659

    template<typename THeap, typename TKey, typename TValue>
    ForceInline void BasePair<THeap, TKey, TValue>::SetValue(const TValue& newValue)
    {
        value = newValue;
    }

    template<typename THeap, typename TKey, typename TValue>
    ForceInline void BasePair<THeap, TKey, TValue>::SetValue(TValue&& newValue)
    {
        value = MoveRValue(newValue);
    }

    template<typename THeap, typename TKey, typename TValue>
    template<typename> requires Concepts::IsClonable<TKey>&& Concepts::IsClonable<TValue>
    ForceInline BasePair<THeap, TKey, TValue> BasePair<THeap, TKey, TValue>::Clone() const
    {
        return BasePair(Core::Clone(this->key), Core::Clone(this->value));
    }

    template<typename THeap, typename TKey, typename TValue>
    template<typename TLambda> requires Concepts::IsInvocableWithResult<TLambda, BasePair<THeap, TKey, TValue>, const BasePair<THeap, TKey, TValue>&>
    ForceInline BasePair<THeap, TKey, TValue> BasePair<THeap, TKey, TValue>::Clone(TLambda&& lambda) const
    {
        return BasePair(lambda(*this));
    }
}
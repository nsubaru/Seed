#pragma once
#include "Seed.Core.Containers.Pair.h"

namespace Seed::Core::Containers
{
    template<typename THeap, typename TKey, typename TValue>
    inline BasePair<THeap, TKey, TValue>& BasePair<THeap, TKey, TValue>::operator=(const BasePair& other)
    {
        this->~BasePair();
        this->BasePair::BasePair(other);
        return  *this;
    }

    template<typename THeap, typename TKey, typename TValue>
    inline BasePair<THeap, TKey, TValue>& BasePair<THeap, TKey, TValue>::operator=(BasePair&& other)
    {
        this->~BasePair();
        this->BasePair::BasePair(MoveRValue(other));
        return *this;
    }

    template<typename THeap, typename TKey, typename TValue>
    template<typename UHeap>
    inline bool BasePair<THeap, TKey, TValue>::operator==(const BasePair<UHeap, TKey, TValue>& other) const
    {
        return this->key == other.key && this->value == other.value;
    }

    template<typename THeap, typename TKey, typename TValue>
    template<typename UHeap>
    inline bool BasePair<THeap, TKey, TValue>::operator!=(const BasePair<UHeap, TKey, TValue>& other) const
    {
        return this->key != other.key || this->value != other.value;
    }

    template<typename THeap, typename TKey, typename TValue>
    inline const TKey& BasePair<THeap, TKey, TValue>::GetKey() const&
    {
        return key;
    }

    template<typename THeap, typename TKey, typename TValue>
    inline TKey BasePair<THeap, TKey, TValue>::GetKey()&&
    {
        return MoveRValue(key);
    } //-V659

    template<typename THeap, typename TKey, typename TValue>
    inline TValue& BasePair<THeap, TKey, TValue>::GetValue()&
    {
        return value;
    }

    template<typename THeap, typename TKey, typename TValue>
    inline const TValue& BasePair<THeap, TKey, TValue>::GetValue() const&
    {
        return value;
    }

    template<typename THeap, typename TKey, typename TValue>
    inline TValue BasePair<THeap, TKey, TValue>::GetValue()&&
    {
        return MoveRValue(value);
    } //-V659

    template<typename THeap, typename TKey, typename TValue>
    inline void BasePair<THeap, TKey, TValue>::SetValue(const TValue& newValue)
    {
        value = newValue;
    }

    template<typename THeap, typename TKey, typename TValue>
    inline void BasePair<THeap, TKey, TValue>::SetValue(TValue&& newValue)
    {
        value = MoveRValue(newValue);
    }
}
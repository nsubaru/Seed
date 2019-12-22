#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Memory.h"

namespace Seed {
    inline namespace Core {
        namespace Containers {
            template<typename THeap, typename TKey, typename TValue>
            class BasePair : 
                public Memory::Allocator<THeap>, 
                public LINQ::TPairLinq<BasePair<THeap, TKey, TValue>, THeap, TKey, TValue, false> 
            {
            public:
                BasePair() : key(), value() {}

                BasePair(const TKey& key, const TValue& value) : key(key), value(value) {}
                BasePair(TKey&& key, const TValue& value) : key(MoveRValue(key)), value(value) {}
                BasePair(const TKey& key, TValue&& value) : key(key), value(MoveRValue(value)) {}
                BasePair(TKey&& key, TValue&& value) : key(MoveRValue(key)), value(MoveRValue(value)) {}

                BasePair(const BasePair& other) : key(other.key), value(other.value) {}
                BasePair(BasePair&& other) : key(MoveRValue(other.key)), value(MoveRValue(other.value)) {}

                BasePair& operator=(const BasePair& other) {
                    this->~BasePair();
                    this->BasePair::BasePair(other);
                    return  *this;
                }

                BasePair& operator=(BasePair&& other) {
                    this->~BasePair();
                    this->BasePair::BasePair(MoveRValue(other));
                    return *this;
                }

            	bool operator==(const BasePair & other) const
                {
	                return this->key == other.key && this->value == other.value;
                }

            	bool operator!=(const BasePair & other) const
                {
	                return this->key != other.key || this->value != other.value;
                }

                const TKey& GetKey() const { return key; }

                TValue& GetValue() { return value; }
                const TValue& GetValue() const { return value; }

                void SetValue(const TValue& newValue) { value = newValue; }
                void SetValue(TValue&& newValue) { value = MoveRValue(newValue); }

                template<typename T>
                friend void Seed::Core::Swap(T& a, T& b);
            public:
                TKey Property(GetKey) Key;
                TValue Property(GetValue, SetValue) Value;
            private:
                TKey key;
                TValue value;
            private:
                template<typename TOtherHeap, typename T, typename TEnable>
                friend struct Seed::Core::NewSerilization::Details::BaseSerlizationHandler;
            };

            namespace {
                template<typename TKey, typename TValue>
                using Pair = BasePair<Memory::Heap, TKey, TValue>;
            }

            namespace {
                template<typename TKey, typename TValue>
                using SensitivePair = BasePair<Memory::SensitiveDataHeap, TKey, TValue>;
            }
        }

        namespace TypeTraits {
            namespace Collection {
                template<typename THeap, typename TKey, typename TValue>
                struct IsPairT<Seed::Core::Containers::BasePair<THeap, TKey, TValue>> : LogicTrue {
                    using Heap = THeap;
                    using Key = TKey;
                    using Value = TValue;
                };
            }
        }


        template<typename THeap, typename TKey, typename TValue>
        void Swap(Containers::BasePair<THeap, TKey, TValue>& a, Containers::BasePair<THeap, TKey, TValue>& b) {
            Swap(a.key, b.key);
            Swap(a.value, b.value);
        }
    }
}
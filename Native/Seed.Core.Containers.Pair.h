#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Memory.h"

namespace Seed {
    inline namespace Core {
        namespace Containers {
            /// <summary>
            /// Об'єкт пари ключа і значення
            /// </summary>
            template<typename THeap, typename TKey, typename TValue>
            class BasePair : 
                public Memory::Allocator<THeap>, 
                public LINQ::TPairLinq<BasePair<THeap, TKey, TValue>, THeap, TKey, TValue, false> 
            {
            public:
                /// <summary>
                /// Пустий конструктор
                /// </summary>
                forceinline BasePair() : key(), value() {}
                
                /// <summary>
                /// Конструктор з ключа і значення
                /// </summary>
                /// <param name="key">Ключ</param>
                /// <param name="value">Значення</param>
                forceinline BasePair(const TKey& key, const TValue& value) : key(key), value(value) {}

                /// <summary>
                /// Конструктор з ключа і значення
                /// </summary>
                /// <param name="key">Ключ</param>
                /// <param name="value">Значення</param>
                forceinline BasePair(TKey&& key, const TValue& value) : key(MoveRValue(key)), value(value) {}

                /// <summary>
                /// Конструктор з ключа і значення
                /// </summary>
                /// <param name="key">Ключ</param>
                /// <param name="value">Значення</param>
                forceinline BasePair(const TKey& key, TValue&& value) : key(key), value(MoveRValue(value)) {}

                /// <summary>
                /// Конструктор з ключа і значення
                /// </summary>
                /// <param name="key">Ключ</param>
                /// <param name="value">Значення</param>
                forceinline BasePair(TKey&& key, TValue&& value) : key(MoveRValue(key)), value(MoveRValue(value)) {}

                /// <summary>
                /// Конструктор копіювання
                /// </summary>
                /// <param name="other">Оригинал</param>
                forceinline BasePair(const BasePair& other) : key(other.key), value(other.value) {}

                /// <summary>
                /// Конструктор переміщення
                /// </summary>
                /// <param name="other">Оригинал</param>
                forceinline BasePair(BasePair&& other) : key(MoveRValue(other.key)), value(MoveRValue(other.value)) {}

                /// <summary>
                /// Оператор копіювання
                /// </summary>
                /// <param name="other">Оригинал</param>
                /// <returns>Силку на даний об'єкт</returns>
                forceinline BasePair& operator=(const BasePair& other) {
                    this->~BasePair();
                    this->BasePair::BasePair(other);
                    return  *this;
                }

                /// <summary>
                /// Оператор переміщення
                /// </summary>
                /// <param name="other">Оригинал</param>
                /// <returns>Силку на даний об'єкт</returns>
                forceinline BasePair& operator=(BasePair&& other) {
                    this->~BasePair();
                    this->BasePair::BasePair(MoveRValue(other));
                    return *this;
                }
                
                /// <summary>
                /// Оператор порівння. вертає true якщо і ключ і значення рівні
                /// </summary>
                /// <param name="other">Лівий операнд</param>
                /// <returns>Вертає true якщо і ключ і значення рівні</returns>
            	forceinline bool operator==(const BasePair & other) const
                {
	                return this->key == other.key && this->value == other.value;
                }

                /// <summary>
                /// Оператор не рівності, вертає true якщо або ключ або значення не рівні
                /// </summary>
                /// <param name="other">Лівий операнд</param>
                /// <returns>Вертає true якщо або ключ або значення не рівні</returns>
            	forceinline bool operator!=(const BasePair & other) const
                {
	                return this->key != other.key || this->value != other.value;
                }

                /// <summary>
                /// Вертає ключ
                /// </summary>
                /// <returns>Ключ</returns>
                forceinline const TKey& GetKey() const { return key; }

                /// <summary>
                /// Вертає значення
                /// </summary>
                /// <returns>Значення</returns>
                forceinline TValue& GetValue() { return value; }

                /// <summary>
                /// Вертає значення
                /// </summary>
                /// <returns>Значення</returns>
                forceinline const TValue& GetValue() const { return value; }

                /// <summary>
                /// Встановлює нове значення
                /// </summary>
                /// <param name="newValue">Нове значення</param>
                forceinline void SetValue(const TValue& newValue) { value = newValue; }

                /// <summary>
                /// Встановлює нове значення
                /// </summary>
                /// <param name="newValue">Нове значення</param>
                forceinline void SetValue(TValue&& newValue) { value = MoveRValue(newValue); }

                template<typename T>
                friend void Seed::Core::Swap(T& a, T& b);
            public:
                /// <summary>
                /// Ключ
                /// </summary>
                TKey Property(GetKey) Key;
                
                /// <summary>
                /// Значення
                /// </summary>
                TValue Property(GetValue, SetValue) Value;
            public:
                /// <summary>
                /// Клонує об'єкт якщо він підтримує клонування
                /// </summary>
                /// <returns>Клонований об'єкт</returns>
                template<typename = void> requires Concepts::IsClonable<TKey> && Concepts::IsClonable<TValue>
                forceinline BasePair Clone() const {
                    if constexpr (TypeTraits::IsPodV<TKey> && TypeTraits::IsPodV<TValue>)
                    {
                        return BasePair(this->key, this->value);
                    }
                    else if constexpr (TypeTraits::IsPodV<TKey>)
                    {
                        return BasePair(this->key.Clone(), this->value);
                    }
                    else if constexpr (TypeTraits::IsPodV<TValue>)
                    {
                        return BasePair(this->key, this->value.Clone());
                    }
                    else
                    {
                        return BasePair(this->key.Clone(), this->value.Clone());
                    }
                }

                /// <summary>
                /// Клонує об'єкт з використанням лямбди lambda яка створює інстанс об'єкта
                /// </summary>
                /// <param name="lambda">Функція, лямбда або функтор який створює інстанс об'єкта на основі існуючого</param>
                /// <returns>Клонований об'єкт</returns>
                template<typename TLambda> requires Concepts::IsInvockableWithResult<TLambda, BasePair, const BasePair&> 
                forceinline BasePair Clone(TLambda&& lambda) const {
                    return BasePair(lambda(*this));
                }
            private:
                TKey key;
                TValue value;
            private:
                template<typename TOtherHeap, typename T, typename TEnable>
                friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
            };

            /// <summary>
            /// Об'єкт пари ключа і значення в простій кучі
            /// </summary>
            namespace {
                template<typename TKey, typename TValue>
                using Pair = BasePair<Memory::Heap, TKey, TValue>;
            }

            /// <summary>
            /// Об'єкт пари ключа і значення в захищеній кучі
            /// </summary>
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
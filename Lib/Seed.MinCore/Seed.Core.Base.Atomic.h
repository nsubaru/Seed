#pragma once
#include "Seed.Core.Base.Definitions.h"
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Swap.h"
#include "Seed.Core.Base.Interlocked.h"
#include "Seed.Core.Base.EnumFlags.h"
#include "Seed.Core.Base.Comparing.h"

namespace Seed::Core::Concepts
{
    /// <summary>
    /// Valida atomic types
    /// </summary>
    template<typename T>
    concept AtomicType = 
        Concepts::IsInteger<T> || 
        Concepts::IsEnum<T> || 
        TypeTraits::IsEnumFlagsV<T> || 
        TypeTraits::IsBoolean<T>;
}

namespace Seed::Core 
{
    template<Concepts::AtomicType T>
    struct Atomic;

    /// <summary>
    /// Read only atomic primitive with interlocked read operation
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template<Concepts::AtomicType T>
    struct ReadOnlyAtomic final {
    private:
        alignas(8) mutable volatile T value;

    public:
        /// <summary>
        /// Empty ctor, create RO atomic with default value
        /// </summary>
        constexpr ReadOnlyAtomic() : value{} {}

        /// <summary>
        /// Copy ctor from RW atomic
        /// </summary>
        /// <param name="src">Src object value will be copied</param>
        constexpr ReadOnlyAtomic(const Atomic<T>& src);

        /// <summary>
        /// Move ctor from RW atomic
        /// </summary>
        /// <param name="src">Src object value will be copied</param>
        constexpr ReadOnlyAtomic(Atomic<T>&& src);

        /// <summary>
        /// Copy ctor from RO atomic
        /// </summary>
        /// <param name="src">Src object value will be copied</param>
        constexpr ReadOnlyAtomic(const ReadOnlyAtomic& src) : value() {
            Interlocked::Store(value, Interlocked::Load(src.value));
        }

        /// <summary>
        /// Move ctor from RO atomic
        /// </summary>
        /// <param name="src">Src object value will be copied</param>
        constexpr ReadOnlyAtomic(ReadOnlyAtomic&& src) : value() {
            Interlocked::Store(value, Interlocked::Load(src.value));
        }

    public:
        /// <summary>
        /// Copy operator for ReadOnlyAtomic
        /// </summary>
        /// <param name="src">Source object</param>
        /// <returns>Reference to current object</returns>
        constexpr ReadOnlyAtomic& operator=(const ReadOnlyAtomic& src) {
            this->~ReadOnlyAtomic();
            this->ReadOnlyAtomic::ReadOnlyAtomic(src);
            return *this;
        }

        /// <summary>
        /// Move operator for ReadOnlyAtomic
        /// </summary>
        /// <param name="src">Source object</param>
        /// <returns>Reference to current object</returns>
        constexpr ReadOnlyAtomic& operator=(ReadOnlyAtomic&& src) {
            this->~ReadOnlyAtomic();
            this->ReadOnlyAtomic::ReadOnlyAtomic(MoveRValue(src));
            return *this;
        }

    private:
        /// <summary>
        /// Ctor from value
        /// </summary>
        /// <param name="value">Raw value</param>
        constexpr ReadOnlyAtomic(const T& value) : value(value) {}

        /// <summary>
        /// Move ctor from value
        /// </summary>
        /// <param name="value">Raw value</param>
        constexpr ReadOnlyAtomic(T&& value) : value(MoveRValue(value)) {}

    public:
        /// <summary>
        /// Atomically read value
        /// </summary>
        /// <returns>Current value</returns>
        constexpr T Read() const {
            if (TypeTraits::IsCompileTimeEvaluating()) 
            {
                return const_cast<T&>(value);
            }
            else 
            {
                return Interlocked::Load(value);
            }
        }

    public:
        /// <summary>
        /// Value
        /// </summary>
        T Property(Read) Value;

    public:
        /// <summary>
        /// Dtor
        /// </summary>
        constexpr ~ReadOnlyAtomic() { value.~T(); }

    public:
        template<Concepts::AtomicType U>
        friend struct Atomic;

        template<typename U>
        friend U CompareAndSwap(Atomic<U>& destination, U exchange, U comperand);
    };

    /// <summary>
    /// Atomic read write value
    /// </summary>
    /// <typeparam name="T">Target type</typeparam>
    template<Concepts::AtomicType T>
    struct Atomic final {
    private:
        ReadOnlyAtomic<T> value;

    public:
        /// <summary>
        /// Default ctor, initialize value with default fot its type
        /// </summary>
        constexpr Atomic() : value() {}

        /// <summary>
        /// Ctor from value
        /// </summary>
        /// <param name="value">Raw value</param>
        constexpr Atomic(const T& value) : value(value) {}

        /// <summary>
        /// Ctor from value
        /// </summary>
        /// <param name="value">Raw value</param>
        constexpr Atomic(T&& value) : value(MoveRValue(value)) {}

    public:
        /// <summary>
        /// Copy ctor
        /// </summary>
        /// <param name="value">Original source</param>
        constexpr Atomic(const Atomic& src) : value(src.value) {}

        /// <summary>
        /// Move ctor
        /// </summary>
        /// <param name="value">Original source</param>
        constexpr Atomic(Atomic&& src) : value(MoveRValue(src.value)) {}

    public:
        /// <summary>
        /// Copy operator for Atomic
        /// </summary>
        /// <param name="src">Source object</param>
        /// <returns>Reference to current object</returns>
        constexpr Atomic& operator=(const Atomic& src) {
            this->~Atomic();
            this->Atomic::Atomic(src);
            return *this;
        }

        /// <summary>
        /// Move operator for Atomic
        /// </summary>
        /// <param name="src">Source object</param>
        /// <returns>Reference to current object</returns>
        constexpr Atomic& operator=(Atomic&& src) {
            this->~Atomic();
            this->Atomic::Atomic(MoveRValue(src));
            return *this;
        }

    public:
        /// <summary>
        /// Store new value in Atomic
        /// </summary>
        /// <param name="val">Source value</param>
        /// <returns>Reference to current object</returns>
        constexpr Atomic& operator=(const T& val) {
            Store(val);
            return *this;
        }

        /// <summary>
        /// Store new value in Atomic
        /// </summary>
        /// <param name="val">Source value</param>
        /// <returns>Reference to current object</returns>
        constexpr Atomic& operator=(T&& val) {
            Store(MoveRValue(val));
            return *this;
        }

    public:
        /// <summary>
        /// Universal comparing with other Atomic
        /// </summary>
        /// <param name="left">Left operand</param>
        /// <returns>Comparing result</returns>
        constexpr auto operator<=>(const Atomic& left) const {
            return this->Read() <=> left.Read();
        }

        /// <summary>
        /// Equality operator
        /// </summary>
        /// <param name="left">Left operand</param>
        /// <returns>Equality result</returns>
        constexpr bool operator==(const Atomic& left) const {
            return this->Read() == left.Read();
        }

        /// <summary>
        /// Universal comparing with value
        /// </summary>
        /// <param name="left">Left operand</param>
        /// <returns>Comparing result</returns>
        constexpr auto operator<=>(const T& left) const {
            return this->Read() <=> left;
        }

        /// <summary>
        /// Equality operator
        /// </summary>
        /// <param name="left">Left operand</param>
        /// <returns>Equality result</returns>
        constexpr bool operator==(const T& left) const {
            return this->Read() == left;
        }

        /// <summary>
        /// Universal comparing with value
        /// </summary>
        /// <param name="right">Left operand</param>
        /// <param name="left">Right operand</param>
        /// <returns>Comparing result</returns>
        constexpr friend auto operator<=>(const T& right, const Atomic<T> left) {
            return right <=> left.Read();
        }

        /// <summary>
        /// Equality operator
        /// </summary>
        /// <param name="right">Left operand</param>
        /// <param name="left">Right operand</param>
        /// <returns>Equality results</returns>
        constexpr friend bool operator==(const T& right, const Atomic<T> left) {
            return right == left.Read();
        }

    public:
        /// <summary>
        /// Atomically read current value
        /// </summary>
        /// <returns>Current value</returns>
        constexpr T Read() const {
            return value.Read();
        }

        /// <summary>
        /// Atomically store current value
        /// </summary>
        /// <param name="newValue">New value</param>
        constexpr void Store(T newValue) {
            if (TypeTraits::IsCompileTimeEvaluating()) 
            {
                value.value = newValue;
            }
            else 
            {
                Interlocked::Store(value.value, newValue);
            }
        }

    public:
        /// <summary>
        /// Current value
        /// </summary>
        T Property(Read, Store) Value;

    public:
        /// <summary>
        /// Atomically increment value
        /// </summary>
        /// <returns>Reference to current object</returns>
        constexpr Atomic& operator++() requires Concepts::IsInteger<T> {
            if (TypeTraits::IsCompileTimeEvaluating())
            {
                ++this->value.value;
            }
            else 
            {
                Interlocked::Increment(this->value.value);
            }

            return *this;
        }

        /// <summary>
        /// Atomically decrement value
        /// </summary>
        /// <returns>Reference to current object</returns>
        constexpr Atomic& operator--() requires Concepts::IsInteger<T> {
            if (TypeTraits::IsCompileTimeEvaluating())
            {
                --this->value.value;
            }
            else
            {
                Interlocked::Decrement(this->value.value);
            }

            return *this;
        }

    public:
        /// <summary>
        /// Converting to read only atomic (returns reference to current object)
        /// </summary>
        /// <returns>Reference to current object value as readonly</returns>
        constexpr ReadOnlyAtomic<T>& AsReadOnly() { return this->value; }

    public:
        constexpr ~Atomic() {}

    public:
        template<Concepts::AtomicType U>
        friend struct ReadOnlyAtomic;

    public:
        template<typename U>
        friend constexpr void Swap(U& l, U& r);

        template<typename U>
        friend U CompareAndSwap(Atomic<U>& destination, U exchange, U comperand);
    };

    /// <summary>
    /// Compare with comperand and if equal swap with exchange and return previous value
    /// </summary>
    /// <typeparam name="T">Target atomic value type</typeparam>
    /// <param name="destination">Target atomic</param>
    /// <param name="exchange">New value which will be set if value equal with comperand</param>
    /// <param name="comperand">Comparable value</param>
    /// <returns>Previous value</returns>
    template<typename T>
    T CompareAndSwap(Atomic<T>& destination, T exchange, T comperand)
    {
        return Interlocked::CompareAndSwap(destination.value.value, exchange, comperand);
    }

    /// <summary>
    /// Atomically swap atomics values
    /// </summary> 
    /// <typeparam name="T">Atomic target type value</typeparam>
    /// <param name="l">Left operand</param>
    /// <param name="r">Right operand</param>
    template<Concepts::AtomicType T>
    constexpr void Swap(Atomic<T>& l, Atomic<T>& r)
    {
        if (TypeTraits::IsCompileTimeEvaluating()) 
        {
            Swap(l.value.value, r.value.value);
        }
        else 
        {
            Interlocked::Swap(l.value.value, r.value.value);
        }
    }

    template<Concepts::AtomicType T>
    constexpr inline ReadOnlyAtomic<T>::ReadOnlyAtomic(const Atomic<T>& src) : value() {
        Interlocked::Store(value, Interlocked::Load(src.value.value));
    }

    template<Concepts::AtomicType T>
    constexpr inline ReadOnlyAtomic<T>::ReadOnlyAtomic(Atomic<T>&& src) : value() {
        Interlocked::Store(value, Interlocked::Load(src.value.value));
    }
}

namespace Seed::Core::TypeTraits {
    template<typename T>
    struct IsAtomicT : LogicFalse {};

    template<typename T>
    struct IsAtomicT<Atomic<T>> : LogicTrue {
        using ValueType = T;
    };

    template<typename T>
    constexpr bool IsAtomicV = IsAtomicT<T>::State;
}

namespace Seed::Core::Concepts {
    template<typename T>
    concept Atomic = TypeTraits::IsAtomicV<T>;
}

#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Containers.API.LINQ.h"

#pragma warning(push)
#pragma warning(disable: 4996)
namespace Seed {
    inline namespace Core {
        namespace Containers {
            template<typename THeap, SizeT TSize, typename T>
            class SeedKernelCall BaseConcurencyFixedStack :
                public LINQ::IFixedStack<BaseConcurencyFixedStack, THeap, T, TSize, true>,
                public IDisposable,
                public IClonable<BaseConcurencyFixedStack<THeap, TSize, T>>
            {
            private:
                struct Data {
                    RefCountT count;
                    RefCountT RefCount;
                    Threading::SRWLock lock;
                    T stack[TSize];

                    Data() : count(0), RefCount(1), lock(), stack() {}
                };
            private:
                Data* This;
            protected:
                virtual void Dispose() final override {}
            public:
                BaseConcurencyFixedStack() : This(new Data()) {}

                BaseConcurencyFixedStack(InitilizerList<T>&& list) : This(new Data()) {
                    if (list.size() != 0) {
                        SizeT ActuallyCopyied = (TSize < list.size()) ? TSize : list.size();
                        auto it = list.begin();
                        SizeT i = ActuallyCopyied;
                        do {
                            i--; This->stack[i] = *it; it++;
                        } while (i != 0);
                        This->count = ActuallyCopyied;
                    }
                }

                void Push(const T& item) {
                    This->lock.ExclusiveLock([&]() mutable {
                        if (This->count == TSize)
                            Throw(NullPointException(ExceptionPosition));
                        This->stack[This->count] = item;
                        This->count++;
                        });
                }

                void Push(T&& item) {
                    This->lock.ExclusiveLock([&]() mutable {
                        if (This->count == TSize)
                            Throw(NullPointException(ExceptionPosition));
                        This->stack[This->count] = MoveRValue(item);
                        This->count++;
                        });
                }

                T Pop() {
                    return MoveRValue(This->lock.ExclusiveLock([&]() mutable {
                        if (This->count == 0)
                            Throw(NullPointException(ExceptionPosition));
                        This->count--;
                        return MoveRValue(This->stack[This->count]);
                        }));
                }

                const T& Top() {
                    return This->lock.ExclusiveLock([&]() mutable -> const T& {
                        if (This->count == 0)
                            Throw(NullPointException(ExceptionPosition));
                        This->count--;
                        return This->stack[This->count];
                        });
                }

                const T& Top() const {
                    return This->lock.ExclusiveLock([&]() mutable -> const T& {
                        if (This->count == 0)
                            Throw(NullPointException(ExceptionPosition));
                        This->count--;
                        return This->stack[This->count];
                        });
                }

                template<typename U>
                friend void Seed::Core::Swap(U& A, U& B);

                SizeT GetCount() { return This->count; }
                SizeT Property(GetCount) Count;

                virtual BaseConcurencyFixedStack Clone() const override final {
                    return This->lock.ShareLock([&]() {
                        BaseConcurencyFixedStack result;
                        result.This->count = This->count;
                        if constexpr (TypeTraits::IsPodV<T>) {
                            Memory::Copy(This->stack, result.This->stack, sizeof(T) * This->count);
                        }
                        else {
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                result.This->stack[i] = This->stack[i];
                            }
                        }
                        return result;
                        });
                }

                RefCountClass(BaseConcurencyFixedStack, This)
            protected:
                template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
                friend class LINQ::ICollection;
                template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
                friend class LINQ::IFixedCollection;
                template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
                friend class LINQ::IFixedStack;
                template<template<typename THeap, typename T> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
                friend class LINQ::ICollectionBase;
                template<typename THeap, typename T, typename Enable>
                friend struct Seed::Core::NewSerilization::Details::BaseSerlizationHandler;
            protected:
#pragma warning(push)
#pragma warning(disable: 4172)

                template<typename LambdaT>
                __forceinline BaseConcurencyFixedStack& ForEachImpl(LambdaT&& lambda) {
                    if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyFixedStack& {
                            if (This->count == 0)
                                return *this;
                            for (SizeT i = This->count; x-- > 0;)
                            {
                                lambda(This->stack[i]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyFixedStack& {
                            if (This->count == 0)
                                return *this;
                            SizeT i = This->count;
                            do {
                                i--;
                                lambda(This->stack[i], i);
                            } while (i != 0);
                            return *this;
                            });
                    }
                    else {
                        static_assert(false);
                    }
                }

                template<typename LambdaT>
                __forceinline const BaseConcurencyFixedStack& ForEachImpl(LambdaT&& lambda) const {
                    if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyFixedStack& {
                            if (This->count == 0)
                                return *this;
                            SizeT i = This->count;
                            do {
                                i--;
                                lambda(This->stack[i]);
                            } while (i != 0);
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyFixedStack& {
                            if (This->count == 0)
                                return *this;
                            SizeT i = This->count;
                            do {
                                i--;
                                lambda(This->stack[i], i);
                            } while (i != 0);
                            return *this;
                            });
                    }
                    else {
                        static_assert(false);
                    }
                }

                template<typename U, typename LambdaT>
                __forceinline U AggregateImpl(LambdaT&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {

                        if (This->count == 0)
                            return U();
                        U result = U();
                        SizeT i = This->count;
                        do {
                            i--;
                            lambda(result, This->stack[i]);
                        } while (i != 0);
                        return MoveRValue(result);
                        }));
                }

                template<typename U, typename LambdaT>
                __forceinline U AggregateImpl(LambdaT&& lambda, const U& Value) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        U result = Value;
                        if (This->count == 0)
                            return result;
                        SizeT i = This->count;
                        do {
                            i--;
                            lambda(result, This->stack[i]);
                        } while (i != 0);
                        return MoveRValue(result);
                        }));
                }

                template<typename U, typename LambdaT>
                __forceinline U AggregateImpl(LambdaT&& lambda, U&& Value) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        if (This->count == 0)
                            return Value;
                        SizeT i = This->count;
                        do {
                            i--;
                            lambda(Value, This->stack[i]);
                        } while (i != 0);
                        return MoveRValue(Value);
                        }));
                }

                bool AnyImpl() const { return This->count > 0; }

                template<typename Lambda>
                __forceinline bool AnyImpl(Lambda&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        if (This->count == 0)
                            return false;
                        SizeT i = This->count;
                        do {
                            i--;
                            if (lambda(This->stack[i]))
                                return true;
                        } while (i != 0);
                        return false;
                        }));
                }

                template<typename Lambda>
                __forceinline bool AllImpl(Lambda&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        if (This->count == 0)
                            return false;
                        SizeT i = This->count;
                        do {
                            i--;
                            if (!lambda(This->stack[i]))
                                return false;
                        } while (i != 0);
                        return true;
                        }));
                }

                template<typename Lambda>
                T& FirstImpl(Lambda&& lambda) {
                    return This->lock.ShareLock([&]() -> T& {
                        if (This->count == 0)
                            Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        SizeT i = This->count;
                        do {
                            i--;
                            if (lambda(This->stack[i]))
                                return static_cast<T&>(This->array[i]);
                        } while (i != 0);

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename Lambda>
                T& LastImpl(Lambda&& lambda) {
                    return This->lock.ShareLock([&]() -> T& {
                        if (This->count == 0)
                            Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        SizeT i = This->count;
                        SizeT Index - -1;
                        do {
                            i--;
                            if (lambda(This->stack[i]))
                                Index = i;
                        } while (i != 0);

                        if (Index != -1)
                            return static_cast<T&>(This->array[Index]);

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename Lambda>
                const T& FirstImpl(Lambda&& lambda) const {
                    return This->lock.ShareLock([&]() -> const T& {
                        if (This->count == 0)
                            Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        SizeT i = This->count;
                        do {
                            i--;
                            if (lambda(This->stack[i]))
                                return static_cast<T&>(This->array[i]);
                        } while (i != 0);

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename Lambda>
                const T& LastImpl(Lambda&& lambda) const {
                    return This->lock.ShareLock([&]() -> const T& {
                        if (This->count == 0)
                            Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        SizeT i = This->count;
                        SizeT Index = -1;
                        do {
                            i--;
                            if (lambda(This->stack[i]))
                                Index = i;
                        } while (i != 0);

                        if (Index != -1)
                            return static_cast<T&>(This->array[Index]);

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                BaseConcurencyFixedStack& ReverseImpl() {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyFixedStack& {
                        for (SizeT i = 0; i < This->count / 2; i++) {
                            Seed::Swap(This->stack[i], This->stack[(This->count - 1) - i]);
                        }
                        return *this;
                        });
                }

                __forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->stack[Index] = MoveRValue(Value);
					This->count = Index + 1;
                }

                __forceinline BaseConcurencyFixedStack& RemoveAllImpl() {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyFixedStack& {
                        for (SizeT i = 0; i < This->count; i++) {
                            This->stack[i].~T();
                        }
                        return *this;
                        });
                }

                template<typename Lambda>
                __forceinline BaseConcurencyFixedStack& RemoveAllImpl(Lambda&& lambda) {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyFixedStack& {
                        for (SizeT i = 0; i < This->count; i++) {
                            lambda(MoveRValue(This->stack[i]));
                        }
                        return *this;
                        });
                }
            };

            namespace {
                template<SizeT TSize, typename T>
                using ConcurencyFixedStack = BaseConcurencyFixedStack<Seed::Core::Memory::Heap, TSize, T>;
            }

            namespace {
                template<SizeT TSize, typename T>
                using ConcurencySensitiveFixedStack = BaseConcurencyFixedStack<Seed::Core::Memory::SensitiveDataHeap, TSize, T>;
            }
        }

        namespace TypeTraits {
            namespace Collection {
                template<typename THeap, SizeT TSize, typename T>
                struct IsFixedStackT<Seed::Core::Containers::BaseConcurencyFixedStack<THeap, TSize, T>> : LogicTrue {};
            }
        }

        template<typename THeap, SizeT TSize, typename T>
        __forceinline void Swap(Containers::BaseConcurencyFixedStack<THeap, TSize, T>& A, Containers::BaseConcurencyFixedStack<THeap, TSize, T>& B) {
            Swap(A.This, B.This);
        }
    }
}
#pragma warning(pop)
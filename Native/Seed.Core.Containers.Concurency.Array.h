#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Containers.Internals.h"

namespace Seed {
    inline namespace Core {
        namespace Containers {
            template<typename THeap, typename T>
            class BaseConcurencyVList;

            /// <summary>
            /// Масив з пітримкою багатопоточності на базі SRWLock
            /// </summary>
            template<typename THeap, typename T>
            class BaseConcurencyArray final :
                Seed::IDisposable,
                public Memory::Allocator<THeap>,
                public LINQ::IArray<BaseConcurencyArray, THeap, T, true> {
            private:
                struct Data : public Memory::Allocator<THeap> {
                    Seed::RefCountT RefCount;
                    SizeT count;
                    T* restrict array;
                    Threading::SRWLock lock;

                    forceinline Data() : RefCount(1), count(0), array(nullptr), lock() {}
                    forceinline Data(T*&& array, SizeT count) : RefCount(1), count(count), array(array) {}
                };
            private:
                Data* This;
            protected:
                forceinline virtual void Dispose() override {
                    delete[] This->array;
                    This->array = nullptr;
                }
            private:
                template<typename Lambda>
                void QuickSort(T* A, SizeT p, SizeT r, Lambda& lambda) {
                    if (p < r) {
                        SizeT q = Partition(A, p, r, lambda);
                        QuickSort(A, p, q, lambda);
                        QuickSort(A, q + 1, r, lambda);
                    }
                }

                template<typename Lambda>
                SizeT Partition(T* A, SizeT p, SizeT r, Lambda& lambda) {
                    T x = A[p]; // pivot
                    SizeT i = p;
                    SizeT j = r;
                    while (true) {
                        while (lambda(A[i], x) && i != r) {
                            i++;
                        }

                        while (!lambda(A[j], x) && j != p) {
                            j--;
                        }
                        if (i < j) {
                            Seed::Core::Swap(A[i], A[j]);
                        }
                        else {
                            return j;
                        }
                    }
                }

            protected:
                template<template<typename, typename> typename, typename, typename, Boolean, Boolean>
                friend class LINQ::ICollection;
                template<template<typename, typename> typename, typename, typename, Boolean>
                friend class LINQ::IArray;
                template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
                friend class LINQ::IFixedCollectionBase;
                template<template<typename THeap, typename T>typename TList, typename THeap, typename T, bool TIsDynamic, bool TIsConcurencySuport>
                friend class LINQ::IOrderable;
                template<typename THeap, typename T>
                friend class BaseConcurencyVList;
                template<typename THeap, typename T, typename Enable>
                friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
            protected:
#pragma warning(push)
#pragma warning(disable: 4172)
                template<typename TLambda>
                BaseConcurencyArray& ForEachImpl(TLambda&& lambda) {
                    if constexpr (TypeTraits::IsInvockable<TLambda, const T&> ||
                                  TypeTraits::IsInvockable<TLambda, T&>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyArray& {
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                lambda(This->array[i]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsInvockable<TLambda, const T&, SizeT> ||
                                       TypeTraits::IsInvockable<TLambda, T&, SizeT>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyArray& {
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                lambda(This->array[i], i);
                            }
                            return *this;
                            });
                    }
                    else {
                        static_assert(false);
                    }
                }

                template<typename TLambda>
                const BaseConcurencyArray& ForEachImpl(TLambda&& lambda) const {
                    if constexpr (TypeTraits::IsInvockable<TLambda, const T&>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyArray& {
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                lambda(This->array[i]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsInvockable<TLambda, const T&, SizeT>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyArray& {
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                lambda(This->array[i], i);
                            }
                            return *this;
                            });
                    }
                    else {
                        static_assert(false);
                    }
                }

                template<typename TLambda>
                BaseConcurencyArray& ForEachImpl(TLambda&& lambda, SizeT count) {
                    if (count > this->Count)
                        count = this->Count;

                    if constexpr (TypeTraits::IsInvockable<TLambda, const T&> ||
                                  TypeTraits::IsInvockable<TLambda, T&>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyArray& {
                            for (SizeT i = 0; i < count; i++)
                            {
                                lambda(This->array[i]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsInvockable<TLambda, const T&, SizeT> ||
                                       TypeTraits::IsInvockable<TLambda, T&, SizeT>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyArray& {
                            for (SizeT i = 0; i < count; i++)
                            {
                                lambda(This->array[i], i);
                            }
                            return *this;
                            });
                    }
                    else {
                        static_assert(false);
                    }
                }

                template<typename TLambda>
                const BaseConcurencyArray& ForEachImpl(TLambda&& lambda, SizeT count) const {
                    if (count > this->Count)
                        count = this->Count;

                    if constexpr (TypeTraits::IsInvockable<TLambda, const T&>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyArray& {
                            for (SizeT i = 0; i < count; i++)
                            {
                                lambda(This->array[i]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsInvockable<TLambda, const T&, SizeT>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyArray& {
                            for (SizeT i = 0; i < count; i++)
                            {
                                lambda(This->array[i], i);
                            }
                            return *this;
                            });
                    }
                    else {
                        static_assert(false);
                    }
                }

                template<typename TLambda>
                BaseConcurencyArray& ForEachImpl(TLambda&& lambda, SizeT startIndex, SizeT count) {
                    if (startIndex > this->Count)
                        Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, startIndex, this->Count);
                    if (startIndex + count > this->Count)
                        count = this->Count - startIndex;

                    if constexpr (TypeTraits::IsInvockable<TLambda, const T&> ||
                                  TypeTraits::IsInvockable<TLambda, T&>) {
                       return This->lock.ShareLock([&]() -> BaseConcurencyArray& {
                            for (SizeT i = startIndex; i < count; i++)
                            {
                                lambda(This->array[i]);
                            }
                            return *this;
                       });
                    }
                    else if constexpr (TypeTraits::IsInvockable<TLambda, const T&, SizeT> ||
                                       TypeTraits::IsInvockable<TLambda, T&, SizeT>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyArray& {
                            for (SizeT i = startIndex; i < count; i++)
                            {
                                lambda(This->array[i], i);
                            }
                            return *this;
                        });
                    }
                    else {
                        static_assert(false);
                    }
                }

                template<typename TLambda>
                const BaseConcurencyArray& ForEachImpl(TLambda&& lambda, SizeT startIndex, SizeT count) const {
                    if (startIndex > this->Count)
                        Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __LINE__, __FUNCSIG__);
                    if (startIndex + count > this->Count)
                        count = this->Count - startIndex;

                    if constexpr (TypeTraits::IsInvockable<TLambda, const T&>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyArray& {
                            for (SizeT i = startIndex; i < count; i++)
                            {
                                lambda(This->array[i]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsInvockable<TLambda, const T&, SizeT>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyArray& {
                            for (SizeT i = startIndex; i < count; i++)
                            {
                                lambda(This->array[i], i);
                            }
                            return *this;
                            });
                    }
                    else {
                        static_assert(false);
                    }
                }
            protected:
                template<typename U, typename TLambda>
                U AggregateImpl(TLambda&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        U result = U();
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            result = lambda(result, This->array[i]);
                        }
                        return result;
                        }));
                }

                template<typename U, typename TLambda>
                U AggregateImpl(TLambda&& lambda, const U& Value) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        U result = Value;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            result = lambda(result, This->array[i]);
                        }
                        return MoveRValue(result);
                        }));
                }

                template<typename U, typename TLambda>
                U AggregateImpl(TLambda&& lambda, U&& result) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            result = lambda(result, This->array[i]);
                        }
                        return MoveRValue(result);
                        }));
                }

                forceinline bool AnyImpl() const { return This->count > 0; }

                template<typename TLambda>
                bool AnyImpl(TLambda&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (lambda(This->array[i]))
                                return true;
                        }
                        return false;
                        }));
                }

                template<typename TLambda>
                bool AllImpl(TLambda&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (!lambda(This->array[i]))
                                return false;
                        }
                        return true;
                        }));
                }

                forceinline BaseConcurencyArray& SortByImpl(bool IsDescending = true) {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyArray& {
                        if (IsDescending) {
                            auto l = [](const T& V1, const T& V2) { return V1 < V2; };
                            QuickSort(This->array, 0, This->count - 1, l);
                        }
                        else {
                            auto l = [](const T& V1, const T& V2) { return V1 > V2; };
                            QuickSort(This->array, 0, This->count - 1, l);
                        }
                        return *this;
                        });
                }

                template<typename TLambda>
				forceinline BaseConcurencyArray& SortByLambdaImpl(TLambda&& lambda) {
                    return This->lock.ExclusiveLock([&] () -> BaseConcurencyArray&
                    {
                        if (This->count == 0)
							return;

                        QuickSort(This->array, 0, This->count - 1, lambda);
                        return *this;
                    });
				}

                template<typename TLambda>
                T& FirstImpl(TLambda&& lambda) {
                    return This->lock.ShareLock([&]() -> T& {
                        for (SizeT i = 0; i < This->count; i++)
                            if (lambda(This->array[i]))
                                return static_cast<T&>(This->array[i]);

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename TLambda>
                T& LastImpl(TLambda&& lambda) {
                    return This->lock.ShareLock([&]() -> T& {
                        SizeT Index = -1;
                        for (SizeT i = 0; i < This->count; i++)
                            if (lambda(This->array[i]))
                                Index = i;

                        if (Index != -1)
                            return static_cast<T&>(This->array[Index]);

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename TLambda>
                const T& FirstImpl(TLambda&& lambda) const {
                    return This->lock.ShareLock([&]() -> const T& {
                        for (SizeT i = 0; i < This->count; i++)
                            if (lambda(This->array[i]))
                                return static_cast<T&>(This->array[i]);

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename TLambda>
                const T& LastImpl(TLambda&& lambda) const {
                    return This->lock.ShareLock([&]() -> const T& {
                        SizeT Index = -1;
                        for (SizeT i = 0; i < This->count; i++)
                            if (lambda(This->array[i]))
                                Index = i;

                        if (Index != -1)
                            return static_cast<T&>(This->array[Index]);

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                forceinline BaseConcurencyArray& ReverseImpl() {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyArray& {
                        for (SizeT i = 0; i < This->count / 2; i++) {
                            Seed::Core::Swap<T>(This->array[i], This->array[(This->count - 1) - i]);
                        }
                        return *this;
                        });
                }

                forceinline void UnsafeSet(const T& Value, SizeT Index) {
                    This->array[Index] = Value;
                }

                forceinline void UnsafeSet(T&& Value, SizeT Index) {
                    This->array[Index] = MoveRValue(Value);
                }

                forceinline BaseConcurencyArray& RemoveAllImpl() {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyArray& {
                        delete[] this->This->array;
                        this->This->count = 0;
                        return *this;
                        });
                }

                template<typename TLambda>
                forceinline BaseConcurencyArray& RemoveAllImpl(TLambda&& lambda) {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyArray& {
                        for (SizeT i = 0; i < This->count; i++) {
                            lambda(MoveRValue(This->array[i]));
                        }
                        delete[] this->This->array;
                        this->This->array = nullptr;
                        this->This->count = 0;
                        return *this;
                        });
                }

                template<typename TLambda>
                SizeT GetCountIfImpl(TLambda&& lambda) {
                    return This->lock.ShareLock([&]() {
                        SizeT selectedCount = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (lambda(This->array[i]))
                                selectedCount++;
                        }
                        return selectedCount;
                        });
                }
#pragma warning(pop)
            public:
#pragma warning(push)
#pragma warning(disable: 6386)
                /// <summary>
                /// Конструктор масиву з нульвою довжиною
                /// </summary>
                forceinline BaseConcurencyArray() : This(new Data()) {}

                /// <summary>
                /// Конструктор з списку елементів
                /// </summary>
                /// <param name="list">Список елементів</param>
                BaseConcurencyArray(InitilizerList<T>&& list) : This(new Data()) {
                    if (list.size() != 0) {
                        This->count = list.size();
                        this->This->array = new T[list.size()];
                        auto it = list.begin();
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            This->array[i] = MoveRValue(*it); it++;
                        }
                    }
                }

                /// <summary>
                /// Конструктор з кількістю елементів
                /// </summary>
                /// <param name="StartCount"></param>
                forceinline  BaseConcurencyArray(SizeT StartCount) : This(new Data()) {
                    if (StartCount != 0) {
                        This->array = new T[StartCount];
                        This->count = StartCount;
                    }
                }

                /// <summary>
                /// Внутрішній констуктор з с-масиву і кількості елементів
                /// </summary>
                /// <param name="Array">Покажчик на масив елементів</param>
                /// <param name="StartCount">Кількість елементів</param>
                InternalApiCall BaseConcurencyArray(const T* Array, SizeT StartCount) : This(new Data()) {
                    if (StartCount != 0) {
                        This->array = new T[StartCount];
                        This->count = StartCount;
                        if constexpr (TypeTraits::IsPodV<T>) {
                            Memory::Copy(Array, This->array, StartCount * sizeof(T));
                        }
                        else {
                            for (SizeT i = 0; i < StartCount; i++)
                            {
                                This->array[i] = Array[i];
                            }
                        }
                    }
                }

                /// <summary>
                /// Внутрішній констуктор з rvalue силки на с-масив і кількості елементів
                /// </summary>
                /// <param name="Array">Rvalue силка на с-масив</param>
                /// <param name="StartCount">Кількість елементів</param>
                forceinline InternalApiCall BaseConcurencyArray(T*&& Array, SizeT StartCount) : This(new Data(MoveRValue(Array), StartCount)) {}
                
                /// <summary>
                /// Оператор отримання голого покажкика на елементи масиву
                /// </summary>
                /// <returns></returns>
                forceinline InternalApiCall operator const T* const() const { return This->array; }
#pragma warning(pop)
            public:
                /// <summary>
                /// Оператор доступу до елементів масиву
                /// </summary>
                /// <param name="Index">Номер елемента</param>
                /// <returns>Силка на елемент</returns>
                forceinline T& operator[](SizeT Index) {
                    return This->lock.ShareLock([&]() -> T& {
                        if (Index < This->count)
                        {
                            return This->array[Index];
                        }
                        else
                        {
                            Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, Index, This->count);
                        }
                        });
                }

                /// <summary>
                /// Константний оператор доступу до елементів масиву
                /// </summary>
                /// <param name="Index">Номер елемента</param>
                /// <returns>Константна силка на елемент</returns>
                forceinline const T& operator[](SizeT Index) const {
                    return This->lock.ShareLock([&]() -> const T& {
                        if (Index < This->count)
                        {
                            return This->array[Index];
                        }
                        else
                        {
                            Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, Index, This->count);
                        }
                        });
                }
#pragma warning(push)
#pragma warning(disable: 4996)
                /// <summary>
                /// Зміна розміру
                /// </summary>
                /// <param name="NewSize">Новий розмір масиву</param>
                /// <returns>Силка на поточний об'єкт</returns>
                BaseConcurencyArray& Resize(SizeT NewSize) {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyArray& {
                        if (This->array == nullptr || This->count == 0)
                        {
                            This->array = new T[NewSize];
                            This->count = NewSize;
                        }
                        else {
                            if (NewSize == 0) {
                                delete[] This->array;
                                This->count = 0;
                            }
                            else {
                                T* buf = new T[NewSize];
                                SizeT CopySize = (NewSize > This->count) ? This->count : NewSize;
                                if constexpr (TypeTraits::IsPodV<T>) {
                                    Memory::Copy(This->array, buf, CopySize * sizeof(T));
                                }
                                else {
                                    for (SizeT i = 0; i < CopySize; i++)
                                    {
                                        buf[i] = MoveRValue(This->array[i]);
                                    }
                                }
                                This->count = NewSize;
                                delete[] This->array;
                                This->array = buf;
                            }
                        }
                        return *this;
                        });
                }

                /// <summary>
                /// Клонування масиву
                /// </summary>
                /// <returns>Вертає новий масив з копією всіх елементів</returns>
                template<typename = void> requires (Concepts::IsClonable<T>) 
                BaseConcurencyArray Clone() const {
                    return This->lock.ShareLock([&] () -> BaseConcurencyArray
                    {
                        if constexpr (TypeTraits::IsPodV<T>)
                        {
                            return BaseConcurencyArray(This->array, This->count);
                        }
                        else
                        {
                            auto array = BaseConcurencyArray(This->array, This->count);
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                array.This->array[i] = This->array[i].Clone();
                            }
                            return MoveRValue(array);
                        }
                    });
                }

                /// <summary>
                /// Клонування масиву
                /// </summary>
                /// <param name="lambda">Функція задопомогою якої відбувається клонування</param>
                /// <returns>Вертає новий масив з копією всіх елементів</returns>
                template<typename TLambda> requires (Concepts::IsInvockableWithResult<TLambda, T, const T&>)
                BaseConcurencyArray Clone(TLambda&& lambda) const {
                    return This->lock.ShareLock([&] () -> BaseConcurencyArray
                    {
                        T* array = nullptr;
                        try
                        {
                            array = new T[This->count];
                            for (SizeT i = 0ui64; i < This->count; i++)
                            {
                                array[i] = lambda(This->array[i]);
                            }

                            return BaseConcurencyArray(MoveRValue(array), This->count);
                        }
                        catch (...)
                        {
                            delete[] array;
                        }
                    });
                }
#pragma warning(pop)
                ///<summary>Оптимізований обмін</summary>
                ///<param name = 'A'>Перший елемент</param>
                ///<param name = 'B'>Другий елемент</param>
                template<typename U>
                friend void Seed::Core::Swap(U& A, U& B);
            public:
                /// <summary>
                /// Вертає поточну кількість елементів
                /// </summary>
                /// <returns></returns>
                forceinline SizeT GetCount() const { return This->count; }

                /// <summary>
                /// Кількість елементів
                /// </summary>
                SizeT Property(GetCount) Count;

                /// <summary>
                /// Вертає сирий покажчик на елементи масиву
                /// </summary>
                /// <returns></returns>
                forceinline const T* const GetRaw() const { return This->array; }
            public:
                RefCountClass(BaseConcurencyArray, This);
            };

            /// <summary>
            /// Масив з підтримкою багатопоковості
            /// </summary>
            namespace {
                template<typename T>
                using ConcurencyArray = BaseConcurencyArray<Seed::Core::Memory::Heap, T>;
            }

            /// <summary>
            /// Масив з підтримкою багатопоковості в захищеній кучі
            /// </summary>
            namespace {
                template<typename T>
                using ConcurencySensitiveArray = BaseConcurencyArray<Seed::Core::Memory::SensitiveDataHeap, T>;
            }

            /// <summary>
            /// ConcurencyVList - список масивів, гібрид масиву і списку
            /// </summary>
            template<typename THeap, typename T>
            class BaseConcurencyVList final :
                Seed::IDisposable,
                public Memory::Allocator<THeap>,
                public LINQ::IArray<BaseConcurencyArray, THeap, T, true> {
            private:
                struct Node : public Memory::Allocator<THeap> {
                    T* restrict arr;
                    SizeT size;
                    Node* next;

                    forceinline Node(SizeT size) : size(size), arr(new T[size]), next(nullptr) {}
                    forceinline Node(SizeT size, T*&& arr) : size(size), arr(arr), next(nullptr) {}
                    forceinline Node(SizeT size, const T*& arr) : size(size), arr(new T[size]), next(nullptr) {
                        if constexpr (TypeTraits::IsPodV<T>) {
                            Memory::Copy(arr, this->arr, sizeof(T) * size);
                        }
                        else {
                            for (SizeT i = 0; i < size; i++)
                            {
                                this->arr[i] = arr[i];
                            }
                        }
                    }
                    forceinline Node(SizeT size, T*& arr) : size(size), arr(new T[size]), next(nullptr) {
                        if constexpr (TypeTraits::IsPodV<T>) {
                            Memory::Copy(arr, this->arr, sizeof(T) * size);
                        }
                        else {
                            for (SizeT i = 0; i < size; i++)
                            {
                                this->arr[i] = arr[i];
                            }
                        }
                    }
                };

                struct Data : public Memory::Allocator<THeap> {
                    RefCountT RefCount;
                    RefCountT count;
                    Node* root;
                    Node* last;
                    Threading::SRWLock lock;

                    forceinline Data() : RefCount(1), count(0), root(nullptr), last(nullptr), lock() {}
                    forceinline Data(SizeT count) : RefCount(1), count(count), root(new Node(count)), last(root), lock() {}
                    forceinline Data(SizeT count, T*&& arr) : RefCount(1), count(count), root(new Node(count, MoveRValue(arr))), last(root), lock() {}
                    forceinline Data(SizeT count, const T*& arr) : RefCount(1), count(count), root(new Node(count, arr)), last(root), lock() {}
                };
            private:
                Data* This;
            public:
                /// <summary>
                /// Пустий конструктор створює пустий екземпляр об'єкта
                /// </summary>
                forceinline BaseConcurencyVList() : This(new Data()) {}

                /// <summary>
                /// Конструктор з списку ініціалізації, переміщає елементи в поточний об'єкт
                /// </summary>
                /// <param name="list">Список елементів</param>
                BaseConcurencyVList(InitilizerList<T>&& list) : This(new Data(list.size())) {
                    auto it = list.begin();
                    for (SizeT i = 0; i < list.size(); i++)
                    {
                        This->root->arr[i] = MoveRValue(*it); ++it;
                    }
                }

                /// <summary>
                /// Конструктор з масиву елементів
                /// </summary>
                /// <param name="array">Масив елементів</param>
                forceinline BaseConcurencyVList(const BaseConcurencyArray<THeap, T>& array) : This(nullptr) {
                    array.This->lock.ShareLock([&]() {
                        this->This = new Data(array.This->count, (const T*&)(array.This->array));
                        });
                }

                /// <summary>
                /// Конструктор з rvalue силки на масив, якщо масиву більше ніде не використовується 
                /// то даний об'єкт оптимізовано стане власником елементів 
                /// </summary>
                /// <param name="array">RValue силка на масив</param>
                BaseConcurencyVList(BaseConcurencyArray<THeap, T>&& array) : This(nullptr) {
                    if (--array.This->RefCount == 0) {
                        This = new Data(array.This->count, MoveRValue(array.This->array));
                        delete array.This;
						array.This = nullptr;
                    }
                    else {
                        this->BaseConcurencyVList::BaseConcurencyVList(array);
                        array.This = nullptr;
                    }
                }
            public:
                /// <summary>
                /// Константний оператор доступу до елементів об'єкта
                /// </summary>
                /// <param name="Index">Індекс елемента</param>
                /// <returns>Константна силка на елемент</returns>
                forceinline const T& operator[](SizeT Index) const {
                    if (Index >= This->count)
                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                    else {
                        return This->lock.ShareLock([&]() -> const T& {
                            auto* node = This->root;
                            SizeT nodeStartIndex = 0;
                            while (true) {
                                if (nodeStartIndex + node->size > Index)
                                    return node->arr[Index - nodeStartIndex];
                                else {
                                    nodeStartIndex += node->size; node = node->next;
                                }
                            }
                            });
                    }
                }

                /// <summary>
                /// Оператор доступу до елементів об'єкта
                /// </summary>
                /// <param name="Index">Індекс елемента</param>
                /// <returns>Силка на елемент</returns>
                forceinline T& operator[](SizeT Index) {
                    if (Index >= This->count)
                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                    else {
                        return This->lock.ShareLock([&]() -> T& {
                            auto* node = This->root;
                            SizeT nodeStartIndex = 0;
                            while (true) {
                                if (nodeStartIndex + node->size > Index)
                                    return node->arr[Index - nodeStartIndex];
                                else {
                                    nodeStartIndex += node->size; node = node->next;
                                }
                            }
                            });
                    }
                }
            public:
                /// <summary>
                /// Оператор додавання масиву до об'єкта, копіює елементи в поточний об'єкт
                /// </summary>
                /// <param name="array">Масив елементів</param>
                /// <returns>Силку на поточний об'єкт</returns>
                BaseConcurencyVList& operator+=(const BaseConcurencyArray<THeap, T>& array) {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyVList& {
                        return array.This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            if (This->root == nullptr)
                                This->root = This->last = new Node(array.This->count);
                            else
                                This->last = This->last->next = new Node(array.This->count);
                            This->count = This->count + array.This->count;
                            for (SizeT i = 0; i < This->last->size; i++)
                            {
                                This->last->arr[i] = array.This->array[i];
                            }
                            return *this;
                            });
                        });
                }

                /// <summary>
                /// Оптимізований оператор додавання масиву до об'єкта, 
                /// якщо масив більше ніде не використовується то відбудеться передача елементів без копіювання,
                /// інакше елементи будуть скопійовані
                /// </summary>
                /// <param name="array">RValue силка на масив</param>
                /// <returns>Силка на поточний елемент</returns>
                BaseConcurencyVList& operator+=(BaseConcurencyArray<THeap, T>&& array) {
                    return This->lock.ExclusiveLock([&, array = MoveRValue(array)]() mutable -> BaseConcurencyVList& {
                        if (--array.This->RefCount == 0) {
                            if (This->root == nullptr)
                                This->last = This->root = new Node(array.This->count, MoveRValue(array.This->array));
                            else
                                This->last = This->last->next = new Node(array.This->count, MoveRValue(array.This->array));
                            This->count = array.This->count;
                            array.This->array = nullptr;
                            array.This->count = 0;
                            delete array.This;
                            array.This = nullptr;
                            return *this;
                        }
                        else {
                            return array.This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                                if (This->root == nullptr)
                                    This->root = This->last = new Node(array.This->count, static_cast<T*&>(array.This->array));
                                else
                                    This->last = This->last->next = new Node(array.This->count, static_cast<T*&>(array.This->array));
                                This->count = This->count + array.This->count;
                                return *this;
                                });
                        }
                    });
                }
            public:
                /// <summary>
                /// Перетворення даного об'єкта в масив
                /// </summary>
                /// <returns>Масив з елементами з даного об'єкта</returns>
                BaseConcurencyArray<THeap, T> ToArray() {
                    return This->lock.ShareLock([&]() mutable -> BaseConcurencyArray<THeap, T> {
                        BaseConcurencyArray<THeap, T> array(this->This->count);
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (node->size == i - nodeStartIndex)
                            {
                                node = node->next;
                                nodeStartIndex += node->size;
                            }
                            array.This->array[i] = node->arr[i - nodeStartIndex];
                        }
                        return array;
                        });
                }
            public:
                /// <summary>
                /// Кількість елементів в об'єкті
                /// </summary>
                /// <returns>Поточна кількість елементів в об'єктів</returns>
                forceinline SizeT GetCount() const { return This->count; }

                /// <summary>
                /// Поточна кількість елементів в об'єктів
                /// </summary>
                SizeT Property(GetCount) Count;
            public:
                /// <summary>
                /// Клонування об'єктів
                /// </summary>
                /// <returns>Новий об'єкт з копією всіх елементів</returns>
                template<typename = void> requires (Concepts::IsClonable<T>) 
                BaseConcurencyVList Clone() const {
                    return This->lock.ShareLock([&]() {
                        BaseConcurencyVList result;
                        if (this->This->root != nullptr) {
                            result.This->root = result.This->last = new Node(this->This->root->size, static_cast<T*&>(this->This->root->arr));
                            if (this->This->root != this->This->last) {
                                Node * node = this->This->last;
                                while (node != nullptr) {
                                    result.This->last = result.This->last->next = new Node(node->size, static_cast<T*&>(node->arr));
                                    node = node->next;
                                }
                            }
                            result.This->count = this->This->count;
                        }
                        return result;
                        });
                }
            public:
                template<typename U>
                friend void Seed::Core::Swap(U& a, U& b);
            protected:
                virtual void Dispose() override final {
                    This->lock.ExclusiveLock([&]() {
                        Node* node = This->root;
                        Node* tmp;
                        SizeT nodeStartIndex = 0;
                        while (node != nullptr)
                        {
                            tmp = node->next; delete[] node->arr; delete node;  node = tmp;
                        }
                        This->count = 0; This->last = This->root = nullptr;

                        return *this;
                        });
                }
            public:
                template<template<typename, typename> typename, typename, typename, Boolean, Boolean>
                friend class LINQ::ICollection;
                template<template<typename, typename> typename, typename, typename, Boolean>
                friend class LINQ::IVList;
                template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
                friend class LINQ::IFixedCollectionBase;
                template<template<typename THeap, typename T>typename TList, typename THeap, typename T, bool TIsDynamic, bool TIsConcurencySuport>
                friend class LINQ::IOrderable;
                template<typename THeap, typename T, typename Enable>
                friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
                template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
                friend class LINQ::IAssociableCollection;
            protected:
#pragma warning(push)
#pragma warning(disable: 4172)
                template<typename TLambda>
                BaseConcurencyVList& ForEachImpl(TLambda&& lambda) {
                    if constexpr (TypeTraits::IsInvockable<TLambda, T&> || TypeTraits::IsInvockable<TLambda, const T&>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                if (i - nodeStartIndex = node->size) { //-V559
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsInvockable<TLambda, T&, SizeT> || TypeTraits::IsInvockable<TLambda, const T&, SizeT>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                if (i - nodeStartIndex = node->size) { //-V559
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex], i);
                            }
                            return *this;
                            });
                    }
                    else {
                        static_assert(false);
                    }
                }

                template<typename TLambda>
                const BaseConcurencyVList& ForEachImpl(TLambda&& lambda) const {
                    if constexpr (TypeTraits::IsInvockable<TLambda, const T&>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                if (i - nodeStartIndex = node->size) { //-V559
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex]);
                            }
                            return *this;
                        });
                    }
                    else if constexpr (TypeTraits::IsInvockable<TLambda, const T&, SizeT>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                if (i - nodeStartIndex = node->size) { //-V559
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex], i);
                            }
                            return *this;
                        });
                    }
                    else {
                        static_assert(false);
                    }
                }

                template<typename TLambda>
                BaseConcurencyVList& ForEachImpl(TLambda&& lambda, SizeT count) {
                    if (count > this->Count)
                        count = this->Count;

                    if constexpr (TypeTraits::IsInvockable<TLambda, T&> || TypeTraits::IsInvockable<TLambda, const T&>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < count; i++)
                            {
                                if (i - nodeStartIndex = node->size) { //-V559
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsInvockable<TLambda, T&, SizeT> || TypeTraits::IsInvockable<TLambda, const T&, SizeT>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < count; i++)
                            {
                                if (i - nodeStartIndex = node->size) { //-V559
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex], i);
                            }
                            return *this;
                            });
                    }
                    else {
                        static_assert(false);
                    }
                }

                template<typename TLambda>
                const BaseConcurencyVList& ForEachImpl(TLambda&& lambda, SizeT count) const {
                    if (count > this->Count)
                        count = this->Count;

                    if constexpr (TypeTraits::IsInvockable<TLambda, const T&>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < count; i++)
                            {
                                if (i - nodeStartIndex = node->size) { //-V559
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsInvockable<TLambda, const T&, SizeT>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < count; i++)
                            {
                                if (i - nodeStartIndex = node->size) { //-V559
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex], i);
                            }
                            return *this;
                            });
                    }
                    else {
                        static_assert(false);
                    }
                }

            protected:
                template<typename U, typename TLambda>
                U AggregateImpl(TLambda&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        U val = U();
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (i - nodeStartIndex = node->size) { //-V559
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            lambda(val, node->arr[i - nodeStartIndex]);
                        }
                        return val;
                        }));
                }

                template<typename U, typename TLambda>
                U AggregateImpl(TLambda&& lambda, const U& Value) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        U result = Value;
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (i - nodeStartIndex = node->size) { //-V559
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            lambda(result, node->arr[i - nodeStartIndex]);
                        }
                        return MoveRValue(result);
                        }));
                }

                template<typename U, typename TLambda>
                U AggregateImpl(TLambda&& lambda, U&& Value) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (i - nodeStartIndex = node->size) { //-V559
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            lambda(Value, node->arr[i - nodeStartIndex]);
                        }
                        return MoveRValue(Value);
                        }));
                }

                forceinline bool AnyImpl() const { return This->count > 0; }

                template<typename TLambda>
                bool AnyImpl(TLambda&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (i - nodeStartIndex = node->size) { //-V559
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            if (lambda(node->arr[i - nodeStartIndex]))
                                return true;
                        }
                        return false;
                    }));
                }

                template<typename TLambda>
                bool AllImpl(TLambda&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (i - nodeStartIndex = node->size) { //-V559
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            if (lambda(node->arr[i - nodeStartIndex]))
                                return false;
                        }
                        return true;
                        }));
                }

                template<typename TLambda>
                T& FirstImpl(TLambda&& lambda) {
                    return This->lock.ShareLock([&]() -> T& {
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (i - nodeStartIndex = node->size) { //-V559
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            if (lambda(node->arr[i - nodeStartIndex]))
                            {
                                return node->arr[i - nodeStartIndex];
                            }
                        }

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename TLambda>
                T& LastImpl(TLambda&& lambda) {
                    return This->lock.ShareLock([&]() -> T& {
                        Node* node = This->root;
                        Node* last = nullptr;
                        SizeT nodeStartIndex = 0;
                        SizeT index = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (i - nodeStartIndex = node->size) { //-V559
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            if (lambda(node->arr[i - nodeStartIndex]))
                            {
                                last = node; index = i - nodeStartIndex;
                            }
                        }
                        if (last != nullptr)
                            return last->arr[index];

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename TLambda>
                const T& FirstImpl(TLambda&& lambda) const {
                    return This->lock.ShareLock([&]() -> const T& {
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (i - nodeStartIndex = node->size) { //-V559
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            if (lambda(node->arr[i - nodeStartIndex]))
                            {
                                return node->arr[i - nodeStartIndex];
                            }
                        }

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename TLambda>
                const T& LastImpl(TLambda&& lambda) const {
                    return This->lock.ShareLock([&]() -> const T& {
                        Node* node = This->root;
                        Node* last = nullptr;
                        SizeT nodeStartIndex = 0;
                        SizeT index = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (i - nodeStartIndex = node->size) { //-V559
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            if (lambda(node->arr[i - nodeStartIndex])) {
                                last = node; index = i - nodeStartIndex;
                            }
                        }
                        if (last != nullptr)
                            return last->arr[index];

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                forceinline void UnsafeSet(const T& Value, SizeT Index) {
                    This->root->arr[Index] = Value;
                }

                forceinline void UnsafeSet(T&& Value, SizeT Index) {
                    This->root->arr[Index] = MoveRValue(Value);
                }

                forceinline BaseConcurencyVList& RemoveAllImpl() {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyVList& {
                        Node* node = This->root;
                        Node* tmp;
                        SizeT nodeStartIndex = 0;
                        while (node != nullptr)
                        {
                            tmp = node; node = node->next; delete[] node->arr; delete node;
                        }
                        This->count = 0; This->last = This->root = nullptr;

                        return *this;
                        });
                }

                template<typename Lambda>
                forceinline BaseConcurencyVList& RemoveAllImpl(Lambda&& lambda) {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyVList& {
                        Node* node = This->root;
                        Node* tmp;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (i - nodeStartIndex = node->size) { //-V559
                                nodeStartIndex += node->size;
                                tmp = node; node = node->next; delete[] node->arr; delete node; node = tmp;
                            }
                            lambda(MoveRValue(node->arr[i - nodeStartIndex]));
                        }
                        This->count = 0; This->last = This->root = nullptr;
                        });
                }

                template<typename Lambda>
                SizeT GetCountIfImpl(Lambda&& lambda) {
                    return This->lock.ShareLock([&]() {
                        Node* node = This->root;
                        SizeT selectedCount = 0;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (i - nodeStartIndex = node->size) { //-V559
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            if (lambda(node->arr[i - nodeStartIndex]))
                                selectedCount++;
                        }
                        return selectedCount;
                        });
                }
#pragma warning(pop)
            public:
                RefCountClass(BaseConcurencyVList, This)
            };

            /// <summary>
			/// Список масивів з оптимізованим додаванням масивів та підтримкою багатопотоковості
			/// </summary>
            namespace {
                template<typename T>
                using ConcurencyVList = BaseConcurencyVList<Seed::Core::Memory::Heap, T>;
            }

            /// <summary>
			/// Список масивів з оптимізованим додаванням масивів в захищеній кучі та підтримкою багатопотоковості
			/// </summary>
            namespace {
                template<typename T>
                using SensitiveConcurencyVList = BaseConcurencyVList<Seed::Core::Memory::SensitiveDataHeap, T>;
            }
        }

        namespace TypeTraits {
            namespace Collection {
                template<typename THeap, typename T>
                struct IsArrayT<Seed::Core::Containers::BaseConcurencyArray<THeap, T>> : LogicTrue {};

                template<typename THeap, typename T>
                struct IsVListT<Seed::Core::Containers::BaseConcurencyVList<THeap, T>> : LogicTrue {};
            }
        }

        template<typename THeap, typename T>
        forceinline void Swap(Containers::BaseConcurencyArray<THeap, T>& A, Containers::BaseConcurencyArray<THeap, T> B) {
            //TODO: Interlocked Exchange
            Swap(A.This, B.This);
        }

        template<typename THeap, typename T>
        forceinline void Swap(Containers::BaseConcurencyVList<THeap, T>& A, Containers::BaseConcurencyVList<THeap, T> B) {
            //TODO: Interlocked Exchange
            Swap(A.This, B.This);
        }
    }
}
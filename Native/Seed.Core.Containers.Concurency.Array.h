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

            template<typename THeap, typename T>
            class BaseConcurencyArray final :
                Seed::IDisposable,
                public IClonable<BaseConcurencyArray<THeap, T>>,
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
                template<typename LambdaT>
                BaseConcurencyArray& ForEachImpl(LambdaT&& lambda) {
                    if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyArray& {
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                lambda(This->array[i]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>) {
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

                template<typename LambdaT>
                const BaseConcurencyArray& ForEachImpl(LambdaT&& lambda) const {
                    if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyArray& {
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                lambda(This->array[i]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
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

                template<typename LambdaT>
                BaseConcurencyArray& ForEachImpl(LambdaT&& lambda, SizeT count) {
                    if (count > this->Count)
                        count = this->Count;
                    if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyArray& {
                            for (SizeT i = 0; i < count; i++)
                            {
                                lambda(This->array[i]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>) {
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

                template<typename LambdaT>
                const BaseConcurencyArray& ForEachImpl(LambdaT&& lambda, SizeT count) const {
                    if (count > this->Count)
                        count = this->Count;
                    if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyArray& {
                            for (SizeT i = 0; i < count; i++)
                            {
                                lambda(This->array[i]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
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
            protected:
                template<typename U, typename LambdaT>
                U AggregateImpl(LambdaT&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        U val = U();
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            lambda(val, This->array[i]);
                        }
                        return val;
                        }));
                }

                template<typename U, typename Lambda>
                U AggregateImpl(Lambda&& lambda, const U& Value) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        U result = Value;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            lambda(result, This->array[i]);
                        }
                        return MoveRValue(result);
                        }));
                }

                template<typename U, typename Lambda>
                U AggregateImpl(Lambda&& lambda, U&& Value) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            lambda(Value, This->array[i]);
                        }
                        return MoveRValue(Value);
                        }));
                }

                forceinline bool AnyImpl() const { return This->count > 0; }

                template<typename Lambda>
                bool AnyImpl(Lambda&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (lambda(This->array[i]))
                                return true;
                        }
                        return false;
                        }));
                }

                template<typename Lambda>
                bool AllImpl(Lambda&& lambda) const {
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

                template<typename Lambda>
                T& FirstImpl(Lambda&& lambda) {
                    return This->lock.ShareLock([&]() -> T& {
                        for (SizeT i = 0; i < This->count; i++)
                            if (lambda(This->array[i]))
                                return static_cast<T&>(This->array[i]);

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename Lambda>
                T& LastImpl(Lambda&& lambda) {
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

                template<typename Lambda>
                const T& FirstImpl(Lambda&& lambda) const {
                    return This->lock.ShareLock([&]() -> const T& {
                        for (SizeT i = 0; i < This->count; i++)
                            if (lambda(This->array[i]))
                                return static_cast<T&>(This->array[i]);

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename Lambda>
                const T& LastImpl(Lambda&& lambda) const {
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

                template<typename Lambda>
                forceinline BaseConcurencyArray& RemoveAllImpl(Lambda&& lambda) {
                    return This->lock.ExclusiveLock([&]() -> BaseConcurencyArray& {
                        for (SizeT i = 0; i < This->count; i++) {
                            lambda(MoveRValue(This->array[i]));
                        }
                        delete[] this->This->array;
                        this->This->count = 0;
                        });
                }

                template<typename Lambda>
                SizeT GetCountIfImpl(Lambda&& lambda) {
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
                forceinline BaseConcurencyArray() : This(new Data()) {}
                BaseConcurencyArray(InitilizerList<T>&& list) : This(new Data()) {
                    if (list.size() != 0) {
                        This->count = list.size();
                        this->This->array = new T[list.size()];
                        auto it = list.begin();
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            This->array[i] = *it; it++;
                        }
                    }
                }
               forceinline  BaseConcurencyArray(SizeT StartCount) : This(new Data()) {
                    if (StartCount != 0) {
                        This->array = new T[StartCount];
                        This->count = StartCount;
                    }
                }
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

                forceinline InternalApiCall BaseConcurencyArray(T*&& Array, SizeT StartCount) : This(new Data(MoveRValue(Array), StartCount)) {}
                
                forceinline InternalApiCall operator const T* const() const { return This->array; }
#pragma warning(pop)
            public:
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
                ///<summary>Зміна розміру</summary>
                ///<param name = 'NewSize'>Новий розмір масиву</param>
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
                                    Memory::Copy(This->array, buf, CopySize);
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

                forceinline virtual BaseConcurencyArray Clone() const override final {
                    return BaseConcurencyArray(This->array, This->count);
                }
#pragma warning(pop)
                ///<summary>Оптимізований обмін</summary>
                ///<param name = 'A'>Перший елемент</param>
                ///<param name = 'B'>Другий елемент</param>
                template<typename U>
                friend void Seed::Core::Swap(U& A, U& B);
            public:
                forceinline SizeT GetCount() const { return This->count; }
                SizeT Property(GetCount) Count;

                forceinline const T* const GetRaw() const { return This->array; }
            public:
                RefCountClass(BaseConcurencyArray, This);
            };

            namespace {
                template<typename T>
                using ConcurencyArray = BaseConcurencyArray<Seed::Core::Memory::Heap, T>;
            }

            namespace {
                template<typename T>
                using ConcurencySensitiveArray = BaseConcurencyArray<Seed::Core::Memory::SensitiveDataHeap, T>;
            }

            template<typename THeap, typename T>
            class BaseConcurencyVList final :
                Seed::IDisposable,
                public IClonable<BaseConcurencyVList<THeap, T>>,
                public Memory::Allocator<THeap>,
                public LINQ::IArray<BaseConcurencyArray, THeap, T, true> {
            private:
                struct Node {
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

                struct Data {
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
                forceinline BaseConcurencyVList() : This(new Data()) {}
                BaseConcurencyVList(InitilizerList<T>&& list) : This(new Data(list.size())) {
                    auto it = list.begin();
                    for (SizeT i = 0; i < list.size(); i++)
                    {
                        This->root->arr[i] = *it; ++it;
                    }
                }
                forceinline BaseConcurencyVList(const BaseConcurencyArray<THeap, T>& array) : This(nullptr) {
                    array.This->lock.ShareLock([&]() {
                        this->This = new Data(array.This->count, (const T*&)(array.This->array));
                        });
                }
                BaseConcurencyVList(BaseConcurencyArray<THeap, T>&& array) : This(nullptr) {
                    if (--array.This->RefCount == 0) {
                        This = new Data(array.This->count, MoveRValue(array.This->array));
                    }
                    else {
                        this->BaseConcurencyVList::BaseConcurencyVList(array);
                        array.This = nullptr;
                    }
                }
            public:
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
                BaseConcurencyVList& operator+=(BaseConcurencyArray<THeap, T>&& array) {
                    return This->lock.ExclusiveLock([&, array = MoveRValue(array)]() -> BaseConcurencyVList& mutable {
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
                BaseConcurencyArray<THeap, T> ToArray() {
                    This->lock.ShareLock([&]() mutable {
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
                        });
                }
            public:
                forceinline SizeT GetCount() const { return This->count; }
                SizeT Property(GetCount) Count;
            public:
                virtual BaseConcurencyVList Clone() const override final {
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
                template<typename LambdaT>
                BaseConcurencyVList& ForEachImpl(LambdaT&& lambda) {
                    if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                if (nodeStartIndex + i = node->size) {
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                if (nodeStartIndex + i = node->size) {
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

                template<typename LambdaT>
                const BaseConcurencyVList& ForEachImpl(LambdaT&& lambda) const {
                    if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                if (nodeStartIndex + i = node->size) {
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
                        return this->This->lock.ShareLock([&]() -> const BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < This->count; i++)
                            {
                                if (nodeStartIndex + i = node->size) {
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

                template<typename LambdaT>
                BaseConcurencyVList& ForEachImpl(LambdaT&& lambda, SizeT count) {
                    if (count > this->Count)
                        count = this->Count;
                    if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < count; i++)
                            {
                                if (nodeStartIndex + i = node->size) {
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < count; i++)
                            {
                                if (nodeStartIndex + i = node->size) {
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

                template<typename LambdaT>
                const BaseConcurencyVList& ForEachImpl(LambdaT&& lambda, SizeT count) const {
                    if (count > this->Count)
                        count = this->Count;
                    if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < count; i++)
                            {
                                if (nodeStartIndex + i = node->size) {
                                    nodeStartIndex += node->size;
                                    node = node->next;
                                }
                                lambda(node->arr[i - nodeStartIndex]);
                            }
                            return *this;
                            });
                    }
                    else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
                        return This->lock.ShareLock([&]() -> BaseConcurencyVList& {
                            Node* node = This->root;
                            SizeT nodeStartIndex = 0;
                            for (SizeT i = 0; i < count; i++)
                            {
                                if (nodeStartIndex + i = node->size) {
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
                template<typename U, typename LambdaT>
                U AggregateImpl(LambdaT&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        U val = U();
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (nodeStartIndex + i = node->size) {
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            lambda(val, node->arr[i - nodeStartIndex]);
                        }
                        return val;
                        }));
                }

                template<typename U, typename LambdaT>
                U AggregateImpl(LambdaT&& lambda, const U& Value) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        U result = Value;
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (nodeStartIndex + i = node->size) {
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            lambda(val, node->arr[i - nodeStartIndex]);
                        }
                        return MoveRValue(result);
                        }));
                }

                template<typename U, typename LambdaT>
                U AggregateImpl(LambdaT&& lambda, U&& Value) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (nodeStartIndex + i = node->size) {
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            lambda(Value, node->arr[i - nodeStartIndex]);
                        }
                        return MoveRValue(Value);
                        }));
                }

                forceinline bool AnyImpl() const { return This->count > 0; }

                template<typename Lambda>
                bool AnyImpl(Lambda&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (nodeStartIndex + i = node->size) {
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            if (lambda(node->arr[i - nodeStartIndex]))
                                return true;
                        }
                        return false;
                        }));
                }

                template<typename Lambda>
                bool AllImpl(Lambda&& lambda) const {
                    return MoveRValue(this->This->lock.ShareLock([&]() {
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (nodeStartIndex + i = node->size) {
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            if (lambda(node->arr[i - nodeStartIndex]))
                                return false;
                        }
                        return true;
                        }));
                }

                template<typename Lambda>
                T& FirstImpl(Lambda&& lambda) {
                    return This->lock.ShareLock([&]() -> T& {
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (nodeStartIndex + i = node->size) {
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            if (lambda(node->arr[i - nodeStartIndex]))
                                return node->arr[i - nodeStartIndex];
                        }

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename Lambda>
                T& LastImpl(Lambda&& lambda) {
                    return This->lock.ShareLock([&]() -> T& {
                        Node* node = This->root;
                        Node* last = nullptr;
                        SizeT nodeStartIndex = 0;
                        SizeT index = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (nodeStartIndex + i = node->size) {
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

                template<typename Lambda>
                const T& FirstImpl(Lambda&& lambda) const {
                    return This->lock.ShareLock([&]() -> const T& {
                        Node* node = This->root;
                        SizeT nodeStartIndex = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (nodeStartIndex + i = node->size) {
                                nodeStartIndex += node->size;
                                node = node->next;
                            }
                            if (lambda(node->arr[i - nodeStartIndex]))
                                return node->arr[i - nodeStartIndex];
                        }

                        Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
                        });
                }

                template<typename Lambda>
                const T& LastImpl(Lambda&& lambda) const {
                    return This->lock.ShareLock([&]() -> const T& {
                        Node* node = This->root;
                        Node* last = nullptr;
                        SizeT nodeStartIndex = 0;
                        SizeT index = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (nodeStartIndex + i = node->size) {
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
                            if (nodeStartIndex + i = node->size) {
                                nodeStartIndex += node->size;
                                tmp = node; node = node->next; delete[] node->arr; delete node;
                            }
                            lambda(MoveRValue(node->arr[i - nodeStartIndex]));
                        }
                        This->count = 0; This->last = This->root = nullptr;
                        });
                }

                template<typename Lambda>
                SizeT GetCountIfImpl(Lambda&& lambda) {
                    return This->lock.ShareLock([&]() {
                        SizeT selectedCount = 0;
                        for (SizeT i = 0; i < This->count; i++)
                        {
                            if (nodeStartIndex + i = node->size) {
                                nodeStartIndex += node->size;
                                tmp = node; node = node->next; delete[] node->arr; delete node;
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

            namespace {
                template<typename T>
                using ConcurencyVList = BaseConcurencyVList<Seed::Core::Memory::Heap, T>;
            }

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
        forceinline void Swap(Containers::BaseConcurencyArray<THeap, T>& A, Containers::BaseConcurencyArray<THeap, T>) {
            //TODO: Interlocked Exchange
            Swap(A.This, B.This);
        }

        template<typename THeap, typename T>
        forceinline void Swap(Containers::BaseConcurencyVList<THeap, T>& A, Containers::BaseConcurencyVList<THeap, T>) {
            //TODO: Interlocked Exchange
            Swap(A.This, B.This);
        }
    }
}
#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Containers.Internals.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			template<typename THeap, typename T>
			class BaseVList;

			template<typename THeap, typename T>
			class SeedKernelCall BaseArray final :
				Seed::IDisposable,
				public IClonable<BaseArray<THeap, T>>,
				public Memory::Allocator<THeap>,
				public LINQ::IArray<BaseArray, THeap, T, false> {
			private:
				struct Data : public Memory::Allocator<THeap> {
					Seed::RefCountT RefCount;
					SizeT count;
					T* array;
					Data() : RefCount(1), count(0), array(nullptr) {}
					Data(T*&& array, SizeT count) : RefCount(1), count(count), array(array) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {
					delete[] This->array;
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
							Swap(A[i], A[j]);
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
				friend class BaseVList;
                template<typename THeap, typename T, typename Enable>
                friend struct Seed::Core::NewSerilization::Details::BaseSerlizationHandler;
			protected:
#pragma warning(push)
#pragma warning(disable: 4172)

				template<typename LambdaT>
				__forceinline BaseArray& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda(This->array[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>) {
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda(This->array[i], i);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BaseArray& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda(This->array[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda(This->array[i], i);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline BaseArray& ForEachImpl(LambdaT&& lambda, SizeT count) {
					if (count > this->Count)
						count = this->Count;
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
						for (SizeT i = 0; i < count; i++)
						{
							lambda(This->array[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>) {
						for (SizeT i = 0; i < count; i++)
						{
							lambda(This->array[i], i);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BaseArray& ForEachImpl(LambdaT&& lambda, SizeT count) const {
					if (count > this->Count)
						count = this->Count;
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						for (SizeT i = 0; i < count; i++)
						{
							lambda(This->array[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						for (SizeT i = 0; i < count; i++)
						{
							lambda(This->array[i], i);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}
			protected:

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda) const {
					U val = 0;
					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(val, This->array[i]);
					}
					return val;
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, const U& Value) const {
					U result = Value;
					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(result, This->array[i]);
					}
					return MoveRValue(result);
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, U&& Value) const {
					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(Value, This->array[i]);
					}
					return MoveRValue(Value);
				}

				bool AnyImpl() const { return This->count > 0; }

				template<typename Lambda>
				__forceinline bool AnyImpl(Lambda&& lambda) const {
					for (SizeT i = 0; i < This->count; i++)
					{
						if (lambda(This->array[i]))
							return true;
					}
					return false;
				}

				template<typename Lambda>
				__forceinline bool AllImpl(Lambda&& lambda) const {
					for (SizeT i = 0; i < This->count; i++)
					{
						if (!lambda(This->array[i]))
							return false;
					}
					return true;
				}

				BaseArray& SortByImpl(bool IsDescending = true) {
					if (IsDescending) {
						auto l = [](const T& V1, const T& V2) { return V1 < V2; };
						QuickSort(This->array, 0, This->count - 1, l);
					}
					else {
						auto l = [](const T& V1, const T& V2) { return V1 > V2; };
						QuickSort(This->array, 0, This->count - 1, l);
					}
					return *this;
				}

				template<typename Lambda>
				T& FirstImpl(Lambda&& lambda) {
					for (SizeT i = 0; i < This->count; i++)
						if (lambda(This->array[i]))
							return static_cast<T&>(This->array[i]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
					SizeT Index = -1;
					for (SizeT i = 0; i < This->count; i++)
						if (lambda(This->array[i]))
							Index = i;

					if (Index != -1)
						return static_cast<T&>(This->array[Index]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
					for (SizeT i = 0; i < This->count; i++)
						if (lambda(This->array[i]))
							return static_cast<T&>(This->array[i]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
					SizeT Index = -1;
					for (SizeT i = 0; i < This->count; i++)
						if (lambda(This->array[i]))
							Index = i;

					if (Index != -1)
						return static_cast<T&>(This->array[Index]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				BaseArray& ReverseImpl() {
					for (SizeT i = 0; i < This->count / 2; i++) {
						Swap(This->array[i], This->array[(This->count - 1) - i]);
					}
					return *this;
				}

				__forceinline void UnsafeSet(const T& Value, SizeT Index) {
					This->array[Index] = Value;
				}

				__forceinline BaseArray& RemoveAllImpl() {
					delete[] this->This->array;
					this->This->array = nullptr;
					this->This->count = 0;
					return *this;
				}

				template<typename Lambda>
				__forceinline BaseArray& RemoveAllImpl(Lambda&& lambda) {
					for (SizeT i = 0; i < This->Count; i++) {
						lambda(MoveRValue(This->array[i]));
					}
					delete[] this->This->array;
					this->This->array = nullptr;
					this->This->count = 0;
                    return *this;
				}
#pragma warning(pop)
			public:
#pragma warning(push)
#pragma warning(disable: 6386)
				BaseArray() : This(new Data()) {}
				BaseArray(InitilizerList<T>&& list) : This(new Data()) {
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
				BaseArray(SizeT StartCount) : This(new Data()) {
					if (StartCount != 0) {
						This->array = new T[StartCount];
						This->count = StartCount;
					}
				}
				InternalApiCall BaseArray(const T* Array, SizeT StartCount) : This(new Data()) {
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

				InternalApiCall BaseArray(T*&& Array, SizeT StartCount) : This(new Data(MoveRValue(Array), StartCount))
				{}

				InternalApiCall operator const T* const() const { return This->array; }
#pragma warning(pop)
			public:
				T& operator[](SizeT Index) {
					return This->array[Index];
				}
				const T& operator[](SizeT Index) const {
					return This->array[Index];
				}
#pragma warning(push)
#pragma warning(disable: 4996)
				///<summary>Зміна розміру</summary>
				///<param name = 'NewSize'>Новий розмір масиву</param>
				__forceinline BaseArray& Resize(SizeT NewSize) {
					if (this->This->array == nullptr || this->This->count == 0)
					{
						this->This->array = new T[NewSize];
						this->This->count = NewSize;
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
				}

				virtual BaseArray Clone() const override final {
					return BaseArray(This->array, This->count);
				}
#pragma warning(pop)
				///<summary>Оптимізований обмін</summary>
				///<param name = 'A'>Перший елемент</param>
				///<param name = 'B'>Другий елемент</param>
				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);
			public:
				SizeT GetCount() const { return This->count; }
				SizeT Property(GetCount) Count;
			public:
				RefCountClass(BaseArray, This);
			};

			namespace {
				template<typename T>
				using Array = BaseArray<Seed::Core::Memory::Heap, T>;
			}

			namespace {
				template<typename T>
				using SensitiveArray = BaseArray<Seed::Core::Memory::SensitiveDataHeap, T>;
			}

			namespace {
				using Blob = BaseArray<Seed::Core::Memory::Heap, Byte>;
			}

			namespace {
				using SensitiveBlob = BaseArray<Seed::Core::Memory::SensitiveDataHeap, Byte>;
			}

			template<typename THeap, typename T>
			class SeedKernelCall BaseVList final :
				Seed::IDisposable,
				public IClonable<BaseVList<THeap, T>>,
				public Memory::Allocator<THeap>,
				public LINQ::IArray<BaseVList, THeap, T, false> {
			private:
				struct Node {
					T* arr;
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

					forceinline Data() : RefCount(1), count(0), root(nullptr), last(nullptr) {}
					forceinline Data(SizeT count) : RefCount(1), count(count), root(new Node(count)), last(root) {}
					forceinline Data(SizeT count, T*&& arr) : RefCount(1), count(count), root(new Node(count, MoveRValue(arr))), last(root) {}
					forceinline Data(SizeT count, T*& arr) : RefCount(1), count(count), root(new Node(count, arr)), last(root) {}
				};
			private:
				Data* This;
			public:
				BaseVList() : This(new Data()) {}
				BaseVList(InitilizerList<T>&& list) : This(new Data(list.size())) {
					auto it = list.begin();
					for (SizeT i = 0; i < list.size(); i++)
					{
						This->root->arr[i] = *it; ++it;
					}
				}
				BaseVList(const BaseArray<THeap, T>& array) : This(nullptr) {
					this->This = new Data(array.This->count, static_cast<T*&>(array.This->array));
				}
				BaseVList(BaseArray<THeap, T>&& array) : This(nullptr) {
					if (--array.This->RefCount == 0) {
						This = new Data(array.This->count, MoveRValue(array.This->array));
					}
					else {
						this->BaseVList::BaseVList(array);
						array.This = nullptr;
					}
				}
			public:
				const T& operator[](SizeT Index) const {
					if (Index >= This->count)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					else {
						auto* node = This->root;
						SizeT nodeStartIndex = 0;
						while (true) {
							if (nodeStartIndex + node->size > Index)
								return node->arr[Index - nodeStartIndex];
							else {
								nodeStartIndex += node->size; node = node->next;
							}
						}
					}
				}
				T& operator[](SizeT Index) {
					if (Index >= This->count)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					else {
						auto* node = This->root;
						SizeT nodeStartIndex = 0;
						while (true) {
							if (nodeStartIndex + node->size > Index)
								return node->arr[Index - nodeStartIndex];
							else {
								nodeStartIndex += node->size; node = node->next;
							}
						}
					}
				}
			public:
				BaseVList& operator+=(const BaseArray<THeap, T>& array) {
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
				}
				BaseVList& operator+=(BaseArray<THeap, T>&& array) {
					if (array.This->RefCount == 1) {
						if(This->root == nullptr)
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
					}
				}
			public:
				BaseArray<THeap, T> toArray() {
					BaseArray<THeap, T> array(this->This->count);
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
				}

				virtual BaseVList Clone() const override final {
					BaseVList result;
					if (this->This->root != nullptr) {
						result.This->root = result.This->last = new Node(this->This->root->size, this->This->root->arr);
						if (this->This->root != this->This->last) {
							Node* node = this->This->last;
							while (node != nullptr) {
								result.This->last = result.This->last->next = new Node(node->size, node->arr);
								node = node->next;
							}
						}
						result.This->count = this->This->count;
					}
					return result;
				}
			public:
				SizeT GetCount() const { return This->count; }
				SizeT Property(GetCount) Count;

				template<typename U>
				friend void Seed::Core::Swap(U& a, U& b);
			protected:
				virtual void Dispose() override final {
					Node* node = This->root;
					Node* tmp;
					SizeT nodeStartIndex = 0;
					while (node != nullptr)
					{
						tmp = node->next; delete[] node->arr; delete node;  node = tmp;
					}
					This->count = 0; This->last = This->root = nullptr;
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
                friend struct Seed::Core::NewSerilization::Details::BaseSerlizationHandler;
			protected:
#pragma warning(push)
#pragma warning(disable: 4172)
				template<typename LambdaT>
				__forceinline BaseVList& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
						Node* node = This->root;
						SizeT nodeStartIndex = 0;
						for (SizeT i = 0; i < This->count; i++)
						{
							if (nodeStartIndex + i == node->size) {
								nodeStartIndex += node->size;
								node = node->next;
							}
							lambda(node->arr[i - nodeStartIndex]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>) {
						Node* node = This->root;
						SizeT nodeStartIndex = 0;
						for (SizeT i = 0; i < This->count; i++)
						{
							if (nodeStartIndex + i == node->size) {
								nodeStartIndex += node->size;
								node = node->next;
							}
							lambda(node->arr[i - nodeStartIndex], i);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BaseVList& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						Node* node = This->root;
						SizeT nodeStartIndex = 0;
						for (SizeT i = 0; i < This->count; i++)
						{
							if (nodeStartIndex + i == node->size) {
								nodeStartIndex += node->size;
								node = node->next;
							}
							lambda(node->arr[i - nodeStartIndex]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						Node* node = This->root;
						SizeT nodeStartIndex = 0;
						for (SizeT i = 0; i < This->count; i++)
						{
							if (nodeStartIndex + i == node->size) {
								nodeStartIndex += node->size;
								node = node->next;
							}
							lambda(node->arr[i - nodeStartIndex], i);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline BaseVList& ForEachImpl(LambdaT&& lambda, SizeT count) {
					if (count > this->Count)
						count = this->Count;
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
						Node* node = This->root;
						SizeT nodeStartIndex = 0;
						for (SizeT i = 0; i < count; i++)
						{
							if (nodeStartIndex + i == node->size) {
								nodeStartIndex += node->size;
								node = node->next;
							}
							lambda(node->arr[i - nodeStartIndex]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>) {
						Node* node = This->root;
						SizeT nodeStartIndex = 0;
						for (SizeT i = 0; i < count; i++)
						{
							if (nodeStartIndex + i == node->size) {
								nodeStartIndex += node->size;
								node = node->next;
							}
							lambda(node->arr[i - nodeStartIndex], i);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BaseVList& ForEachImpl(LambdaT&& lambda, SizeT count) const {
					if (count > this->Count)
						count = this->Count;
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						Node* node = This->root;
						SizeT nodeStartIndex = 0;
						for (SizeT i = 0; i < count; i++)
						{
							if (nodeStartIndex + i == node->size) {
								nodeStartIndex += node->size;
								node = node->next;
							}
							lambda(node->arr[i - nodeStartIndex]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						Node* node = This->root;
						SizeT nodeStartIndex = 0;
						for (SizeT i = 0; i < count; i++)
						{
							if (nodeStartIndex + i == node->size) {
								nodeStartIndex += node->size;
								node = node->next;
							}
							lambda(node->arr[i - nodeStartIndex], i);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}

			protected:
				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda) const {
					U val = U();
					Node* node = This->root;
					SizeT nodeStartIndex = 0;
					for (SizeT i = 0; i < This->count; i++)
					{
						if (nodeStartIndex + i == node->size) {
							nodeStartIndex += node->size;
							node = node->next;
						}
						lambda(val, node->arr[i - nodeStartIndex]);
					}
					return val;
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, const U& Value) const {
					T result = Value;
					Node* node = This->root;
					SizeT nodeStartIndex = 0;
					for (SizeT i = 0; i < This->count; i++)
					{
						if (nodeStartIndex + i == node->size) {
							nodeStartIndex += node->size;
							node = node->next;
						}
						lambda(val, node->arr[i - nodeStartIndex]);
					}
					return MoveRValue(result);
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, U&& Value) const {
					Node* node = This->root;
					SizeT nodeStartIndex = 0;
					for (SizeT i = 0; i < This->count; i++)
					{
						if (nodeStartIndex + i == node->size) {
							nodeStartIndex += node->size;
							node = node->next;
						}
						lambda(Value, node->arr[i - nodeStartIndex]);
					}
					return MoveRValue(Value);
				}

				bool AnyImpl() const { return This->count > 0; }

				template<typename Lambda>
				__forceinline bool AnyImpl(Lambda&& lambda) const {
					Node* node = This->root;
					SizeT nodeStartIndex = 0;
					for (SizeT i = 0; i < This->count; i++)
					{
						if (nodeStartIndex + i == node->size) {
							nodeStartIndex += node->size;
							node = node->next;
						}
						if (lambda(node->arr[i - nodeStartIndex]))
							return true;
					}
					return false;
				}

				template<typename Lambda>
				__forceinline bool AllImpl(Lambda&& lambda) const {
					Node* node = This->root;
					SizeT nodeStartIndex = 0;
					for (SizeT i = 0; i < This->count; i++)
					{
						if (nodeStartIndex + i == node->size) {
							nodeStartIndex += node->size;
							node = node->next;
						}
						if (lambda(node->arr[i - nodeStartIndex]))
							return false;
					}
					return true;
				}

				template<typename Lambda>
				T& FirstImpl(Lambda&& lambda) {
					Node* node = This->root;
					SizeT nodeStartIndex = 0;
					for (SizeT i = 0; i < This->count; i++)
					{
						if (nodeStartIndex + i == node->size) {
							nodeStartIndex += node->size;
							node = node->next;
						}
						if (lambda(node->arr[i - nodeStartIndex]))
							return node->arr[i - nodeStartIndex];
					}

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
					Node* node = This->root;
					Node* last = nullptr;
					SizeT nodeStartIndex = 0;
					SizeT index = 0;
					for (SizeT i = 0; i < This->count; i++)
					{
						if (nodeStartIndex + i == node->size) {
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
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
					Node* node = This->root;
					SizeT nodeStartIndex = 0;
					for (SizeT i = 0; i < This->count; i++)
					{
						if (nodeStartIndex + i == node->size) {
							nodeStartIndex += node->size;
							node = node->next;
						}
						if (lambda(node->arr[i - nodeStartIndex]))
							return node->arr[i - nodeStartIndex];
					}

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
					Node* node = This->root;
					Node* last = nullptr;
					SizeT nodeStartIndex = 0;
					SizeT index = 0;
					for (SizeT i = 0; i < This->count; i++)
					{
						if (nodeStartIndex + i == node->size) {
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
				}

				__forceinline void UnsafeSet(const T& Value, SizeT Index) {
					This->root->arr[Index] = Value;
				}

				__forceinline BaseVList& RemoveAllImpl() {
					Node* node = This->root;
					Node* tmp;
					SizeT nodeStartIndex = 0;
					while (node != nullptr)
					{
						tmp = node; node = node->next; delete[] node->arr; delete node;
					}
					This->count = 0; This->last = This->root = nullptr;

					return *this;
				}

				template<typename Lambda>
				__forceinline BaseVList& RemoveAllImpl(Lambda&& lambda) {
					Node* node = This->root;
					Node* tmp;
					SizeT nodeStartIndex = 0;
					for (SizeT i = 0; i < This->count; i++)
					{
						if (nodeStartIndex + i == node->size) {
							nodeStartIndex += node->size;
							tmp = node; node = node->next; delete[] tmp->arr; delete tmp;
						}
						lambda(MoveRValue(node->arr[i - nodeStartIndex]));
					}
					This->count = 0; This->last = This->root = nullptr;
                    return *this;
				}
#pragma warning(pop)
			public:
				RefCountClass(BaseVList, This)
			};

			namespace {
				template<typename T>
				using VList = BaseVList<Seed::Core::Memory::Heap, T>;
			}

			namespace {
				template<typename T>
				using SensitiveVList = BaseVList<Seed::Core::Memory::SensitiveDataHeap, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename T>
				struct IsArrayT<Seed::Core::Containers::BaseArray<THeap, T>> : LogicTrue {};

				template<typename THeap, typename T>
				struct IsVListT<Seed::Core::Containers::BaseVList<THeap, T>> : LogicTrue {};
			}
		}

		template<typename THeap, typename T>
		__forceinline void Swap(Containers::BaseArray<THeap, T>& A, Containers::BaseArray<THeap, T>) {
			Swap(A.This, B.This);
		}

		template<typename THeap, typename T>
		__forceinline void Swap(Containers::BaseVList<THeap, T>& A, Containers::BaseVList<THeap, T>) {
			Swap(A.This, B.This);
		}
	}
}
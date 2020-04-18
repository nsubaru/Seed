#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Containers.Internals.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			template<typename THeap, SizeT TSize, typename T>
			class BaseFixedArray final :
				Seed::IDisposable,
				public LINQ::IFixedArray<BaseFixedArray, THeap, T, TSize, false>,
				public Memory::Allocator<THeap>,
				public IClonable<BaseFixedArray<THeap, TSize, T>>
			{
			private:
				struct Data : public Memory::Allocator<THeap> {
					Seed::RefCountT RefCount;
					T array[TSize] = {};

					forceinline Data() : RefCount(1) {}
				};
			private:
				Data* This;
			protected:
				forceinline virtual void Dispose() override {}
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
			public:
				forceinline SizeT GetCount() const { return TSize; }
				SizeT Property(GetCount) Count;
			protected:
				template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollection;
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				friend class LINQ::IFixedCollection;
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				friend class LINQ::IFixedArray;
				template<template<typename THeap, typename T> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollectionBase;
                template<typename THeap, typename T, typename Enable>
                friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
			protected:
#pragma warning(push)
#pragma warning(disable: 4172)

				template<typename LambdaT>
				BaseFixedArray& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
						for (SizeT i = 0; i < TSize; i++)
						{
							lambda(This->array[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>) {
						for (SizeT i = 0; i < TSize; i++)
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
				const BaseFixedArray& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						for (SizeT i = 0; i < TSize; i++)
						{
							lambda(This->array[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						for (SizeT i = 0; i < TSize; i++)
						{
							lambda(This->array[i], i);
						}
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename U, typename LambdaT>
				U AggregateImpl(LambdaT&& lambda) const {
					U val = U();
					for (SizeT i = 0; i < TSize; i++)
					{
						lambda(val, This->array[i]);
					}
					return val;
				}

				template<typename U, typename LambdaT>
				T AggregateImpl(LambdaT&& lambda, const U& Value) const {
					U result = Value;
					for (SizeT i = 0; i < TSize; i++)
					{
						lambda(result, This->array[i]);
					}
					return MoveRValue(result);
				}

				template<typename U, typename LambdaT>
				U AggregateImpl(LambdaT&& lambda, U&& Value) const {
					for (SizeT i = 0; i < TSize; i++)
					{
						lambda(Value, This->array[i]);
					}
					return MoveRValue(Value);
				}

				forceinline bool AnyImpl() const { return TSize > 0; }

				template<typename Lambda>
				bool AnyImpl(Lambda&& lambda) const {
					for (SizeT i = 0; i < TSize; i++)
					{
						if (lambda(This->array[i]))
							return true;
					}
					return false;
				}

				template<typename Lambda>
				bool AllImpl(Lambda&& lambda) const {
					for (SizeT i = 0; i < TSize; i++)
					{
						if (!lambda(This->array[i]))
							return false;
					}
					return true;
				}

				BaseFixedArray& SortByImpl(bool IsDescending = true) {
					if (IsDescending) {
						auto l = [](const T& V1, const T& V2) { return V1 < V2; };
						QuickSort(This->array, 0, TSize - 1, l);
					}
					else {
						auto l = [](const T& V1, const T& V2) { return V1 > V2; };
						QuickSort(This->array, 0, TSize - 1, l);
					}
					return *this;
				}

				template<typename Lambda>
				forceinline BaseFixedArray& SortByLambdaImpl(Lambda&& lambda) {
					if constexpr (TSize == 0)
					{
						return *this;
					}
					else
					{
						QuickSort(This->array, 0, TSize - 1, lambda);
						return *this;
					}
				}

				template<typename Lambda>
				T& FirstImpl(Lambda&& lambda) {
					for (SizeT i = 0; i < TSize; i++)
						if (lambda(This->array[i]))
							return static_cast<T&>(This->array[i]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
					SizeT Index = -1;
					for (SizeT i = 0; i < TSize; i++)
						if (lambda(This->array[i]))
							Index = i;

					if (Index != -1)
						return static_cast<T&>(This->array[Index]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
					for (SizeT i = 0; i < TSize; i++)
						if (lambda(This->array[i]))
							return static_cast<T&>(This->array[i]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
					SizeT Index = -1;
					for (SizeT i = 0; i < TSize; i++)
						if (lambda(This->array[i]))
							Index = i;

					if (Index != -1)
						return static_cast<T&>(This->array[Index]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				forceinline BaseFixedArray& ReverseImpl() {
					for (SizeT i = 0; i < TSize / 2; i++) {
						Swap(This->array[i], This->array[(TSize - 1) - i]);
					}
					return *this;
				}

				forceinline void UnsafeSet(const T& Value, SizeT Index) {
					This->array[Index] = Value;
				}

				forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->array[Index] = MoveRValue(Value);
				}

				forceinline BaseFixedArray& RemoveAllImpl() {
					for (SizeT i = 0; i < TSize; i++) {
						This->array[i].~T();
					}
					return *this;
				}

				template<typename Lambda>
				forceinline BaseFixedArray& RemoveAllImpl(Lambda&& lambda) {
					for (SizeT i = 0; i < TSize; i++) {
						lambda(MoveRValue(This->array[i]));
					}
					return *this;
				}
#pragma warning(pop)
			public:
#pragma warning(push)
#pragma warning(disable: 6386)
				forceinline constexpr BaseFixedArray() : This(new Data()) {}

				constexpr BaseFixedArray(InitilizerList<T>&& list) : This(new Data()) {
					if (list.size() != 0) {
						SizeT ActuallyCopyied = (TSize < list.size()) ? TSize : list.size();
						auto it = list.begin();
						SizeT i;
						for (i = 0; i < ActuallyCopyied; i++)
						{
							This->array[i] = *it; it++;
						}
					}
				}
#pragma warning(pop)
			public:
				forceinline constexpr T& operator[](SizeT Index) {
					if (Index < TSize)
					{
						return This->array[Index];
					}
					else
					{
						Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, Index, TSize);
					}
				}
				forceinline constexpr const T& operator[](SizeT Index) const {
					if (Index < TSize)
					{
						return This->array[Index];
					}
					else
					{
						Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, Index, TSize);
					}
				}

#pragma warning(push)
#pragma warning(disable : 4996)
				virtual BaseFixedArray Clone() const override final {
					BaseFixedArray result;
					if constexpr (TypeTraits::IsPodV<T>) {
						Memory::Copy(This->array, result.This->array, TSize * sizeof(T));
					}
					else {
						for (SizeT i = 0; i < TSize; i++)
						{
							result.This->array[i] = This->array[i];
						}
					}
					return result;
				}
#pragma warning(pop)
				
				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);
			public:
				RefCountClass(BaseFixedArray, This)
			};

			namespace {
				template<SizeT TSize, typename T>
				using FixedArray = BaseFixedArray<Seed::Core::Memory::Heap, TSize, T>;
			}

			namespace {
				template<SizeT TSize, typename T>
				using SensitiveFixedArray = BaseFixedArray<Seed::Core::Memory::SensitiveDataHeap, TSize, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, SizeT TSize, typename T>
				struct IsFixedArrayT<Seed::Core::Containers::BaseFixedArray<THeap, TSize, T>> : LogicTrue {};
			}
		}

		template<typename THeap, SizeT TSize, typename T>
		forceinline void Swap(Containers::BaseFixedArray<THeap, TSize, T>& A, Containers::BaseFixedArray<THeap, TSize, T>& B) {
			Swap(A.This, B.This);
		}
	}
}
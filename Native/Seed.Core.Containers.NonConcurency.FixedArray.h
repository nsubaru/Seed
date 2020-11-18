#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Containers.Internals.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			/// <summary>
			/// Масив з фіксованою кількістю елементів
			/// </summary>
			template<typename THeap, SizeT TSize, typename T>
			class BaseFixedArray final :
				Seed::IDisposable,
				public LINQ::IFixedArray<BaseFixedArray, THeap, T, TSize, false>,
				public Memory::Allocator<THeap>,
				public IClonable<BaseFixedArray<THeap, TSize, T>>
			{
			public: 
				using RawArray = T(&)[TSize];
			private:
				struct Data : public Memory::Allocator<THeap> {
					Seed::RefCountT RefCount;
					T array[TSize] = {};

					ForceInline Data() : RefCount(1) {}
				};
			private:
				Data* This;
			protected:
				ForceInline virtual void Dispose() override {}
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
				ForceInline SizeT GetCount() const { return TSize; }
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
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				friend class LINQ::IFixedCollection;
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

				ForceInline bool AnyImpl() const { return TSize > 0; }

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
				ForceInline BaseFixedArray& SortByLambdaImpl(Lambda&& lambda) {
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

				ForceInline BaseFixedArray& ReverseImpl() {
					for (SizeT i = 0; i < TSize / 2; i++) {
						Swap(This->array[i], This->array[(TSize - 1) - i]);
					}
					return *this;
				}

				ForceInline void UnsafeSet(const T& Value, SizeT Index) {
					This->array[Index] = Value;
				}

				ForceInline void UnsafeSet(T&& Value, SizeT Index) {
					This->array[Index] = MoveRValue(Value);
				}

				ForceInline BaseFixedArray& RemoveAllImpl() {
					for (SizeT i = 0; i < TSize; i++) {
						This->array[i].~T();
					}
					return *this;
				}

				template<typename Lambda>
				ForceInline BaseFixedArray& RemoveAllImpl(Lambda&& lambda) {
					for (SizeT i = 0; i < TSize; i++) {
						lambda(MoveRValue(This->array[i]));
					}
					return *this;
				}
#pragma warning(pop)
			public:
#pragma warning(push)
#pragma warning(disable: 6386)
				/// <summary>
				/// Пустий конструктор ініціалізовує масив стандартним значенням
				/// </summary>
				ForceInline constexpr BaseFixedArray() : This(new Data()) {}

				/// <summary>
				/// Конструктор з списку ініціалізації, копіює всі елементи з списку, 
				/// якщо їх більше то тільки ті які влізли, 
				/// менше то решта залишаються ініціалізовані стандартним значення
				/// </summary>
				/// <param name="list">Список елементів</param>
				constexpr BaseFixedArray(InitilizerList<T>&& list) : This(new Data()) {
					if (list.size() != 0) {
						SizeT ActuallyCopyied = (TSize < list.size()) ? TSize : list.size();
						auto it = list.begin();
						SizeT i;
						for (i = 0; i < ActuallyCopyied; i++)
						{
							This->array[i] = MoveRValue(*it); it++;
						}
					}
				}

				/// <summary>
				/// Конструктор з статичного масиву який копіюється
				/// </summary>
				/// <param name="arr">Статичний масив</param>
				ForceInline constexpr BaseFixedArray(const T(&arr)[TSize]) : This(new Data) {
					for (SizeT i = 0; i < TSize; i++)
					{
						This->array[i] = arr[i];
					}
				}
#pragma warning(pop)
			public:
				/// <summary>
				/// Оператор доступу до елементів масиву
				/// </summary>
				/// <param name="Index"></param>
				/// <returns>Силка на елементи</returns>
				ForceInline constexpr T& operator[](SizeT Index) {
					if (Index < TSize)
					{
						return This->array[Index];
					}
					else
					{
						Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, Index, TSize);
					}
				}

				/// <summary>
				/// Константний оператор доступу до елементів масиву
				/// </summary>
				/// <param name="Index"></param>
				/// <returns>Константна силка на елементи</returns>
				ForceInline constexpr const T& operator[](SizeT Index) const {
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
				/// <summary>
				/// Клонує об'єкт масиву
				/// </summary>
				/// <returns>Новий об'єкт з копією всіх елементів</returns>
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

				operator const RawArray () const {
					return This->array;
				}
				
				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);
			public:
				template<typename THeap, typename T, SizeT TSize1, SizeT TSize2>
				friend BaseFixedArray<THeap, TSize1 + TSize2, T> Combine(
					const BaseFixedArray<THeap, TSize1, T>& first,
					const BaseFixedArray<THeap, TSize2, T>& second
				);
			public:
				RefCountClass(BaseFixedArray, This)
			};

			template<typename THeap, typename T, SizeT TSize1, SizeT TSize2>
			BaseFixedArray<THeap, TSize1 + TSize2, T> Combine(
				const BaseFixedArray<THeap, TSize1, T>& first,
				const BaseFixedArray<THeap, TSize2, T>& second
			) {
				BaseFixedArray<THeap, TSize1 + TSize2, T> result;

				if constexpr (TypeTraits::IsPodV<T>)
				{
					Memory::Copy(first.This->array, result.This->array, TSize1 * sizeof(T));
					Memory::Copy(second.This->array, result.This->array + TSize1 * sizeof(T), TSize2 * sizeof(T));
				}
				else
				{
					for (SizeT i = 0; i < TSize1; i++)
					{
						result.This->array[i] = first.This->array[i];
					}
					for (SizeT i = 0; i < TSize2; i++)
					{
						result.This->array[TSize1 + i] = second.This->array[i];
					}
				}

				return result;
			}

			/// <summary>
			/// Масив з фіксованою кількістю елементів
			/// </summary>
			namespace {
				template<SizeT TSize, typename T>
				using FixedArray = BaseFixedArray<Seed::Core::Memory::Heap, TSize, T>;
			}

			/// <summary>
			/// Масив з фіксованою кількістю елементів в захищеній кучі
			/// </summary>
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
		ForceInline void Swap(Containers::BaseFixedArray<THeap, TSize, T>& A, Containers::BaseFixedArray<THeap, TSize, T>& B) {
			Swap(A.This, B.This);
		}
	}
}
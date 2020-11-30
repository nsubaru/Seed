#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Containers.Internals.h"

#pragma warning(push)
#pragma warning(disable: 4172)
#pragma warning(disable: 6386)
#pragma warning(disable : 4996)

namespace Seed {
	inline namespace Core {
		namespace Containers {
			/// <summary>
			/// Масив з фіксованою кількістю елементів і підтримкою багатопотоковості
			/// </summary>
			template<typename THeap, SizeT TSize, typename T>
			class BaseConcurencyFixedArray final :
				Seed::IDisposable,
				public LINQ::IFixedArray<BaseConcurencyFixedArray, THeap, T, TSize, true>,
				public Memory::Allocator<THeap>,
				public IClonable<BaseConcurencyFixedArray<THeap, TSize, T>>
				{
			private:
				struct Data : public Memory::Allocator<THeap> {
					Seed::RefCountT RefCount;
					Threading::SRWLock lock;
					T array[TSize] = {};

					ForceInline Data() : RefCount(1), lock() {}
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
				template<typename Lambda>
				BaseConcurencyFixedArray& ForEachImpl(Lambda&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyFixedArray& InlineLambda {
							for (SizeT i = 0; i < TSize; i++)
							{
								lambda(This->array[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&, SizeT>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyFixedArray& InlineLambda {
							for (SizeT i = 0; i < TSize; i++)
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

				template<typename Lambda>
				const BaseConcurencyFixedArray& ForEachImpl(Lambda&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&>) {
						return this->This->lock.ShareLock([&]() -> const BaseConcurencyFixedArray& InlineLambda {
							for (SizeT i = 0; i < TSize; i++)
							{
								lambda(This->array[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&, SizeT>) {
						return this->This->lock.ShareLock([&]() -> const BaseConcurencyFixedArray& InlineLambda {
							for (SizeT i = 0; i < TSize; i++)
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

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda) const {
					return MoveRValue(this->This->lock.ShareLock([&]() InlineLambda{
						U val = U();
						for (SizeT i = 0; i < TSize; i++)
						{
							lambda(val, This->array[i]);
						}
						return val;
						}));
				}

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda, const U& Value) const {
					return MoveRValue(this->This->lock.ShareLock([&]() InlineLambda {
						U result = Value;
						for (SizeT i = 0; i < TSize; i++)
						{
							lambda(result, This->array[i]);
						}
						return MoveRValue(result);
						}));
				}

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda, U&& Value) const {
					return MoveRValue(this->This->lock.ShareLock([&]() InlineLambda {
						for (SizeT i = 0; i < TSize; i++)
						{
							lambda(Value, This->array[i]);
						}
						return MoveRValue(Value);
						}));
				}

				ForceInline bool AnyImpl() const { return TSize > 0; }

				template<typename Lambda>
				bool AnyImpl(Lambda&& lambda) const {
					return MoveRValue(this->This->lock.ShareLock([&]() InlineLambda {
						for (SizeT i = 0; i < TSize; i++)
						{
							if (lambda(This->array[i]))
								return true;
						}
						return false;
						}));
				}

				template<typename Lambda>
				bool AllImpl(Lambda&& lambda) const {
					return MoveRValue(this->This->lock.ShareLock([&]() InlineLambda {
						for (SizeT i = 0; i < TSize; i++)
						{
							if (!lambda(This->array[i]))
								return false;
						}
						return true;
						}));
				}

				ForceInline BaseConcurencyFixedArray& SortByImpl(bool IsDescending = true) {
					return This->lock.ExclusiveLock([&]() -> BaseConcurencyFixedArray& InlineLambda {
						if (IsDescending) {
							auto l = [](const T& V1, const T& V2) { return V1 < V2; };
							QuickSort(This->array, 0, TSize - 1, l);
						}
						else {
							auto l = [](const T& V1, const T& V2) { return V1 > V2; };
							QuickSort(This->array, 0, TSize - 1, l);
						}
						return *this;
						});
				}

				template<typename Lambda>
				ForceInline BaseConcurencyFixedArray& SortByLambdaImpl(Lambda&& lambda) {
					if constexpr (TSize == 0)
					{
						return *this;
					}
					else
					{
						return This->lock.ExclusiveLock([&] () -> BaseConcurencyFixedArray& InlineLambda
						{
							QuickSort(This->array, 0, TSize - 1, lambda);
							return *this;
						});
					}
				}

				template<typename Lambda>
				T& FirstImpl(Lambda&& lambda) {
					return This->lock.ShareLock([&]() -> T& InlineLambda {
						for (SizeT i = 0; i < TSize; i++)
							if (lambda(This->array[i]))
								return static_cast<T&>(This->array[i]);

						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						});
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
					return This->lock.ShareLock([&]() -> T& InlineLambda {
						SizeT Index = -1;
						for (SizeT i = 0; i < TSize; i++)
							if (lambda(This->array[i]))
								Index = i;

						if (Index != -1)
							return static_cast<T&>(This->array[Index]);

						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						});
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
					return This->lock.ShareLock([&]() -> const T& InlineLambda {
						for (SizeT i = 0; i < TSize; i++)
							if (lambda(This->array[i]))
								return static_cast<T&>(This->array[i]);

						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						});
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
					return This->lock.ShareLock([&]() -> const T& InlineLambda {
						SizeT Index = -1;
						for (SizeT i = 0; i < TSize; i++)
							if (lambda(This->array[i]))
								Index = i;

						if (Index != -1)
							return static_cast<T&>(This->array[Index]);

						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						});
				}

				BaseConcurencyFixedArray& ReverseImpl() {
					return This->lock.ExclusiveLock([&]() -> BaseConcurencyFixedArray& InlineLambda {
						for (SizeT i = 0; i < TSize / 2; i++) {
							Seed::Core::Swap(static_cast<T&>(This->array[i]), static_cast<T&>(This->array[(TSize - 1) - i]));
						}
						return *this;
						});
				}

				ForceInline void UnsafeSet(const T& Value, SizeT Index) {
					This->array[Index] = Value;
				}

				ForceInline void UnsafeSet(T&& Value, SizeT Index) {
					This->array[Index] = MoveRValue(Value);
				}

				BaseConcurencyFixedArray& RemoveAllImpl() {
					return This->lock.ExclusiveLock([&]() -> BaseConcurencyFixedArray& InlineLambda {
						for (SizeT i = 0; i < TSize; i++) {
							This->array[i].~T();
						}
						return *this;
						});
				}

				template<typename Lambda>
				BaseConcurencyFixedArray& RemoveAllImpl(Lambda&& lambda) {
					return This->lock.ExclusiveLock([&]() -> BaseConcurencyFixedArray& InlineLambda {
						for (SizeT i = 0; i < TSize; i++) {
							lambda(MoveRValue(This->array[i]));
						}
						return *this;
						});
				}
			public:
				/// <summary>
				/// Пустий конструктор ініціалізовує масив стандартним значенням
				/// </summary>
				ForceInline constexpr BaseConcurencyFixedArray() : This(new Data()) {}

				/// <summary>
				/// Конструктор з списку ініціалізації, копіює всі елементи з списку, 
				/// якщо їх більше то тільки ті які влізли, 
				/// менше то решта залишаються ініціалізовані стандартним значення
				/// </summary>
				/// <param name="list">Список елементів</param>
				constexpr BaseConcurencyFixedArray(InitilizerList<T>&& list) : This(new Data()) {
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
			public:
				/// <summary>
				/// Оператор доступу до елементів масиву
				/// </summary>
				/// <param name="Index"></param>
				/// <returns>Силка на елементи</returns>
				ForceInline constexpr T& operator[](SizeT Index) {
					return This->lock.ShareLock([&]() -> T& InlineLambda {
						if (Index < TSize)
						{
							return This->array[Index];
						}
						else
						{
							Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, Index, TSize);
						}
						});
				}

				/// <summary>
				/// Константний оператор доступу до елементів масиву
				/// </summary>
				/// <param name="Index"></param>
				/// <returns>Константна силка на елементи</returns>
				ForceInline constexpr const T& operator[](SizeT Index) const {
					return This->lock.ShareLock([&]() -> const T& InlineLambda {
						if (Index < TSize)
						{
							return This->array[Index];
						}
						else
						{
							Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, Index, TSize);
						}
						});
				}

				/// <summary>
				/// Клонує об'єкт масиву
				/// </summary>
				/// <returns>Новий об'єкт з копією всіх елементів</returns>
				virtual BaseConcurencyFixedArray Clone() const override final {
					return This->lock.ShareLock([&]() InlineLambda {
						BaseConcurencyFixedArray result;
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
						});
				}
				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);
			public:
				RefCountClass(BaseConcurencyFixedArray, This)
			};

			/// <summary>
			/// Масив з фіксованою кількістю елементів і підтримкою багатопотоковості
			/// </summary>
			namespace {
				template<SizeT TSize, typename T>
				using ConcurencyFixedArray = BaseConcurencyFixedArray<Seed::Core::Memory::Heap, TSize, T>;
			}

			/// <summary>
			/// Масив з фіксованою кількістю елементів і підтримкою багатопотоковості в захищеній кучі
			/// </summary>
			namespace {
				template<SizeT TSize, typename T>
				using ConcurencySensitiveFixedArray = BaseConcurencyFixedArray<Seed::Core::Memory::SensitiveDataHeap, TSize, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, SizeT TSize, typename T>
				struct IsFixedArrayT<Seed::Core::Containers::BaseConcurencyFixedArray<THeap, TSize, T>> : LogicTrue {};
			}
		}

		template<typename THeap, SizeT TSize, typename T>
		ForceInline void Swap(Containers::BaseConcurencyFixedArray<THeap, TSize, T>& A, Containers::BaseConcurencyFixedArray<THeap, TSize, T>& B) {
			//TODO: Interlocked Exchange
			Swap(A.This, B.This);
		}
	}
}

#pragma warning(pop)
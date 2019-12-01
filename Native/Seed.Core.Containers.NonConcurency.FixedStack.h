#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.API.LINQ.h"

#pragma warning(push)
#pragma warning(disable: 4996)
namespace Seed {
	inline namespace Core {
		namespace Containers {
			template<typename THeap, SizeT TSize, typename T>
			class SeedKernelCall BaseFixedStack :
				public LINQ::IFixedStack<BaseFixedStack, THeap, T, TSize, true>,
				public IDisposable
			{
			private:
				struct Data {
					RefCountT count;
					RefCountT RefCount;
					T stack[TSize];

					Data() : count(0), RefCount(1), stack() {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() final override {}
			public:
				BaseFixedStack() : This(new Data()) {}

				BaseFixedStack(InitilizerList<T>&& list) : This(new Data()) {
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
					if (This->count == TSize)
						Throw(NullPointException(ExceptionPosition));
					This->stack[This->count] = item;
					This->count++;
				}

				void Push(T&& item) {
					if (This->count == TSize)
						Throw(NullPointException(ExceptionPosition));
					This->stack[This->count] = MoveRValue(item);
					This->count++;
				}

				T Pop() {
					if (This->count == 0)
						Throw(NullPointException(ExceptionPosition));
					This->count--;
					return MoveRValue(This->stack[This->count]);
				}

				const T& Top() {
					if (This->count == 0)
						Throw(NullPointException(ExceptionPosition));
					This->count--;
					return This->stack[This->count];
				}

				const T& Top() const {
					if (This->count == 0)
						Throw(NullPointException(ExceptionPosition));
					This->count--;
					return This->stack[This->count];
				}

				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);

				SizeT GetCount() { return This->count; }
				SizeT Property(GetCount) Count;

				virtual BaseFixedStack Clone() const override final {
					BaseFixedStack result;
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
				}

				RefCountClass(BaseFixedStack, This)
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
				__forceinline BaseFixedStack& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
						if (This->count == 0)
							return *this;
						SizeT i = This->count;
						do {
							i--;
							lambda(This->stack[i]);
						} while (i != 0);
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>) {
						if (This->count == 0)
							return *this;
						SizeT i = This->count;
						do {
							i--;
							lambda(This->stack[i], i);
						} while (i != 0);
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BaseFixedStack& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						if (This->count == 0)
							return *this;
						SizeT i = This->count;
						do {
							i--;
							lambda(This->stack[i]);
						} while (i != 0);
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						if (This->count == 0)
							return *this;
						SizeT i = This->count;
						do {
							i--;
							lambda(This->stack[i], i);
						} while (i != 0);
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda) const {
					if (This->count == 0)
						return U();
					U result = U();
					SizeT i = This->count;
					do {
						i--;
						lambda(result, This->stack[i]);
					} while (i != 0);
					return MoveRValue(result);
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, const U& Value) const {
					U result = Value;
					if (This->count == 0)
						return result;
					SizeT i = This->count;
					do {
						i--;
						lambda(result, This->stack[i]);
					} while (i != 0);
					return MoveRValue(result);
				}

				template<typename LambdaT>
				__forceinline T AggregateImpl(LambdaT&& lambda, T&& Value) const {
					if (This->count == 0)
						return Value;
					SizeT i = This->count;
					do {
						i--;
						lambda(Value, This->stack[i]);
					} while (i != 0);
					return MoveRValue(Value);
				}

				bool AnyImpl() const { return This->count > 0; }

				template<typename Lambda>
				__forceinline bool AnyImpl(Lambda&& lambda) const {
					if (This->count == 0)
						return false;
					SizeT i = This->count;
					do {
						i--;
						if (lambda(This->stack[i]))
							return true;
					} while (i != 0);
					return false;
				}

				template<typename Lambda>
				__forceinline bool AllImpl(Lambda&& lambda) const {
					if (This->count == 0)
						return false;
					SizeT i = This->count;
					do {
						i--;
						if (!lambda(This->stack[i]))
							return false;
					} while (i != 0);
					return true;
				}

				template<typename Lambda>
				T& FirstImpl(Lambda&& lambda) {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					SizeT i = This->count;
					do {
						i--;
						if (lambda(This->stack[i]))
							return static_cast<T&>(This->array[i]);
					} while (i != 0);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
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
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					SizeT i = This->count;
					do {
						i--;
						if (lambda(This->stack[i]))
							return static_cast<T&>(This->array[i]);
					} while (i != 0);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0)
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
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
				}

				BaseFixedStack& ReverseImpl() {
					for (SizeT i = 0; i < This->count / 2; i++) {
						Swap(This->stack[i], This->stack[(This->count - 1) - i]);
					}
					return *this;
				}

				__forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->stack[Index] = MoveRValue(Value);
					This->count = Index + 1;
				}

				__forceinline BaseFixedStack& RemoveAllImpl() {
					for (SizeT i = 0; i < This->count; i++) {
						This->stack[i].~T();
					}
					return *this;
				}

				template<typename Lambda>
				__forceinline BaseFixedStack& RemoveAllImpl(Lambda&& lambda) {
					for (SizeT i = 0; i < This->count; i++) {
						lambda(MoveRValue(This->stack[i]));
					}
					return *this;
				}
			};

			namespace {
				template<SizeT TSize, typename T>
				using FixedStack = BaseFixedStack<Seed::Core::Memory::Heap, TSize, T>;
			}

			namespace {
				template<SizeT TSize, typename T>
				using SensitiveFixedStack = BaseFixedStack<Seed::Core::Memory::SensitiveDataHeap, TSize, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, SizeT TSize, typename T>
				struct IsFixedStackT<Seed::Core::Containers::BaseFixedStack<THeap, TSize, T>> : LogicTrue {};
			}
		}

		template<typename THeap, SizeT TSize, typename T>
		__forceinline void Swap(Containers::BaseFixedStack<THeap, TSize, T>& A, Containers::BaseFixedStack<THeap, TSize, T>& B) {
			Swap(A.This, B.This);
		}
	}
}
#pragma warning(pop)
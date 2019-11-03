#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.API.LINQ.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			template<typename THeap, typename T>
			class SeedKernelCall BaseDynamicStack :
				public LINQ::IDynamicStack<BaseDynamicStack, THeap, T, false>,
				public IDisposable,
				public IClonable<BaseDynamicStack<THeap, T>>
			{
			private:
				struct Data {
					RefCountT count;
					RefCountT RefCount;
					SizeT bufferSize;
					T* stack;

					Data(SizeT bufferSize) : count(0), RefCount(1), stack(new T[bufferSize]), bufferSize(bufferSize) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() final override {}
			public:
				BaseDynamicStack() : This(new Data(CurrentSettings().MemoryManager.StackReserveSize)) {}

				BaseDynamicStack(InitilizerList<T>&& list) {
					if (list.size() != 0) {
						This = new Data(list.size() + CurrentSettings().MemoryManager.StackReserveSize);
						auto it = list.begin();
						SizeT i = list.size();
						do {
							i--; This->stack[i] = *it; it++;
						} while (i != 0);
						This->count = list.size();
					}
					else {
						This = new Data(CurrentSettings().MemoryManager.StackReserveSize);
					}
				}

				void Push(const T& item) {
					if (This->count == This->bufferSize) {
						ExtendBufferImpl(CurrentSettings().MemoryManager.StackReserveSize);
					}
					This->stack[This->count] = item;
					This->count++;
				}

				void Push(T&& item) {
					if (This->count == This->bufferSize) {
						ExtendBufferImpl(CurrentSettings().MemoryManager.StackReserveSize);
					}
					This->stack[This->count] = MoveRValue(item);
					This->count++;
				}

				T Pop() {
					if (This->count == 0)
						Throw(NullPointException(ExceptionPosition));
					This->count--;
					return MoveRValue(This->stack[This->count]);
				}

				const T& Top() const {
					if (This->count == 0)
						Throw(NullPointException(ExceptionPosition));
					This->count--;
					return This->stack[This->count];
				}

				const T& Top() {
					if (This->count == 0)
						Throw(NullPointException(ExceptionPosition));
					This->count--;
					return This->stack[This->count];
				}

				void ExtendBuffer(SizeT AdditionalSize) {
					ExtendBufferImpl(AdditionalSize);
				}

				void Shrink() {
					T* newBuffer = new T[This->count];
					for (SizeT i = 0; i < This->count; i++)
					{
						newBuffer[i] = MoveRValue(This->stack[i]);
					}
					delete This->stack;
					This->stack = newBuffer;
					This->bufferSize = This->count;
				}

				virtual BaseDynamicStack Clone() const override final {
					BaseDynamicStack result;
					result.This->bufferSize = This->bufferSize;
					result.This->count = This->count;
					result.This->stack = new T[This->bufferSize];
					for (SizeT i = 0; i < This->count; i++)
					{
						result.This->stack[i] = This->stack[i];
					}
					return result;
				}

				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);

				SizeT GetCount() { return This->count; }
				SizeT Property(GetCount) Count;

				RefCountClass(BaseDynamicStack, This)
			private:
				void ExtendBufferImpl(SizeT AdditionalSize) {
					T* newBuffer = new T[This->bufferSize + AdditionalSize];
					for (SizeT i = 0; i < This->count; i++)
					{
						newBuffer[i] = MoveRValue(This->stack[i]);
					}
					delete This->stack;
					This->stack = newBuffer;
					This->bufferSize = This->bufferSize + AdditionalSize;
				}
			protected:
				template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollection;
				template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsConcurencySuport>
				friend class LINQ::IDynamicStack;
				template<template<typename THeap, typename T> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollectionBase;
			protected:
#pragma warning(push)
#pragma warning(disable: 4172)
				template<typename LambdaT>
				__forceinline BaseDynamicStack& ForEachImpl(LambdaT&& lambda) {
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
				__forceinline const BaseDynamicStack& ForEachImpl(LambdaT&& lambda) const {
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
				__forceinline T AggregateImpl(LambdaT&& lambda) const {
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

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, Y&& Value) const {
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

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
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

				BaseDynamicStack& ReverseImpl() {
					for (SizeT i = 0; i < This->count / 2; i++) {
						Swap(This->stack[i], This->stack[(This->count - 1) - i]);
					}
					return *this;
				}

				__forceinline void UnsafeSet(const T& Value, SizeT Index) {
					This->stack[Index] = Value;
				}

				__forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->stack[Index] = MoveRValue(Value);
				}

				__forceinline BaseDynamicStack& RemoveAllImpl() {
					for (SizeT i = 0; i < This->count; i++) {
						This->stack[i].~T();
					}
					return *this;
				}

				template<typename Lambda>
				__forceinline BaseDynamicStack& RemoveAllImpl(Lambda&& lambda) {
					for (SizeT i = 0; i < This->count; i++) {
						lambda(MoveRValue(This->stack[i]));
					}
					return *this;
				}
			};


			namespace {
				template<typename T>
				using DynamicStack = BaseDynamicStack<Seed::Core::Memory::Heap, T>;
			}

			namespace {
				template<typename T>
				using SensitiveDynamicStack = BaseDynamicStack<Seed::Core::Memory::SensitiveDataHeap, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename T>
				struct IsDynamicStackT<Seed::Core::Containers::BaseDynamicStack<THeap, T>> : LogicTrue {};
			}
		}

		template<typename THeap, typename T>
		__forceinline void Swap(Containers::BaseDynamicStack<THeap, T>& A, Containers::BaseDynamicStack<THeap, T>& B) {
			Swap(A.This, B.This);
		}
	}
}
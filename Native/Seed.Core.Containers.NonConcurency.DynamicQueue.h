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
			class SeedKernelCall BaseDynamicQueue :
				public LINQ::IDynamicStack<BaseDynamicQueue, THeap, T, true>,
				public IDisposable
			{
			private:
				struct Data {
					SizeT front;
					SizeT rear;
					RefCountT count;
					SizeT bufferSize;
					RefCountT RefCount;
					T* queue;

					T& operator[](SizeT Index) {
						return queue[(front + Index) % bufferSize];
					}

					Data() :
						front(0),
						rear(0),
						count(0),
						RefCount(1),
						queue(),
						flowControler()
					{}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() final override {}
			public:
				BaseDynamicQueue() : This(new Data()) {}

				BaseDynamicQueue(InitilizerList<T>&& list) : BaseDynamicQueue() {
					if (list.size() != 0) {
						SizeT ActuallyCopyied = list.size();
						This->bufferSize = list.size();
						This->queue = new T[This->bufferSize];
						auto it = list.begin();
						SizeT i = ActuallyCopyied;
						for (SizeT i = 0; i < ActuallyCopyied; i++) {
							This->queue[i] = *it; it++;
						}
						This->rear = This->count = ActuallyCopyied;
						This->front = 0;
					}
				}

				void Enqueue(const T& Value, bool IsResizeIfFull = false) {
					if (++This->count > This->bufferSize) {
						if (!IsResizeIfFull)
							This->flowControler.WaitForConcumer();
						else
							Throw(NullPointException(ExceptionPosition));
					}

					SizeT newRear = (This->rear + 1) % This->bufferSize;
					This->queue[This->rear] = Value;
					This->rear = newRear;
				}

				void Enqueue(T&& Value, bool IsResizeIfFull = false) {
					if (++This->count > This->bufferSize) {
						if (!IsResizeIfFull)
							This->flowControler.WaitForConcumer();
						else
							Throw(NullPointException(ExceptionPosition));
					}

					SizeT newRear = (This->rear + 1) % This->bufferSize;
					This->queue[This->rear] = MoveRValue(Value);
					This->rear = newRear;
				}

				T Dequeue() {
					if (This->count == 0)
						Throw(NullPointException(ExceptionPosition));

					T res = MoveRValue(This->queue[This->front]);
					This->front = (This->front + 1) % This->bufferSize;
					This->count--;

					return MoveRValue(res);
				}

				const T& Top() {
					if (This->front == This->rear)
						Throw(NullPointException(ExceptionPosition));
					return This->queue[This->front];
				}

				const T& Top() const {
					if (This->front == This->rear)
						Throw(NullPointException(ExceptionPosition));
					return This->queue[This->front];
				}

				void Extend(SizeT AdditionalSize) {
					ExtendImpl(AdditionalSize);
				}

				void Shrink() {
					SizeT newSize = This->count;
					T* newBuffer = new T[newSize];
					for (SizeT i = 0; i < This->count; i++)
					{
						newBuffer[i] = (*This)[i];
					}
					This->front = 0; This->rear = This->count;
					delete This->queue;
					This->queue = newBuffer;
				}

				///<summary>Оптимізований обмін</summary>
				///<param name = 'A'>Перший елемент</param>
				///<param name = 'B'>Другий елемент</param>
				__forceinline friend void Swap(BaseDynamicQueue& A, BaseDynamicQueue& B);

				SizeT GetCount() { return This->count; }
				SizeT Property(GetCount) Count;

				RefCountClass(BaseDynamicQueue, This)
			private:
				void ExtendImpl(SizeT AdditionalSize) {
					SizeT newSize = This->bufferSize + AdditionalSize;
					T* newBuffer = new T[newSize];
					for (SizeT i = 0; i < This->count; i++)
					{
						newBuffer[i] = (*This)[i];
					}
					This->front = 0; This->rear = This->count;
					delete This->queue;
					This->queue = newBuffer;
				}
			protected:
				template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollection;
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT bufferSize, Boolean TIsConcurencySuport>
				friend class LINQ::IFixedCollection;
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT bufferSize, Boolean TIsConcurencySuport>
				friend class LINQ::IFixedStack;
				template<template<typename THeap, typename T> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollectionBase;
			protected:
#pragma warning(push)
#pragma warning(disable: 4172)

				template<typename LambdaT>
				__forceinline BaseDynamicQueue& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda((*This)[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						SizeT Index = 0;

						for (SizeT i = 0; i < This->count; i++, Index)
						{
							lambda((*This)[i], Index);
						}

						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BaseDynamicQueue& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda((*This)[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						SizeT Index = 0;

						for (SizeT i = 0; i < This->count; i++, Index)
						{
							lambda((*This)[i], Index);
						}

						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda) const {
					U result = U();

					if (This->count == 0) {
						return MoveRValue(result);
					}

					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(result, (*This)[i]);
					}

					return MoveRValue(result);
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, const U& Value) const {
					U result = Value;
					if (This->count == 0) {
						return MoveRValue(result);
					}

					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(result, (*This)[i]);
					}

					return MoveRValue(result);
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, U&& Value) const {
					U result = MoveRValue(Value);

					if (This->count == 0)
						return MoveRValue(result);

					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(result, (*This)[i]);
					}

					return MoveRValue(result);
				}

				bool AnyImpl() const { return This->count > 0; }

				template<typename Lambda>
				__forceinline bool AnyImpl(Lambda&& lambda) const {
					if (This->count == 0)
						return false;

					for (SizeT i = 0; i < This->count; i++)
					{
						if (lambda((*This)[i]))
							return true;
					}

					return false;
				}

				template<typename Lambda>
				__forceinline bool AllImpl(Lambda&& lambda) const {
					if (This->count == 0)
						return false;

					for (SizeT i = 0; i < This->count; i++)
					{
						if (!lambda((*This)[i]))
							return false;
					}

					return true;
				}

				template<typename Lambda>
				T& FirstImpl(Lambda&& lambda) {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

					T& ret = This->queue[This->front];

					return ret;
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

					T& ret = This->queue[(This->rear == 0) ? bufferSize - 1 : This->rear - 1];

					return ret;
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

					T& ret = This->queue[This->front];

					return ret;
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

					T& ret = This->queue[(This->rear == 0) ? bufferSize - 1 : This->rear - 1];

					return ret;
				}

				BaseDynamicQueue& ReverseImpl() {
					for (SizeT i = 0; i < This->count / 2; i++)
					{
						Swap((*This)[i], (*This)[(This->count - 1) - i]);
					}
					return *this;
				}

				__forceinline void UnsafeSet(const T& Value, SizeT Index) {
					This->queue[Index] = Value;
				}

				__forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->queue[Index] = MoveRValue(Value);
				}

				__forceinline BaseDynamicQueue& RemoveAllImpl() {
					if (This->count == 0)
						return *this;
					for (SizeT i = 0; i < This->count; i++)
					{
						(*This)[i].~T();
					}
					return *this;
				}

				template<typename Lambda>
				__forceinline BaseDynamicQueue& RemoveAllImpl(Lambda&& lambda) {
					if (This->count == 0)
						return *this;
					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(MoveRValue((*This)[i]));
					}
					return *this;
				}
			};

			template<typename THeap, typename T>
			__forceinline void Swap(BaseDynamicQueue<THeap, T>& A, BaseDynamicQueue<THeap, T>& B) {
				//TODO: Interlocked Exchange
				Swap(A.This, B.This);
			}

			namespace {
				template<typename T>
				using DynamicQueue = BaseDynamicQueue<Seed::Core::Memory::Heap, T>;
			}

			namespace {
				template<typename T>
				using SensitiveDynamicQueue = BaseDynamicQueue<Seed::Core::Memory::SensitiveDataHeap, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename T>
				struct IsDynamicQueueT<Seed::Core::Containers::BaseDynamicQueue<THeap, T>> : LogicTrue {};
			}
		}
	}
}
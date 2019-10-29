#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Threading.DataFlowControler.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.API.LINQ.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			template<typename THeap, typename T>
			class SeedKernelCall BaseConcurencyDynamicQueue :
				public LINQ::IDynamicStack<BaseConcurencyDynamicQueue, THeap, T, true>,
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
					Threading::DataFlowControler flowControler;

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
				BaseConcurencyDynamicQueue() : This(new Data()) {}

				BaseConcurencyDynamicQueue(InitilizerList<T>&& list) : BaseConcurencyDynamicQueue() {
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
					This->flowControler.Produce([&]() {
						if (++This->count > This->bufferSize) {
							if (!IsResizeIfFull)
								This->flowControler.WaitForConcumer();
							else
								ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
						}

						SizeT newRear = (This->rear + 1) % This->bufferSize;
						This->queue[This->rear] = Value;
						This->rear = newRear;
						});
				}

				void Enqueue(T&& Value, bool IsResizeIfFull = false) {
					This->flowControler.Produce([&, Value = MoveRValue(Value)]() mutable {
						if (++This->count > This->bufferSize) {
							if (!IsResizeIfFull)
								This->flowControler.WaitForConcumer();
							else
								ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
						}

						SizeT newRear = (This->rear + 1) % This->bufferSize;
						This->queue[This->rear] = MoveRValue(Value);
						This->rear = newRear;
					});
				}

				T Dequeue() {
					return This->flowControler.Concume([&]() {
						while (This->count == 0) This->flowControler.WaitForProducer();

						T res = MoveRValue(This->queue[This->front]);
						This->front = (This->front + 1) % This->bufferSize;
						This->count--;

						return MoveRValue(res);
						});
				}

				const T& Top() {
					return This->flowControler.SharedLock([&]() {
						if (This->front == This->rear)
							Throw(NullPointException(ExceptionPosition));
						return This->queue[This->front];
						});
				}

				const T& Top() const {
					return This->flowControler.SharedLock([&]() {
						if (This->front == This->rear)
							Throw(NullPointException(ExceptionPosition));
						return This->queue[This->front];
						});
				}

				void Extend(SizeT AdditionalSize) {
					This->flowControler.ExclusiveLock([&]() {
						ExtendImpl(AdditionalSize);
						});
				}

				void Shrink() {
					This->flowControler.ExclusiveLock([&]() {
						SizeT newSize = This->count;
						T* newBuffer = new T[newSize];
						for (SizeT i = 0; i < This->count; i++)
						{
							newBuffer[i] = (*This)[i];
						}
						This->front = 0; This->rear = This->count;
						delete This->queue;
						This->queue = newBuffer;
						});
				}

				///<summary>Оптимізований обмін</summary>
				///<param name = 'A'>Перший елемент</param>
				///<param name = 'B'>Другий елемент</param>
				__forceinline friend void Swap(BaseConcurencyDynamicQueue& A, BaseConcurencyDynamicQueue& B);

				SizeT GetCount() { return This->count; }
				SizeT Property(GetCount) Count;

				RefCountClass(BaseConcurencyDynamicQueue, This)
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
				__forceinline BaseConcurencyDynamicQueue& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {

						return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& {
							for (SizeT i = 0; i < This->count; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {

						return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& {
							SizeT Index = 0;

							for (SizeT i = 0; i < This->count; i++, Index)
							{
								lambda((*This)[i], Index);
							}

							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BaseConcurencyDynamicQueue& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						return This->flowControler.SharedLock([&]() -> const BaseConcurencyDynamicQueue& {
							for (SizeT i = 0; i < This->count; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						return This->flowControler.SharedLock([&]() -> const BaseConcurencyDynamicQueue& {
							SizeT Index = 0;

							for (SizeT i = 0; i < This->count; i++, Index)
							{
								lambda((*This)[i], Index);
							}

							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda) const {
					return This->flowControler.SharedLock([&]() {
						U result = U();

						if (This->count == 0) {
							return MoveRValue(result);
						}

						for (SizeT i = 0; i < This->count; i++)
						{
							lambda(result, (*This)[i]);
						}

						return MoveRValue(result);
						});
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, const U& Value) const {
					return This->flowControler.SharedLock([&]() {
						U result = Value;
						if (This->count == 0) {
							return MoveRValue(result);
						}

						for (SizeT i = 0; i < This->count; i++)
						{
							lambda(result, (*This)[i]);
						}

						return MoveRValue(result);
						});
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, U&& Value) const {
					return This->flowControler.SharedLock([&, Value = MoveRValue(Value)]() mutable {
						U result = MoveRValue(Value);

						if (This->count == 0)
							return MoveRValue(result);

						for (SizeT i = 0; i < This->count; i++)
						{
							lambda(result, (*This)[i]);
						}

						return MoveRValue(result);
					});
				}

				bool AnyImpl() const { return This->count > 0; }

				template<typename Lambda>
				__forceinline bool AnyImpl(Lambda&& lambda) const {
					return This->flowControler.SharedLock([&]() {
						if (This->count == 0)
							return false;

						for (SizeT i = 0; i < This->count; i++)
						{
							if (lambda((*This)[i]))
								return true;
						}

						return false;
						});
				}

				template<typename Lambda>
				__forceinline bool AllImpl(Lambda&& lambda) const {
					return This->flowControler.SharedLock([&]() {
						if (This->count == 0)
							return false;

						for (SizeT i = 0; i < This->count; i++)
						{
							if (!lambda((*This)[i]))
								return false;
						}

						return true;
						});
				}

				template<typename Lambda>
				T& FirstImpl(Lambda&& lambda) {
					return This->flowControler.SharedLock([&]() {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T& ret = This->queue[This->front];

						return ret;
						});
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
					return This->flowControler.SharedLock([&]() {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T& ret = This->queue[(This->rear == 0) ? bufferSize - 1 : This->rear - 1];

						return ret;
						});
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
					return This->flowControler.SharedLock([&]() {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T& ret = This->queue[This->front];

						return ret;
						});
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
					return This->flowControler.SharedLock([&]() {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T& ret = This->queue[(This->rear == 0) ? bufferSize - 1 : This->rear - 1];

						return ret;
						});
				}

				BaseConcurencyDynamicQueue& ReverseImpl() {
					return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& {
						for (SizeT i = 0; i < This->count / 2; i++)
						{
							Swap((*This)[i], (*This)[(This->count - 1) - i]);
						}
						return *this;
						});
				}

				__forceinline void UnsafeSet(const T& Value, SizeT Index) {
					This->queue[Index] = Value;
				}

				__forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->queue[Index] = MoveRValue(Value);
				}

				__forceinline BaseConcurencyDynamicQueue& RemoveAllImpl() {
					return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& {
						if (This->count == 0)
							return *this;
						for (SizeT i = 0; i < This->count; i++)
						{
							(*This)[i].~T();
						}
						return *this;
						});
				}

				template<typename Lambda>
				__forceinline BaseConcurencyDynamicQueue& RemoveAllImpl(Lambda&& lambda) {
					return This->flowControler.SharedLock([&, lambda = MoveRValue(lambda)]()->BaseConcurencyDynamicQueue& {
						if (This->count == 0)
							return *this;
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda(MoveRValue((*This)[i]));
						}
						return *this;
					});
				}

				template<typename Lambda>
				__forceinline SizeT GetCountIfImpl(Lambda&& lambda) {
					return This->lock.SharedLock([&]() {
						SizeT selectedCount = 0;

						for (SizeT i = 0; i < This->count; i++)
						{
							if (lambda((*This)[i]))
								selectedCount++;
						}

						return selectedCount;
						});
				}
			};

			template<typename THeap, typename T>
			__forceinline void Swap(BaseConcurencyDynamicQueue<THeap, T>& A, BaseConcurencyDynamicQueue<THeap, T>& B) {
				//TODO: Interlocked Exchange
				Swap(A.This, B.This);
			}

			namespace {
				template<typename T>
				using ConcurencyDynamicQueue = BaseConcurencyDynamicQueue<Seed::Core::Memory::Heap, T>;
			}

			namespace {
				template<typename T>
				using SensitiveConcurencyDynamicQueue = BaseConcurencyDynamicQueue<Seed::Core::Memory::SensitiveDataHeap, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename T>
				struct IsDynamicQueueT<Seed::Core::Containers::BaseConcurencyDynamicQueue<THeap, T>> : LogicTrue {};
			}
		}
	}
}
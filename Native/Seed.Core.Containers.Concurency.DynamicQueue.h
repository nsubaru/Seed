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
			class BaseConcurencyDynamicQueue final :
				public LINQ::IDynamicStack<BaseConcurencyDynamicQueue, THeap, T, true>,
				public IDisposable,
				public IClonable<BaseConcurencyDynamicQueue<THeap, T>>
			{
			private:
				struct Data {
					SizeT front;
					SizeT rear;
					RefCountT count;
					T* restrict queue;
					SizeT bufferSize;
					RefCountT RefCount;
					
					Threading::DataFlowControler flowControler;

					forceinline T& operator[](SizeT index) {
						return queue[(front + index) % bufferSize];
					}

					forceinline Data()
						: front(0),
						  rear(0),
						  count(0),
						  queue(),
						  bufferSize{0},
						  RefCount(1) {}

					forceinline Data(SizeT startCount)
						: front(0),
						  rear(0),
						  count(0),
						  queue(new T[startCount]),
						  bufferSize(startCount),
						  RefCount(1) {}
				};
			private:
				Data* This;
			protected:
				forceinline virtual void Dispose() final override
				{
					if (This->bufferSize != 0)
					{
						delete[] This->queue;
						This->front = This->rear = This->count = This->bufferSize = 0;
						This->queue = nullptr;
					}
				}
			public:
				forceinline BaseConcurencyDynamicQueue() : This(new Data()) {}

				forceinline BaseConcurencyDynamicQueue(SizeT startCount) : This(new Data(startCount)) {}

				BaseConcurencyDynamicQueue(InitilizerList<T>&& list) : BaseConcurencyDynamicQueue() {
					if (list.size() != 0) {
						SizeT actuallyCopyied = list.size();
						This->bufferSize = list.size();
						This->queue = new T[This->bufferSize];
						auto it = list.begin();
						for (SizeT i = 0; i < actuallyCopyied; i++) {
							This->queue[i] = *it; ++it;
						}
						This->rear = This->count = actuallyCopyied;
						This->front = 0;
					}
				}

				void Enqueue(const T& value, bool isResizeIfFull = false) {
					This->flowControler.Produce([&]() {
						if (++This->count > This->bufferSize) {
							if (!isResizeIfFull)
								This->flowControler.WaitForConcumer();
							else
								ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
						}

						SizeT newRear = (This->rear + 1) % This->bufferSize;
						This->queue[This->rear] = value;
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
						--This->count;

						return MoveRValue(res);
						});
				}

				forceinline const T& Top() {
					return This->flowControler.SharedLock([&]() {
						if (This->front == This->rear)
							Throw(NullPointException(ExceptionPosition));
						return This->queue[This->front];
						});
				}

				forceinline const T& Top() const {
					return This->flowControler.SharedLock([&]() {
						if (This->front == This->rear)
							Throw(NullPointException(ExceptionPosition));
						return This->queue[This->front];
						});
				}

				forceinline void Extend(SizeT AdditionalSize) {
					This->flowControler.ExclusiveLock([&]() {
						ExtendImpl(AdditionalSize);
						});
				}

				void Shrink() {
					This->flowControler.ExclusiveLock([&]() {
						const SizeT newSize = This->count;
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

				template<typename TU>
				forceinline friend void Seed::Core::Swap(TU& a, TU& b);

				forceinline SizeT GetCount() const { return This->count; }
				SizeT Property(GetCount) Count;

				virtual BaseConcurencyDynamicQueue Clone() const override final {
					return This->flowControler.SharedLock([&]() {
						BaseConcurencyDynamicQueue result;
						result.This->queue = new T[This->bufferSize];
						result.This->front = This->front;
						result.This->rear = This->rear;
						result.This->count = This->count;
						result.This->bufferSize = This->bufferSize;
						for (SizeT i = 0; i < This->count; i++)
						{
							(*result.This)[i] = (*This)[i];
						}
						return result;
						});
				}

				RefCountClass(BaseConcurencyDynamicQueue, This)
			private:
				void ExtendImpl(SizeT additionalSize) {
					const SizeT newSize = This->bufferSize + additionalSize;
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
				template <
					template<typename, typename> typename TCollection,
					typename TAnyHeap,
					typename TAny,
					Boolean TIsDynamic,
					Boolean TIsConcurencySuport
				>
				friend class LINQ::ICollection;
				template<
					template<typename, SizeT, typename> typename TCollection,
					typename TAnyHeap,
					typename TAny,
					SizeT TBufferSize,
					Boolean TIsConcurencySuport
				>
				friend class LINQ::IFixedCollection;
				template<
					template<typename, SizeT, typename> typename TCollection,
					typename TAnyHeap,
					typename TAny,
					SizeT TBufferSize,
					Boolean TIsConcurencySuport
				>
				friend class LINQ::IFixedStack;
				template<
					template<typename, typename> typename TCollection,
					typename TAnyHeap,
					typename TAny,
					Boolean TIsDynamic,
					Boolean TIsConcurencySuport
				>
				friend class LINQ::ICollectionBase;
                template<
					typename TAnyHeap,
					typename TAny,
					typename TEnable
				>
                friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
                friend class LINQ::IAssociableCollection;
			protected:
#pragma warning(push)
#pragma warning(disable: 4172)

				template<typename TLambdaT>
				BaseConcurencyDynamicQueue& ForEachImpl(TLambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<TLambdaT, void, T&> ||
						TypeTraits::IsLambdaConvertibleV<TLambdaT, void, const T&>) {

						return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& {
							for (SizeT i = 0; i < This->count; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<TLambdaT, void, T&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<TLambdaT, void, const T&>) {

						return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& {
							SizeT index = 0;

							for (SizeT i = 0; i < This->count; i++, index)
							{
								lambda((*This)[i], index);
							}

							return *this;
							});
					}
					else {
						// ReSharper disable once CppStaticAssertFailure
						static_assert(false);
						__assume(false);
						return *this;
					}
				}

				template<typename TLambdaT>
				const BaseConcurencyDynamicQueue& ForEachImpl(TLambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<TLambdaT, void, const T&>) {
						return This->flowControler.SharedLock([&]() -> const BaseConcurencyDynamicQueue& {
							for (SizeT i = 0; i < This->count; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<TLambdaT, void, const T&, SizeT>) {
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
						// ReSharper disable once CppStaticAssertFailure
						static_assert(false);
						__assume(false);
						return *this;
					}
				}

				template<typename T, typename Lambda>
				T AggregateImpl(Lambda&& lambda) const {
					return This->flowControler.SharedLock([&]() {
						T result = T();

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

				template<typename T, typename Lambda>
				T AggregateImpl(Lambda&& lambda, const T& value) const {
					return This->flowControler.SharedLock([&]() {
						T result = value;
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

				template<typename T, typename Lambda>
				forceinline T AggregateImpl(Lambda&& lambda, T&& value) const {
					return This->flowControler.SharedLock([&, Value = MoveRValue(value)]() mutable {
						T result = MoveRValue(Value);

						if (This->count == 0)
							return MoveRValue(result);

						for (SizeT i = 0; i < This->count; i++)
						{
							lambda(result, (*This)[i]);
						}

						return MoveRValue(result);
					});
				}

				forceinline bool AnyImpl() const { return This->count > 0; }

				template<typename Lambda>
				bool AnyImpl(Lambda&& lambda) const {
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
				bool AllImpl(Lambda&& lambda) const {
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

						T& ret = This->queue[(This->rear == 0) ? This->bufferSize - 1 : This->rear - 1];

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

						T& ret = This->queue[(This->rear == 0) ? This->bufferSize - 1 : This->rear - 1];

						return ret;
						});
				}

				BaseConcurencyDynamicQueue& ReverseImpl() {
					return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& {
						for (SizeT i = 0; i < This->count / 2; i++)
						{
							Seed::Swap((*This)[i], (*This)[(This->count - 1) - i]);
						}
						return *this;
						});
				}

				forceinline void UnsafeSet(const T& value, SizeT index) {
					This->queue[index] = value;
				}

				forceinline void UnsafeSet(T&& value, SizeT index) {
					This->queue[index] = MoveRValue(value);
				}

				BaseConcurencyDynamicQueue& RemoveAllImpl() {
					return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& {
						if (This->count == 0)
							return *this;
						delete[] This->queue;
						This->front = This->rear = This->count = This->bufferSize = 0;
						This->queue = nullptr;
						return *this;
						});
				}

				template<typename Lambda>
				BaseConcurencyDynamicQueue& RemoveAllImpl(Lambda&& lambda) {
					return This->flowControler.SharedLock([&, lambda = MoveRValue(lambda)]()->BaseConcurencyDynamicQueue& {
						if (This->count == 0)
							return *this;
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda(MoveRValue((*This)[i]));
						}
						delete[] This->queue;
						This->front = This->rear = This->count = This->bufferSize = 0;
						This->queue = nullptr;
						return *this;
					});
				}

				template<typename Lambda>
				SizeT GetCountIfImpl(Lambda&& lambda) {
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

			namespace {
				template<typename T>
				using ConcurencyDynamicQueue = BaseConcurencyDynamicQueue<Seed::Core::Memory::Heap, T>;
			}

			namespace {
				template<typename T>
				using ConcurencySensitiveDynamicQueue = BaseConcurencyDynamicQueue<Seed::Core::Memory::SensitiveDataHeap, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename T>
				struct IsDynamicQueueT<Seed::Core::Containers::BaseConcurencyDynamicQueue<THeap, T>> : LogicTrue {};
			}
		}

		template<typename THeap, typename T>
		forceinline void Swap(Containers::BaseConcurencyDynamicQueue<THeap, T>& A, Containers::BaseConcurencyDynamicQueue<THeap, T>& B) {
			//TODO: Interlocked Exchange
			Swap(A.This, B.This);
		}
	}
}
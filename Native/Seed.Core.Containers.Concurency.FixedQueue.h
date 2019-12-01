#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Threading.DataFlowControler.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.API.LINQ.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			template<typename THeap, SizeT TSize, typename T>
			class SeedKernelCall BaseConcurencyFixedQueue :
				public LINQ::IFixedQueue<BaseConcurencyFixedQueue, THeap, T, TSize, true>,
				public IDisposable,
				public IClonable<BaseConcurencyFixedQueue<THeap, TSize, T>>
			{
			private:
				struct Data {
					SizeT front;
					SizeT rear;
					RefCountT count;
					RefCountT RefCount;
					T queue[TSize];
					Threading::DataFlowControler flowControler;

					T& operator[](SizeT Index) {
						return queue[(front + Index) % TSize];
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
				BaseConcurencyFixedQueue() : This(new Data()) {}

				BaseConcurencyFixedQueue(InitilizerList<T>&& list) : BaseConcurencyFixedQueue() {
					if (list.size() != 0) {
						SizeT ActuallyCopyied = (TSize < list.size()) ? TSize : list.size();
						auto it = list.begin();
						SizeT i = ActuallyCopyied;
						for (SizeT i = 0; i < ActuallyCopyied; i++) {
							This->queue[i] = *it; it++;
						}
						This->rear = This->count = ActuallyCopyied;
						This->front = 0;
					}
				}

				void Enqueue(const T& Value) {
					This->flowControler.Produce([&]() {
						if (++This->count > TSize)
							This->flowControler.WaitForConcumer();
						SizeT newRear = (This->rear + 1) % TSize;
						This->queue[This->rear] = Value;
						This->rear = newRear;
						});
				}

				void Enqueue(T&& Value) {
					This->flowControler.Produce([&, Value = MoveRValue(Value)]() {
						if (++This->count > TSize)
							This->flowControler.WaitForConcumer();
						SizeT newRear = (This->rear + 1) % TSize;
						This->queue[This->rear] = MoveRValue(Value);
						This->rear = newRear;
					});
				}

				T Dequeue() {
					return This->flowControler.Concume([&]() -> T {
						if (This->count == 0)
							This->flowControler.WaitForProducer();
						T res = MoveRValue(This->queue[This->front]);
						This->front = (This->front + 1) % TSize;
						--This->count; //-V803
						return res;
						});
				}

				const T& Top() {
					return This->flowControler.SharedLock([&]() -> const T& {
						if (This->front == This->rear)
							Throw(NullPointException(ExceptionPosition));
						return This->queue[This->front];
						});
				}

				const T& Top() const {
					return This->flowControler.SharedLock([&]() -> const T& {
						if (This->front == This->rear)
							Throw(NullPointException(ExceptionPosition));
						return This->queue[This->front];
						});
				}

				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);

				SizeT GetCount() const { return This->count; }
				SizeT Property(GetCount) Count;

				virtual BaseConcurencyFixedQueue Clone() const override final {
					BaseConcurencyFixedQueue result;
					result.This->front = This->front;
					result.This->rear = This->rear;
					result.This->count = This->count;
					for (SizeT i = 0; i < This->count; i++)
					{
						(*result.This)[i] = (*This)[i];
					}
					return result;
				}

				RefCountClass(BaseConcurencyFixedQueue, This)
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
				__forceinline BaseConcurencyFixedQueue& ForEachImpl(LambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {

						return This->flowControler.SharedLock([&]() -> BaseConcurencyFixedQueue& {
							for (SizeT i = 0; i < This->count; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {

						return This->flowControler.SharedLock([&]() -> BaseConcurencyFixedQueue& {
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
				__forceinline const BaseConcurencyFixedQueue& ForEachImpl(LambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
						return This->flowControler.SharedLock([&]() -> const BaseConcurencyFixedQueue& {
							for (SizeT i = 0; i < This->count; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>) {
						return This->flowControler.SharedLock([&]() -> const BaseConcurencyFixedQueue& {
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
							ReleaseLockSharedSRWLock(&This->Lock);
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

				BaseConcurencyFixedQueue& ReverseImpl() {
					return This->flowControler.SharedLock([&]() -> BaseConcurencyFixedQueue& {
						for (SizeT i = 0; i < This->count / 2; i++)
						{
							Seed::Swap((*This)[i], (*This)[(This->count - 1) - i]);
						}
						return *this;
						});
				}

				__forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->queue[Index] = MoveRValue(Value);
					This->count = Index;
					This->rear = Index;
				}

				__forceinline BaseConcurencyFixedQueue& RemoveAllImpl() {
					return This->flowControler.SharedLock([&]() -> BaseConcurencyFixedQueue& {
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
				__forceinline BaseConcurencyFixedQueue& RemoveAllImpl(Lambda&& lambda) {
					return This->flowControler.SharedLock([&, lambda = MoveRValue(lambda)]()->BaseConcurencyFixedQueue& {
						if (This->count == 0)
							return *this;
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda(MoveRValue((*This)[i]));
						}
						return *this;
					});
				}
			};

			namespace {
				template<SizeT TSize, typename T>
				using ConcurencyFixedQueue = BaseConcurencyFixedQueue<Seed::Core::Memory::Heap, TSize, T>;
			}

			namespace {
				template<SizeT TSize, typename T>
				using ConcurencySensitiveFixedQueue = BaseConcurencyFixedQueue<Seed::Core::Memory::SensitiveDataHeap, TSize, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, SizeT TSize, typename T>
				struct IsFixedQueueT<Seed::Core::Containers::BaseConcurencyFixedQueue<THeap, TSize, T>>: LogicTrue {};
			}
		}

		template<typename THeap, SizeT TSize, typename T>
		__forceinline void Swap(Containers::BaseConcurencyFixedQueue<THeap, TSize, T>& A, Containers::BaseConcurencyFixedQueue<THeap, TSize, T>& B) {
			//TODO: Interlooked exchange
			Swap(A.This, B.This);
		}
	}
}
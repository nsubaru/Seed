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
			class BaseConcurencyDynamicQueue final
					:
				public LINQ::IDynamicStack<BaseConcurencyDynamicQueue, THeap, T, true>,
				public IDisposable,
				public IClonable<BaseConcurencyDynamicQueue<THeap, T>>
			{
			private:
				struct Data {
					SizeT front;
					SizeT rear;
					RefCountT count;
					T* queue;
					SizeT bufferSize;
					// ReSharper disable CppInconsistentNaming
					RefCountT RefCount;
					// ReSharper restore CppInconsistentNaming
					
					Threading::DataFlowControler flowControler;

					T& operator[](SizeT index) {
						return queue[(front + index) % bufferSize];
					}

					Data()
						: front(0),
						  rear(0),
						  count(0),
						  queue(),
						  bufferSize{0},
						  RefCount(1) {}

					Data(SizeT startCount)
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
				virtual void Dispose() final override
				{
					if(This->bufferSize != 0)
						delete[] This->queue;
				}
			public:
				BaseConcurencyDynamicQueue() : This(new Data()) {}

				BaseConcurencyDynamicQueue(SizeT startCount) : This(new Data(startCount)) {}

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
				__forceinline friend void Seed::Core::Swap(TU& a, TU& b);

				SizeT GetCount() const { return This->count; }
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
                friend struct Seed::Core::NewSerilization::Details::BaseSerlizationHandler;
			protected:
#pragma warning(push)
#pragma warning(disable: 4172)

				template<typename TLambdaT>
				__forceinline BaseConcurencyDynamicQueue& ForEachImpl(TLambdaT&& lambda) {
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
				__forceinline const BaseConcurencyDynamicQueue& ForEachImpl(TLambdaT&& lambda) const {
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

				template<typename TU, typename TLambdaT>
				__forceinline TU AggregateImpl(TLambdaT&& lambda) const {
					return This->flowControler.SharedLock([&]() {
						TU result = TU();

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

				template<typename TU, typename TLambdaT>
				__forceinline TU AggregateImpl(TLambdaT&& lambda, const TU& value) const {
					return This->flowControler.SharedLock([&]() {
						TU result = value;
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

				template<typename TU, typename TLambdaT>
				__forceinline TU AggregateImpl(TLambdaT&& lambda, TU&& value) const {
					return This->flowControler.SharedLock([&, Value = MoveRValue(value)]() mutable {
						TU result = MoveRValue(Value);

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

				template<typename TLambda>
				__forceinline bool AnyImpl(TLambda&& lambda) const {
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

				template<typename TLambda>
				__forceinline bool AllImpl(TLambda&& lambda) const {
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

				template<typename TLambda>
				T& FirstImpl(TLambda&& lambda) {
					return This->flowControler.SharedLock([&]() {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T& ret = This->queue[This->front];

						return ret;
						});
				}

				template<typename TLambda>
				T& LastImpl(TLambda&& lambda) {
					return This->flowControler.SharedLock([&]() {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T& ret = This->queue[(This->rear == 0) ? This->bufferSize - 1 : This->rear - 1];

						return ret;
						});
				}

				template<typename TLambda>
				const T& FirstImpl(TLambda&& lambda) const {
					return This->flowControler.SharedLock([&]() {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T& ret = This->queue[This->front];

						return ret;
						});
				}

				template<typename TLambda>
				const T& LastImpl(TLambda&& lambda) const {
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

				__forceinline void UnsafeSet(const T& value, SizeT index) {
					This->queue[index] = value;
				}

				__forceinline void UnsafeSet(T&& value, SizeT index) {
					This->queue[index] = MoveRValue(value);
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
		__forceinline void Swap(Containers::BaseConcurencyDynamicQueue<THeap, T>& A, Containers::BaseConcurencyDynamicQueue<THeap, T>& B) {
			//TODO: Interlocked Exchange
			Swap(A.This, B.This);
		}
	}
}
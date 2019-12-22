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
			class BaseDynamicQueue final
			:
				public LINQ::IDynamicStack<BaseDynamicQueue, THeap, T, true>,
				public IDisposable,
				public IClonable<BaseDynamicQueue<THeap, T>>
			{
			private:
				struct Data {
					SizeT front;
					SizeT rear;
					RefCountT count;
					T* queue;
					SizeT bufferSize;
					RefCountT RefCount;
					

					T& operator[](SizeT Index) {
						return queue[(front + Index) % bufferSize];
					}

					Data()
						: front(0),
						  rear(0),
						  count(0),
						  queue(),
						  bufferSize(0),
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
				BaseDynamicQueue() : This(new Data()) {}

				BaseDynamicQueue(SizeT startCount) : This(new Data(startCount)) {}

				BaseDynamicQueue(InitilizerList<T>&& list) : BaseDynamicQueue() {
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
					if (++This->count > This->bufferSize) {
						if (isResizeIfFull)
							this->ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
						else
							Throw(NullPointException(ExceptionPosition));
					}

					SizeT newRear = (This->rear + 1) % This->bufferSize;
					This->queue[This->rear] = value;
					This->rear = newRear;
				}

				void Enqueue(T&& value, bool isResizeIfFull = false) {
					if (++This->count > This->bufferSize) {
						if (isResizeIfFull)
							this->ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
						else
							Throw(NullPointException(ExceptionPosition));
					}

					SizeT newRear = (This->rear + 1) % This->bufferSize;
					This->queue[This->rear] = MoveRValue(value);
					This->rear = newRear;
				}

				T Dequeue() {
					if (This->count == 0)
						Throw(NullPointException(ExceptionPosition));

					T res = MoveRValue(This->queue[This->front]);
					This->front = (This->front + 1) % This->bufferSize;
					--This->count;

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

				void Extend(SizeT additionalSize) {
					ExtendImpl(additionalSize);
				}

				void Shrink() {
					const SizeT newSize = This->count;
					T* newBuffer = new T[newSize];
					for (SizeT i = 0; i < This->count; i++)
					{
						newBuffer[i] = (*This)[i];
					}
					This->front = 0; This->rear = This->count;
					delete This->queue;
					This->queue = newBuffer;
				}

				template<typename TU>
				friend void Seed::Core::Swap(TU& a, TU& b);

				SizeT GetCount() const { return This->count; }
				SizeT Property(GetCount) Count;

				virtual BaseDynamicQueue Clone() const override final {
					BaseDynamicQueue result;
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
				}

				RefCountClass(BaseDynamicQueue, This)
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
#pragma warning(push)
#pragma warning(disable: 4172)

				template<typename TLambdaT>
				__forceinline BaseDynamicQueue& ForEachImpl(TLambdaT&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<TLambdaT, void, T&> ||
						TypeTraits::IsLambdaConvertibleV<TLambdaT, void, const T&>) {
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda((*This)[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<TLambdaT, void, T&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<TLambdaT, void, const T&>) {
						SizeT index = 0;

						for (SizeT i = 0; i < This->count; i++, index)
						{
							lambda((*This)[i], index);
						}

						return *this;
					}
					else {
						// ReSharper disable once CppStaticAssertFailure
						static_assert(false);
						__assume(false);
						return *this;
					}
				}

				template<typename TLambdaT>
				__forceinline const BaseDynamicQueue& ForEachImpl(TLambdaT&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<TLambdaT, void, const T&>) {
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda((*This)[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<TLambdaT, void, const T&, SizeT>) {
						SizeT index = 0;

						for (SizeT i = 0; i < This->count; i++, index)
						{
							lambda((*This)[i], index);
						}

						return *this;
					}
					else {
						// ReSharper disable CppStaticAssertFailure
						static_assert(false);
						// ReSharper restore CppStaticAssertFailure
						__assume(false);
						return *this;
					}
				}

				template<typename TU, typename TLambdaT>
				__forceinline TU AggregateImpl(TLambdaT&& lambda) const {
					TU result = TU();

					if (This->count == 0) {
						return MoveRValue(result);
					}

					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(result, (*This)[i]);
					}

					return MoveRValue(result);
				}

				template<typename TU, typename TLambdaT>
				__forceinline TU AggregateImpl(TLambdaT&& lambda, const TU& value) const {
					TU result = value;
					if (This->count == 0) {
						return MoveRValue(result);
					}

					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(result, (*This)[i]);
					}

					return MoveRValue(result);
				}

				template<typename TU, typename TLambdaT>
				__forceinline TU AggregateImpl(TLambdaT&& lambda, TU&& value) const {
					TU result = MoveRValue(value);

					if (This->count == 0)
						return MoveRValue(result);

					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(result, (*This)[i]);
					}

					return MoveRValue(result);
				}

				bool AnyImpl() const { return This->count > 0; }

				template<typename TLambda>
				__forceinline bool AnyImpl(TLambda&& lambda) const {
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

					T& ret = This->queue[(This->rear == 0) ? This->bufferSize - 1 : This->rear - 1];

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

					T& ret = This->queue[(This->rear == 0) ? This->bufferSize - 1 : This->rear - 1];

					return ret;
				}

				BaseDynamicQueue& ReverseImpl() {
					for (SizeT i = 0; i < This->count / 2; i++)
					{
						Swap((*This)[i], (*This)[(This->count - 1) - i]);
					}
					return *this;
				}

				__forceinline void UnsafeSet(const T& value, SizeT index) {
					This->queue[index] = value;
				}

				__forceinline void UnsafeSet(T&& value, SizeT index) {
					This->queue[index] = MoveRValue(value);
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

				template<typename TLambda>
				__forceinline BaseDynamicQueue& RemoveAllImpl(TLambda&& lambda) {
					if (This->count == 0)
						return *this;
					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(MoveRValue((*This)[i]));
					}
					return *this;
				}
			};

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

		template<typename THeap, typename T>
		__forceinline void Swap(Containers::BaseDynamicQueue<THeap, T>& A, Containers::BaseDynamicQueue<THeap, T>& B) {
			Swap(A.This, B.This);
		}
	}
}
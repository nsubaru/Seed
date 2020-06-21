#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.API.LINQ.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			/// <summary>
			/// Черга елементів
			/// </summary>
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
					T* restrict queue;
					SizeT bufferSize;
					RefCountT RefCount;
					

					forceinline T& operator[](SizeT Index) {
						return queue[(front + Index) % bufferSize];
					}

					forceinline Data()
						: front(0),
						  rear(0),
						  count(0),
						  queue(),
						  bufferSize(0),
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
				/// <summary>
				/// Пустий конструктор конкуретної черги
				/// </summary>
				forceinline BaseDynamicQueue() : This(new Data()) {}

				/// <summary>
				/// Конструктор черги з певною максимальною кількістю елементів
				/// </summary>
				/// <param name="startCount">Максимальна кількість елементів в черзі</param>
				forceinline BaseDynamicQueue(SizeT startCount) : This(new Data(startCount)) {}

				/// <summary>
				/// Конструктор черги з елементами з списку ініціалізації
				/// </summary>
				/// <param name="list">Список ініціалізації</param>
				BaseDynamicQueue(InitilizerList<T>&& list) : BaseDynamicQueue() {
					if (list.size() != 0) {
						SizeT actuallyCopyied = list.size();
						This->bufferSize = list.size();
						This->queue = new T[This->bufferSize];
						auto it = list.begin();
						for (SizeT i = 0; i < actuallyCopyied; i++) {
							This->queue[i] = MoveRValue(*it); ++it;
						}
						This->rear = This->count = actuallyCopyied;
						This->front = 0;
					}
				}

				/// <summary>
				/// Додавання елементів до черги
				/// </summary>
				/// <param name="value">Значення елемента</param>
				/// <param name="isResizeIfFull">Якщо true то чергу розширить автоматично при потребі, 
				/// якщо false і черга немає місця то вона згенерує помилку
				/// </param>
				forceinline void Enqueue(const T& value, bool isResizeIfFull = false) {
					if (++This->count > This->bufferSize) {
						if (isResizeIfFull)
							this->ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
						else
							Throw(FullQueueException(ExceptionPosition, This->bufferSize));
					}

					SizeT newRear = (This->rear + 1) % This->bufferSize;
					This->queue[This->rear] = value;
					This->rear = newRear;
				}

				/// <summary>
				/// Оптимізоване додавання елементів до черги
				/// </summary>
				/// <param name="value">RValue до </param>
				/// <param name="isResizeIfFull">Якщо true то чергу розширить автоматично при потребі, 
				/// якщо false і черга немає місця то вона згенерує помилку
				/// </param>
				forceinline void Enqueue(T&& value, bool isResizeIfFull = false) {
					if (++This->count > This->bufferSize) {
						if (isResizeIfFull)
							this->ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
						else
							Throw(FullQueueException(ExceptionPosition, This->bufferSize));
					}

					SizeT newRear = (This->rear + 1) % This->bufferSize;
					This->queue[This->rear] = MoveRValue(value);
					This->rear = newRear;
				}

				/// <summary>
				/// Виймання елемента з черги, якщо елементів немає 
				/// якщо черга пуста то генерує помилку
				/// </summary>
				/// <returns>Значення елементу</returns>
				forceinline T Dequeue() {
					if (This->count == 0)
						Throw(EmptyQueueException(ExceptionPosition));

					T res = MoveRValue(This->queue[This->front]);
					This->front = (This->front + 1) % This->bufferSize;
					--This->count;

					return MoveRValue(res);
				}

				/// <summary>
				/// Вертає значення елемента без виймання з черги
				/// якщо черга пуста то генерує помилку
				/// </summary>
				/// <returns></returns>
				forceinline const T& Top() {
					if (This->front == This->rear)
						Throw(EmptyQueueException(ExceptionPosition));
					return This->queue[This->front];
				}

				/// <summary>
				/// Вертає значення елемента без виймання з черги
				/// якщо черга пуста то генерує помилку
				/// </summary>
				/// <returns></returns>
				forceinline const T& Top() const {
					if (This->front == This->rear)
						Throw(EmptyQueueException(ExceptionPosition));
					return This->queue[This->front];
				}

				/// <summary>
				/// Розширення черги на задану кількість елемент
				/// </summary>
				/// <param name="AdditionalSize">Кількість на яку збільшити треба</param>
				forceinline void Extend(SizeT additionalSize) {
					ExtendImpl(additionalSize);
				}

				/// <summary>
				/// Зменшити розмір черги до поточної кількості елементів
				/// </summary>
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

				/// <summary>
				/// Кількість елементів в черзі
				/// </summary>
				/// <returns>Поточна кількість елементів</returns>
				forceinline SizeT GetCount() const { return This->count; }

				/// <summary>
				/// Поточна кількість елементів
				/// </summary>
				SizeT Property(GetCount) Count;

				/// <summary>
				/// Клонування об'єкта
				/// </summary>
				/// <returns>Новий об'єкт з копією елементів</returns>
				virtual BaseDynamicQueue Clone() const override final {
					if constexpr (TypeTraits::IsConstructible<T>)
					{
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
					else if constexpr (TypeTraits::IsBasedOnV<IClonable<T>, T>)
					{
						BaseDynamicQueue result;
						result.This->queue = new T[This->bufferSize];
						result.This->front = This->front;
						result.This->rear = This->rear;
						result.This->count = This->count;
						result.This->bufferSize = This->bufferSize;
						for (SizeT i = 0; i < This->count; i++)
						{
							(*result.This)[i] = (*This)[i].Clone();
						}
						return result;
					}
					else
					{
						Throw(NotImplementedException(ExceptionPosition));
					}
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
                friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
                friend class LINQ::IAssociableCollection;
#pragma warning(push)
#pragma warning(disable: 4172)

				template<typename TLambdaT>
				BaseDynamicQueue& ForEachImpl(TLambdaT&& lambda) {
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
				const BaseDynamicQueue& ForEachImpl(TLambdaT&& lambda) const {
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
						static_assert(false);
						__assume(false);
						return *this;
					}
				}

				template<typename T, typename Lambda>
				T AggregateImpl(Lambda&& lambda) const {
					T result = T();

					if (This->count == 0) {
						return MoveRValue(result);
					}

					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(result, (*This)[i]);
					}

					return MoveRValue(result);
				}

				template<typename T, typename Lambda>
				T AggregateImpl(Lambda&& lambda, const T& value) const {
					T result = value;
					if (This->count == 0) {
						return MoveRValue(result);
					}

					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(result, (*This)[i]);
					}

					return MoveRValue(result);
				}

				template<typename T, typename Lambda>
				T AggregateImpl(Lambda&& lambda, T&& value) const {
					T result = MoveRValue(value);

					if (This->count == 0)
						return MoveRValue(result);

					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(result, (*This)[i]);
					}

					return MoveRValue(result);
				}

				forceinline bool AnyImpl() const { return This->count > 0; }

				template<typename TLambda>
				bool AnyImpl(TLambda&& lambda) const {
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
				bool AllImpl(Lambda&& lambda) const {
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
				forceinline T& FirstImpl(Lambda&& lambda) {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

					T& ret = This->queue[This->front];

					return ret;
				}

				template<typename Lambda>
				forceinline T& LastImpl(Lambda&& lambda) {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

					T& ret = This->queue[(This->rear == 0) ? This->bufferSize - 1 : This->rear - 1];

					return ret;
				}

				template<typename Lambda>
				forceinline const T& FirstImpl(Lambda&& lambda) const {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

					T& ret = This->queue[This->front];

					return ret;
				}

				template<typename Lambda>
				forceinline const T& LastImpl(Lambda&& lambda) const {
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

				forceinline void UnsafeSet(const T& value, SizeT index) {
					This->queue[index] = value;
				}

				forceinline void UnsafeSet(T&& value, SizeT index) {
					This->queue[index] = MoveRValue(value);
				}

				forceinline BaseDynamicQueue& RemoveAllImpl() {
					if (This->count == 0)
						return *this;
					delete[] This->queue;
					This->front = This->rear = This->count = This->bufferSize = 0;
					This->queue = nullptr;
					return *this;
				}

				template<typename TLambda>
				forceinline BaseDynamicQueue& RemoveAllImpl(TLambda&& lambda) {
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
				}
			};

			/// <summary>
			/// Черга елементів
			/// </summary>
			namespace {
				template<typename T>
				using DynamicQueue = BaseDynamicQueue<Seed::Core::Memory::Heap, T>;
			}

			/// <summary>
			/// Черга елементів в захищеній кучі
			/// </summary>
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
		forceinline void Swap(Containers::BaseDynamicQueue<THeap, T>& A, Containers::BaseDynamicQueue<THeap, T>& B) {
			Swap(A.This, B.This);
		}
	}
}
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
			/// <summary>
			/// Черга з підтримкою багатопотоковості, також вона реалізовує контроль потоку
			/// </summary>
			template<typename THeap, typename T>
			class BaseConcurencyDynamicQueue final :
				public LINQ::IDynamicStack<BaseConcurencyDynamicQueue, THeap, T, true>,
				public IDisposable
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

					ForceInline T& operator[](SizeT index) {
						return queue[(front + index) % bufferSize];
					}

					ForceInline Data()
						: front(0),
						  rear(0),
						  count(0),
						  queue(),
						  bufferSize{0},
						  RefCount(1) {}

					ForceInline Data(SizeT startCount)
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
				ForceInline virtual void Dispose() final override
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
				ForceInline BaseConcurencyDynamicQueue() : This(new Data()) {}

				/// <summary>
				/// Конструктор черги з певною максимальною кількістю елементів
				/// </summary>
				/// <param name="startCount">Максимальна кількість елементів в черзі</param>
				ForceInline BaseConcurencyDynamicQueue(SizeT startCount) : This(new Data(startCount)) {}

				/// <summary>
				/// Конструктор черги з елементами з списку ініціалізації
				/// </summary>
				/// <param name="list">Список ініціалізації</param>
				BaseConcurencyDynamicQueue(InitilizerList<T>&& list) : BaseConcurencyDynamicQueue() {
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
				/// якщо false і черга немає місця то вона зупинить виконання команди
				/// до моменту коли зявиться місце в черзі</param>
				void Enqueue(const T& value, bool isResizeIfFull = false) {
					This->flowControler.Produce([&]() InlineLambda {
						if (This->count >= This->bufferSize) {
							if (!isResizeIfFull)
								This->flowControler.WaitForConcumer();
							else
								ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
						}

						SizeT newRear = (This->rear + 1) % This->bufferSize;
						This->queue[This->rear] = value;
						This->rear = newRear;
						++This->count;
						});
				}

				/// <summary>
				/// Оптимізоване додавання елементів до черги
				/// </summary>
				/// <param name="value">RValue до </param>
				/// <param name="isResizeIfFull">Якщо true то чергу розширить автоматично при потребі, 
				/// якщо false і черга немає місця то вона зупинить виконання команди 
				/// до моменту коли зявиться місце в черзі</param>
				void Enqueue(T&& Value, bool IsResizeIfFull = false) {
					This->flowControler.Produce([&, Value = MoveRValue(Value)]() mutable InlineLambda {
						if (This->count >= This->bufferSize) {
							if (!IsResizeIfFull)
								This->flowControler.WaitForConcumer();
							else
								ExtendImpl(CurrentSettings().MemoryManager.QueueReserveSize);
						}

						SizeT newRear = (This->rear + 1) % This->bufferSize;
						This->queue[This->rear] = MoveRValue(Value);
						This->rear = newRear;
						++This->count;
					});
				}


				/// <summary>
				/// Виймання елемента з черги, якщо елементів немає 
				/// то метод зупинить виконання потоку до моменту появи елемента
				/// </summary>
				/// <returns>Значення елементу</returns>
				T Dequeue() {
					return This->flowControler.Concume([&]() InlineLambda {
						while (This->count == 0) This->flowControler.WaitForProducer();

						T res = This->queue[This->front];
						This->front = (This->front + 1) % This->bufferSize;
						--This->count;

						return res;
						});
				}

				/// <summary>
				/// Вертає значення елемента без виймання з черги
				/// якщо черга пуста то генерує помилку
				/// </summary>
				/// <returns></returns>
				ForceInline const T& Top() {
					return This->flowControler.SharedLock([&]() -> const T& InlineLambda {
						if (This->front == This->rear)
							Throw(EmptyQueueException(ExceptionPosition));
						return This->queue[This->front];
						});
				}

				/// <summary>
				/// Вертає значення елемента без виймання з черги
				/// якщо черга пуста то генерує помилку
				/// </summary>
				/// <returns></returns>
				ForceInline const T& Top() const {
					return This->flowControler.SharedLock([&]() -> const T& InlineLambda {
						if (This->front == This->rear)
							Throw(EmptyQueueException(ExceptionPosition));
						return This->queue[This->front];
						});
				}

				/// <summary>
				/// Розширення черги на задану кількість елемент
				/// </summary>
				/// <param name="AdditionalSize">Кількість на яку збільшити треба</param>
				ForceInline void Extend(SizeT AdditionalSize) {
					This->flowControler.ExclusiveLock([&]() InlineLambda {
						ExtendImpl(AdditionalSize);
						});
				}

				/// <summary>
				/// Зменшити розмір черги до поточної кількості елементів
				/// </summary>
				void Shrink() {
					This->flowControler.ExclusiveLock([&]() InlineLambda {
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
				ForceInline friend void Seed::Core::Swap(TU& a, TU& b);

				/// <summary>
				/// Кількість елементів в черзі
				/// </summary>
				/// <returns>Поточна кількість елементів</returns>
				ForceInline SizeT GetCount() const { return This->count; }

				/// <summary>
				/// Поточна кількість елементів
				/// </summary>
				SizeT Property(GetCount) Count;				/// <summary>
				/// Максимальна елементів в черзі
				/// </summary>
				/// <returns>Максимальна кількість елементів</returns>
				ForceInline SizeT GetMaxCount() const { return This->bufferSize; }

				/// <summary>
				/// Максимальна кількість елементів
				/// </summary>
				SizeT Property(GetMaxCount) MaxCount;

				/// <summary>
				/// Вказує чи черга заповнена
				/// </summary>
				/// <returns>Вказує чи черга заповнена</returns>
				Boolean GetIsFull() const {
					return This->count == This->bufferSize;
				}

				/// <summary>
				/// Вказує чи черга заповнена
				/// </summary>
				Boolean Property(GetIsFull) IsFull;

				/// <summary>
                /// Клонування масиву
                /// </summary>
                /// <returns>Вертає новий масив з копією всіх елементів</returns>
                template<typename = void> requires (Concepts::IsClonable<T>) 
                BaseConcurencyDynamicQueue Clone() const {
                    if constexpr (TypeTraits::IsPodV<T>)
                    {
						return This->flowControler.SharedLock([&] () -> BaseConcurencyDynamicQueue InlineLambda
						{
							auto item = BaseConcurencyDynamicQueue(this->This->count);
							Memory::Copy(this->This->queue, item.This->queue, sizeof(T) * this->This->count);
							item.This->front = this->This->front;
							item.This->rear = this->This->rear;
							item.This->count = this->This->count;
							return item;
						});
                    }
                    else
                    {
						return This->flowControler.SharedLock([&] () -> BaseConcurencyDynamicQueue InlineLambda
						{
							auto item = BaseConcurencyDynamicQueue(this->This->count);
							for (SizeT i = 0; i < This->count; i++)
							{
								item.This->queue[i] = This->queue[i].Clone();
							}
							item.This->front = this->This->front;
							item.This->rear = this->This->rear;
							item.This->count = this->This->count;
							return item;
						});
                    }
                }

				/// <summary>
                /// Клонування масиву
                /// </summary>
                /// <param name="lambda">Функція задопомогою якої відбувається клонування</param>
                /// <returns>Вертає новий масив з копією всіх елементів</returns>
                template<typename TLambda> requires (Concepts::IsInvockableWithResult<TLambda, T, const T&>)
                BaseConcurencyDynamicQueue Clone(TLambda&& lambda) const {
					return This->flowControler.SharedLock([&] () -> BaseConcurencyDynamicQueue InlineLambda
					{
						auto item = BaseConcurencyDynamicQueue(this->This->count);
						for (SizeT i = 0; i < This->count; i++)
						{
							item.This->queue[i] = lambda(This->queue[i]);
						}
						item.This->front = this->This->front;
						item.This->rear = this->This->rear;
						item.This->count = this->This->count;
						return item;
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
					This->bufferSize = newSize;
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

				template<typename TLambda>
				BaseConcurencyDynamicQueue& ForEachImpl(TLambda&& lambda) {
					if constexpr (TypeTraits::IsInvockable<TLambda, T&> ||
						TypeTraits::IsInvockable<TLambda, const T&>) {

						return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& InlineLambda {
							for (SizeT i = 0; i < This->count; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsInvockable<TLambda, T&, SizeT> ||
						TypeTraits::IsInvockable<TLambda, const T&, SizeT>) {

						return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& InlineLambda {
							SizeT index = 0;

							for (SizeT i = 0; i < This->count; i++, index++)
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

				template<typename TLambda>
				const BaseConcurencyDynamicQueue& ForEachImpl(TLambda&& lambda) const {
					if constexpr (TypeTraits::IsInvockable<TLambda, const T&>) {
						return This->flowControler.SharedLock([&]() -> const BaseConcurencyDynamicQueue& InlineLambda {
							for (SizeT i = 0; i < This->count; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsInvockable<TLambda, const T&, SizeT>) {
						return This->flowControler.SharedLock([&]() -> const BaseConcurencyDynamicQueue& InlineLambda {
							SizeT Index = 0;

							for (SizeT i = 0; i < This->count; i++, Index++)
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

				template<typename TLambda>
				BaseConcurencyDynamicQueue& ForEachImpl(TLambda&& lambda, SizeT requiredCount) {
					if (requiredCount > this->Count)
                        requiredCount = this->Count;

					if constexpr (TypeTraits::IsInvockable<TLambda, T&> ||
						TypeTraits::IsInvockable<TLambda, const T&>) {

						return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& InlineLambda {
							for (SizeT i = 0; i < requiredCount; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsInvockable<TLambda, T&, SizeT> ||
						TypeTraits::IsInvockable<TLambda, const T&, SizeT>) {

						return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& InlineLambda {
							SizeT index = 0;

							for (SizeT i = 0; i < requiredCount; i++, index++)
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

				template<typename TLambda>
				const BaseConcurencyDynamicQueue& ForEachImpl(TLambda&& lambda, SizeT requiredCount) const {
					if (requiredCount > this->Count)
                        requiredCount = this->Count;

					if constexpr (TypeTraits::IsInvockable<TLambda, const T&>) {
						return This->flowControler.SharedLock([&]() -> const BaseConcurencyDynamicQueue& InlineLambda {
							for (SizeT i = 0; i < requiredCount; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsInvockable<TLambda, const T&, SizeT>) {
						return This->flowControler.SharedLock([&]() -> const BaseConcurencyDynamicQueue& InlineLambda {
							SizeT Index = 0;

							for (SizeT i = 0; i < requiredCount; i++, Index++)
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
							result = lambda(result, (*This)[i]);
						}

						return MoveRValue(result);
						});
				}

				template<typename T, typename Lambda>
				T AggregateImpl(Lambda&& lambda, const T& value) const {
					return This->flowControler.SharedLock([&]() InlineLambda {
						T result = value;
						if (This->count == 0) {
							return MoveRValue(result);
						}

						for (SizeT i = 0; i < This->count; i++)
						{
							result = lambda(result, (*This)[i]);
						}

						return MoveRValue(result);
						});
				}

				template<typename T, typename Lambda>
				ForceInline T AggregateImpl(Lambda&& lambda, T&& value) const {
					return This->flowControler.SharedLock([&, Value = MoveRValue(value)]() mutable InlineLambda {
						T result = MoveRValue(Value);

						if (This->count == 0)
							return MoveRValue(result);

						for (SizeT i = 0; i < This->count; i++)
						{
							result = lambda(result, (*This)[i]);
						}

						return MoveRValue(result);
					});
				}

				ForceInline bool AnyImpl() const { return This->count > 0; }

				template<typename Lambda>
				bool AnyImpl(Lambda&& lambda) const {
					return This->flowControler.SharedLock([&]() InlineLambda {
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
					return This->flowControler.SharedLock([&]() InlineLambda {
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
					return This->flowControler.SharedLock([&]() -> T& InlineLambda {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T& ret = This->queue[This->front];

						return ret;
						});
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
					return This->flowControler.SharedLock([&]() -> T& InlineLambda {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T& ret = This->queue[(This->rear == 0) ? This->bufferSize - 1 : This->rear];

						return ret;
						});
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
					return This->flowControler.SharedLock([&]() -> const T& InlineLambda {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T& ret = This->queue[This->front];

						return ret;
						});
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
					return This->flowControler.SharedLock([&]() -> const T& InlineLambda {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T& ret = This->queue[(This->rear == 0) ? This->bufferSize - 1 : This->rear];

						return ret;
						});
				}

				BaseConcurencyDynamicQueue& ReverseImpl() {
					return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& InlineLambda {
						for (SizeT i = 0; i < This->count / 2; i++)
						{
							Seed::Swap((*This)[i], (*This)[(This->count - 1) - i]);
						}
						return *this;
						});
				}

				ForceInline void UnsafeSet(const T& value, SizeT index) {
					This->queue[index] = value;
					This->rear = index;
					This->count = This->bufferSize;
				}

				ForceInline void UnsafeSet(T&& value, SizeT index) {
					This->queue[index] = MoveRValue(value);
					This->rear = index;
					This->count = This->bufferSize;
				}

				BaseConcurencyDynamicQueue& RemoveAllImpl() {
					return This->flowControler.SharedLock([&]() -> BaseConcurencyDynamicQueue& InlineLambda {
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
					return This->flowControler.SharedLock([&, lambda = MoveRValue(lambda)]()->BaseConcurencyDynamicQueue& InlineLambda {
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
					return This->lock.SharedLock([&]() InlineLambda {
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

			/// <summary>
			/// Черга з підтримкою багатопотоковості, також вона реалізовує контроль потоку
			/// </summary>
			namespace {
				template<typename T>
				using ConcurencyDynamicQueue = BaseConcurencyDynamicQueue<Seed::Core::Memory::Heap, T>;
			}

			/// <summary>
			/// Черга з підтримкою багатопотоковості в захищеній кучі, також вона реалізовує контроль потоку
			/// </summary>
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
		ForceInline void Swap(Containers::BaseConcurencyDynamicQueue<THeap, T>& A, Containers::BaseConcurencyDynamicQueue<THeap, T>& B) {
			//TODO: Interlocked Exchange
			Swap(A.This, B.This);
		}
	}
}
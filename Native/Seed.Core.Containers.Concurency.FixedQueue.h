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
			/// <summary>
			/// Черга з фіксованою максимальною кількістю, контролем потоку і підтримкою багатопотоковості
			/// </summary>
			template<typename THeap, SizeT TSize, typename T>
			class BaseConcurencyFixedQueue :
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

					forceinline T& operator[](SizeT Index) {
						return queue[(front + Index) % TSize];
					}

					forceinline Data() :
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
				forceinline virtual void Dispose() final override {}
			public:
				/// <summary>
				/// Пустий конструктор ініціалізовує пусту чергу
				/// </summary>
				forceinline BaseConcurencyFixedQueue() : This(new Data()) {}

				/// <summary>
				/// Конструктор з списку елементів, якщо елементів забагато то решта ігнорується
				/// </summary>
				/// <param name="list">Список елементів</param>
				forceinline BaseConcurencyFixedQueue(InitilizerList<T>&& list) : BaseConcurencyFixedQueue() {
					if (list.size() != 0) {
						SizeT ActuallyCopyied = (TSize < list.size()) ? TSize : list.size();
						auto it = list.begin();
						SizeT i = ActuallyCopyied;
						for (SizeT i = 0; i < ActuallyCopyied; i++) {
							This->queue[i] = MoveRValue(*it); it++;
						}
						This->rear = This->count = ActuallyCopyied;
						This->front = 0;
					}
				}

				/// <summary>
				/// Додавання елемента до черги, 
				/// якщо в черзі немає місця то потік очікує звільнення місця
				/// </summary>
				/// <param name="Value">Значення елемента</param>
				forceinline void Enqueue(const T& Value) {
					This->flowControler.Produce([&]() {
						if (++This->count > TSize)
							This->flowControler.WaitForConcumer();
						SizeT newRear = (This->rear + 1) % TSize;
						This->queue[This->rear] = Value;
						This->rear = newRear;
						});
				}

				/// <summary>
				/// Додавання елемента до черги, 
				/// якщо в черзі немає місця то потік очікує звільнення місця
				/// </summary>
				/// <param name="Value">Значення елемента</param>
				forceinline void Enqueue(T&& Value) {
					This->flowControler.Produce([&, Value = MoveRValue(Value)]() {
						if (++This->count > TSize)
							This->flowControler.WaitForConcumer();
						SizeT newRear = (This->rear + 1) % TSize;
						This->queue[This->rear] = MoveRValue(Value);
						This->rear = newRear;
					});
				}

				/// <summary>
				/// Виймає елемент з черги, якщо черга пуста то призупиняє виконання потоку
				/// </summary>
				/// <returns>Вийнятий елемент</returns>
				forceinline T Dequeue() {
					return This->flowControler.Concume([&]() -> T {
						if (This->count == 0)
							This->flowControler.WaitForProducer();
						T res = MoveRValue(This->queue[This->front]);
						This->front = (This->front + 1) % TSize;
						--This->count; //-V803
						return res;
						});
				}

				/// <summary>
				/// Вертає перший елемент з черги без виймання з неї, якщо елемента немає то генерує помилку
				/// </summary>
				/// <returns>Значення елемента</returns>
				forceinline const T& Top() {
					return This->flowControler.SharedLock([&]() -> const T& {
						if (This->front == This->rear)
							Throw(EmptyQueueException(ExceptionPosition));
						return This->queue[This->front];
						});
				}

				/// <summary>
				/// Вертає перший елемент з черги без виймання з неї, якщо елемента немає то генерує помилку
				/// </summary>
				/// <returns>Значення елемента</returns>
				forceinline const T& Top() const {
					return This->flowControler.SharedLock([&]() -> const T& {
						if (This->front == This->rear)
							Throw(EmptyQueueException(ExceptionPosition));
						return This->queue[This->front];
						});
				}

				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);

				/// <summary>
				/// Вертає кількість елементів в черзі
				/// </summary>
				/// <returns>Поточна кількість елементів в черзі</returns>
				forceinline SizeT GetCount() const { return This->count; }

				/// <summary>
				/// Поточна кількість елементів в черзі
				/// </summary>
				SizeT Property(GetCount) Count;

				/// <summary>
				/// Клонує об'єкт
				/// </summary>
				/// <returns>Вертає новий об'єкт з копією всіх</returns>
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
                friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
			protected:
#pragma warning(push)
#pragma warning(disable: 4172)
				template<typename Lambda>
				BaseConcurencyFixedQueue& ForEachImpl(Lambda&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&> ||
						TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&>) {

						return This->flowControler.SharedLock([&]() -> BaseConcurencyFixedQueue& {
							for (SizeT i = 0; i < This->count; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&>) {

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

				template<typename Lambda>
				const BaseConcurencyFixedQueue& ForEachImpl(Lambda&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&>) {
						return This->flowControler.SharedLock([&]() -> const BaseConcurencyFixedQueue& {
							for (SizeT i = 0; i < This->count; i++)
							{
								lambda((*This)[i]);
							}
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&, SizeT>) {
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

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda) const {
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

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda, const U& Value) const {
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

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda, U&& Value) const {
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
					return This->flowControler.SharedLock([&]() -> T& {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						for (SizeT i = 0; i < This->count; i++)
						{
							if (!lambda((*This)[i]))
							{
								return (*This)[i];
							}
						}

						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						});
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
					return This->flowControler.SharedLock([&]() -> T& {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T* item = nullptr;

						for (SizeT i = 0; i < This->count; i++)
						{
							if (!lambda((*This)[i]))
							{
								item = &(*This)[i];
							}
						}

						if (item != nullptr)
						{
							return *item;
						}
						else
						{
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						}
						});
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
					return This->flowControler.SharedLock([&]() -> const T& {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						for (SizeT i = 0; i < This->count; i++)
						{
							if (!lambda((*This)[i]))
							{
								return (*This)[i];
							}
						}

						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						});
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
					return This->flowControler.SharedLock([&]() -> const T& {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

						T* item = nullptr;

						for (SizeT i = 0; i < This->count; i++)
						{
							if (!lambda((*This)[i]))
							{
								item = &(*This)[i];
							}
						}

						if (item != nullptr)
						{
							return *item;
						}
						else
						{
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						}
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

				forceinline void UnsafeSet(const T& Value, SizeT Index) {
					This->queue[Index] = Value;
					This->count = Index;
					This->rear = Index;
				}

				forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->queue[Index] = MoveRValue(Value);
					This->count = Index;
					This->rear = Index;
				}

				forceinline BaseConcurencyFixedQueue& RemoveAllImpl() {
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
				forceinline BaseConcurencyFixedQueue& RemoveAllImpl(Lambda&& lambda) {
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

			/// <summary>
			/// Черга з фіксованою максимальною кількістю в прості кучі, 
			/// контролем потоку і підтримкою багатопотоковості
			/// </summary>
			namespace {
				template<SizeT TSize, typename T>
				using ConcurencyFixedQueue = BaseConcurencyFixedQueue<Seed::Core::Memory::Heap, TSize, T>;
			}

			/// <summary>
			/// Черга з фіксованою максимальною кількістю в захищеній кучі, 
			/// контролем потоку і підтримкою багатопотоковості
			/// </summary>
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
		forceinline void Swap(Containers::BaseConcurencyFixedQueue<THeap, TSize, T>& A, Containers::BaseConcurencyFixedQueue<THeap, TSize, T>& B) {
			//TODO: Interlooked exchange
			Swap(A.This, B.This);
		}
	}
}
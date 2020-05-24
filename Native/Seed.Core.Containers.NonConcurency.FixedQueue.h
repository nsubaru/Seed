#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Threading.DataFlowControler.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.API.LINQ.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			/// <summary>
			/// Черга з фіксованою максимальною кількістю
			/// </summary>
			template<typename THeap, SizeT TSize, typename T>
			class BaseFixedQueue :
				public LINQ::IFixedQueue<BaseFixedQueue, THeap, T, TSize, true>,
				public IDisposable,
				public IClonable<BaseFixedQueue<THeap, TSize, T>>
			{
			private:
				struct Data {
					SizeT front;
					SizeT rear;
					RefCountT count;
					RefCountT RefCount;
					T queue[TSize];

					forceinline T& operator[](SizeT Index) {
						return queue[(front + Index) % TSize];
					}

					forceinline Data() :
						front(0),
						rear(0),
						count(0),
						RefCount(1),
						queue{}
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
				forceinline BaseFixedQueue() : This(new Data()) {}

				/// <summary>
				/// Конструктор з списку елементів, якщо елементів забагато то решта ігнорується
				/// </summary>
				/// <param name="list">Список елементів</param>
				BaseFixedQueue(InitilizerList<T>&& list) : BaseFixedQueue() {
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
				/// якщо в черзі немає місця то генерується помилка
				/// </summary>
				/// <param name="Value">Значення елемента</param>
				forceinline void Enqueue(const T& Value) {
					if (++This->count > TSize)
						Throw(NullPointException(ExceptionPosition));
					SizeT newRear = (This->rear + 1) % TSize;
					This->queue[This->rear] = Value;
					This->rear = newRear;
				}

				/// <summary>
				/// Додавання елемента до черги, 
				/// якщо в черзі немає місця то генерується помилка
				/// </summary>
				/// <param name="Value">Значення елемента</param>
				forceinline void Enqueue(T&& Value) {
					if (++This->count > TSize)
						Throw(NullPointException(ExceptionPosition));
					SizeT newRear = (This->rear + 1) % TSize;
					This->queue[This->rear] = MoveRValue(Value);
					This->rear = newRear;
				}

				/// <summary>
				/// Виймає елемент з черги, якщо черга пуста то генерує помилку
				/// </summary>
				/// <returns>Вийнятий елемент</returns>
				forceinline T Dequeue() {
					if (This->count == 0)
						Throw(NullPointException(ExceptionPosition));
					T res = MoveRValue(This->queue[This->front]);
					This->front = (This->front + 1) % TSize;
					--This->count; //-V803
					return res;
				}

				/// <summary>
				/// Вертає перший елемент з черги без виймання з неї, якщо елемента немає то генерує помилку
				/// </summary>
				/// <returns>Значення елемента</returns>
				forceinline const T& Top() {
					if (This->front == This->rear)
						Throw(NullPointException(ExceptionPosition));
					return This->queue[This->front];
				}

				/// <summary>
				/// Вертає перший елемент з черги без виймання з неї, якщо елемента немає то генерує помилку
				/// </summary>
				/// <returns>Значення елемента</returns>
				forceinline const T& Top() const {
					if (This->front == This->rear)
						Throw(NullPointException(ExceptionPosition));
					return This->queue[This->front];
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
				virtual BaseFixedQueue Clone() const override final {
					BaseFixedQueue result;
					result.This->front = This->front;
					result.This->rear = This->rear;
					result.This->count = This->count;
					for (SizeT i = 0; i < This->count; i++)
					{
						(*result.This)[i] = (*This)[i];
					}
					return result;
				}

				RefCountClass(BaseFixedQueue, This)
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
				BaseFixedQueue& ForEachImpl(Lambda&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&> ||
						TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&>) {
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda((*This)[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&, SizeT> ||
						TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&>) {
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

				template<typename Lambda>
				const BaseFixedQueue& ForEachImpl(Lambda&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&>) {
						for (SizeT i = 0; i < This->count; i++)
						{
							lambda((*This)[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&, SizeT>) {
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

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda) const {
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

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda, const U& Value) const {
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
				}

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda, U&& Value) const {
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
				forceinline bool AnyImpl(Lambda&& lambda) const {
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
				T& FirstImpl(Lambda&& lambda) {
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
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
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
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
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
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);

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
				}

				forceinline BaseFixedQueue& ReverseImpl() {
					for (SizeT i = 0; i < This->count / 2; i++)
					{
						Seed::Swap((*This)[i], (*This)[(This->count - 1) - i]);
					}
					return *this;
				}

				forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->queue[Index] = MoveRValue(Value);
					This->count = Index + 1;
					This->rear = Index + 1;
				}

				forceinline void UnsafeSet(const T& Value, SizeT Index) {
					This->queue[Index] = Value;
					This->count = Index + 1;
					This->rear = Index + 1;
				}

				forceinline BaseFixedQueue& RemoveAllImpl() {
					if (This->count == 0)
						return *this;
					for (SizeT i = 0; i < This->count; i++)
					{
						(*This)[i].~T();
					}
					return *this;
				}

				template<typename Lambda>
				forceinline BaseFixedQueue& RemoveAllImpl(Lambda&& lambda) {
					if (This->count == 0)
						return *this;
					for (SizeT i = 0; i < This->count; i++)
					{
						lambda(MoveRValue((*This)[i]));
					}
					return *this;
				}
			};

			/// <summary>
			/// Черга з фіксованою максимальною кількістю в простій кучі
			/// </summary>
			namespace {
				template<SizeT TSize, typename T>
				using FixedQueue = BaseFixedQueue<Seed::Core::Memory::Heap, TSize, T>;
			}

			/// <summary>
			/// Черга з фіксованою максимальною кількістю в захищеній кучі
			/// </summary>
			namespace {
				template<SizeT TSize, typename T>
				using SensitiveFixedQueue = BaseFixedQueue<Seed::Core::Memory::SensitiveDataHeap, TSize, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, SizeT TSize, typename T>
				struct IsFixedQueueT<Seed::Core::Containers::BaseFixedQueue<THeap, TSize, T>> : LogicTrue {};
			}
		}

		template<typename THeap, SizeT TSize, typename T>
		forceinline void Swap(Containers::BaseFixedQueue<THeap, TSize, T>& A, Containers::BaseFixedQueue<THeap, TSize, T>& B) {
			Swap(A.This, B.This);
		}
	}
}
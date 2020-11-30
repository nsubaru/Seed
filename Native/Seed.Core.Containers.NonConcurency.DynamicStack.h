#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.API.LINQ.h"

#pragma warning(push)
#pragma warning(disable: 4172)

namespace Seed {
	inline namespace Core {
		namespace Containers {
			/// <summary>
			/// Стек елементів
			/// </summary>
			template<typename THeap, typename T>
			class BaseDynamicStack :
				public LINQ::IDynamicStack<BaseDynamicStack, THeap, T, false>,
				public IDisposable,
				public IClonable<BaseDynamicStack<THeap, T>>
			{
			private:
				struct Data {
					RefCountT count;
					RefCountT RefCount;
					SizeT bufferSize;
					T* stack;

					ForceInline Data(SizeT bufferSize) : count(0), RefCount(1), stack(new T[bufferSize]), bufferSize(bufferSize) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() final override {
					delete[] This->stack;
					This->count = This->bufferSize = 0;
				}
			public:
				/// <summary>
				/// Пустий конструктор ініціалізовує стек з стандартною максимальною кількістю елементів
				/// </summary>
				ForceInline BaseDynamicStack() : This(new Data(CurrentSettings().MemoryManager.StackReserveSize)) {}

				/// <summary>
				/// Конструктор з списку ініціалізації, 
				/// переміщає елементи з списку, розмір ставить 
				/// як суму кількості елементів плюс стандартну кількість елементів
				/// </summary>
				/// <param name="list">Список елементів</param>
				BaseDynamicStack(InitilizerList<T>&& list) {
					if (list.size() != 0) {
						This = new Data(list.size() + CurrentSettings().MemoryManager.StackReserveSize);
						auto it = list.begin();
						SizeT i = list.size();
						do {
							i--; This->stack[i] = MoveRValue(*it); it++;
						} while (i != 0);
						This->count = list.size();
					}
					else {
						This = new Data(CurrentSettings().MemoryManager.StackReserveSize);
					}
				}

				/// <summary>
				/// Додає елемент в стек, якщо стек заповнений 
				/// то він розширюється на кількість задану в налаштуваннях
				/// </summary>
				/// <param name="item">Силка на елемент</param>
				ForceInline void Push(const T& item) {
					if (This->count == This->bufferSize) {
						ExtendBufferImpl(CurrentSettings().MemoryManager.StackReserveSize);
					}
					This->stack[This->count] = item;
					++This->count;
				}

				/// <summary>
				/// Додає елемент в стек, якщо стек заповнений 
				/// то він розширюється на кількість задану в налаштуваннях
				/// </summary>
				/// <param name="item">RValue силка на елемент</param>
				ForceInline void Push(T&& item) {
					if (This->count == This->bufferSize) {
						ExtendBufferImpl(CurrentSettings().MemoryManager.StackReserveSize);
					}
					This->stack[This->count] = MoveRValue(item);
					++This->count;
				}

				/// <summary>
				/// Виймає елемент з стеку, якщо стек пустий то генерує помилку
				/// </summary>
				/// <returns>Вийннятий елемент з стеку</returns>
				ForceInline T Pop() {
					if (This->count == 0)
						Throw(EmptyStackException(ExceptionPosition));
					--This->count;
					return MoveRValue(This->stack[This->count]);
				}

				/// <summary>
				/// Вертає верхній елемент без виймання з стеку, 
				/// якщо стек пустий то генерує помилку
				/// </summary>
				/// <returns>Константна силка на елемент</returns>
				ForceInline const T& Top() const {
					if (This->count == 0)
						Throw(EmptyStackException(ExceptionPosition));
					--This->count;
					return This->stack[This->count];
				}

				/// <summary>
				/// Вертає верхній елемент без виймання з стеку
				/// якщо стек пустий то генерує помилку
				/// </summary>
				/// <returns>Константна силка на елемент</returns>
				ForceInline const T& Top() {
					if (This->count == 0)
						Throw(EmptyStackException(ExceptionPosition));
					--This->count;
					return This->stack[This->count];
				}

				/// <summary>
				/// Розширення стеку на задану кількість елементів
				/// </summary>
				/// <param name="AdditionalSize">Кількість на яку необхідно розширити стек</param>
				ForceInline void ExtendBuffer(SizeT AdditionalSize) {
					ExtendBufferImpl(AdditionalSize);
				}

				/// <summary>
				/// Стиснення розміру стек до поточної кількості елементів
				/// </summary>
				void Shrink() {
					T* newBuffer = new T[This->count];
					for (SizeT i = 0; i < This->count; i++)
					{
						newBuffer[i] = MoveRValue(This->stack[i]);
					}
					delete This->stack;
					This->stack = newBuffer;
					This->bufferSize = This->count;
				}

				/// <summary>
				/// Клонування об'єкту стека
				/// </summary>
				/// <returns>Нови об'єкт стеку з копією елементів</returns>
				virtual BaseDynamicStack Clone() const override final {
					BaseDynamicStack result;
					result.This->bufferSize = This->bufferSize;
					result.This->count = This->count;
					result.This->stack = new T[This->bufferSize];
					for (SizeT i = 0; i < This->count; i++)
					{
						result.This->stack[i] = This->stack[i];
					}
					return result;
				}

				///<summary>Оптимізований обмін</summary>
				///<param name = 'A'>Перший елемент</param>
				///<param name = 'B'>Другий елемент</param>
				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);

				/// <summary>
				/// Кількість елементів в стеку
				/// </summary>
				/// <returns>Поточна кількість елементів в стеку</returns>
				ForceInline SizeT GetCount() const { return This->count; }

				/// <summary>
				/// Поточна кількість елементів в стеку
				/// </summary>
				SizeT Property(GetCount) Count;

				RefCountClass(BaseDynamicStack, This)
			private:
				void ExtendBufferImpl(SizeT AdditionalSize) {
					T* newBuffer = new T[This->bufferSize + AdditionalSize];
					for (SizeT i = 0; i < This->count; i++)
					{
						newBuffer[i] = MoveRValue(This->stack[i]);
					}
					delete This->stack;
					This->stack = newBuffer;
					This->bufferSize = This->bufferSize + AdditionalSize;
				}
			protected:
				template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollection;
				template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsConcurencySuport>
				friend class LINQ::IDynamicStack;
				template<template<typename THeap, typename T> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollectionBase;
                template<typename THeap, typename T, typename Enable>
                friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
                friend class LINQ::IAssociableCollection;
			protected:
				template<typename Lambda>
				BaseDynamicStack& ForEachImpl(Lambda&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&>) {
						if (This->count == 0)
							return *this;
						SizeT i = This->count;
						do {
							i--;
							lambda(This->stack[i]);
						} while (i != 0);
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&, SizeT>) {
						if (This->count == 0)
							return *this;
						SizeT i = This->count;
						do {
							i--;
							lambda(This->stack[i], i);
						} while (i != 0);
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename Lambda>
				const BaseDynamicStack& ForEachImpl(Lambda&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&>) {
						if (This->count == 0)
							return *this;
						SizeT i = This->count;
						do {
							i--;
							lambda(This->stack[i]);
						} while (i != 0);
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&, SizeT>) {
						if (This->count == 0)
							return *this;
						SizeT i = This->count;
						do {
							i--;
							lambda(This->stack[i], i);
						} while (i != 0);
						return *this;
					}
					else {
						static_assert(false);
					}
				}

				template<typename U, typename Lambda>
				T AggregateImpl(Lambda&& lambda) const {
					if (This->count == 0)
						return U();
					U result = U();
					SizeT i = This->count;
					do {
						i--;
						lambda(result, This->stack[i]);
					} while (i != 0);
					return MoveRValue(result);
				}

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda, const U& Value) const {
					U result = Value;
					if (This->count == 0)
						return result;
					SizeT i = This->count;
					do {
						i--;
						lambda(result, This->stack[i]);
					} while (i != 0);
					return MoveRValue(result);
				}

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda, U&& Value) const {
					if (This->count == 0)
						return Value;
					SizeT i = This->count;
					do {
						i--;
						lambda(Value, This->stack[i]);
					} while (i != 0);
					return MoveRValue(Value);
				}

				ForceInline bool AnyImpl() const { return This->count > 0; }

				template<typename Lambda>
				bool AnyImpl(Lambda&& lambda) const {
					if (This->count == 0)
						return false;
					SizeT i = This->count;
					do {
						i--;
						if (lambda(This->stack[i]))
							return true;
					} while (i != 0);
					return false;
				}

				template<typename Lambda>
				bool AllImpl(Lambda&& lambda) const {
					if (This->count == 0)
						return false;
					SizeT i = This->count;
					do {
						i--;
						if (!lambda(This->stack[i]))
							return false;
					} while (i != 0);
					return true;
				}

				template<typename Lambda>
				T& FirstImpl(Lambda&& lambda) {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					SizeT i = This->count;
					do {
						i--;
						if (lambda(This->stack[i]))
							return static_cast<T&>(This->array[i]);
					} while (i != 0);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					SizeT i = This->count;
					SizeT Index = -1;
					do {
						i--;
						if (lambda(This->stack[i]))
							Index = i;
					} while (i != 0);

					if (Index != -1)
						return static_cast<T&>(This->array[Index]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					SizeT i = This->count;
					do {
						i--;
						if (lambda(This->stack[i]))
							return static_cast<T&>(This->array[i]);
					} while (i != 0);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
					if (This->count == 0)
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
					SizeT i = This->count;
					SizeT Index = -1;
					do {
						i--;
						if (lambda(This->stack[i]))
							Index = i;
					} while (i != 0);

					if (Index != -1)
						return static_cast<T&>(This->array[Index]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				ForceInline BaseDynamicStack& ReverseImpl() {
					for (SizeT i = 0; i < This->count / 2; i++) {
						Swap(This->stack[i], This->stack[(This->count - 1) - i]);
					}
					return *this;
				}

				ForceInline void UnsafeSet(T&& Value, SizeT Index) {
					This->stack[Index] = MoveRValue(Value);
					This->count = Index + 1;
				}

				ForceInline void UnsafeSet(const T& Value, SizeT Index) {
					This->stack[Index] = Value;
					This->count = Index + 1;
				}

				ForceInline BaseDynamicStack& RemoveAllImpl() {
					delete[] This->stack;
					This->count = This->bufferSize = 0;
					return *this;
				}

				template<typename Lambda>
				ForceInline BaseDynamicStack& RemoveAllImpl(Lambda&& lambda) {
					for (SizeT i = 0; i < This->count; i++) {
						lambda(MoveRValue(This->stack[i]));
					}
					delete[] This->stack;
					This->count = This->bufferSize = 0;
					return *this;
				}
			};

			/// <summary>
			/// Стек елементів
			/// </summary>
			namespace {
				template<typename T>
				using DynamicStack = BaseDynamicStack<Seed::Core::Memory::Heap, T>;
			}

			/// <summary>
			/// Стек елементів в захищеній кучі
			/// </summary>
			namespace {
				template<typename T>
				using SensitiveDynamicStack = BaseDynamicStack<Seed::Core::Memory::SensitiveDataHeap, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename T>
				struct IsDynamicStackT<Seed::Core::Containers::BaseDynamicStack<THeap, T>> : LogicTrue {};
			}
		}

		template<typename THeap, typename T>
		ForceInline void Swap(Containers::BaseDynamicStack<THeap, T>& A, Containers::BaseDynamicStack<THeap, T>& B) {
			Swap(A.This, B.This);
		}
	}
}

#pragma warning(pop)
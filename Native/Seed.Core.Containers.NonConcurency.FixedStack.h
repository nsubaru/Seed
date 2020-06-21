#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.API.LINQ.h"

#pragma warning(push)
#pragma warning(disable: 4996)
namespace Seed {
	inline namespace Core {
		namespace Containers {
            /// <summary>
            /// Стек з фіксованою максимальною кількістю елементів
            /// </summary>
			template<typename THeap, SizeT TSize, typename T>
			class BaseFixedStack final :
				public LINQ::IFixedStack<BaseFixedStack, THeap, T, TSize, true>,
				public IDisposable,
				public IClonable<BaseFixedStack<THeap, TSize, T>>
			{
			private:
				struct Data {
					RefCountT count;
					RefCountT RefCount;
					T stack[TSize];

					forceinline Data() : count(0), RefCount(1), stack() {}
				};
			private:
				Data* This;
			protected:
				forceinline virtual void Dispose() final override {}
			public:
				/// <summary>
                /// Пустий конструктор
                /// </summary>
				forceinline BaseFixedStack() : This(new Data()) {}

				/// <summary>
                /// Конструктор з списку ініціалізаці, 
                /// якщо в списку більше елементів решта ігнорується
                /// </summary>
                /// <param name="list">Список ініціалізації</param>
				BaseFixedStack(InitilizerList<T>&& list) : This(new Data()) {
					if (list.size() != 0) {
						SizeT ActuallyCopyied = (TSize < list.size()) ? TSize : list.size();
						auto it = list.begin();
						SizeT i = ActuallyCopyied;
						do {
							i--; This->stack[i] = MoveRValue(*it); it++;
						} while (i != 0);
						This->count = ActuallyCopyied;
					}
				}

				/// <summary>
                /// Додає елемент в стек, якщо стек заповнений то генерує помилку
                /// </summary>
                /// <param name="item">Силка на елемент</param>
				forceinline void Push(const T& item) {
					if (This->count == TSize)
						Throw(FullStackException(ExceptionPosition, TSize));
					This->stack[This->count] = item;
					This->count++;
				}

				/// <summary>
                /// Додає елемент в стек, якщо стек заповнений то генерує помилку
                /// </summary>
                /// <param name="item">RValue силка на елемент</param>
				forceinline void Push(T&& item) {
					if (This->count == TSize)
						Throw(FullStackException(ExceptionPosition, TSize));
					This->stack[This->count] = MoveRValue(item);
					This->count++;
				}

				/// <summary>
                /// Виймання з стеку елемента, якщо стек пустий то генерується помилка
                /// </summary>
                /// <returns></returns>
				forceinline T Pop() {
					if (This->count == 0)
						Throw(EmptyStackException(ExceptionPosition));
					This->count--;
					return MoveRValue(This->stack[This->count]);
				}

				/// <summary>
                /// Отримання верхнього елементу в стеку, якщо стек пустий то генерується помилка 
                /// </summary>
                /// <returns>Силка на вехній елемент</returns>
				forceinline const T& Top() {
					if (This->count == 0)
						Throw(EmptyStackException(ExceptionPosition));
					This->count--;
					return This->stack[This->count];
				}

				/// <summary>
                /// Отримання верхнього елементу в стеку, якщо стек пустий то генерується помилка 
                /// </summary>
                /// <returns>Силка на вехній елемент</returns>
				forceinline const T& Top() const {
					if (This->count == 0)
						Throw(EmptyStackException(ExceptionPosition));
					This->count--;
					return This->stack[This->count];
				}

				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);

				/// <summary>
                /// Кількість елементів в стеку
                /// </summary>
                /// <returns>Поточна кількість елементів в стеку</returns>
				forceinline SizeT GetCount() { return This->count; }

				/// <summary>
                /// оточна кількість елементів в стеку
                /// </summary>
				SizeT Property(GetCount) Count;

				/// <summary>
                /// Клонування стеку
                /// </summary>
                /// <returns>Стек з копією елементів</returns>
				virtual BaseFixedStack Clone() const override final {
					BaseFixedStack result;
					result.This->count = This->count;
					if constexpr (TypeTraits::IsPodV<T>) {
						Memory::Copy(This->stack, result.This->stack, sizeof(T) * This->count);
					}
					else {
						for (SizeT i = 0; i < This->count; i++)
						{
							result.This->stack[i] = This->stack[i];
						}
					}
					return result;
				}

				RefCountClass(BaseFixedStack, This)
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
				BaseFixedStack& ForEachImpl(Lambda&& lambda) {
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
				const BaseFixedStack& ForEachImpl(Lambda&& lambda) const {
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
				forceinline U AggregateImpl(Lambda&& lambda) const {
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
				forceinline U AggregateImpl(Lambda&& lambda, const U& Value) const {
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

				template<typename Lambda>
				forceinline T AggregateImpl(Lambda&& lambda, T&& Value) const {
					if (This->count == 0)
						return Value;
					SizeT i = This->count;
					do {
						i--;
						lambda(Value, This->stack[i]);
					} while (i != 0);
					return MoveRValue(Value);
				}

				forceinline bool AnyImpl() const { return This->count > 0; }

				template<typename Lambda>
				forceinline bool AnyImpl(Lambda&& lambda) const {
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
				forceinline bool AllImpl(Lambda&& lambda) const {
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

				BaseFixedStack& ReverseImpl() {
					for (SizeT i = 0; i < This->count / 2; i++) {
						Swap(This->stack[i], This->stack[(This->count - 1) - i]);
					}
					return *this;
				}

				forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->stack[Index] = MoveRValue(Value);
					This->count = Index + 1;
				}

				forceinline void UnsafeSet(const T& Value, SizeT Index) {
					This->stack[Index] = Value;
					This->count = Index + 1;
				}

				forceinline BaseFixedStack& RemoveAllImpl() {
					for (SizeT i = 0; i < This->count; i++) {
						This->stack[i].~T();
					}
					return *this;
				}

				template<typename Lambda>
				forceinline BaseFixedStack& RemoveAllImpl(Lambda&& lambda) {
					for (SizeT i = 0; i < This->count; i++) {
						lambda(MoveRValue(This->stack[i]));
					}
					return *this;
				}
			};

			/// <summary>
			/// Стек з фіксованою максимальною кількістю елементів в стандартній кучі
			/// </summary>
			namespace {
				template<SizeT TSize, typename T>
				using FixedStack = BaseFixedStack<Seed::Core::Memory::Heap, TSize, T>;
			}

			/// <summary>
			/// Стек з фіксованою максимальною кількістю елементів в захищеній кучі
			/// </summary>
			namespace {
				template<SizeT TSize, typename T>
				using SensitiveFixedStack = BaseFixedStack<Seed::Core::Memory::SensitiveDataHeap, TSize, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, SizeT TSize, typename T>
				struct IsFixedStackT<Seed::Core::Containers::BaseFixedStack<THeap, TSize, T>> : LogicTrue {};
			}
		}

		template<typename THeap, SizeT TSize, typename T>
		forceinline void Swap(Containers::BaseFixedStack<THeap, TSize, T>& A, Containers::BaseFixedStack<THeap, TSize, T>& B) {
			Swap(A.This, B.This);
		}
	}
}
#pragma warning(pop)
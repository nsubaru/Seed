#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Containers.API.LINQ.h"

#pragma warning(push)
#pragma warning(disable: 4172)

namespace Seed {
	inline namespace Core {
		namespace Containers {
			/// <summary>
			/// Стек з підтримкою багатопоточності
			/// </summary>
			template<typename THeap, typename T>
			class BaseConcurencyDynamicStack :
				public LINQ::IDynamicStack<BaseConcurencyDynamicStack, THeap, T, true>,
				public IDisposable,
				public IClonable<BaseConcurencyDynamicStack<THeap, T>>
			{
			private:
				struct Data {
					RefCountT count;
					RefCountT RefCount;
					SizeT bufferSize;
					Threading::SRWLock lock;
					T* restrict stack;

					ForceInline Data(SizeT bufferSize) : count(0), RefCount(1), lock(), stack(new T[bufferSize]), bufferSize(bufferSize) {}
				};
			private:
				Data* This;
			protected:
				ForceInline virtual void Dispose() final override {
					delete[] This->stack;
					This->count = This->bufferSize = 0;
				}
			public:
				/// <summary>
				/// Пустий конструктор ініціалізовує стек з стандартною максимальною кількістю елементів
				/// </summary>
				ForceInline BaseConcurencyDynamicStack() : This(new Data(CurrentSettings().MemoryManager.StackReserveSize)) {}

				/// <summary>
				/// Конструктор з списку ініціалізації, 
				/// переміщає елементи з списку, розмір ставить 
				/// як суму кількості елементів плюс стандартну кількість елементів
				/// </summary>
				/// <param name="list">Список елементів</param>
				BaseConcurencyDynamicStack(InitilizerList<T>&& list) {
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
					return This->lock.ExclusiveLock([&]() mutable InlineLambda {
						if (This->count == This->bufferSize) {
							ExtendBufferImpl(CurrentSettings().MemoryManager.StackReserveSize);
						}
						This->stack[This->count] = item;
						++This->count;
						});
				}

				/// <summary>
				/// Додає елемент в стек, якщо стек заповнений 
				/// то він розширюється на кількість задану в налаштуваннях
				/// </summary>
				/// <param name="item">RValue силка на елемент</param>
				ForceInline void Push(T&& item) {
					return This->lock.ExclusiveLock([&, item = MoveRValue(item)]() mutable InlineLambda {
						if (This->count == This->bufferSize) {
							ExtendBufferImpl(CurrentSettings().MemoryManager.StackReserveSize);
						}
						This->stack[This->count] = MoveRValue(item);
						++This->count;
					});
				}

				/// <summary>
				/// Виймає елемент з стеку, якщо стек пустий то генерує помилку
				/// </summary>
				/// <returns>Вийннятий елемент з стеку</returns>
				ForceInline T Pop() {
					return This->lock.ExclusiveLock([&]() mutable InlineLambda {
						if (This->count == 0)
							Throw(EmptyStackException(ExceptionPosition));
						--This->count;
						return MoveRValue(This->stack[This->count]);
						});
				}

				/// <summary>
				/// Вертає верхній елемент без виймання з стеку, 
				/// якщо стек пустий то генерує помилку
				/// </summary>
				/// <returns>Константна силка на елемент</returns>
				ForceInline const T& Top() const {
					return This->lock.ShareLock([&]() mutable -> const T& InlineLambda {
						if (This->count == 0)
							Throw(EmptyStackException(ExceptionPosition));
						--This->count;
						return This->stack[This->count];
						});
				}

				/// <summary>
				/// Вертає верхній елемент без виймання з стеку
				/// якщо стек пустий то генерує помилку
				/// </summary>
				/// <returns>Константна силка на елемент</returns>
				ForceInline const T& Top() {
					return This->lock.ShareLock([&]() mutable -> const T& InlineLambda {
						if (This->count == 0)
							Throw(EmptyStackException(ExceptionPosition));
						--This->count;
						return This->stack[This->count];
						});
				}

				/// <summary>
				/// Розширення стеку на задану кількість елементів
				/// </summary>
				/// <param name="AdditionalSize">Кількість на яку необхідно розширити стек</param>
				ForceInline void ExtendBuffer(SizeT AdditionalSize) {
					This->lock.ExclusiveLock([&]() mutable InlineLambda {
						ExtendBufferImpl(AdditionalSize);
						});
				}

				/// <summary>
				/// Стиснення розміру стек до поточної кількості елементів
				/// </summary>
				void Shrink() {
					This->lock.ExclusiveLock([&]() mutable InlineLambda {
						T* newBuffer = new T[This->count];
						for (SizeT i = 0; i < This->count; i++)
						{
							newBuffer[i] = MoveRValue(This->stack[i]);
						}
						delete This->stack;
						This->stack = newBuffer;
						This->bufferSize = This->count;
						});
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

				/// <summary>
				/// Клонування об'єкту стека
				/// </summary>
				/// <returns>Нови об'єкт стеку з копією елементів</returns>
				virtual BaseConcurencyDynamicStack Clone() const override final {
					return This->lock.ShareLock([&]() InlineLambda {
						BaseConcurencyDynamicStack result;
						result.This->bufferSize = This->bufferSize;
						result.This->count = This->count;
						result.This->stack = new T[This->bufferSize];
						for (SizeT i = 0; i < This->count; i++)
						{
							result.This->stack[i] = This->stack[i];
						}
						return result;
						});
				}

				RefCountClass(BaseConcurencyDynamicStack, This)
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
				BaseConcurencyDynamicStack& ForEachImpl(Lambda&& lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyDynamicStack& InlineLambda {
							if (This->count == 0)
								return *this;
							SizeT i = This->count;
							do {
								i--;
								lambda(This->stack[i]);
							} while (i != 0);
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&, SizeT>) {
						return This->lock.ShareLock([&]() -> BaseConcurencyDynamicStack& InlineLambda {
							if (This->count == 0)
								return *this;
							SizeT i = This->count;
							do {
								i--;
								lambda(This->stack[i], i);
							} while (i != 0);
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}

				template<typename Lambda>
				const BaseConcurencyDynamicStack& ForEachImpl(Lambda&& lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&>) {
						return this->This->lock.ShareLock([&]() -> const BaseConcurencyDynamicStack& InlineLambda {
							if (This->count == 0)
								return *this;
							SizeT i = This->count;
							do {
								i--;
								lambda(This->stack[i]);
							} while (i != 0);
							return *this;
							});
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&, SizeT>) {
						return this->This->lock.ShareLock([&]() -> const BaseConcurencyDynamicStack& InlineLambda {
							if (This->count == 0)
								return *this;
							SizeT i = This->count;
							do {
								i--;
								lambda(This->stack[i], i);
							} while (i != 0);
							return *this;
							});
					}
					else {
						static_assert(false);
					}
				}

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda) const {
					return MoveRValue(this->This->lock.ShareLock([&]() InlineLambda {

						if (This->count == 0)
							return U();
						U result = U();
						SizeT i = This->count;
						do {
							i--;
							lambda(result, This->stack[i]);
						} while (i != 0);
						return MoveRValue(result);
						}));
				}

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda, const U& Value) const {
					return MoveRValue(this->This->lock.ShareLock([&]() InlineLambda {
						U result = Value;
						if (This->count == 0)
							return result;
						SizeT i = This->count;
						do {
							i--;
							lambda(result, This->stack[i]);
						} while (i != 0);
						return MoveRValue(result);
						}));
				}

				template<typename U, typename Lambda>
				U AggregateImpl(Lambda&& lambda, U&& Value) const {
					return MoveRValue(this->This->lock.ShareLock([&]() InlineLambda {
						if (This->count == 0)
							return Value;
						SizeT i = This->count;
						do {
							i--;
							lambda(Value, This->stack[i]);
						} while (i != 0);
						return MoveRValue(Value);
						}));
				}

				ForceInline bool AnyImpl() const { return This->count > 0; }

				template<typename Lambda>
				bool AnyImpl(Lambda&& lambda) const {
					return MoveRValue(this->This->lock.ShareLock([&]() InlineLambda {
						if (This->count == 0)
							return false;
						SizeT i = This->count;
						do {
							i--;
							if (lambda(This->stack[i]))
								return true;
						} while (i != 0);
						return false;
						}));
				}

				template<typename Lambda>
				bool AllImpl(Lambda&& lambda) const {
					return MoveRValue(this->This->lock.ShareLock([&]() InlineLambda {
						if (This->count == 0)
							return false;
						SizeT i = This->count;
						do {
							i--;
							if (!lambda(This->stack[i]))
								return false;
						} while (i != 0);
						return true;
						}));
				}

				template<typename Lambda>
				T& FirstImpl(Lambda&& lambda) {
					return This->lock.ShareLock([&]() -> T& InlineLambda {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						SizeT i = This->count;
						do {
							i--;
							if (lambda(This->stack[i]))
								return static_cast<T&>(This->array[i]);
						} while (i != 0);

						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						});
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda) {
					return This->lock.ShareLock([&]() -> T& InlineLambda {
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
						});
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const {
					return This->lock.ShareLock([&]() -> const T& InlineLambda {
						if (This->count == 0)
							Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						SizeT i = This->count;
						do {
							i--;
							if (lambda(This->stack[i]))
								return static_cast<T&>(This->array[i]);
						} while (i != 0);

						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
						});
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const {
					return This->lock.ShareLock([&]() -> const T& InlineLambda {
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
						});
				}

				BaseConcurencyDynamicStack& ReverseImpl() {
					return This->lock.ExclusiveLock([&]() -> BaseConcurencyDynamicStack& InlineLambda {
						for (SizeT i = 0; i < This->count / 2; i++) {
							Swap(This->stack[i], This->stack[(This->count - 1) - i]);
						}
						return *this;
						});
				}

				ForceInline void UnsafeSet(const T& Value, SizeT Index) {
					This->stack[Index] = Value;
					This->count = Index + 1;
				}

				ForceInline void UnsafeSet(T&& Value, SizeT Index) {
					This->stack[Index] = MoveRValue(Value);
					This->count = Index + 1;
				}

				BaseConcurencyDynamicStack& RemoveAllImpl() {
					return This->lock.ExclusiveLock([&]() -> BaseConcurencyDynamicStack& InlineLambda {
						delete[] This->stack;
						This->count = This->bufferSize = 0;
						return *this;
						});
				}

				template<typename Lambda>
				BaseConcurencyDynamicStack& RemoveAllImpl(Lambda&& lambda) {
					return This->lock.ExclusiveLock([&]() -> BaseConcurencyDynamicStack& InlineLambda {
						for (SizeT i = 0; i < This->count; i++) {
							lambda(MoveRValue(This->stack[i]));
						}
						delete[] This->stack;
						This->count = This->bufferSize = 0;
						return *this;
						});
				}

				template<typename Lambda>
				SizeT GetCountIfImpl(Lambda&& lambda) {
					return This->lock.ShareLock([&]() InlineLambda {
						SizeT selectedCount = 0;

						SizeT i = This->count;
						do {
							i--;
							if (lambda(This->stack[i]))
								selectedCount++;
						} while (i != 0);

						return selectedCount;
						});
				}
			};

			/// <summary>
			/// Стек з підтримкою багатопоточності
			/// </summary>
			namespace {
				template<typename T>
				using ConcurencyDynamicStack = BaseConcurencyDynamicStack<Seed::Core::Memory::Heap, T>;
			}

			/// <summary>
			/// Стек з підтримкою багатопоточності в захищеній кучі
			/// </summary>
			namespace {
				template<typename T>
				using ConcurencySensitiveDynamicStack = BaseConcurencyDynamicStack<Seed::Core::Memory::SensitiveDataHeap, T>;
			}
		}

		namespace TypeTraits {
			namespace Collection {
				template<typename THeap, typename T>
				struct IsDynamicStackT<Seed::Core::Containers::BaseConcurencyDynamicStack<THeap, T>> : LogicTrue {};
			}
		}

		template<typename THeap, typename T>
		ForceInline void Swap(Containers::BaseConcurencyDynamicStack<THeap, T>& A, Containers::BaseConcurencyDynamicStack<THeap, T>& B) {
			Swap(A.This, B.This);
		}
	}
}

#pragma warning(pop)
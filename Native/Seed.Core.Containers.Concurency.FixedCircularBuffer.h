#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.API.LINQ.IFixedCircularBuffer.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Containers
		{
			/// <summary>
			/// Динамічний циклічний буфер, його можна ресайзити по запиту
			/// Можна отримати значення по якомусь фільтру і також додати, 
			/// якщо елементів більше ніж може бути то елементи затираються
			/// </summary>
			template<typename THeap, SizeT TSize, typename T>
			class BaseConcurencyFixedCircularBuffer final : 
				public Memory::Allocator<THeap>, 
				protected IDisposable, 
				public LINQ::IFixedCircularBuffer<BaseConcurencyFixedCircularBuffer, TSize, THeap, T, true> {
			private:
				struct Data {
					T buffer[TSize];
					SizeT itemIndex;
					Threading::SRWLock lock;
					RefCountT ActualSize;
					RefCountT RefCount;

					Data() : buffer(), lock(), ActualSize(0), RefCount(1), itemIndex(0) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {
					This->lock.ExclusiveLock([&] ()
					{
						for (SizeT i = 0; i < TSize; i++)
						{
							This->buffer[i].~T();
						}
					});
				}
			public:
				/// <summary>
				/// Конструктор ініціалізовує буфер стандартним значенням
				/// </summary>
				BaseConcurencyFixedCircularBuffer() : This(new Data()) {}
			protected:
				void AddImpl(const T& value) {
					This->lock.ExclusiveLock([&] ()
					{
						if (This->ActualSize != TSize)
						{
							This->buffer[++This->ActualSize] = value;
						}
						else
						{
							if (This->itemIndex == TSize)
							{
								This->itemIndex = 0;
							}

							This->buffer[This->itemIndex++] = value;
						}
					});
				}
				void AddImpl(T&& value) {
					This->lock.ExclusiveLock([&, value = MoveRValue(value)] ()
					{
						if (This->ActualSize != TSize)
						{
							This->buffer[This->ActualSize++] = MoveRValue(value);
						}
						else
						{
							if (This->itemIndex == TSize)
							{
								This->itemIndex = 0;
							}

							This->buffer[This->itemIndex++] = MoveRValue(value);
						}
					});
				}
			protected:
				template<typename Lambda>
				T& GetByImpl(Lambda&& lambda) {
					return This->lock.ShareLock([&] () -> T&
					{
						if (This->ActualSize != TSize)
						{
							for (SizeT i = 0; i < This->ActualSize; i++)
							{
								if (lambda(This->buffer[i]))
									return This->buffer[i];
							}
						}
						else
						{
							for (SizeT i = 0; i < TSize; i++)
							{
								if (lambda(This->buffer[i]))
									return This->buffer[i];
							}
						}
						Throw(NullPointException(ExceptionPosition));
					});
				}

				template<typename Lambda>
				const T& GetByImpl(Lambda lambda) const{
					return This->lock.ShareLock([&] () -> const T&
					{
						if (This->ActualSize != TSize)
						{
							for (SizeT i = 0; i < This->ActualSize; i++)
							{
								if (lambda(This->buffer[i]))
									return This->buffer[i];
							}
						}
						else
						{
							for (SizeT i = 0; i < TSize; i++)
							{
								if (lambda(This->buffer[i]))
									return This->buffer[i];
							}
						}
						Throw(NullPointException(ExceptionPosition));
					});
				}
			protected:
				void FlushImpl() {
					This->lock.ExclusiveLock([&] ()
					{
						for (SizeT i = 0; i < TSize; i++)
						{
							This->buffer[i].~T();
						}
						This->ActualSize = 0;
					});
				}
			protected:
				SizeT GetActualSizeImpl() const {
					return This->ActualSize;
				}
				Boolean GetIsEmptyImpl() const {
					return This->ActualSize == 0;
				}
				SizeT GetSizeImpl() const {
					return TSize;
				}
			protected:
				template<typename Lambda>
				BaseConcurencyFixedCircularBuffer& ForEachImpl(Lambda&& lambda, SizeT count = Math::Max<SizeT>()) {
					return This->lock.ShareLock([&] () -> BaseConcurencyFixedCircularBuffer&
					{
						if (This->ActualSize != TSize)
						{
							if (count < This->ActualSize)
								count = This->ActualSize;
						}
						else
						{
							if (count < TSize)
								count = TSize;
						}

						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&>)
							{
								lambda(This->buffer[i]);
							}
							else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, T&, SizeT>)
							{
								lambda(This->buffer[i], i);
							}
							else
							{
								static_assert(false);
							}
						}

						return *this;
					});
				}

				template<typename Lambda>
				const BaseConcurencyFixedCircularBuffer& ForEachImpl(Lambda&& lambda, SizeT count = Math::Max<SizeT>()) const {
					return This->lock.ShareLock([&] () -> const BaseConcurencyFixedCircularBuffer&
					{
						if (This->ActualSize != TSize)
						{
							if (count < This->ActualSize)
								count = This->ActualSize;
						}
						else
						{
							if (count < TSize)
								count = TSize;
						}

						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&>)
							{
								lambda(This->buffer[i]);
							}
							else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const T&, SizeT>)
							{
								lambda(This->buffer[i], i);
							}
							else
							{
								static_assert(false);
							}
						}

						return *this;
					});
				}
			protected:
				template<typename U, typename Lambda>
				forceinline U AggregateImpl(Lambda&& lambda) const {
					return This->lock.ShareLock([&] () -> U
					{
						U value;

						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							lambda(value, This->buffer[i]);
						}

						return value;
					});
				}

				template<typename U, typename Lambda>
				forceinline U AggregateImpl(Lambda&& lambda, const U& startValue) const {
					return This->lock.ShareLock([&] () -> U
					{
						U value = startValue;

						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							lambda(value, This->buffer[i]);
						}

						return value;
					});
				}

				template<typename U, typename Lambda>
				forceinline U AggregateImpl(Lambda&& lambda, U&& value) const {
					return This->lock.ShareLock([&, value = MoveRValue(value)]() mutable -> U
					{
						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							lambda(value, This->buffer[i]);
						}

						return value;
					});
				}
			protected:
				forceinline bool AnyImpl() const {
					return TSize != 0;
				}

				template<typename Lambda>
				forceinline bool AnyImpl(Lambda&& lambda) const {
					return This->lock.ShareLock([&] () -> bool
					{
						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>)
							{
								if (lambda(This->buffer[i]))
									return true;
							}
							else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>)
							{
								if (lambda(This->buffer[i], i))
									return true;
							}
							else
							{
								static_assert(false);
							}
						}

						return false;
					});
				}

				template<typename Lambda>
				forceinline bool AllImpl(Lambda&& lambda) const {
					return This->lock.ShareLock([&] () -> bool
					{
						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>)
							{
								if (!lambda(This->buffer[i]))
									return false;
							}
							else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>)
							{
								if (!lambda(This->buffer[i], i))
									return false;
							}
							else
							{
								static_assert(false);
							}
						}

						return true;
					});
				}
			protected:
				template<typename Lambda>
				forceinline T& FirstImpl(Lambda&& lambda) {
					return This->lock.ShareLock([&] () -> T&
					{
						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							if (lambda(This->buffer[i]))
								return This->buffer[i];
						}

						Throw(NullPointException(ExceptionPosition));
					});
				}

				template<typename Lambda>
				forceinline T& LastImpl(Lambda&& lambda) {
					return This->lock.ShareLock([&] () -> T&
					{
						T* item = nullptr;

						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							if (lambda(This->buffer[i]))
								item = &This->buffer[i];
						}

						if (item != nullptr)
						{
							return *item;
						}
						else
						{
							Throw(NullPointException(ExceptionPosition));
						}
					});
				}

				template<typename Lambda>
				forceinline const T& FirstImpl(Lambda&& lambda) const {
					return This->lock.ShareLock([&] () -> const T&
					{
						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							if (lambda(This->buffer[i]))
								return This->buffer[i];
						}

						Throw(NullPointException(ExceptionPosition));
					});
				}

				template<typename Lambda>
				forceinline const T& LastImpl(Lambda&& lambda) const {
					return This->lock.ExclusiveLock([&] () -> const T&
					{
						T* item = nullptr;

						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							if (lambda(This->buffer[i]))
								item = &This->buffer[i];
						}

						if (item != nullptr)
						{
							return *item;
						}
						else
						{
							Throw(NullPointException(ExceptionPosition));
						}
					});
				}
			protected:
				forceinline BaseConcurencyFixedCircularBuffer& RemoveAllImpl() {
					return This->lock.ExclusiveLock([&] () -> BaseConcurencyFixedCircularBuffer&
					{
						Dispose();
						return *this;
					});
				}

				template<typename Lambda>
				forceinline BaseConcurencyFixedCircularBuffer& RemoveAllImpl(Lambda&& lambda) {
					return This->lock.ExclusiveLock([&] () -> BaseConcurencyFixedCircularBuffer&
					{
						try
						{
							for (SizeT i = 0; i < This->ActualSize; i++)
							{
								lambda(MoveRValue(This->buffer[i]));
							}
							Dispose();
						}
						catch (...)
						{
							Dispose();
							ReThrow;
						}

						return *this;
					});
				}

				template<typename Lambda>
				forceinline SizeT GetCountIfImpl(Lambda && lambda) {
					return This->lock.ExclusiveLock([&] () -> SizeT
					{
						if (This->ActualSize != TSize)
						{
							count = This->ActualSize;
						}
						else
						{
							count = TSize;
						}

						SizeT res = 0;

						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							if (lambda(This->buffer[i]))
								res++;
						}

						return res;
					});
				}
			protected:
				BaseConcurencyFixedCircularBuffer& ReverseImpl() {
					return This->lock.ExclusiveLock([&] () -> BaseConcurencyFixedCircularBuffer&
					{
						for (SizeT i = 0; i < This->ActualSize / 2; i++)
						{
							Seed::Swap(This->buffer[i], This->buffer[(This->ActualSize - 1) - i]);
						}
					});
				}
			protected:
				forceinline void UnsafeSet(const T& Value, SizeT Index) {
					This->buffer[Index] = Value;
				}

				forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->buffer[Index] = MoveRValue(Value);
				}
			public:
				RefCountClass(BaseConcurencyFixedCircularBuffer, This);
			protected:
				template<template<typename THeap, SizeT TSize, typename T>typename TCircularBuffer, SizeT TSize, typename THeap, typename T, bool TIsConcurencySuport>
				friend class LINQ::IFixedCircularBuffer;
				template<typename TCircularBuffer, typename THeap, typename T, bool TIsConcurencySuport>
				friend class LINQ::ICircularBuffer;
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				friend class LINQ::IFixedCollection;
				template<template<typename THeap, typename T> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollectionBase;
			public:
				template<typename U>
				friend void Seed::Core::Swap(U& A, U& B);
			};

			namespace {
				template<typename T, SizeT TSize>
				using FixedConcurencyCircularBuffer = BaseConcurencyFixedCircularBuffer<Seed::Core::Memory::Heap, TSize, T>;
			}

			namespace {
				template<typename T, SizeT TSize>
				using SensitiveFixedConcurencyCircularBuffer = BaseConcurencyFixedCircularBuffer<Seed::Core::Memory::SensitiveDataHeap, TSize, T>;
			}
		}

		template<typename THeap, SizeT TSize, typename T>
		forceinline void Swap(Containers::BaseConcurencyFixedCircularBuffer<THeap, TSize, T>& A, Containers::BaseConcurencyFixedCircularBuffer<THeap, TSize, T>& B) {
			Swap(A.This, B.This);
		}
	}
}
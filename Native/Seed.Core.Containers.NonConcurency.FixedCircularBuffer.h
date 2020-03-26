#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.API.LINQ.IFixedCircularBuffer.h"
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
			class BaseFixedCircularBuffer final : 
				public Memory::Allocator<THeap>, 
				protected IDisposable, 
				public LINQ::IFixedCircularBuffer<BaseFixedCircularBuffer, TSize, THeap, T, false> {
			private:
				struct Data {
					T buffer[TSize];
					SizeT itemIndex;
					RefCountT ActualSize;
					RefCountT RefCount;

					Data() : buffer(), ActualSize(0), RefCount(1), itemIndex(0) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {
					for (SizeT i = 0; i < TSize; i++)
					{
						This->buffer[i].~T();
					}
				}
			public:
				/// <summary>
				/// Конструктор ініціалізовує буфер стандартним значенням
				/// </summary>
				BaseFixedCircularBuffer() : This(new Data()) {}
			protected:
				void AddImpl(const T& value) {
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
				}
				void AddImpl(T&& value) {
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
				}
			protected:
				template<typename Lambda>
				T& GetByImpl(Lambda&& lambda) {
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
				}

				template<typename Lambda>
				const T& GetByImpl(Lambda lambda) const{
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
				}
			protected:
				void FlushImpl() {
					for (SizeT i = 0; i < TSize; i++)
					{
						This->buffer[i].~T();
					}
					This->ActualSize = 0;
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
				BaseFixedCircularBuffer& ForEachImpl(Lambda&& lambda, SizeT count = Math::Max<SizeT>()) {
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
						if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>) {
							lambda(This->buffer[i]);
						}
						else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>)
						{
							lambda(This->buffer[i], i);
						}
						else
						{
							static_assert(false);
						}
					}

					return *this;
				}

				template<typename Lambda>
				const BaseFixedCircularBuffer& ForEachImpl(Lambda&& lambda, SizeT count = Math::Max<SizeT>()) const {
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
						if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
							lambda(This->buffer[i]);
						}
						else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>)
						{
							lambda(This->buffer[i], i);
						}
						else
						{
							static_assert(false);
						}
					}

					return *this;
				}
			protected:
				template<typename U, typename Lambda>
				forceinline U AggregateImpl(Lambda&& lambda) const {
					U value;

					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						lambda(value, This->buffer[i]);
					}

					return value;
				}

				template<typename U, typename Lambda>
				forceinline U AggregateImpl(Lambda&& lambda, const U& startValue) const {
					U value = startValue;

					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						lambda(value, This->buffer[i]);
					}

					return value;
				}

				template<typename U, typename Lambda>
				forceinline U AggregateImpl(Lambda&& lambda, U&& value) const {
					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						lambda(value, This->buffer[i]);
					}

					return value;
				}
			protected:
				forceinline bool AnyImpl() const {
					return TSize != 0;
				}

				template<typename Lambda>
				forceinline bool AnyImpl(Lambda&& lambda) const {
					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
							if(lambda(This->buffer[i]))
								return true;
						}
						else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>)
						{
							if(lambda(This->buffer[i], i))
								return true;
						}
						else
						{
							static_assert(false);
						}
					}

					return false;
				}

				template<typename Lambda>
				forceinline bool AllImpl(Lambda&& lambda) const {
					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>) {
							if(!lambda(This->buffer[i]))
								return false;
						}
						else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>)
						{
							if(!lambda(This->buffer[i], i))
								return false;
						}
						else
						{
							static_assert(false);
						}
					}

					return true;
				}
			protected:
				template<typename Lambda>
				forceinline T& FirstImpl(Lambda&& lambda) {
					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						if(lambda(This->buffer[i]))
							return This->buffer[i];
					}

					Throw(NullPointException(ExceptionPosition));
				}

				template<typename Lambda>
				forceinline T& LastImpl(Lambda&& lambda) {
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
				}

				template<typename Lambda>
				forceinline const T& FirstImpl(Lambda&& lambda) const {
					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						if(lambda(This->buffer[i]))
							return This->buffer[i];
					}

					Throw(NullPointException(ExceptionPosition));
				}

				template<typename Lambda>
				forceinline const T& LastImpl(Lambda&& lambda) const {
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
				}
			protected:
				forceinline BaseFixedCircularBuffer& RemoveAllImpl() {
					Dispose();
					return *this;
				}

				template<typename Lambda>
				forceinline BaseFixedCircularBuffer& RemoveAllImpl(Lambda&& lambda) {
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
				}

				template<typename Lambda>
				forceinline SizeT GetCountIfImpl(Lambda && lambda) {
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
				}
			protected:
				BaseFixedCircularBuffer& ReverseImpl() {
					for (SizeT i = 0; i < This->ActualSize / 2; i++) {
                        Seed::Swap(This->buffer[i], This->buffer[(This->ActualSize - 1) - i]);
                    }
				}
			protected:
				forceinline void UnsafeSet(const T& Value, SizeT Index) {
					This->buffer[Index] = Value;
				}

				forceinline void UnsafeSet(T&& Value, SizeT Index) {
					This->buffer[Index] = MoveRValue(Value);
				}
			public:
				RefCountClass(BaseFixedCircularBuffer, This);
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
				using FixedCircularBuffer = BaseFixedCircularBuffer<Seed::Core::Memory::Heap, TSize, T>;
			}

			namespace {
				template<typename T, SizeT TSize>
				using SensitiveFixedCircularBuffer = BaseFixedCircularBuffer<Seed::Core::Memory::SensitiveDataHeap, TSize, T>;
			}
		}

		template<typename THeap, SizeT TSize, typename T>
		forceinline void Swap(Containers::BaseFixedCircularBuffer<THeap, TSize, T>& A, Containers::BaseFixedCircularBuffer<THeap, TSize, T>& B) {
			Swap(A.This, B.This);
		}
	}
}
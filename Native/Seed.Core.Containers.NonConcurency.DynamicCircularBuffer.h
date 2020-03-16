#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.API.LINQ.IDynamicCircularBuffer.h"
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
			template<typename THeap, typename T>
			class BaseDynamicCircularBuffer final : 
				public Memory::Allocator<THeap>, 
				protected IDisposable, 
				public LINQ::IDynamicCircularBuffer<BaseDynamicCircularBuffer, THeap, T, false> {
			private:
				struct Data {
					T* buffer;
					SizeT itemIndex;
					RefCountT Size;
					RefCountT ActualSize;
					RefCountT RefCount;

					Data(SizeT size) : buffer(new T[size]), Size(size), ActualSize(0), RefCount(1), itemIndex(0) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {
					delete[] This->buffer;
					This->buffer = nullptr;
				}
			public:
				/// <summary>
				/// Конструктор ініціалізовує буфер стандартним значенням
				/// </summary>
				BaseDynamicCircularBuffer() : This(new Data(CurrentSettings().MemoryManager.DynamicCircularBufferStartSize)) {}
				/// <summary>
				/// Конструктор створює буфер заданого розміру
				/// </summary>
				/// <param name="bufferSize">Розмір буфера в кількості елементів</param>
				BaseDynamicCircularBuffer(SizeT bufferSize) : This(new Data(bufferSize)) {}
			protected:
				void AddImpl(const T& value) {
					if (This->ActualSize != This->Size)
					{
						This->buffer[++This->ActualSize] = value;
					}
					else
					{
						if (This->itemIndex == This->Size)
						{
							This->itemIndex = 0;
						}

						This->buffer[This->itemIndex++] = value;
					}
				}
				void AddImpl(T&& value) {
					if (This->ActualSize != This->Size)
					{
						This->buffer[This->ActualSize++] = MoveRValue(value);
					}
					else
					{
						if (This->itemIndex == This->Size)
						{
							This->itemIndex = 0;
						}

						This->buffer[This->itemIndex++] = MoveRValue(value);
					}
				}
			protected:
				template<typename Lambda>
				T& GetByImpl(Lambda&& lambda) {
					if (This->ActualSize != This->Size)
					{
						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							if (lambda(This->buffer[i]))
								return This->buffer[i];
						}
					}
					else
					{
						for (SizeT i = 0; i < This->Size; i++)
						{
							if (lambda(This->buffer[i]))
								return This->buffer[i];
						}
					}
					Throw(NullPointException(ExceptionPosition));
				}

				template<typename Lambda>
				const T& GetByImpl(Lambda lambda) const{
					if (This->ActualSize != This->Size)
					{
						for (SizeT i = 0; i < This->ActualSize; i++)
						{
							if (lambda(This->buffer[i]))
								return This->buffer[i];
						}
					}
					else
					{
						for (SizeT i = 0; i < This->Size; i++)
						{
							if (lambda(This->buffer[i]))
								return This->buffer[i];
						}
					}
					Throw(NullPointException(ExceptionPosition));
				}
			protected:
				void FlushImpl() {
					delete[] This->buffer;
					This->ActualSize = This->Size = 0;
				}
			protected:
				void ResizeImpl(SizeT newSize) {
					auto newBuffer = new T[newSize];
					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						newBuffer[i] = This->buffer[i];
					}
					delete[] This->buffer;
					This->Size = newSize;
					This->buffer = newBuffer;
					This->itemIndex = 0;
				}
			protected:
				SizeT GetActualSizeImpl() const {
					return This->ActualSize;
				}
				Boolean GetIsEmptyImpl() const {
					return This->ActualSize == 0;
				}
				SizeT GetSizeImpl() const {
					return This->Size;
				}
			protected:
				template<typename Lambda>
				BaseDynamicCircularBuffer& ForEachImpl(Lambda&& lambda, SizeT count = Math::Max<SizeT>()) {
					if (This->ActualSize != This->Size)
					{
						if (count < This->ActualSize)
							count = This->ActualSize;
					}
					else
					{
						if (count < This->Size)
							count = This->Size;
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
				const BaseDynamicCircularBuffer& ForEachImpl(Lambda&& lambda, SizeT count = Math::Max<SizeT>()) const {
					if (This->ActualSize != This->Size)
					{
						if (count < This->ActualSize)
							count = This->ActualSize;
					}
					else
					{
						if (count < This->Size)
							count = This->Size;
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
				__forceinline U AggregateImpl(Lambda&& lambda) const {
					U value;

					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						lambda(value, This->buffer[i]);
					}

					return value;
				}

				template<typename U, typename Lambda>
				__forceinline U AggregateImpl(Lambda&& lambda, const U& startValue) const {
					U value = startValue;

					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						lambda(value, This->buffer[i]);
					}

					return value;
				}

				template<typename U, typename Lambda>
				__forceinline U AggregateImpl(Lambda&& lambda, U&& value) const {
					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						lambda(value, This->buffer[i]);
					}

					return value;
				}
			protected:
				__forceinline bool AnyImpl() const {
					return This->Size != 0;
				}

				template<typename Lambda>
				__forceinline bool AnyImpl(Lambda&& lambda) const {
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
				__forceinline bool AllImpl(Lambda&& lambda) const {
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
				__forceinline T& FirstImpl(Lambda&& lambda) {
					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						if(lambda(This->buffer[i]))
							return This->buffer[i];
					}

					Throw(NullPointException(ExceptionPosition));
				}

				template<typename Lambda>
				__forceinline T& LastImpl(Lambda&& lambda) {
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
				__forceinline const T& FirstImpl(Lambda&& lambda) const {
					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						if(lambda(This->buffer[i]))
							return This->buffer[i];
					}

					Throw(NullPointException(ExceptionPosition));
				}

				template<typename Lambda>
				__forceinline const T& LastImpl(Lambda&& lambda) const {
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
				__forceinline BaseDynamicCircularBuffer& RemoveAllImpl() {
					Dispose();
					return *this;
				}

				template<typename Lambda>
				__forceinline BaseDynamicCircularBuffer& RemoveAllImpl(Lambda&& lambda) {
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
				__forceinline SizeT GetCountIfImpl(Lambda && lambda) {
					if (This->ActualSize != This->Size)
					{
						count = This->ActualSize;
					}
					else
					{
						count = This->Size;
					}

					SizeT res = 0;

					for (SizeT i = 0; i < This->ActualSize; i++)
					{
						if (lambda(This->buffer[i]))
							res++;
					}
					
					return res;
				}
			public:
				RefCountClass(BaseDynamicCircularBuffer, This);
			protected:
				template<template<typename THeap, typename T>typename TCircularBuffer, typename THeap, typename T, bool TIsConcurencySuport>
				friend class LINQ::IDynamicCircularBuffer;
				template<template<typename THeap, typename T>typename TCircularBuffer, typename THeap, typename T, bool TIsConcurencySuport>
				friend class LINQ::ICircularBuffer;
				template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollection;
				template<template<typename THeap, typename T> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollectionBase;
			};

			namespace {
				template<typename T>
				using DynamicCircularBuffer = BaseDynamicCircularBuffer<Seed::Core::Memory::Heap, T>;
			}

			namespace {
				template<typename T>
				using SensitiveDynamicCircularBuffer = BaseDynamicCircularBuffer<Seed::Core::Memory::SensitiveDataHeap, T>;
			}
		}
	}
}
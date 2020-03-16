#pragma once
#include "Seed.Core.Containers.API.LINQ.ICollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Абстракція динамічної черги
				/// </summary>
				/// <typeparam name="TDynamicQueue">Тип динамічної черги</typeparam>
				/// <typeparam name="THeap">Тип кучі колекції</typeparam>
				/// <typeparam name="T">Тип елементів</typeparam>
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості колекції</typeparam>
				template<template<typename THeap, typename T>typename TCircularBuffer, typename THeap, typename T, bool TIsConcurencySuport>
				class ICircularBuffer : 
					public ICollection<TCircularBuffer, THeap, T, false, TIsConcurencySuport> {
				private:
					using ThisCollection = typename TCircularBuffer<THeap, T>;
				public:
					void Add(const T& item) {
						return static_cast<ThisCollection&>(*this).AddImpl(item);
					}
					void Add(T&& item) {
						return static_cast<ThisCollection&>(*this).AddImpl(MoveRValue(item));
					}
				public:
					ThisCollection& operator+=(const T& item) {
						Add(item);
						return static_cast<ThisCollection&>(*this);
					}

					ThisCollection& operator+=(T&& item) {
						Add(MoveRValue(item));
						return static_cast<ThisCollection&>(*this);
					}
				public:
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					T& GetBy(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).GetByImpl(MoveRValue(lambda));
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					const T& GetBy(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).GetByImpl(MoveRValue(lambda));
					}
				public:
					void Flush() {
						return static_cast<ThisCollection&>(*this).FlushImpl();
					}
				public:
					SizeT GetActualSize() const {
						return static_cast<const ThisCollection&>(*this).GetActualSizeImpl();
					}
					Boolean GetIsEmpty() const {
						return static_cast<const ThisCollection&>(*this).GetIsEmptyImpl();
					}
					SizeT GetSize() const {
						return static_cast<const ThisCollection&>(*this).GetSizeImpl(); 
					}
				public:
					virtual ~ICircularBuffer() = 0 {};
				};
			}
		}
	}
}
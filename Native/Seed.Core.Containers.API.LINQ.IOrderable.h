#pragma once
#include "Seed.Core.Containers.API.LINQ.ICollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Впорядковані колекції
				/// </summary>
				/// <typeparam name="TList">Тип колекції</typeparam>
				/// <typeparam name="THeap">Тип кучі пам'яті яку використовує колекція</typeparam>
				/// <typeparam name="T">Тип елементів колекції</typeparam>
				/// <typeparam name="TIsDynamic">Флаг динамічності розміру колекції</typeparam>
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості</typeparam>
				template<template<typename THeap, typename T>typename TList, typename THeap, typename T, bool TIsDynamic, bool TIsConcurencySuport>
				class IOrderable :
					public ICollection<TList, THeap, T, TIsDynamic, TIsConcurencySuport> {
				protected:
					using ThisCollection = TList<THeap, T>;
				public:
					/// <summary>
					/// Сортування елементів колекції. Використовує оператор >= або <= для сортування
					/// </summary>
					/// <param name="IsDescending">Якщо true то сортування спадаючому порядку відбувається</param>
					/// <returns>Силку на поточну колекцію</returns>
					forceinline ThisCollection& Sort(bool IsDescending = true) {
						return static_cast<ThisCollection&>(*this).SortByImpl(IsDescending);
					}

					/// <summary>
					/// Сортування елементів колекції. Використовує лямбду яка вказує як порівнювати елементи
					/// </summary>
					/// <param name="lambda">Лямбда вигляду bool(const T&amp;, const T&amp;)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&, const T&>::State>
						, bool> = true>
					forceinline ThisCollection& SortBy(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).SortByLambdaImpl(lambda);
					}

					/// <summary>
					/// Реверсує порядок елементів в колекції
					/// </summary>
					/// <returns>Силку на поточну колекцію</returns>
					forceinline ThisCollection& Reverse() {
						return static_cast<ThisCollection&>(*this).ReverseImpl();
					}
				public:
					forceinline virtual ~IOrderable() = 0 {}
				};
			}
		}
	}
}
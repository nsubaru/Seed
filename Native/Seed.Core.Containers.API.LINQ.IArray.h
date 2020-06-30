#pragma once
#include "Seed.Core.Containers.API.LINQ.IOrderable.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Клас LINQ розширення масиву
				/// </summary>
				/// <typeparam name="TArray">Тип масиву </typeparam>
				/// <typeparam name="THeap">Куча масиву </typeparam>
				/// <typeparam name="T">Тип елементів </typeparam>
				/// <typeparam name="TIsConcurencySuport">Вказує чи колекція підтримує багато поточність </typeparam>
				template<template<typename THeap, typename T>typename TArray, typename THeap, typename T, bool TIsConcurencySuport>
				class IArray :
					public IOrderable<TArray, THeap, T, false, TIsConcurencySuport> {
				protected:
					using ThisCollection = TArray<THeap, T>;
				public:
					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(T&amp;) або void(*)(T&amp;, SizeT)</param>
					/// <param name="сount">Кількість елементів які треба пройти</param>
					/// <param name="startIndex">Індекс першого елемента який потрібно пройти</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename Lambda> 
					requires (Concepts::Invockable<Lambda, T&> || Concepts::Invockable<Lambda, T&, SizeT> ||
							  Concepts::Invockable<Lambda, const T&> || Concepts::Invockable<Lambda, const T&, SizeT>)
					forceinline ThisCollection& ForEach(Lambda&& lambda, SizeT startIndex = Math::Min<SizeT>(), SizeT сount = Math::Max<SizeT>()) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), startIndex, сount);
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(const T&amp;) або void(*)(const T&amp;, SizeT)</param>
					/// <param name="сount">Кількість елементів які треба пройти</param>
					/// <param name="startIndex">Індекс першого елемента який потрібно пройти</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename Lambda> 
					requires (Concepts::Invockable<Lambda, const T&> || Concepts::Invockable<Lambda, const T&, SizeT>)
					forceinline const ThisCollection& ForEach(Lambda&& lambda, SizeT startIndex = Math::Min<SizeT>(), SizeT count = Math::Max<SizeT>()) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), startIndex, count);
					}

					forceinline virtual ~IArray() = 0 {}
				};
			}
		}
	}
}
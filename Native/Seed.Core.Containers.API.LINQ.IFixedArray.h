#pragma once
#include "Seed.Core.Containers.API.LINQ.IFixedCollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Абстракція фіксованого масиву
				/// </summary>
				/// <typeparam name="TCollection">Тип фіксованого масиву</typeparam>
				/// <typeparam name="THeap">Тип кучі колекції</typeparam>
				/// <typeparam name="TSize">Кількість елементів в колекції</typeparam>
				/// <typeparam name="T">Тип елементів колекції</typeparam>
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості колекції</typeparam>
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				class IFixedArray : public IFixedCollection<TCollection, THeap, T, TSize, TIsConcurencySuport> {
					using ThisCollection = TCollection<THeap, TSize, T>;
				public:
					/// <summary>
					/// Сортування елементів колекції. Використовує оператор >= або <= для сортування
					/// </summary>
					/// <param name="IsDescending">Якщо true то сортування спадаючому порядку відбувається</param>
					/// <returns>Силку на поточну колекцію</returns>
					ThisCollection& SortBy(bool IsDescending = true) {
						return static_cast<ThisCollection&>(*this).SortByImpl(IsDescending);
					}

					/// <summary>
					/// Генерація колекції з елементами які генерує лямбда
					/// </summary>
					/// <param name="generator">Функція генератор формату T(*)() або T(*)(SizeT)</param>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					template<typename Lambda, 
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
							TypeTraits::IsLambdaConvertibleT<Lambda, T>::State ||
							TypeTraits::IsLambdaConvertibleT<Lambda, T, SizeT>::State
						>
						, bool> = true>
					static forceinline ThisCollection Generate(Lambda generator) {
						if constexpr (TypeTraits::IsLambdaConvertibleT<Lambda, T>::State)
						{
							if constexpr (!ThisCollection::IsDynamic)
							{
								ThisCollection collection = ThisCollection();
								for (SizeT i = 0; i < TSize; i++)
								{
									collection.UnsafeSet(MoveRValue(generator()), i);
								}
								return collection;
							}
							else
							{
								ThisCollection collection = ThisCollection();
								for (SizeT i = 0; i < TSize; i++)
								{
									collection.UnsafeAdd(MoveRValue(generator()));
								}
								return collection;
							}
						}
						else
						{
							if constexpr (!ThisCollection::IsDynamic)
							{
								ThisCollection collection = ThisCollection();
								for (SizeT i = 0; i < TSize; i++)
								{
									collection.UnsafeSet(generator(i), i);
								}
								return collection;
							}
							else
							{
								ThisCollection collection = ThisCollection();
								for (SizeT i = 0; i < TSize; i++)
								{
									collection.UnsafeAdd(generator(i));
								}
								return collection;
							}
						}
					}
				public:
					virtual ~IFixedArray() = 0 {}
				};
			}
		}
	}
}
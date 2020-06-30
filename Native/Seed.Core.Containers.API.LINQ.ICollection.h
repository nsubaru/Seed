#pragma once
#include "Seed.Core.Containers.API.LINQ.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// LINQ розширення до загальних колекцій
				/// </summary>
				/// <typeparam name="TCollection">Тип колекції</typeparam>
				/// <typeparam name="THeap">Тип кучі</typeparam>
				/// <typeparam name="T">Тип елементів</typeparam>
				/// <typeparam name="TIsDynamic">Чи колекція динамічно змінює розмір</typeparam>
				/// <typeparam name="TIsConcurencySuport">Чи колекція підтримує багато потоковість</typeparam>
				template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				class ICollection :
					public ICollectionBase< TCollection, THeap, T, TIsDynamic, TIsConcurencySuport>
				{
				protected:
					using ThisCollection = TCollection<THeap, T>;
				public:
					/// <summary>
					/// Копіює елементи колекції в нову колекцію
					/// </summary>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <returns>Нова колекція з копіями елементів</returns>
					template<template <typename> typename NewCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<T> CopyTo() {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<T>::IsDynamic == false) {
							NewCollection<T> Instance = NewCollection<T>(ThisCollectionRef.Count);
							ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
								Instance.UnsafeSet(Item, Index);
								});
							return Instance;
						}
						else {
							NewCollection<T> Instance = NewCollection<T>();
							ThisCollectionRef.ForEach([&](const T& Item) {
								Instance.UnsafeAdd(Item);
								});
							return Instance;
						}
					}

					/// <summary>
					/// Копіює елементи колекції в нову колекцію
					/// </summary>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <returns>Нова колекція з копіями елементів</returns>
					template<template <typename> typename NewCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<T> CopyTo() const {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<T>::IsDynamic == false) {
							NewCollection<T> Instance = NewCollection<T>(ThisCollectionRef.Count);
							ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
								Instance.UnsafeSet(Item, Index);
								});
							return Instance;
						}
						else {
							NewCollection<T> Instance = NewCollection<T>();
							ThisCollectionRef.ForEach([&](const T& Item) {
								Instance.UnsafeAdd(Item);
								});
							return Instance;
						}
					}

					/// <summary>
					/// Переміщення елементів в нову колекцію, з старої колекції елементи зникають
					/// </summary>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <returns>Нова колекція з елементами</returns>
					template<template <typename> typename NewCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<T> MoveTo() {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<T>::IsDynamic == false) {
							NewCollection<T> Instance = NewCollection<T>(ThisCollectionRef.Count);
							SizeT Index = 0;
							ThisCollectionRef.RemoveAll([&](T&& Item) {
								Instance.UnsafeSet(MoveRValue(Item), Index);
								Index++;
								});
							return Instance;
						}
						else {
							NewCollection<T> Instance = NewCollection<T>();
							ThisCollectionRef.RemoveAll([&](T&& Item) {
								Instance.UnsafeAdd(MoveRValue(Item));
								});
							return Instance;
						}
					}
				public:
					/// <summary>
					/// Копіює елементи колекції в нову колекцію
					/// </summary>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <returns>Нова колекція з копіями елементів</returns>
					template<template <typename> typename NewCollection,
						typename Lambda,
						typename U = typename TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						(
							TypeTraits::IsLambdaConvertibleV<Lambda, U, const T&> ||  
							TypeTraits::IsLambdaConvertibleV<Lambda, U, const T&, SizeT>
						)&&
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<U> CopyTo(Lambda&& lambda) {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<U>::IsDynamic == false) {
							NewCollection<U> Instance = NewCollection<U>(ThisCollectionRef.Count);
							ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
								if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, U, const T&, SizeT>)
								{
									Instance.UnsafeSet(lambda(Item, Index), Index);
								}
								else
								{
									Instance.UnsafeSet(lambda(Item), Index);
								}
								});
							return Instance;
						}
						else {
							NewCollection<U> Instance = NewCollection<U>();
							if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, U, const T&, SizeT>)
							{
								ThisCollectionRef.ForEach([&] (const T& Item, SizeT Index)
								{
									Instance.UnsafeAdd(lambda(Item, Index));
								});
							}
							else
							{
								ThisCollectionRef.ForEach([&] (const T& Item, SizeT Index)
								{
									Instance.UnsafeAdd(lambda(Item));
								});
							}
							return Instance;
						}
					}

					/// <summary>
					/// Копіює елементи колекції в нову колекцію
					/// </summary>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <returns>Нова колекція з копіями елементів</returns>
					template<template <typename> typename NewCollection,
						typename Lambda,
						typename U = typename TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						(
							TypeTraits::IsLambdaConvertibleV<Lambda, U, const T&> ||  
							TypeTraits::IsLambdaConvertibleV<Lambda, U, const T&, SizeT>
						)&&
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<U> CopyTo(Lambda&& lambda) const {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<U>::IsDynamic == false) {
							NewCollection<U> Instance = NewCollection<U>(ThisCollectionRef.Count);
							ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
								if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, U, const T&, SizeT>)
								{
									Instance.UnsafeSet(lambda(Item, Index), Index);
								}
								else
								{
									Instance.UnsafeSet(lambda(Item), Index);
								}
								});
							return Instance;
						}
						else {
							NewCollection<U> Instance = NewCollection<U>();
							if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, U, const T&, SizeT>)
							{
								ThisCollectionRef.ForEach([&] (const T& Item, SizeT Index)
								{
									Instance.UnsafeAdd(lambda(Item, Index));
								});
							}
							else
							{
								ThisCollectionRef.ForEach([&] (const T& Item, SizeT Index)
								{
									Instance.UnsafeAdd(lambda(Item));
								});
							}
							return Instance;
						}
					}

					/// <summary>
					/// Переміщення елементів в нову колекцію, з старої колекції елементи зникають
					/// </summary>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <returns>Нова колекція з елементами</returns>
					template<template <typename> typename NewCollection,
						typename Lambda,
						typename U = typename TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						(
							TypeTraits::IsLambdaConvertibleV<Lambda, U, T&&> ||  
							TypeTraits::IsLambdaConvertibleV<Lambda, U, T&&, SizeT>
						)&&
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<U> MoveTo(Lambda&& lambda) {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<U>::IsDynamic == false) {
							NewCollection<U> Instance = NewCollection<U>(ThisCollectionRef.Count);
							SizeT Index = 0;
							ThisCollectionRef.RemoveAll([&](T&& Item) {
								if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, U, T&&, SizeT>)
								{
									Instance.UnsafeSet(MoveRValue(lambda(MoveRValue(Item), Index)), Index);
								}
								else
								{
									Instance.UnsafeSet(MoveRValue(lambda(MoveRValue(Item))), Index);
								}
								Index++;
								});
							return Instance;
						}
						else {
							NewCollection<U> Instance = NewCollection<U>();
							if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, U, T&&, SizeT>){ 
								SizeT Index = 0;
								ThisCollectionRef.RemoveAll([&] (T&& Item)
								{
									Instance.UnsafeAdd(MoveRValue(lambda(MoveRValue(Item), Index)));
									Index++;
								});
							}
							else
							{
								ThisCollectionRef.RemoveAll([&] (T&& Item)
								{
									Instance.UnsafeAdd(MoveRValue(lambda(MoveRValue(Item))));
								});
							}
							return Instance;
						}
					}
				public:
					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(T&amp;) або void(*)(T&amp;, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename Lambda> 
					requires (Concepts::IsInvockable<Lambda, T&> || Concepts::IsInvockable<Lambda, T&, SizeT> ||
							  Concepts::IsInvockable<Lambda, const T&> || Concepts::IsInvockable<Lambda, const T&, SizeT>)
					forceinline ThisCollection& ForEach(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(const T&amp;) або void(*)(const T&amp;, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename Lambda> 
					requires (Concepts::IsInvockable<Lambda, const T&> || Concepts::IsInvockable<Lambda, const T&, SizeT>)
					forceinline const ThisCollection& ForEach(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(T&amp;) або void(*)(T&amp;, SizeT)</param>
					/// <param name="сount">Кількість елементів які треба пройти</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename Lambda> 
					requires (Concepts::IsInvockable<Lambda, T&> || Concepts::IsInvockable<Lambda, T&, SizeT> ||
							  Concepts::IsInvockable<Lambda, const T&> || Concepts::IsInvockable<Lambda, const T&, SizeT>)
					forceinline ThisCollection& ForEach(Lambda&& lambda, SizeT сount) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), сount);
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(const T&amp;) або void(*)(const T&amp;, SizeT)</param>
					/// <param name="сount">Кількість елементів які треба пройти</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename Lambda> 
					requires (Concepts::IsInvockable<Lambda, const T&> || Concepts::IsInvockable<Lambda, const T&, SizeT>)
					forceinline const ThisCollection& ForEach(Lambda&& lambda, SizeT count) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), count);
					}
				public:
					/// <summary>
					/// Агрегація елементів колекції
					/// </summary>
					/// <typeparam name="U">Вихідний тип</typeparam>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда типу void(*)(U&amp;, const T&amp;)</param>
					template<typename U = T, typename Lambda>
					requires Concepts::IsInvockable<Lambda, U&, const T&>
					forceinline U Aggregate(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl<U, Lambda>(MoveRValue(lambda), U());
					}

					/// <summary>
					/// Агрегація елементів колекції
					/// </summary>
					/// <typeparam name="U">Вихідний тип</typeparam>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда типу void(*)(U&amp;, const T&amp;)</param>
					/// <param name="startValue">Стартове значенняя</param>
					/// <returns>Результат обчислення</returns>
					template<typename U = T, typename Lambda>
					requires Concepts::IsInvockable<Lambda, U&, const T&> 
					forceinline U Aggregate(Lambda&& lambda, const U& startValue) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl<U, Lambda>(MoveRValue(lambda), startValue);
					}

					/// <summary>
					/// Агрегація елементів колекції
					/// </summary>
					/// <typeparam name="U">Вихідний тип</typeparam>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда типу void(*)(U&amp;, const T&amp;)</param>
					/// <param name="startValue">Стартове значенняя</param>
					/// <returns>Результат обчислення</returns>
					template<typename U = T, typename Lambda>
					requires Concepts::IsInvockable<Lambda, U&, const T&>
					forceinline U Aggregate(Lambda&& lambda, U&& startValue) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl<U, Lambda>(MoveRValue(lambda), MoveRValue(startValue));
					}

					/// <summary>
					/// Вертає true якщо в контейнері хоч щось є
					/// </summary>
					/// <returns>Вертає true якщо в контейнері хоч щось є</returns>
					forceinline bool Any() const {
						return static_cast<const ThisCollection&>(*this).AnyImpl();
					}

					/// <summary>
					/// Вертає true якщо в контейнері хоч щось є що підпадає під умову лямбди
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає true якщо в контейнері хоч щось є що підпадає під умову лямбди</returns>
					template<typename Lambda>
					requires Concepts::IsInvockableWithResult<Lambda, bool, const T&>
					forceinline bool Any(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).AnyImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає true якщо всі елементи в контейнері підпадають під умову лямбди
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає true якщо всі елементи в контейнері підпадають під умову лямбди</returns>
					template<typename Lambda>
					requires Concepts::IsInvockableWithResult<Lambda, bool, const T&>
					forceinline bool All(Lambda&& lambda) const  {
						return static_cast<const ThisCollection&>(*this).AllImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda>
					requires Concepts::IsInvockableWithResult<Lambda, bool, const T&>
					forceinline T& First(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).FirstImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					forceinline T& Last(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).LastImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda>
					requires Concepts::IsInvockableWithResult<Lambda, bool, const T&>
					forceinline const T& First(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).FirstImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda>
					requires Concepts::IsInvockableWithResult<Lambda, bool, const T&>
					forceinline const T& Last(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).LastImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Видаляє всі елементи з колекції
					/// </summary>
					/// <returns>Силку на поточний об'єкт колекції</returns>
					forceinline ThisCollection& RemoveAll() {
						return static_cast<ThisCollection&>(*this).RemoveAllImpl();
					}

					/// <summary>
					/// Видаляє всі елементи з колекції викликаючи для кожного елементу лямбду
					/// </summary>
					/// <param name="Lambda">Лямбда формату void(*)(T&amp;&amp;)</param>
					/// <returns>Силку на поточний об'єкт колекції</returns>
					template<typename Lambda>
					requires Concepts::IsInvockable<Lambda, T&&>
					forceinline ThisCollection& RemoveAll(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).RemoveAllImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Підраховує кількість елементів які підпадають під умову
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Кількість елементів які підпадають під умову</returns>
					template<typename Lambda>
					requires Concepts::IsInvockableWithResult<Lambda, bool, const T&>
					forceinline SizeT GetCountIf(Lambda && lambda) {
						return static_cast<ThisCollection&>(*this).GetCountIfImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Генерація колекції з елементами які генерує лямбда
					/// </summary>
					/// <param name="generator">Функція генератор формату T(*)() або T(*)(SizeT)</param>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					template<typename TLambda> 
					requires Concepts::IsInvockableWithResult<TLambda, T> || Concepts::IsInvockableWithResult<TLambda, T, SizeT>
					forceinline static ThisCollection Generate(TLambda&& generator, SizeT count) {
						if constexpr (TypeTraits::IsLambdaConvertibleT<Lambda, T>::State)
						{
							if constexpr (!ThisCollection::IsDynamic)
							{
								ThisCollection collection = ThisCollection(count);
								for (SizeT i = 0; i < count; i++)
								{
									collection.UnsafeSet(MoveRValue(generator()), i);
								}
								return collection;
							}
							else
							{
								ThisCollection collection = ThisCollection();
								for (SizeT i = 0; i < count; i++)
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
								ThisCollection collection = ThisCollection(count);
								for (SizeT i = 0; i < count; i++)
								{
									collection.UnsafeSet(generator(i), i);
								}
								return collection;
							}
							else
							{
								ThisCollection collection = ThisCollection();
								for (SizeT i = 0; i < count; i++)
								{
									collection.UnsafeAdd(generator(i));
								}
								return collection;
							}
						}
					}
				public:
					forceinline virtual ~ICollection() = 0 {}
				};
			}
		}
	}
}
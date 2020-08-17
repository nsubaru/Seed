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
							ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) InlineLambda {
								Instance.UnsafeSet(Item, Index);
								});
							return Instance;
						}
						else {
							NewCollection<T> Instance = NewCollection<T>();
							ThisCollectionRef.ForEach([&](const T& Item) InlineLambda {
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
							ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) InlineLambda {
								Instance.UnsafeSet(Item, Index);
								});
							return Instance;
						}
						else {
							NewCollection<T> Instance = NewCollection<T>();
							ThisCollectionRef.ForEach([&](const T& Item) InlineLambda {
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
							ThisCollectionRef.RemoveAll([&](T&& Item) InlineLambda {
								Instance.UnsafeSet(MoveRValue(Item), Index);
								Index++;
								});
							return Instance;
						}
						else {
							NewCollection<T> Instance = NewCollection<T>();
							ThisCollectionRef.RemoveAll([&](T&& Item) InlineLambda {
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
					/// <typeparam name="TLambda">Тип лямбди перетворення елементів</typeparam>
					/// <returns>Нова колекція з копіями елементів</returns>
					template<template <typename> typename NewCollection,
						typename TLambda,
						typename U = typename TypeTraits::LambdaType<TLambda>::ReturnType,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						(
							TypeTraits::IsLambdaConvertibleV<TLambda, U, const T&> ||  
							TypeTraits::IsLambdaConvertibleV<TLambda, U, const T&, SizeT>
						)&&
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<U> CopyTo(TLambda&& lambda) {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<U>::IsDynamic == false) {
							NewCollection<U> Instance = NewCollection<U>(ThisCollectionRef.Count);
							ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) InlineLambda {
								if constexpr (TypeTraits::IsLambdaConvertibleV<TLambda, U, const T&, SizeT>)
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
							if constexpr (TypeTraits::IsLambdaConvertibleV<TLambda, U, const T&, SizeT>)
							{
								ThisCollectionRef.ForEach([&] (const T& Item, SizeT Index) InlineLambda
								{
									Instance.UnsafeAdd(lambda(Item, Index));
								});
							}
							else
							{
								ThisCollectionRef.ForEach([&] (const T& Item, SizeT Index) InlineLambda
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
					/// <typeparam name="TLambda">Тип лямбди перетворення елементів</typeparam>
					/// <returns>Нова колекція з копіями елементів</returns>
					template<template <typename> typename NewCollection,
						typename TLambda,
						typename U = typename TypeTraits::LambdaType<TLambda>::ReturnType,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						(
							TypeTraits::IsLambdaConvertibleV<TLambda, U, const T&> ||  
							TypeTraits::IsLambdaConvertibleV<TLambda, U, const T&, SizeT>
						)&&
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<U> CopyTo(TLambda&& lambda) const {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<U>::IsDynamic == false) {
							NewCollection<U> Instance = NewCollection<U>(ThisCollectionRef.Count);
							ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) InlineLambda {
								if constexpr (TypeTraits::IsLambdaConvertibleV<TLambda, U, const T&, SizeT>)
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
							if constexpr (TypeTraits::IsLambdaConvertibleV<TLambda, U, const T&, SizeT>)
							{
								ThisCollectionRef.ForEach([&] (const T& Item, SizeT Index) InlineLambda
								{
									Instance.UnsafeAdd(lambda(Item, Index));
								});
							}
							else
							{
								ThisCollectionRef.ForEach([&] (const T& Item, SizeT Index) InlineLambda
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
						typename TLambda,
						typename U = typename TypeTraits::LambdaType<TLambda>::ReturnType,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						(
							TypeTraits::IsLambdaConvertibleV<TLambda, U, T&&> ||  
							TypeTraits::IsLambdaConvertibleV<TLambda, U, T&&, SizeT>
						)&&
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<U> MoveTo(TLambda&& lambda) {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<U>::IsDynamic == false) {
							NewCollection<U> Instance = NewCollection<U>(ThisCollectionRef.Count);
							SizeT Index = 0;
							ThisCollectionRef.RemoveAll([&](T&& Item) InlineLambda {
								if constexpr (TypeTraits::IsLambdaConvertibleV<TLambda, U, T&&, SizeT>)
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
							if constexpr (TypeTraits::IsLambdaConvertibleV<TLambda, U, T&&, SizeT>){ 
								SizeT Index = 0;
								ThisCollectionRef.RemoveAll([&] (T&& Item) InlineLambda
								{
									Instance.UnsafeAdd(MoveRValue(lambda(MoveRValue(Item), Index)));
									Index++;
								});
							}
							else
							{
								ThisCollectionRef.RemoveAll([&] (T&& Item) InlineLambda
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
					/// <typeparam name="TLambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(T&amp;) або void(*)(T&amp;, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename TLambda> 
					requires (Concepts::IsInvockable<TLambda, T&> || Concepts::IsInvockable<TLambda, T&, SizeT> ||
							  Concepts::IsInvockable<TLambda, const T&> || Concepts::IsInvockable<TLambda, const T&, SizeT>)
					ForceInline ThisCollection& ForEach(TLambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="TLambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(const T&amp;) або void(*)(const T&amp;, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename TLambda> 
					requires (Concepts::IsInvockable<TLambda, const T&> || Concepts::IsInvockable<TLambda, const T&, SizeT>)
					ForceInline const ThisCollection& ForEach(TLambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="TLambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(T&amp;) або void(*)(T&amp;, SizeT)</param>
					/// <param name="сount">Кількість елементів які треба пройти</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename TLambda> 
					requires (Concepts::IsInvockable<TLambda, T&> || Concepts::IsInvockable<TLambda, T&, SizeT> ||
							  Concepts::IsInvockable<TLambda, const T&> || Concepts::IsInvockable<TLambda, const T&, SizeT>)
					ForceInline ThisCollection& ForEach(TLambda&& lambda, SizeT сount) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), сount);
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="TLambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(const T&amp;) або void(*)(const T&amp;, SizeT)</param>
					/// <param name="сount">Кількість елементів які треба пройти</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename TLambda> 
					requires (Concepts::IsInvockable<TLambda, const T&> || Concepts::IsInvockable<TLambda, const T&, SizeT>)
					ForceInline const ThisCollection& ForEach(TLambda&& lambda, SizeT count) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), count);
					}
				public:
					/// <summary>
					/// Агрегація елементів колекції
					/// </summary>
					/// <typeparam name="U">Вихідний тип</typeparam>
					/// <typeparam name="TLambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда типу void(*)(U&amp;, const T&amp;)</param>
					template<typename U = T, typename TLambda>
					requires Concepts::IsInvockableWithResult<TLambda, U, const U&, const T&>
					ForceInline U Aggregate(TLambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl<U, TLambda>(MoveRValue(lambda), U());
					}

					/// <summary>
					/// Агрегація елементів колекції
					/// </summary>
					/// <typeparam name="U">Вихідний тип</typeparam>
					/// <typeparam name="TLambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда типу void(*)(U&amp;, const T&amp;)</param>
					/// <param name="startValue">Стартове значенняя</param>
					/// <returns>Результат обчислення</returns>
					template<typename U, typename TLambda>
					requires Concepts::IsInvockableWithResult<TLambda, U, const U&, const T&> 
					ForceInline U Aggregate(TLambda&& lambda, const U& startValue) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl<U, TLambda>(MoveRValue(lambda), startValue);
					}

					/// <summary>
					/// Агрегація елементів колекції
					/// </summary>
					/// <typeparam name="U">Вихідний тип</typeparam>
					/// <typeparam name="TLambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда типу void(*)(U&amp;, const T&amp;)</param>
					/// <param name="startValue">Стартове значенняя</param>
					/// <returns>Результат обчислення</returns>
					template<typename U, typename TLambda>
					requires Concepts::IsInvockableWithResult<TLambda, U, const U&, const T&>
					ForceInline U Aggregate(TLambda&& lambda, U&& startValue) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl<U, TLambda>(MoveRValue(lambda), MoveRValue(startValue));
					}

					/// <summary>
					/// Вертає true якщо в контейнері хоч щось є
					/// </summary>
					/// <returns>Вертає true якщо в контейнері хоч щось є</returns>
					ForceInline bool Any() const {
						return static_cast<const ThisCollection&>(*this).AnyImpl();
					}

					/// <summary>
					/// Вертає true якщо в контейнері хоч щось є що підпадає під умову лямбди
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає true якщо в контейнері хоч щось є що підпадає під умову лямбди</returns>
					template<typename TLambda>
					requires Concepts::IsInvockableWithResult<TLambda, bool, const T&>
					ForceInline bool Any(TLambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).AnyImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає true якщо всі елементи в контейнері підпадають під умову лямбди
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає true якщо всі елементи в контейнері підпадають під умову лямбди</returns>
					template<typename TLambda>
					requires Concepts::IsInvockableWithResult<TLambda, bool, const T&>
					ForceInline bool All(TLambda&& lambda) const  {
						return static_cast<const ThisCollection&>(*this).AllImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename TLambda>
					requires Concepts::IsInvockableWithResult<TLambda, bool, const T&>
					ForceInline T& First(TLambda&& lambda) {
						return static_cast<ThisCollection&>(*this).FirstImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename TLambda>
					requires Concepts::IsInvockableWithResult<TLambda, bool, const T&>
					ForceInline T& Last(TLambda&& lambda) {
						return static_cast<ThisCollection&>(*this).LastImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename TLambda>
					requires Concepts::IsInvockableWithResult<TLambda, bool, const T&>
					ForceInline const T& First(TLambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).FirstImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename TLambda>
					requires Concepts::IsInvockableWithResult<TLambda, bool, const T&>
					ForceInline const T& Last(TLambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).LastImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Видаляє всі елементи з колекції
					/// </summary>
					/// <returns>Силку на поточний об'єкт колекції</returns>
					ForceInline ThisCollection& RemoveAll() {
						return static_cast<ThisCollection&>(*this).RemoveAllImpl();
					}

					/// <summary>
					/// Видаляє всі елементи з колекції викликаючи для кожного елементу лямбду
					/// </summary>
					/// <param name="TLambda">Лямбда формату void(*)(T&amp;&amp;)</param>
					/// <returns>Силку на поточний об'єкт колекції</returns>
					template<typename TLambda>
					requires Concepts::IsInvockable<TLambda, T&&>
					ForceInline ThisCollection& RemoveAll(TLambda&& lambda) {
						return static_cast<ThisCollection&>(*this).RemoveAllImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Підраховує кількість елементів які підпадають під умову
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Кількість елементів які підпадають під умову</returns>
					template<typename TLambda>
					requires Concepts::IsInvockableWithResult<TLambda, bool, const T&>
					ForceInline SizeT GetCountIf(TLambda && lambda) {
						return static_cast<ThisCollection&>(*this).GetCountIfImpl(MoveRValue(lambda));
					}
				public:
					/// <summary>
					/// Пошук максимального елемента в колекції
					/// </summary>
					/// <returns>Максимальне значення</returns>
					ForceInline T Max() const
					requires Concepts::IsComparableWith<T, T>
					{
						const ThisCollection& This = static_cast<const ThisCollection&>(*this);

						auto maxItem = This.First([] ([[maybe_unused]] const auto&) { return true; });

						This.ForEach([&maxItem](const T& item){
							if (maxItem < item)
								maxItem = item;
						});

						return maxItem;
					}
					/// <summary>
					/// Пошук максимального елемента в колекції з використанням лямбди яка вертає значення поля чи елемента
					/// </summary>
					/// <param name="lambda">Лямбда яка вертає значення серед якого буде відбуватись пошук максимального</param>
					/// <returns>Максимальне значення</returns>
					template<typename TLambda>
					requires Concepts::IsInvockable<TLambda, const T&> && 
							 (!Concepts::IsInvockableReturnVoid<TLambda, const T&>) && 
							 Concepts::IsComparableWith<TypeTraits::InvockableResult<TLambda, const T&>, TypeTraits::InvockableResult<TLambda, const T&>>
					ForceInline decltype(auto) Max(TLambda&& lambda) const {
						const ThisCollection& This = static_cast<const ThisCollection&>(*this);
						auto maxItem = lambda(This.First(ArrowFunct(x, true)));

						This.ForEach([&maxItem, lambda = MoveRValue(lambda)](const T& item){
							auto itemValue = lambda(item);
							if (maxItem < itemValue)
								maxItem = itemValue;
						});

						return maxItem;
					}
					/// <summary>
					/// Пошук максимального елемента в колекції з використанням лямбди яка вертає значення поля чи елемента
					/// </summary>
					/// <param name="lambda">Лямбда яка вертає значення серед якого буде відбуватись пошук максимального</param>
					/// <returns>Максимальне значення</returns>
					template<typename TLambda>
					requires Concepts::IsInvockable<TLambda, const T&> && 
							 (!Concepts::IsInvockableReturnVoid<TLambda, const T&>) && 
							 Concepts::IsComparableWith<TypeTraits::InvockableResult<TLambda, const T&>, TypeTraits::InvockableResult<TLambda, const T&>>
					ForceInline T MaxBy(TLambda&& lambda) const {
						const ThisCollection& This = static_cast<const ThisCollection&>(*this);

						auto maxItem = This.First([] ([[maybe_unused]] const auto&) { return true; });

						This.ForEach([&maxItem, lambda = MoveRValue(lambda)](const T& item){
							if (lambda(maxItem) < lambda(item))
								maxItem = item;
						});

						return maxItem;
					}

					/// <summary>
					/// Пошук мінімального елемента в колекції
					/// </summary>
					/// <returns>Максимальне значення</returns>
					ForceInline T Min()
					requires Concepts::IsComparableWith<T, T>
					{
						const ThisCollection& This = static_cast<const ThisCollection&>(*this);

						auto maxItem = This.First([] ([[maybe_unused]] const auto&) { return true; });

						This.ForEach([&maxItem] (const T& item){
							if (maxItem > item)
								maxItem = item;
						});

						return maxItem;
					}
					/// <summary>
					/// Пошук мінімального елемента в колекції з використанням лямбди яка вертає значення поля чи елемента
					/// </summary>
					/// <param name="lambda">Лямбда яка вертає значення серед якого буде відбуватись пошук мінімального</param>
					/// <returns>Мінімальне значення</returns>
					template<typename TLambda>
					requires Concepts::IsInvockable<TLambda, const T&> && 
							 (!Concepts::IsInvockableReturnVoid<TLambda, const T&>) && 
							 Concepts::IsComparableWith<TypeTraits::InvockableResult<TLambda, const T&>, TypeTraits::InvockableResult<TLambda, const T&>>
					ForceInline decltype(auto) Min(TLambda&& lambda) const {
						const ThisCollection& This = static_cast<const ThisCollection&>(*this);
						auto maxItem = lambda(This.First([] ([[maybe_unused]] const auto&) { return true; }));

						This.ForEach([&maxItem, lambda = MoveRValue(lambda)] (const T& item){
							auto itemValue = lambda(item);
							if (maxItem > itemValue)
								maxItem = itemValue;
						});

						return maxItem;
					}
					/// <summary>
					/// Пошук мінімального елемента в колекції з використанням лямбди яка вертає значення поля чи елемента
					/// </summary>
					/// <param name="lambda">Лямбда яка вертає значення серед якого буде відбуватись пошук мінімального</param>
					/// <returns>Мінімальне значення</returns>
					template<typename TLambda>
					requires Concepts::IsInvockable<TLambda, const T&> && 
							 (!Concepts::IsInvockableReturnVoid<TLambda, const T&>) && 
							 Concepts::IsComparableWith<TypeTraits::InvockableResult<TLambda, const T&>, TypeTraits::InvockableResult<TLambda, const T&>>
					ForceInline T MinBy(TLambda&& lambda) const {
						const ThisCollection& This = static_cast<const ThisCollection&>(*this);
						auto maxItem = This.First([] ([[maybe_unused]] const auto&) { return true; });

						This.ForEach([&maxItem, lambda = MoveRValue(lambda)] (const T& item) {
							if (lambda(maxItem) > lambda(item))
								maxItem = item;
						});

						return maxItem;
					}
				public:
					/// <summary>
					/// Генерація колекції з елементами які генерує лямбда
					/// </summary>
					/// <param name="generator">Функція генератор формату T(*)() або T(*)(SizeT)</param>
					/// <typeparam name="TLambda">Тип лямбди</typeparam>
					template<typename TLambda> 
					requires Concepts::IsInvockableWithResult<TLambda, T> || Concepts::IsInvockableWithResult<TLambda, T, SizeT>
					ForceInline static ThisCollection Generate(TLambda&& generator, SizeT count) {
						if constexpr (TypeTraits::IsInvockableWithResult<TLambda, T>)
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
					ForceInline virtual ~ICollection() = 0 {}
				};
			}
		}
	}
}
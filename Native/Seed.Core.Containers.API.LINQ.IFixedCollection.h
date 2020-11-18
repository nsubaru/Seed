#pragma once
#include "Seed.Core.Containers.API.LINQ.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ
			{
				/// <summary>
				/// Абстракція фіксованої колекції
				/// </summary>
				/// <typeparam name="TCollection">Тип колекції</typeparam>
				/// <typeparam name="THeap">Тип кучі колекції</typeparam>
				/// <typeparam name="TSize">Кількість елементів в колекції</typeparam>
				/// <typeparam name="T">Тип елементів колекції</typeparam>
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості колекції</typeparam>
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				class IFixedCollection :
					public IFixedCollectionBase<TCollection, THeap, T, TSize, TIsConcurencySuport> {
				private:
					using ThisCollection = TCollection<THeap, TSize, T>;
				public:
					template<template<SizeT, typename> typename NewCollection>
					ForceInline NewCollection<TSize, T> CopyToFixed() {
						NewCollection<TSize, T> newCollection;
						ThisCollection& thisCollection = static_cast<ThisCollection&>(*this);
						thisCollection.ForEach([&](const T& Value, SizeT Index) InlineLambda{
							newCollection.UnsafeSet(Value, Index);
							});
						return newCollection;
					}

					template<template<SizeT, typename> typename NewCollection>
					ForceInline NewCollection<TSize, T> CopyToFixed() const {
						NewCollection<TSize, T> newCollection;
						const ThisCollection& thisCollection = static_cast<const ThisCollection&>(*this);
						thisCollection.ForEach([&](const T& Value, SizeT Index) InlineLambda{
							newCollection.UnsafeSet(Value, Index);
							});
						return newCollection;
					}
				public:
					template<template<SizeT, typename> typename NewCollection, typename TLambda>
					requires (
						Concepts::IsInvockableReturnSameAs<T, TLambda, const T&> || 
						Concepts::IsInvockableReturnSameAs<T, TLambda, const T&, SizeT>)
					ForceInline NewCollection<TSize, T> CopyToFixed(TLambda&& lambda) {

						NewCollection<TSize, T> newCollection;
						ThisCollection& thisCollection = static_cast<ThisCollection&>(*this);

						thisCollection.ForEach([&](const T& Value, SizeT Index) InlineLambda {

							if constexpr (TypeTraits::IsInvockableWithResult<T, TLambda, const T&>)
							{
								newCollection.UnsafeSet(lambda(Value), Index);
							}
							else
							{
								newCollection.UnsafeSet(lambda(Value, Index), Index);
							}
							
							});

						return newCollection;
					}

					template<template<SizeT, typename> typename NewCollection, typename TLambda> 
					requires (
						Concepts::IsInvockableReturnSameAs<TLambda, T, const T&> ||
						Concepts::IsInvockableReturnSameAs<TLambda, T, const T&, SizeT>)
					ForceInline NewCollection<TSize, T> CopyToFixed(TLambda&& lambda) const {
						NewCollection<TSize, T> newCollection;
						const ThisCollection& thisCollection = static_cast<const ThisCollection&>(*this);
						
						thisCollection.ForEach([&](const T& Value, SizeT Index) InlineLambda{

							if constexpr (TypeTraits::IsInvockableWithResult<T, TLambda, const T&>)
							{
								newCollection.UnsafeSet(lambda(Value), Index);
							}
							else
							{
								newCollection.UnsafeSet(lambda(Value, Index), Index);
							}

						});

						return newCollection;
					}
				public:
					template<template<SizeT, typename> typename NewCollection>
					ForceInline NewCollection<TSize, T> MoveToFixed() {
						NewCollection<TSize, T> newCollection;
						ThisCollection& thisCollection = static_cast<ThisCollection&>(*this);
						SizeT Index = 0;
						thisCollection.RemoveAll([&](T&& Value) -> void InlineLambda {
							newCollection.UnsafeSet(MoveRValue(Value), Index);
							Index++;
							});
						return newCollection;
					}

					template<template<SizeT, typename> typename NewCollection, SizeT TNewSize>
					ForceInline NewCollection<TNewSize, T> CopyToFixed() {
						if constexpr (TNewSize == TSize) {
							NewCollection<TNewSize, T> newCollection;
							ThisCollection& thisCollection = static_cast<ThisCollection&>(*this);
							thisCollection.ForEach([&](const T& Value, SizeT Index) InlineLambda {
								newCollection.UnsafeSet(Value, Index);
								});
							return newCollection;
						}
						else if constexpr (TNewSize < TSize) {
                            Internals::Exceptions::ThrowIndexOutOfRange(__FILE__,
                                                                        __FUNCSIG__,
                                                                        __LINE__,
                                                                        TSize,
                                                                        TNewSize);
						}
					}
				public:
					template<template <typename> typename TNewCollection> requires Concepts::IsBasedOn<ICollectionGenericBase, TNewCollection<T>>
					TNewCollection<T> CopyTo() {
						auto thisCollectionRef = static_cast<const ThisCollection&>(*this);
						
						if constexpr (TNewCollection<T>::IsDynamic == false)
						{
							TNewCollection<T> Instance = TNewCollection<T>(thisCollectionRef.Count);

							thisCollectionRef.ForEach([&](const T& Item, SizeT Index) InlineLambda{
								Instance.UnsafeSet(Item, Index);
							});

							return Instance;
						}
						else
						{
							TNewCollection<T> Instance = TNewCollection<T>();

							thisCollectionRef.ForEach([&](const T& Item, SizeT Index) InlineLambda
							{
								Instance.UnsafeAdd(Item);
							});

							return Instance;
						}
					}

					template<template <typename> typename TNewCollection, typename TLambda> 
					requires Concepts::IsBasedOn<ICollectionBase, TNewCollection<T>> && 
						(TypeTraits::IsInvockable<TLambda, const T&> || TypeTraits::IsInvockable<TLambda, const T&, SizeT>)
					auto CopyTo(TLambda&& lambda) {
							auto thisCollectionRef = static_cast<const ThisCollection&>(*this);
							
							using U = typename TypeTraits::ConditionV<
								TypeTraits::IsInvockable<TLambda, const T&>,
								TypeTraits::InvockableResult<TLambda, const T&>,
								TypeTraits::InvockableResult<TLambda, const T&, SizeT>>;

							if constexpr (TNewCollection<U>::IsDynamic == false)
							{
								TNewCollection<U> Instance = TNewCollection<U>(thisCollectionRef.Count);

								thisCollectionRef.ForEach([&](const T& Item, SizeT Index) InlineLambda{
									Instance.UnsafeSet(lambda(Item), Index);
									});

								return Instance;
							}
							else
							{
								TNewCollection<U> Instance = TNewCollection<U>();

								thisCollectionRef.ForEach([&](const T& Item) InlineLambda
									{
										Instance.UnsafeAdd(lambda(Item));
									});

								return Instance;
							}
						}
				public:
					template<template <typename> typename TNewCollection> requires Concepts::IsBasedOn<ICollectionGenericBase, TNewCollection<T>>
						TNewCollection<T> CopyTo() const {
							auto thisCollectionRef = static_cast<const ThisCollection&>(*this);

							if constexpr (TNewCollection<T>::IsDynamic == false)
							{
								TNewCollection<T> Instance = TNewCollection<T>(thisCollectionRef.Count);
								thisCollectionRef.ForEach([&](const T& Item, SizeT Index) InlineLambda{
									Instance.UnsafeSet(lambda(Item), Index);
									});

								return Instance;
							}
							else
							{
								TNewCollection<T> Instance = TNewCollection<T>();

								thisCollectionRef.ForEach([&](const T& Item, SizeT Index) InlineLambda
									{
										Instance.UnsafeAdd(lambda(Item));
									});

								return Instance;
							}
						}

						template<template <typename> typename TNewCollection, typename TLambda>
						requires Concepts::IsBasedOn<ICollectionBase, TNewCollection<T>> &&
							(TypeTraits::IsInvockable<TLambda, const T&> || TypeTraits::IsInvockable<TLambda, const T&, SizeT>)
						auto CopyTo(TLambda&& lambda) const {
							auto thisCollectionRef = static_cast<const ThisCollection&>(*this);

							using U = typename TypeTraits::ConditionV<
								TypeTraits::IsInvockable<TLambda, const T&>,
								TypeTraits::InvockableResult<TLambda, const T&>,
								TypeTraits::InvockableResult<TLambda, const T&, SizeT>>;

							if constexpr (TNewCollection<U>::IsDynamic == false)
							{
								TNewCollection<U> Instance = TNewCollection<U>(thisCollectionRef.Count);

								thisCollectionRef.ForEach([&](const T& Item, SizeT Index) InlineLambda{
									Instance.UnsafeSet(lambda(Item), Index);
									});

								return Instance;
							}
							else
							{
								TNewCollection<U> Instance = TNewCollection<U>();

								thisCollectionRef.ForEach([&](const T& Item) InlineLambda
									{
										Instance.UnsafeAdd(lambda(Item));
									});

								return Instance;
							}
						}
				public:
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, T&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, T&, SizeT>::State
						>,
						bool
					> = true>
						ForceInline ThisCollection& ForEach(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					template<
						typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const T&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const T&, SizeT>::State
						>,
						bool
					> = true>
						ForceInline const ThisCollection& ForEach(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Агрегація елементів колекції
					/// </summary>
					/// <typeparam name="U">Вихідний тип</typeparam>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда типу void(*)(U&, const T&)</param>
					template<typename U = T, typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, void, U&, const T&>::State>, bool> = true>
					ForceInline U Aggregate(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl<U, Lambda>(MoveRValue(lambda), U());
					}

					/// <summary>
					/// Агрегація елементів колекції
					/// </summary>
					/// <typeparam name="U">Вихідний тип</typeparam>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда типу void(*)(U&, const T&)</param>
					/// <param name="startValue">Стартове значенняя</param>
					/// <returns>Результат обчислення</returns>
					template<typename U = T, typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, void, U&, const T&>::State>, bool> = true>
					ForceInline U Aggregate(Lambda&& lambda, const U& StartValue) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl<U, Lambda>(MoveRValue(lambda), StartValue);
					}

					/// <summary>
					/// Агрегація елементів колекції
					/// </summary>
					/// <typeparam name="U">Вихідний тип</typeparam>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда типу void(*)(U&, const T&)</param>
					/// <param name="startValue">Стартове значенняя</param>
					/// <returns>Результат обчислення</returns>
					template<typename U = T, typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, void, U&, const T&>::State>, bool> = true>
					ForceInline U Aggregate(Lambda&& lambda, U&& StartValue) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl<U, Lambda>(MoveRValue(lambda), MoveRValue(StartValue));
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
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає true якщо в контейнері хоч щось є що підпадає під умову лямбди</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					ForceInline bool Any(Lambda&& lambda) {
						return static_cast<const ThisCollection&>(*this).AnyImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає true якщо всі елементи в контейнері підпадають під умову лямбди
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає true якщо всі елементи в контейнері підпадають під умову лямбди</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					ForceInline bool All(Lambda&& lambda) {
						return static_cast<const ThisCollection&>(*this).AllImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					ForceInline T& First(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).FirstImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					ForceInline T& Last(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).LastImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					ForceInline const T& First(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).FirstImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					ForceInline const T& Last(Lambda&& lambda) const {
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
					/// <param name="Lambda">Лямбда формату void(*)(T&&)</param>
					/// <returns>Силку на поточний об'єкт колекції</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, void, T&&>::State>, bool> = true>
					ForceInline ThisCollection& RemoveAll(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).RemoveAllImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Реверсує порядок елементів в колекції
					/// </summary>
					/// <returns>Силку на поточну колекцію</returns>
					ForceInline ThisCollection& Reverse() {
						return static_cast<ThisCollection&>(*this).ReverseImpl();
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
					ForceInline virtual ~IFixedCollection() = 0 {}
				};
			}
		}
	}
}
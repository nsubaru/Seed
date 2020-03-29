#pragma once
#include "Seed.Core.Containers.API.LINQ.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
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
					public IFixedCollectionBase<TCollection, THeap, T, TSize, TIsConcurencySuport>
				{
				private:
					using ThisCollection = TCollection<THeap, TSize, T>;
				public:
					template<template<SizeT, typename> typename NewCollection>
					forceinline NewCollection<TSize, T> CopyToFixed() {
						NewCollection<TSize, T> newCollection;
						ThisCollection& thisCollection = static_cast<ThisCollection&>(*this);
						thisCollection.ForEach([&](const T& Value, SizeT Index) {
							newCollection.UnsafeSet(Value, Index);
							});
						return newCollection;
					}

					template<template<SizeT, typename> typename NewCollection>
					forceinline NewCollection<TSize, T> CopyToFixed() const {
						NewCollection<TSize, T> newCollection;
						const ThisCollection& thisCollection = static_cast<const ThisCollection&>(*this);
						thisCollection.ForEach([&](const T& Value, SizeT Index) {
							newCollection.UnsafeSet(Value, Index);
							});
						return newCollection;
					}

					template<template<SizeT, typename> typename NewCollection>
					forceinline NewCollection<TSize, T> MoveToFixed() {
						NewCollection<TSize, T> newCollection;
						ThisCollection& thisCollection = static_cast<ThisCollection&>(*this);
						SizeT Index = 0;
						thisCollection.RemoveAll([&](T&& Value) -> void {
							newCollection.UnsafeSet(MoveRValue(Value), Index);
							Index++;
							});
						return newCollection;
					}

					template<template<SizeT, typename> typename NewCollection, SizeT TNewSize>
					forceinline NewCollection<TNewSize, T> CopyToFixed() {
						if constexpr (TNewSize == TSize) {
							NewCollection<TNewSize, T> newCollection;
							ThisCollection& thisCollection = static_cast<ThisCollection&>(*this);
							thisCollection.ForEach([&](const T& Value, SizeT Index) {
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
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, T&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, T&, SizeT>::State
						>,
						bool
					> = true>
						forceinline ThisCollection& ForEach(Lambda&& lambda) {
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
						forceinline const ThisCollection& ForEach(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Агрегація елементів колекції
					/// </summary>
					/// <typeparam name="U">Вихідний тип</typeparam>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда типу void(*)(U&, const T&)</param>
					template<typename U = T, typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, void, U&, const T&>::State>, bool> = true>
					forceinline U Aggregate(Lambda&& lambda) const {
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
					forceinline U Aggregate(Lambda&& lambda, const U& StartValue) const {
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
					forceinline U Aggregate(Lambda&& lambda, U&& StartValue) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl<U, Lambda>(MoveRValue(lambda), MoveRValue(StartValue));
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
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає true якщо в контейнері хоч щось є що підпадає під умову лямбди</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					forceinline bool Any(Lambda&& lambda) {
						return static_cast<const ThisCollection&>(*this).AnyImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає true якщо всі елементи в контейнері підпадають під умову лямбди
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає true якщо всі елементи в контейнері підпадають під умову лямбди</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					forceinline bool All(Lambda&& lambda) {
						return static_cast<const ThisCollection&>(*this).AllImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					forceinline T& First(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).FirstImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					forceinline T& Last(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).LastImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає перший елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
					forceinline const T& First(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).FirstImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає останій елемент який підпадає під умову лямбди в колекції якщо він існує</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const T&>::State>, bool> = true>
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
					/// <param name="Lambda">Лямбда формату void(*)(T&&)</param>
					/// <returns>Силку на поточний об'єкт колекції</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, void, T&&>::State>, bool> = true>
					forceinline ThisCollection& RemoveAll(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).RemoveAllImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Реверсує порядок елементів в колекції
					/// </summary>
					/// <returns>Силку на поточну колекцію</returns>
					forceinline ThisCollection& Reverse() {
						return static_cast<ThisCollection&>(*this).ReverseImpl();
					}
				public:
					forceinline virtual ~IFixedCollection() = 0 {}
				};
			}
		}
	}
}
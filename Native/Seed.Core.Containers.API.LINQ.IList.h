#pragma once
#include "Seed.Core.Containers.API.LINQ.IOrderable.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// LINQ розширення для списку
				/// </summary>
				/// <typeparam name="TList">Тип списку</typeparam>
				/// <typeparam name="THeap">Тип кучі</typeparam>
				/// <typeparam name="TIsConcurencySuport">True якщо підтримує багато потоковість</typeparam>
				template<template<typename THeap, typename T>typename TList, typename THeap, typename T, bool TIsConcurencySuport>
				class IList : public IOrderable<TList, THeap, T, true, TIsConcurencySuport> {
				protected:
					using ListType = TList<THeap, T>;
				public:
					/// <summary>
					/// Видалення елементів які підпадають під умову FilteringLambda
					/// </summary>
					/// <typeparam name="FilteringLambda">Тип лямбди з сигнатурою bool(*)(const T&amp;) або bool(*)(const T&amp;, SizeT)</typeparam>
					/// <typeparam name="Lambda">Тип лямбди яка знищує елементи з сигнатурою void(*)(T&amp;&amp;)</typeparam>
					/// <param name="filter">Лямбда з сигнатурою bool(*)(const T&amp;) або bool(*)(const T&amp;, SizeT)</param>
					/// <param name="lambda">Лямбда яка знищує елементи з сигнатурою void(*)(T&amp;&amp;)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<
						typename Lambda,
						typename FilteringLambda,
						TypeTraits::EnableIfTV<TypeTraits::LogicConst<
						(TypeTraits::IsLambdaConvertibleT<Lambda, void, T&&>::State ||
							TypeTraits::IsLambdaConvertibleT<Lambda, void, T&&, SizeT>::State) &&
							(TypeTraits::IsLambdaConvertibleT<FilteringLambda, Boolean, const T&>::State ||
								TypeTraits::IsLambdaConvertibleT<FilteringLambda, Boolean, const T&, SizeT>::State)
					>, bool> = true>
						forceinline ListType& RemoveIf(FilteringLambda filter, Lambda&& lambda) {
						return static_cast<ListType&>(*this).RemoveIfImpl(MoveRValue(filter), MoveRValue(lambda));
					}

					/// <summary>
					/// Видалення елементів які підпадають під умову FilteringLambda
					/// </summary>
					/// <typeparam name="FilteringLambda">Тип лямбди з сигнатурою bool(*)(const T&amp;) або bool(*)(const T&amp;, SizeT)</typeparam>
					/// <param name="filter">Лямбда з сигнатурою bool(*)(const T&) або bool(*)(const T&amp;, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<
						typename FilteringLambda,
						TypeTraits::EnableIfTV<TypeTraits::LogicConst<
							(TypeTraits::IsLambdaConvertibleT<FilteringLambda, Boolean, const T&>::State ||
								TypeTraits::IsLambdaConvertibleT<FilteringLambda, Boolean, const T&, SizeT>::State)
					>, bool> = true>
						forceinline ListType& RemoveIf(FilteringLambda filter) {
						return static_cast<ListType&>(*this).RemoveIfImpl(MoveRValue(filter), [] (T&&){});
					}
				public:
					virtual ~IList() = 0 {}
				};
			}
		}
	}
}
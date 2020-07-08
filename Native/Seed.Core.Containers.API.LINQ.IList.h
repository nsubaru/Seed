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
					template<typename TLambda, typename TFilteringLambda>
					requires(Concepts::IsInvockableWithResult<TFilteringLambda, Boolean, const T&> ||
							 Concepts::IsInvockableWithResult<TFilteringLambda, Boolean, const T&, SizeT>) &&
							(Concepts::IsInvockable<TLambda, T&&> || Concepts::IsInvockable<TLambda, T&&, SizeT>)
						forceinline ListType& RemoveIf(TFilteringLambda&& filter, TLambda&& lambda) {
						return static_cast<ListType&>(*this).RemoveIfImpl(MoveRValue(filter), MoveRValue(lambda));
					}

					/// <summary>
					/// Видалення елементів які підпадають під умову FilteringLambda
					/// </summary>
					/// <typeparam name="FilteringLambda">Тип лямбди з сигнатурою bool(*)(const T&amp;) або bool(*)(const T&amp;, SizeT)</typeparam>
					/// <param name="filter">Лямбда з сигнатурою bool(*)(const T&amp;) або bool(*)(const T&amp;, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename TFilteringLambda>
					requires Concepts::IsInvockableWithResult<TFilteringLambda, Boolean, const T&> ||
							 Concepts::IsInvockableWithResult<TFilteringLambda, Boolean, const T&, SizeT>
					forceinline ListType& RemoveIf(TFilteringLambda&& filter) {
						return static_cast<ListType&>(*this).RemoveIfImpl(MoveRValue(filter), [] (T&&){});
					}
				public:
					forceinline virtual ~IList() = 0 {}
				};
			}
		}
	}
}
#pragma once
#include "Seed.Core.Containers.API.LINQ.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// Клас LINQ розширення рядкового типу
				/// </summary>
				/// <typeparam name="TArray">Тип рядка</typeparam>
				/// <typeparam name="THeap">Куча рядка </typeparam>
				/// <typeparam name="TChar">Тип символів(елементів)</typeparam>
				/// <typeparam name="TIsConcurencySuport">Вказує чи колекція підтримує багато поточність </typeparam>
				template<template<typename THeap> typename TString, typename THeap, typename TChar, Boolean TIsConcurencySuport>
				class IString : public IStringCollectionBaseLinq<TString, THeap, TChar, TIsConcurencySuport> {
				private:
					using ThisCollection = TString<THeap>;
				public:
					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(TChar&amp;) або void(*)(TChar&amp;, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename TLambda> 
					requires (Concepts::IsInvockable<TLambda, TChar&> || Concepts::IsInvockable<TLambda, TChar&, SizeT> ||
							  Concepts::IsInvockable<TLambda, const TChar&> || Concepts::IsInvockable<TLambda, const TChar&, SizeT>)
					ForceInline ThisCollection& ForEach(TLambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(TChar&amp;) або void(*)(TChar&amp;, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename TLambda> 
					requires (Concepts::IsInvockable<TLambda, const TChar&> || Concepts::IsInvockable<TLambda, const TChar&, SizeT>)
						ForceInline const ThisCollection& ForEach(TLambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(TChar&amp;) або void(*)(TChar&amp;, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename TLambda> 
					requires (Concepts::IsInvockable<TLambda, TChar&> || Concepts::IsInvockable<TLambda, TChar&, SizeT> ||
							  Concepts::IsInvockable<TLambda, const TChar&> || Concepts::IsInvockable<TLambda, const TChar&, SizeT>)
						ForceInline ThisCollection& ForEach(TLambda&& lambda, SizeT Count) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), Count);
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(TChar&amp;) або void(*)(TChar&amp;, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename TLambda> 
					requires (Concepts::IsInvockable<TLambda, const TChar&> || Concepts::IsInvockable<TLambda, const TChar&, SizeT>)
						ForceInline const ThisCollection& ForEach(TLambda&& lambda, SizeT Count) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), Count);
					}
				public:
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
					requires Concepts::IsInvockableWithResult<TLambda, bool, const TChar&>
					ForceInline bool Any(TLambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).AnyImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає true якщо всі елементи в контейнері підпадають під умову лямбди
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&amp;)</param>
					/// <returns>Вертає true якщо всі елементи в контейнері підпадають під умову лямбди</returns>
					template<typename TLambda>
					requires Concepts::IsInvockableWithResult<TLambda, bool, const TChar&>
					ForceInline bool All(TLambda&& lambda) const  {
						return static_cast<const ThisCollection&>(*this).AllImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Реверсує порядок елементів в колекції
					/// </summary>
					/// <returns>Силку на поточну колекцію</returns>
					ForceInline ThisCollection& Reverse() {
						return static_cast<ThisCollection&>(*this).ReverseImpl();
					}
				public:
					ForceInline virtual ~IString() = 0 {}
				};
			}
		}
	}
}
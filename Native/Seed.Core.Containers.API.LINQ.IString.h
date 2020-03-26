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
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(TChar&) або void(*)(TChar&, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, TChar&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, TChar&, SizeT>::State
						>,
						bool
					> = true>
						ThisCollection& ForEach(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(TChar&) або void(*)(TChar&, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<
						typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TChar&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TChar&, SizeT>::State
						>,
						bool
					> = true>
						forceinline const ThisCollection& ForEach(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(TChar&) або void(*)(TChar&, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, TChar&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, TChar&, SizeT>::State
						>,
						bool
					> = true>
						ThisCollection& ForEach(Lambda&& lambda, SizeT Count) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), Count);
					}

					/// <summary>
					/// Обхід усіх елементів в колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди яка викликатиметься для кожного елементу колекції</typeparam>
					/// <param name="lambda">Лямбди яка викликатиметься для кожного елементу колекції. ЇЇ вигляд void(*)(TChar&) або void(*)(TChar&, SizeT)</param>
					/// <returns>Силку на поточну колекцію</returns>
					template<
						typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TChar&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TChar&, SizeT>::State
						>,
						bool
					> = true>
						forceinline const ThisCollection& ForEach(Lambda&& lambda, SizeT Count) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), Count);
					}
				public:
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
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const TChar&>::State>, bool> = true>
					forceinline bool Any(Lambda&& lambda) {
						return static_cast<const ThisCollection&>(*this).AnyImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Вертає true якщо всі елементи в контейнері підпадають під умову лямбди
					/// </summary>
					/// <param name="lambda">Лямбда формату bool(*)(const T&)</param>
					/// <returns>Вертає true якщо всі елементи в контейнері підпадають під умову лямбди</returns>
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const TChar&>::State>, bool> = true>
					forceinline bool All(Lambda&& lambda) {
						return static_cast<const ThisCollection&>(*this).AllImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Реверсує порядок елементів в колекції
					/// </summary>
					/// <returns>Силку на поточну колекцію</returns>
					forceinline ThisCollection& Reverse() {
						return static_cast<ThisCollection&>(*this).ReverseImpl();
					}
				public:
					virtual ~IString() = 0 {}
				};
			}
		}
	}
}
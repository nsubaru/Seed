#pragma once
#include "Seed.Core.Containers.API.LINQ.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				/// <summary>
				/// LINQ розширення асоціативних колекцій
				/// </summary>
				/// <typeparam name="TCollection">Тип асоціативної колекції</typeparam>
				/// <typeparam name="THeap">Тип кучі</typeparam>
				/// <typeparam name="TKey">Тип ключа</typeparam>
				/// <typeparam name="TValue">Тип значення</typeparam>
				/// <typeparam name="TPair">Тип пари ключа - значення</typeparam>
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				class IAssociableCollection :
					public IAssociableCollectionBase<TCollection, THeap, TKey, TValue, TPair, IsConcurencySuportT>
				{
				private:
					/// <summary>
					/// Тип асоціативної колекції
					/// </summary>
					using ThisCollection = TCollection<THeap, TKey, TValue>;
				public:
					/// <summary>
					/// Обхід асоціативної колекції, викликає лямбду для кожної пари ключ - значення
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда функція яку викличе для кожної пари ключ значення може приймати або пару або пару і номер</param>
					/// <returns>Силку на колекцію</returns>
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, TPair&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, TPair&, SizeT>::State
						>,
						bool
					> = true>
						ThisCollection& ForEach(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід константної асоціативної колекції, виклиає лямбду для кожної пари ключ - значення
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда функція яку викличе для кожної пари ключ значення може приймати або силку пару або її і номер</param>
					/// <returns>Константну силку на колекцію</returns>
					template<
						typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TPair&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TPair&, SizeT>::State
						>,
						bool
					> = true>
						__forceinline const ThisCollection& ForEach(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}
				public:
					/// <summary>
					/// Обхід асоціативної колекції, виклиає лямбду для кожного ключа
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда яку викличе для кожного ключа, може приймати константну силку на ключ або її і номер елемента</param>
					/// <returns>Силку на колекцію</returns>
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TKey&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TKey&, SizeT>::State
						>,
						bool
					> = true>
						ThisCollection& ForEachKey(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachKeyImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід константної асоціативної колекції, виклиає лямбду для кожного ключа
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда яку викличе для кожного ключа, може приймати константну силку на ключ або її і номер елемента</param>
					/// <returns>Константну силку на колекцію</returns>
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TKey&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TKey&, SizeT>::State
						>,
						bool
					> = true>
						const ThisCollection& ForEachKey(Lambda&& lambda) const {
						return static_cast<ThisCollection&>(*this).ForEachKeyImpl(MoveRValue(lambda));
					}
				public:
					/// <summary>
					/// Обхід асоціативної колекції, виклиає лямбду для кожного значення
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда яку викличе для кожного значення, може приймати силку на значення або її і номер елемента</param>
					/// <returns>Силку на колекцію</returns>
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, TValue&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, TValue&, SizeT>::State
						>,
						bool
					> = true>
						ThisCollection& ForEachValue(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachValueImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід константної асоціативної колекції, виклиає лямбду для кожного значення
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда яку викличе для кожного значення, може приймати силку на значення або її і номер елемента</param>
					/// <returns>Константну силку на колекцію</returns>
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TValue&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TValue&, SizeT>::State
						>,
						bool
					> = true>
						const ThisCollection& ForEachValue(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachValueImpl(MoveRValue(lambda));
					}
				public:
					/// <summary>
					/// Видаляє всі елементи з колекції
					/// </summary>
					/// <returns>Силку на колекцію</returns>
					ThisCollection& RemoveAll() {
						return static_cast<ThisCollection&>(*this).RemoveAllImpl();
					}

					/// <summary>
					/// Видаляє всі елементи з колекції і перед цим викликає для кожної пари лямбду
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда, приймає rvalue силку на пару</param>
					/// <returns>Силку на колекцію</returns>
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, TPair&&>::State
						>,
						bool
					> = true>
						ThisCollection& RemoveAllWith(Lambda && lambda) {
						return static_cast<ThisCollection&>(*this).RemoveAllWithImpl(MoveRValue(lambda));
					}
				public:
					/// <summary>
					/// Деструктор чисто віртуальний бо це лиш розширення асоціативних колекцій
					/// </summary>
					virtual ~IAssociableCollection() = 0 {};
				};
			}
		}
	}
}
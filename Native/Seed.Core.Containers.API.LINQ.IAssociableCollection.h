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
				/// <typeparam name="TIsConcurencySuport">Флаг підтримки багатопотоковості колекції</typeparam>
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
					template<typename TLambda>
						requires 
							(Concepts::IsInvockable<TLambda, TPair&> || Concepts::IsInvockable<TLambda, TPair&, SizeT> ||
							Concepts::IsInvockable<TLambda, const TPair&> || Concepts::IsInvockable<TLambda, const TPair&, SizeT>)
						ForceInline ThisCollection& ForEach(TLambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід константної асоціативної колекції, виклиає лямбду для кожної пари ключ - значення
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда функція яку викличе для кожної пари ключ значення може приймати або силку пару або її і номер</param>
					/// <returns>Константну силку на колекцію</returns>
					template<typename TLambda>
						requires Concepts::IsInvockable<TLambda, const TPair&> || Concepts::IsInvockable<TLambda, const TPair&, SizeT>
						ForceInline const ThisCollection& ForEach(TLambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}
				public:
					/// <summary>
					/// Обхід асоціативної колекції, виклиає лямбду для кожного ключа
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда яку викличе для кожного ключа, може приймати константну силку на ключ або її і номер елемента</param>
					/// <returns>Силку на колекцію</returns>
					template<typename TLambda>
					requires Concepts::IsInvockable<TLambda, const TKey&> || Concepts::IsInvockable<TLambda, const TKey&, SizeT>
						ForceInline ThisCollection& ForEachKey(TLambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachKeyImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід константної асоціативної колекції, виклиає лямбду для кожного ключа
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда яку викличе для кожного ключа, може приймати константну силку на ключ або її і номер елемента</param>
					/// <returns>Константну силку на колекцію</returns>
					template<typename TLambda>
					requires Concepts::IsInvockable<TLambda, const TKey&> || Concepts::IsInvockable<TLambda, const TKey&, SizeT>
						ForceInline const ThisCollection& ForEachKey(TLambda&& lambda) const {
						return static_cast<ThisCollection&>(*this).ForEachKeyImpl(MoveRValue(lambda));
					}
				public:
					/// <summary>
					/// Обхід асоціативної колекції, виклиає лямбду для кожного значення
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда яку викличе для кожного значення, може приймати силку на значення або її і номер елемента</param>
					/// <returns>Силку на колекцію</returns>
					template<typename TLambda>
					requires Concepts::IsInvockable<TLambda, TValue&> || Concepts::IsInvockable<TLambda, TValue&, SizeT>
						ForceInline ThisCollection& ForEachValue(TLambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachValueImpl(MoveRValue(lambda));
					}

					/// <summary>
					/// Обхід константної асоціативної колекції, виклиає лямбду для кожного значення
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда яку викличе для кожного значення, може приймати силку на значення або її і номер елемента</param>
					/// <returns>Константну силку на колекцію</returns>
					template<typename TLambda>
					requires Concepts::IsInvockable<TLambda, const TValue&> || Concepts::IsInvockable<TLambda, const TValue&, SizeT>
						ForceInline const ThisCollection& ForEachValue(TLambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachValueImpl(MoveRValue(lambda));
					}
				public:
					/// <summary>
					/// Видаляє всі елементи з колекції
					/// </summary>
					/// <returns>Силку на колекцію</returns>
					ForceInline ThisCollection& RemoveAll() {
						return static_cast<ThisCollection&>(*this).RemoveAllImpl();
					}

					/// <summary>
					/// Видаляє всі елементи з колекції і перед цим викликає для кожної пари лямбду
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди</typeparam>
					/// <param name="lambda">Лямбда, приймає rvalue силку на пару</param>
					/// <returns>Силку на колекцію</returns>
					template<typename TLambda>
					requires Concepts::IsInvockable<TLambda, TPair&&>
						ForceInline ThisCollection& RemoveAllWith(TLambda && lambda) {
						return static_cast<ThisCollection&>(*this).RemoveAllWithImpl(MoveRValue(lambda));
					}
				public:
					/// <summary>
					/// Копіює елементи до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="FilterLambda">Тип лямбди фільтрування елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="UHeap">Тип кучі нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename THeap, typename U> class NewCollection,
						typename TLambda,
						typename TFilterLambda, 
						typename UHeap, 
						typename U = TypeTraits::InvockableResult<TLambda, const TPair&>
					>
						requires   (Concepts::IsInvockable<TLambda, const TPair&> &&
									Concepts::IsInvockable<TFilterLambda, const TPair&> &&
									Concepts::IsBasedOn<ICollectionGenericBase, NewCollection<UHeap, U>>&&
									Concepts::IsSame<bool, TypeTraits::InvockableResult<TFilterLambda, const TPair&>>)
					NewCollection<UHeap, U> CopyToCollection(TFilterLambda&& filterLambda, TLambda&& lambda) const {
						if constexpr (NewCollection<UHeap, U>::IsDynamic == false)
						{
							TFilterLambda filterCopy = filterLambda;
							NewCollection<UHeap, U> result = NewCollection<UHeap, U>(static_cast<ThisCollection&>(*this)
								.GetCountIf(MoveRValue(filterCopy)));

							SizeT Index = 0;
							this->ForEach([&] (const TPair& pair) InlineLambda
							{
								if (filterLambda(pair))
								{
									result.UnsafeSet(lambda(pair), Index);
									Index++;
								}
							});
							return result;
						}
						else
						{
							NewCollection<UHeap, U> result;
							this->ForEach([&] (const TPair& pair) InlineLambda
							{
								if (filterLambda(pair))
								{
									result.UnsafeAdd(lambda(pair));
								}
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює елементи до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="FilterLambda">Тип лямбди фільтрування елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="UHeap">Тип кучі нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						typename TLambda,
						typename TFilterLambda,  
						typename U = TypeTraits::InvockableResult<TLambda, const TPair&>
					>
						requires(Concepts::IsInvockable<TLambda, const TPair&> &&
								 Concepts::IsInvockable<TFilterLambda, const TPair&> &&
								 Concepts::IsBasedOn<ICollectionGenericBase, NewCollection<U>> &&
								 Concepts::IsSame<bool, TypeTraits::InvockableResult<TFilterLambda, const TPair&>>)
					NewCollection<U> CopyToCollection(TFilterLambda&& filterLambda, TLambda&& lambda) const {
						if constexpr (NewCollection<U>::IsDynamic == false)
						{
							TFilterLambda filterCopy = filterLambda;
							NewCollection<U> result = NewCollection<U>(static_cast<ThisCollection&>(*this)
								.GetCountIf(MoveRValue(filterCopy)));

							SizeT Index = 0;
							this->ForEach([&] (const TPair& pair) InlineLambda
							{
								if (filterLambda(pair))
								{
									result.UnsafeSet(lambda(pair), Index);
									Index++;
								}
							});
							return result;
						}
						else
						{
							NewCollection<U> result;
							this->ForEach([&] (const TPair& pair) InlineLambda
							{
								if (filterLambda(pair))
								{
									result.UnsafeAdd(lambda(pair));
								}
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює елементи до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename UHeap, typename U> class NewCollection,
						typename Lambda,
						typename UHeap,
						typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<Lambda, U, const TPair&> &&
								TypeTraits::IsLambdaConvertibleV<Lambda, U, TPair&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<UHeap, U>>
								>,
							bool> 
						= true
					>
					NewCollection<UHeap, U> CopyToCollection(Lambda&& lambda) const {
						if constexpr (NewCollection<UHeap, U>::IsDynamic == false)
						{
							NewCollection<UHeap, U> result = NewCollection<UHeap, U>(static_cast<ThisCollection&>(*this).GetCount());
							this->ForEach([&] (const TPair& pair, SizeT Index) InlineLambda
							{
								result.UnsafeSet(lambda(pair), Index);
							});
							return result;
						}
						else
						{
							NewCollection<UHeap, U> result;
							this->ForEach([&] (const TPair& pair) InlineLambda
							{
								result.UnsafeAdd(lambda(pair));
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює елементи до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						typename Lambda,
						typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<Lambda, U, const TPair&> &&
								TypeTraits::IsLambdaConvertibleV<Lambda, U, TPair&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<U>>
								>,
							bool> 
						= true
					>
					NewCollection<U> CopyToCollection(Lambda&& lambda) const {
						if constexpr (NewCollection<U>::IsDynamic == false)
						{
							NewCollection<U> result = NewCollection<U>(static_cast<ThisCollection&>(*this).GetCount());
							this->ForEach([&] (const TPair& pair, SizeT Index) InlineLambda
							{
								result.UnsafeSet(lambda(pair), Index);
							});
							return result;
						}
						else
						{
							NewCollection<U> result;
							this->ForEach([&] (const TPair& pair) InlineLambda
							{
								result.UnsafeAdd(lambda(pair));
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює елементи до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="UHeap">Тип кучі нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename UHeap, typename U> class NewCollection,
						typename UHeap, 
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<UHeap, TPair>>
								>,
							bool> 
						= true
					>
					ForceInline NewCollection<UHeap, TPair> CopyToCollection() const {
						return this->CopyToCollection<NewCollection, UHeap>([] (const TPair& pair) -> TPair InlineLambda
						{
							return pair;
						});
					}

					/// <summary>
					/// Копіює елементи до колекції
					/// </summary>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<TPair>>
								>,
							bool> 
						= true
					>
					ForceInline NewCollection<TPair> CopyToCollection() const {
						return this->CopyToCollection<NewCollection>([] (const TPair& pair) -> TPair InlineLambda
						{
							return pair;
						});
					}

					/// <summary>
					/// Копіює елементи до колекції
					/// </summary>
					/// <typeparam name="FilterLambda">Тип лямбди фільтрування елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						typename FilterLambda,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<FilterLambda, bool, const TPair&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<TPair>>
								>,
							bool> 
						= true
					>
					ForceInline NewCollection<TPair> CopyIfToCollection(FilterLambda && filter) const {
						return this->CopyToCollection<NewCollection>(MoveRValue(filter), [] (const TPair& pair) -> TPair InlineLambda
						{
							return pair;
						});
					}
				public:
					/// <summary>
					/// Копіює ключі до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="FilterLambda">Тип лямбди фільтрування елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="UHeap">Тип кучі нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename UHeap, typename U> class NewCollection,
						typename Lambda,
						typename FilterLambda, 
						typename UHeap, 
						typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<Lambda, U, const TKey&> &&
								TypeTraits::IsLambdaConvertibleV<FilterLambda, bool, const TKey&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<UHeap, U>>
								>,
							bool> 
						= true
					>
					NewCollection<UHeap, U> CopyKeyToCollection(FilterLambda&& filterLambda, Lambda&& lambda) const {
						if constexpr (NewCollection<UHeap, U>::IsDynamic == false)
						{
							NewCollection<UHeap, U> result = NewCollection<UHeap, U>(static_cast<ThisCollection&>(*this)
								.GetCountIf([&] (const TPair& pair) InlineLambda
							{
								return filterLambda(pair.Key);
							}));

							SizeT Index = 0;
							this->ForEachKey([&] (const TKey& key) InlineLambda
							{
								if (filterLambda(key))
								{
									result.UnsafeSet(lambda(key), Index);
								}
							});
							return result;
						}
						else
						{
							NewCollection<UHeap, U> result;
							this->ForEachKey([&] (const TKey& key) InlineLambda
							{
								if (filterLambda(key))
								{
									result.UnsafeAdd(lambda(key));
								}
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює ключі до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="FilterLambda">Тип лямбди фільтрування елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="UHeap">Тип кучі нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						typename Lambda,
						typename FilterLambda,  
						typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<Lambda, U, const TKey&> &&
								TypeTraits::IsLambdaConvertibleV<FilterLambda, bool, const TKey&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<U>>
								>,
							bool> 
						= true
					>
					NewCollection<U> CopyKeyToCollection(FilterLambda&& filterLambda, Lambda&& lambda) const {
						if constexpr (NewCollection<U>::IsDynamic == false)
						{
							NewCollection<U> result = NewCollection<U>(static_cast<ThisCollection&>(*this) 
								.GetCountIf([&] (const TPair& pair) InlineLambda
							{
								return filterLambda(pair.Key);
							}));

							this->ForEachKey([&] (const TKey& key, SizeT Index) InlineLambda
							{
								if (filterLambda(key))
								{
									result.UnsafeSet(lambda(key), Index);
								}
							});
							return result;
						}
						else
						{
							NewCollection<U> result;
							this->ForEachKey([&] (const TKey& key) InlineLambda
							{
								if (filterLambda(key))
								{
									result.UnsafeAdd(lambda(key));
								}
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює ключі до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="UHeap">Тип кучі нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename UHeap, typename U> class NewCollection,
						typename Lambda,
						typename UHeap, 
						typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<Lambda, U, const TKey&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<UHeap, U>>
								>,
							bool> 
						= true
					>
					NewCollection<UHeap, U> CopyKeyToCollection(Lambda&& lambda) const {
						if constexpr (NewCollection<UHeap, U>::IsDynamic == false)
						{
							NewCollection<UHeap, U> result = NewCollection<UHeap, U>(static_cast<ThisCollection&>(*this).GetCount());
							this->ForEachKey([&] (const TKey& pair, SizeT Index) InlineLambda
							{
								result.UnsafeSet(lambda(pair), Index);
							});
							return result;
						}
						else
						{
							NewCollection<UHeap, U> result;
							this->ForEachKey([&] (const TKey& pair) InlineLambda
							{
								result.UnsafeAdd(lambda(pair));
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює ключі до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						typename Lambda,
						typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<Lambda, U, const TKey&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<U>>
								>,
							bool> 
						= true
					>
					NewCollection<U> CopyKeyToCollection(Lambda&& lambda) const {
						if constexpr (NewCollection<U>::IsDynamic == false)
						{
							NewCollection<U> result = NewCollection<U>(static_cast<ThisCollection&>(*this).GetCount());
							this->ForEachKey([&] (const TKey& pair, SizeT Index) InlineLambda
							{
								result.UnsafeSet(lambda(pair), Index);
							});
							return result;
						}
						else
						{
							NewCollection<U> result;
							this->ForEachKey([&] (const TKey& pair) InlineLambda
							{
								result.UnsafeAdd(lambda(pair));
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює ключі до колекції
					/// </summary>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<TKey>>
								>,
							bool> 
						= true
					>
					ForceInline NewCollection<TKey> CopyKeyToCollection() const {
						return this->CopyKeyToCollection<NewCollection>([] (const TKey& pair) -> TKey InlineLambda
						{
							return pair;
						});
					}

					/// <summary>
					/// Копіює ключі до колекції
					/// </summary>
					/// <typeparam name="FilterLambda">Тип лямбди фільтрування елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						typename FilterLambda,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<FilterLambda, bool, const TKey&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<TKey>>
								>,
							bool> 
						= true
					>
					ForceInline NewCollection<TKey> CopyKeyIfToCollection(FilterLambda&& filter) const {
						return this->CopyKeyToCollection<NewCollection>(MoveRValue(filter), [] (const TKey& pair) -> TKey InlineLambda
						{
							return pair;
						});
					}
				public:
					/// <summary>
					/// Копіює значення до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="FilterLambda">Тип лямбди фільтрування елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="UHeap">Тип кучі нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename UHeap, typename U> class NewCollection,
						typename Lambda,
						typename FilterLambda, 
						typename UHeap, 
						typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<Lambda, U, const TValue&> &&
								TypeTraits::IsLambdaConvertibleV<FilterLambda, bool, const TValue&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<UHeap, U>>
								>,
							bool> 
						= true
					>
					NewCollection<UHeap, U> CopyValueToCollection(FilterLambda&& filterLambda, Lambda&& lambda) const {
						if constexpr (NewCollection<UHeap, U>::IsDynamic == false)
						{
							NewCollection<UHeap, U> result = NewCollection<UHeap, U>(static_cast<ThisCollection&>(*this)
								.GetCountIf([&] (const TPair& pair) InlineLambda
							{
								return filterLambda(pair.Value);
							}));

							SizeT Index = 0;
							this->ForEachValue([&] (const TValue& value) InlineLambda
							{
								if (filterLambda(value))
								{
									result.UnsafeSet(lambda(value), Index);
									Index++;
								}
							});
							return result;
						}
						else
						{
							NewCollection<UHeap, U> result;
							this->ForEachValue([&] (const TValue& value) InlineLambda
							{
								if (filterLambda(value))
								{
									result.UnsafeAdd(lambda(value));
								}
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює значення до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="FilterLambda">Тип лямбди фільтрування елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="UHeap">Тип кучі нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						typename Lambda,
						typename FilterLambda,  
						typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<Lambda, U, const TValue&> &&
								TypeTraits::IsLambdaConvertibleV<FilterLambda, bool, const TValue&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<U>>
								>,
							bool> 
						= true
					>
					NewCollection<U> CopyValueToCollection(FilterLambda&& filterLambda, Lambda&& lambda) const {
						if constexpr (NewCollection<U>::IsDynamic == false)
						{
							NewCollection<U> result = NewCollection<U>(static_cast<ThisCollection&>(*this)
								.GetCountIf([&] (const TPair& pair) InlineLambda
							{
								return filterLambda(pair.Value);
							}));

							SizeT Index = 0;
							this->ForEachValue([&] (const TValue& value) InlineLambda
							{
								if (filterLambda(value))
								{
									result.UnsafeSet(lambda(value), Index);
									Index++;
								}
							});
							return result;
						}
						else
						{
							NewCollection<U> result;
							this->ForEachValue([&] (const TValue& value) InlineLambda
							{
								if (filterLambda(value))
								{
									result.UnsafeAdd(lambda(value));
								}
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює значення до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="UHeap">Тип кучі нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename THeap, typename U> class NewCollection,
						typename Lambda,
						typename UHeap, 
						typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<Lambda, U, const TValue&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<UHeap, U>>
								>,
							bool> 
						= true
					>
					NewCollection<UHeap, U> CopyValueToCollection(Lambda&& lambda) const {
						if constexpr (NewCollection<THeap, U>::IsDynamic == false)
						{
							NewCollection<THeap, U> result = NewCollection<THeap, U>(static_cast<ThisCollection&>(*this).GetCount());
							this->ForEachValue([&] (const TValue& pair, SizeT Index) InlineLambda
							{
								result.UnsafeSet(lambda(pair), Index);
							});
							return result;
						}
						else
						{
							NewCollection<THeap, U> result;
							this->ForEachValue([&] (const TValue& pair) InlineLambda
							{
								result.UnsafeAdd(lambda(pair));
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює значення до колекції
					/// </summary>
					/// <typeparam name="Lambda">Тип лямбди перетворення елементів</typeparam>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						typename Lambda,
						typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<Lambda, U, const TValue&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<U>>
								>,
							bool> 
						= true
					>
					NewCollection<U> CopyValueToCollection(Lambda&& lambda) const {
						if constexpr (NewCollection<U>::IsDynamic == false)
						{
							NewCollection<U> result = NewCollection<U>(static_cast<ThisCollection&>(*this).GetCount());
							this->ForEach([&] (const TValue& pair, SizeT Index) InlineLambda
							{
								result.UnsafeSet(lambda(pair), Index);
							});
							return result;
						}
						else
						{
							NewCollection<U> result;
							this->ForEach([&] (const TValue& pair) InlineLambda
							{
								result.UnsafeAdd(lambda(pair));
							});
							return result;
						}
					}

					/// <summary>
					/// Копіює значення до колекції
					/// </summary>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<TPair>>
								>,
							bool> 
						= true
					>
					NewCollection<TValue> CopyValueToCollection() const {
						return this->CopyValueToCollection<NewCollection>([] (const TValue& pair) -> TValue
						{
							return pair;
						});
					}

					/// <summary>
					/// Копіює значення до колекції
					/// </summary>
					/// <typeparam name="NewCollection">Тип нової колекції</typeparam>
					/// <typeparam name="U">Тип елементів в новій колекції</typeparam>
					/// <returns>Колекція з копією всіх елементів</returns>
					template<
						template<typename U> class NewCollection,
						typename FilterLambda,
						TypeTraits::EnableIfTV<
							TypeTraits::LogicConst<
								TypeTraits::IsLambdaConvertibleV<FilterLambda, bool, const TValue&> &&
								TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<TPair>>
								>,
							bool> 
						= true
					>
					ForceInline NewCollection<TValue> CopyValueIfToCollection(FilterLambda&& filter) const {
						return this->CopyValueToCollection<NewCollection>(MoveRValue(filter), [] (const TValue& pair) -> TValue InlineLambda
						{
							return pair;
						});
					}
				public:
					/// <summary>
					/// Деструктор чисто віртуальний бо це лиш розширення асоціативних колекцій
					/// </summary>
					ForceInline virtual ~IAssociableCollection() = 0 {};
				};
			}
		}
	}
}
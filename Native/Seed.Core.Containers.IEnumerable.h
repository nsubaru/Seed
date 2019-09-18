#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace TypeTraits {
			///<summary>Статична провірка чи колекція підтримує зміну розміру</summary>
			///<param name = "Collection">Тип колекції</param>
			template<typename Collection>
			struct TIsResizable : TypeTraits::LogicFalse {};

			///<summary>Статична провірка чи колекція підтримує гарячу зміну розміру і її не треба інформування про зміну розміру</summary>
			///<param name = "Collection">Тип колекції</param>
			template<typename Collection>
			struct TIsDynamicSize : TypeTraits::LogicFalse {};

			template<typename Pair>
			struct PairTypes : LogicFalse {};
		}

		namespace Queryable {
#pragma region Lambda Holder
			template<typename RetType, typename ... Args>
			class LambdaHolder {
			public:
				virtual RetType operator()(Args&& ... args) & = 0;
				virtual ~LambdaHolder() {}
			};

			template<typename Lambda, typename RetType, typename ... Args>
			class LambdaHolderImpl : public LambdaHolder<RetType, Args...> {
				Lambda lambda;
			public:
				LambdaHolderImpl(Lambda&& lambda) : lambda(lambda) {}	
				virtual auto operator()(Args &&... args) & -> RetType {
					if constexpr (TypeTraits::IsVoidV<RetType>)
					{
						lambda(ForwardReference<Args>(args)...);
						return;
					}
					else {
						return lambda(ForwardReference<Args>(args)...);
					}
				}
				virtual ~LambdaHolderImpl() {}
			};
#pragma endregion

			template<typename Container, typename Pair, typename TKey, typename TValue>
			class IAssociable;
#pragma region Base Interfaces for checking Collection Type
			///<summary>Батьківський клас для всіх IQueryable</summary>
			class SeedKernelCall IQueryableBase {
			protected:
				virtual ~IQueryableBase() = 0 {};
				//Всі IQueryable бачать це й класс, це зроблено для провірки чи тип є IQueryable
				template<typename Container, typename T>
				friend class IQueryable;
			};

			///<summary>Батьківський клас для всіх IQueryable</summary>
			class SeedKernelCall IAssociableBase {
			protected:
				virtual ~IAssociableBase() = 0 {};
				//Всі IAssociable бачать це й класс, це зроблено для провірки чи тип є IAssociable
				template<typename Container, typename Pair, typename TKey, typename TValue>
				friend class IAssociable;
			};
#pragma endregion
			///<summary>Інтерфейс функціональної обробки елементів колекцій</summary>
			///<param name = "Container">Колекція, яка використовує інтерфейс</param>
			///<param name = "T">Тип елементів в колекції</param>
			template<class Container, typename T>
			class SeedKernelCall IQueryable : IQueryableBase {
			//protected:
			//	///<summary>Виконує лямбду в скоопі блокировки 
			//	virtual void ExecuteInScope(LambdaHolder<void>& lambda) = 0;
			protected:
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& ForEachImpl(LambdaHolder<void, T&>& lambda) = 0;
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& ForEachImpl(LambdaHolder<void, T&, SizeT>& lambda) = 0;
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& ForEachImpl(LambdaHolder<void, const T&>& lambda) = 0;
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& ForEachImpl(LambdaHolder<void, const T&, SizeT>& lambda) = 0;
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual const Container& ForEachImpl(LambdaHolder<void, const T&>& lambda) const = 0;
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Константа силка на поточний контейнер</returns>
				virtual const Container& ForEachImpl(LambdaHolder<void, const T&, SizeT>& lambda) const = 0;
			protected:
				///<summary>Підготовка кінцевого контейнера до копіювання\переміщення в нього елементів якщо він не уміє змінювати розмір динамічно</summary>
				virtual void PrepareCopyToOrMove(SizeT Count) = 0;
				///<summary>Переміщення елемента в новий контенер</summary>
				virtual void TransferTo(T&& Value, SizeT ItemIndex) = 0;
			protected:
				virtual Container& RemoveAllImpl(LambdaHolder<void, T&&>& lambda) = 0;
				virtual Container& RemoveAllImpl(LambdaHolder<void, T&&, SizeT>& lambda) = 0;
			protected:
				//virtual SizeT CountIfImpl(LambdaHolder<bool, const T&>& lambda) = 0;
			public:
#pragma region ForEach
				///<summary>Виконує дію для кожного елемента</summary>
				///<param name = "lambda">Лямбда вираз який виконається для кожного елемента</param>
				///<param name = "Lambda">Тип класу лямбди</param>
				///<returns>Силку на поточний контейнер</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, T&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, const T&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, T&, SizeT>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, const T&, SizeT>::State>,
						bool
					> = true
				>
				Container& ForEach(Lambda lambda) {
					using namespace TypeTraits;

					if constexpr (IsLambdaConvertible<Lambda, void, T&>::State) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, T&>(MoveRValue(lambda)));
					}
					else if constexpr (IsLambdaConvertible<Lambda, void, const T&>::State) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&>(MoveRValue(lambda)));
					}
					else if constexpr (IsLambdaConvertible<Lambda, void, T&, SizeT>::State) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, T&, SizeT>(MoveRValue(lambda)));
					}
					else if constexpr (IsLambdaConvertible<Lambda, void, const T&, SizeT>::State) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&, SizeT>(MoveRValue(lambda)));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::ForEach specific");
					}
				}
				///<summary>Виконує дію для кожного елемента в константній колекції</summary>
				///<param name = "lambda">Лямбда вираз який виконається для кожного елемента</param>
				///<param name = "Lambda">Тип класу лямбди</param>
				///<returns>Константна силка на поточний контейнер</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, const T&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, const T&, SizeT>::State>,
						bool
					> = true
				>
				const Container& ForEach(Lambda lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, void, const T&>) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&>(lambda));
					}
					else if constexpr (TypeTraits::IsLambdaConvertible<Lambda, void, const T&, SizeT>) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&, SizeT>(lambda));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::ForEach const specific");
					}
				}
#pragma endregion
#pragma region Copy and Move to IQueryable
				///<summary>Копіює всі елементи в нову колекцію</summary>
				///<param name = "lambda">Лямбда вираз яка описує правило перетворення елемента</param>
				///<param name = "U">Тип елементів нової колекціп</param>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<param name = "Lambda">Тип класу лямбди</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename U> typename NewContainer, 
					typename Lambda, 
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, U, const T&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, U, const T&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<U>>::State>,
					bool
					> = true
				>
				NewContainer<U> CopyTo(Lambda lambda) {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					using TargetIQueryableType = IQueryable<NewContainer<U>, U>;
					//Створюєм об'єкт нової колекції
					NewContainer<U> target = NewContainer<U>();
					//Інтерфейс IQueryable об'єкта нової колекції
					TargetIQueryableType& targetIQueryable = static_cast<TargetIQueryableType&>(target);
					//Інтерфейс IQueryable об'єкт поточної колекції
					IQueryable<Container, T>& This = *this;
					//Отримуєм кількість елементів поточної колекції
					SizeT count = This.GetCount();
					//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
					if (!TargetType::IsDynamicSize)
						targetIQueryable.PrepareCopyToOrMove(this->GetCount());
					//Провірка чи лямбда вирах перетворення приймає індекс елемента
					if constexpr (IsLambdaConvertible<Lambda, U, const T&>::State) {
						//Створення проміжного лямбда виразу який отримуватиме в циклі елементи старої колекції і копіюватиму їх в ному викликаючи лямба вираз перетворення 
						auto loopLambda = [&](const T& Value, SizeT Index) mutable {
							targetIQueryable.TransferTo(MoveRValue(lambda(Value)), Index);
						};
						//Виклик обходу старої колекції з лямбда виразом
						ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&, SizeT>(MoveRValue(loopLambda)));
					}
					else {
						//Створення проміжного лямбда виразу який отримуватиме в циклі елементи старої колекції і копіюватиму їх в ному викликаючи лямба вираз перетворення 
						auto loopLambda = [&](const T& Value, SizeT Index) mutable {
							targetIQueryable.TransferTo(MoveRValue(lambda(Value, Index)), Index);
						};
						//Виклик обходу старої колекції з лямбда виразом
						ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&, SizeT>(MoveRValue(loopLambda)));
					}
					//Повернення нової колекції
					return target;
				}

				///<summary>Копіює всі елементи в нову колекцію</summary>
				///<param name = "lambda">Лямбда вираз яка описує правило перетворення елемента</param>
				///<param name = "U">Тип елементів нової колекціп</param>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<param name = "Lambda">Тип класу лямбди</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename U> typename NewContainer,
					typename Lambda,
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, U, const T&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, U, const T&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<U>>::State>,
					bool
					> = true
				>
				NewContainer<U> CopyTo(Lambda lambda) const {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					using TargetIQueryableType = IQueryable<NewContainer<U>, U>;
					//Створюєм об'єкт нової колекції
					NewContainer<U> target = NewContainer<U>();
					//Інтерфейс IQueryable об'єкта нової колекції
					TargetIQueryableType& targetIQueryable = static_cast<TargetIQueryableType&>(target);
					//Інтерфейс IQueryable об'єкт поточної колекції
					const IQueryable<Container, T>& This = *this;
					//Отримуєм кількість елементів поточної колекції
					SizeT count = This.GetCount();
					//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
					if (!TargetType::IsDynamicSize)
						targetIQueryable.PrepareCopyToOrMove(this->GetCount());
					//Провірка чи лямбда вирах перетворення приймає індекс елемента
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, U, const T&>::State) {
						//Створення проміжного лямбда виразу який отримуватиме в циклі елементи старої колекції і копіюватиму їх в ному викликаючи лямба вираз перетворення 
						auto loopLambda = [&](const T& Value, SizeT Index) mutable {
							targetIQueryable.TransferTo(MoveRValue(lambda(Value)), Index);
						};
						//Виклик обходу старої колекції з лямбда виразом
						ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&, SizeT>(MoveRValue(loopLambda)));
					}
					else {
						//Створення проміжного лямбда виразу який отримуватиме в циклі елементи старої колекції і копіюватиму їх в ному викликаючи лямба вираз перетворення 
						auto loopLambda = [&](const T& Value, SizeT Index) mutable {
							targetIQueryable.TransferTo(MoveRValue(lambda(Value, Index)), Index);
						};
						//Виклик обходу старої колекції з лямбда виразом
						ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&, SizeT>(MoveRValue(loopLambda)));
					}
					//Повернення нової колекції
					return target;
				}

				///<summary>Копіює елементи які задовільняють умову в нову колекцію</summary>
				///<param name = "lambda">Лямбда вираз перетворення</param>
				///<param name = "filter">Лямбда вираз фільтрування</param>
				///<param name = "U">Тип елементів нової колекціп</param>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<param name = "Lambda">Тип класу лямбди перетворення</param>
				///<param name = "FilterLambda">Тип класу лямбди фільтрування</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename U> typename NewContainer, 
					typename Lambda, 
					typename FilterLambda, 
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, U, const T&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, U, const T&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<FilterLambda, bool, const T&>::State ||
					TypeTraits::IsLambdaConvertible<FilterLambda, bool, const T&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<U>>::State>,
					bool
					> = true
				>
				NewContainer<U> CopyToIf(Lambda lambda, FilterLambda filter) {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					using TargetIQueryableType = IQueryable<NewContainer<U>, U>;
					//Створюєм об'єкт нової колекції
					NewContainer<U> target;
					//Інтерфейс IQueryable об'єкта нової колекції
					TargetIQueryableType& targetIQueryable = target;
					//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
					if (!TargetType::IsDynamicSize)
						target.PrepareCopyToOrMove(this->GetCountIf(filter));
					//Змінна для індексування номерів
					SizeT Index = 0;
					//Проміжний лямбда вираз який викличеться для кожного елемента старої колекції
					auto loopLambda = [&](const T& Value, SizeT OldCollectionIndex) {
						//Провірка типу фільтруючої лямбди
						if constexpr (IsLambdaConvertible<FilterLambda, U, const T&>) {
							//Виклик з значення елемента
							if (filter(Value)) {
								if constexpr (IsLambdaConvertible<Lambda, U, const T&>)
									target.TransferTo(MoveRValue(lambda(Value)), Index);
								else
									targer.TransferTo(MoveRValue(lambda(Value, Index)), Index);
								//Інкремет індекса елемента
								Index++;
							}
						}
						else{
							//Виклик з значенням елемента і його індексом в старій колекції
							if (filter(Value, OldCollectionIndex)) {
								//Провірка типу лямбди перетворення і виклик з занесення її результату в нову колекцію
								if constexpr (IsLambdaConvertible<Lambda, U, const T&>)
									target.TransferTo(MoveRValue(lambda(Value)), Index);
								else
									targer.TransferTo(MoveRValue(lambda(Value, Index)), Index);
								//Інкремет індекса елемента
								Index++;
							}
						}
					};
					//Виклик проміжної лямбди для кожного елемента старої колекції
					this->ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&>(loopLambda));
					//Повернення результуючої колекції
					return target;
				}

				///<summary>Копіює елементи які задовільняють умову в нову колекцію</summary>
				///<param name = "lambda">Лямбда вираз перетворення</param>
				///<param name = "filter">Лямбда вираз фільтрування</param>
				///<param name = "U">Тип елементів нової колекціп</param>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<param name = "Lambda">Тип класу лямбди перетворення</param>
				///<param name = "FilterLambda">Тип класу лямбди фільтрування</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename U> typename NewContainer,
					typename Lambda,
					typename FilterLambda,
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, U, const T&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, U, const T&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, bool, const T&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<U>>::State>,
					bool 
					> = true
				>
				NewContainer<U> CopyToIf(Lambda lambda, FilterLambda filter) const {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					using TargetIQueryableType = IQueryable<NewContainer<U>, U>;
					//Створюєм об'єкт нової колекції
					NewContainer<U> target;
					//Інтерфейс IQueryable об'єкта нової колекції
					TargetIQueryableType& targetIQueryable = target;
					//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
					if (!TargetType::IsDynamicSize)
						targetIQueryable.PrepareCopyToOrMove(this->GetCountIf(filter));
					//Змінна для індексування номерів
					SizeT Index = 0;
					//Проміжний лямбда вираз який викличеться для кожного елемента старої колекції
					auto loopLambda = [&](const T& Value, SizeT OldCollectionIndex) {
						//Провірка типу фільтруючої лямбди
						if constexpr (IsLambdaConvertible<FilterLambda, U, const T&>) {
							//Виклик з значення елемента
							if (filter(Value)) {
								if constexpr (IsLambdaConvertible<Lambda, U, const T&>)
									target.TransferTo(MoveRValue(lambda(Value)), Index);
								else
									targer.TransferTo(MoveRValue(lambda(Value, Index)), Index);
								//Інкремет індекса елемента
								Index++;
							}
						}
						else {
							//Виклик з значенням елемента і його індексом в старій колекції
							if (filter(Value, OldCollectionIndex)) {
								//Провірка типу лямбди перетворення і виклик з занесення її результату в нову колекцію
								if constexpr (IsLambdaConvertible<Lambda, U, const T&>)
									target.TransferTo(MoveRValue(lambda(Value)), Index);
								else
									targer.TransferTo(MoveRValue(lambda(Value, Index)), Index);
								//Інкремет індекса елемента
								Index++;
							}
						}
					};
					//Виклик проміжної лямбди для кожного елемента старої колекції
					this->ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&>(loopLambda));
					//Повернення результуючої колекції
					return target;
				}

				///<summary>Переміщує всі елементи в нову колекцію. Стара колекція стає пустою</summary> 
				///<param name = "lambda">Лямбда вираз перетворення</param>
				///<param name = "filter">Лямбда вираз фільтрування</param>
				///<param name = "U">Тип елементів нової колекціп</param>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<param name = "Lambda">Тип класу лямбди перетворення</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename U> typename NewContainer,
					typename Lambda,
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, U, T&&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, U, T&&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<U>>::State>,
					bool
					> = true
				>
				NewContainer<U> MoveTo(Lambda lambda) {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					if constexpr (IsLambdaConvertible<Lambda, U, T&&>::State) {
						using TargetIQueryableType = IQueryable<NewContainer<U>, U>;
						//Створення об'єкта нової колекції
						NewContainer<U> target;
						//Отрмання інтерфейса IQueryable нової колекції
						TargetIQueryableType& targetIQueryable = target;
						//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
						if (!TargetType::IsDynamicSize)
							targetIQueryable.PrepareCopyToOrMove(this->GetCount());
						//Створення проміжної лямбди
						auto loopLambda = [&](T&& Value, SizeT Index) -> void {
							//Переміщення елемента в нову колекцію
							targetIQueryable.TransferTo(MoveRValue(lambda(MoveRValue(Value))), Index);
						};
						//Видалення всіх елементів з викликом для кожного елемента переміщення в нову колекцію
						RemoveAllImpl(LambdaHolderImpl<decltype(loopLambda), void, T&&, SizeT>(MoveRValue(loopLambda)));
						//Повернення нової колекції
						return target;
					}
					//Провірка чи лямбда вираз перетворення потребує індекса елемента
					else {
						using TargetIQueryableType = IQueryable<NewContainer<U>, U>;
						//Створення об'єкта нової колекції
						NewContainer<U> target;
						//Отрмання інтерфейса IQueryable нової колекції
						TargetIQueryableType& targetIQueryable = target;
						//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
						if (!TargetType::IsDynamicSize)
							targetIQueryable.PrepareCopyToOrMove(this->GetCountIf(filter));
						//Створення проміжної лямбди
						auto loopLambda = [&](T&& Value, SizeT Index) mutable {
							//Переміщення елемента в нову колекцію
							targetIQueryable.TransferTo(MoveRValue(lambda(MoveRValue(Value), Index)), Index);
						};
						//Видалення всіх елементів з викликом для кожного елемента переміщення в нову колекцію
						this->RemoveAllImpl(LambdaHolder<decltype(loopLambda), void, T&&, SizeT>(loopLambda));
						//Повернення нової колекції
						return target;
					}
				}

				///<summary>Переміщує елементи які задовільняють умову в нову колекцію. Стара колекція стає пустою</summary> 
				///<param name = "lambda">Лямбда вираз перетворення</param>
				///<param name = "filter">Лямбда вираз фільтрування</param>
				///<param name = "U">Тип елементів нової колекціп</param>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<param name = "Lambda">Тип класу лямбди перетворення</param>
				///<param name = "FilterLambda">Тип класу лямбди фільтрування</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename U> typename NewContainer,
					typename Lambda,
					typename FilterLambda,
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, U, T&&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, U, T&&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, bool, const T&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<U>>::State>,
					bool
					> = true
				>
				NewContainer<U> MoveToIf(Lambda lambda, FilterLambda filter) {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					using TargetIQueryableType = IQueryable<NewContainer<U>, U>;
					//Створення об'єкта нової колекції
					NewContainer<U> target;
					//Отрмання інтерфейса IQueryable нової колекції
					TargetIQueryableType& targetIQueryable = target;
					//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
					if (!TargetType::IsDynamicSize)
						target.PrepareCopyToOrMove(this->GetCountIf(filter));
					//Змінна для індексування номерів
					SizeT Index = 0;
					//Проміжна лямбда для переміщення елмементів
					auto loopLambda = [&](T&& Value, SizeT OldCollectionIndex) {
						//Провірка типу фільтруючої лямбди
						if constexpr (IsLambdaConvertible<FilterLambda, U, const T&>) {
							//Виклик з значення елемента
							if (filter(Value)) {
								if constexpr (IsLambdaConvertible<Lambda, void, T&&>)
									target.TransferTo(MoveRValue(lambda(MoveRValue(Value))), Index);
								else
									targer.TransferTo(MoveRValue(lambda(MoveRValue(Value), Index)), Index);
								//Інкремет індекса елемента
								Index++;
							}
						}
						//Виклик з значенням елемента і його індексом в старій колекції
						else {
							if (filter(Value, OldCollectionIndex)) {
								if constexpr (IsLambdaConvertible<Lambda, void, T&&>)
									target.TransferTo(MoveRValue(lambda(MoveRValue(Value))), Index);
								else
									targer.TransferTo(MoveRValue(lambda(MoveRValue(Value), Index)), Index);
								//Інкремет індекса елемента
								Index++;
							}
						}
					};
					//Виклик проміжної лямбди для кожного елемента старої колекції
					this->RemoveAllImpl(LambdaHolder<decltype(loopLambda), void, T&&>(loopLambda));
					//Повернення нової колекції
					return target;
				}

				///<summary>Копіює всі елементи в нову колекцію</summary>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename T> typename NewContainer,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<T>>::State>,
					bool
					> = true
				>
					NewContainer<T> CopyTo() {
					using namespace TypeTraits;
					using TargetType = NewContainer<T>;
					using TargetIQueryableType = IQueryable<NewContainer<T>, T>;
					//Створюєм об'єкт нової колекції
					NewContainer<T> target = NewContainer<T>();
					//Інтерфейс IQueryable об'єкта нової колекції
					TargetIQueryableType& targetIQueryable = static_cast<TargetIQueryableType&>(target);
					//Інтерфейс IQueryable об'єкт поточної колекції
					IQueryable<Container, T>& This = *this;
					//Отримуєм кількість елементів поточної колекції
					SizeT count = This.GetCount();
					//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
					if (!TargetType::IsDynamicSize)
						targetIQueryable.PrepareCopyToOrMove(this->GetCount());
					//Створення проміжного лямбда виразу який отримуватиме в циклі елементи старої колекції і копіюватиму їх в ному викликаючи лямба вираз перетворення 
					auto loopLambda = [&](const T& Value, SizeT Index) mutable {
						T t(Value);
						targetIQueryable.TransferTo(MoveRValue(t), Index);
					};
					//Виклик обходу старої колекції з лямбда виразом
					ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&, SizeT>(MoveRValue(loopLambda)));
					//Повернення нової колекції
					return target;
				}

				///<summary>Переміщує всі елементи в нову колекцію. Стара колекція стає пустою</summary> 
				///<param name = "lambda">Лямбда вираз перетворення</param>
				///<param name = "filter">Лямбда вираз фільтрування</param>
				///<param name = "U">Тип елементів нової колекціп</param>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<param name = "Lambda">Тип класу лямбди перетворення</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename U> typename NewContainer,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<T>>::State>,
					bool
					> = true
				>
					NewContainer<T> MoveTo() {
					using namespace TypeTraits;
					using TargetType = NewContainer<T>;
					using TargetIQueryableType = IQueryable<NewContainer<T>, T>;
					//Створюєм об'єкт нової колекції
					NewContainer<U> target = NewContainer<T>();
					//Інтерфейс IQueryable об'єкта нової колекції
					TargetIQueryableType& targetIQueryable = static_cast<TargetIQueryableType&>(target);
					//Інтерфейс IQueryable об'єкт поточної колекції
					IQueryable<Container, T>& This = *this;
					//Отримуєм кількість елементів поточної колекції
					SizeT count = This.GetCount();
					//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
					if (!TargetType::IsDynamicSize)
						targetIQueryable.PrepareCopyToOrMove(this->GetCount());
					//Створення проміжного лямбда виразу який отримуватиме в циклі елементи старої колекції і копіюватиму їх в ному викликаючи лямба вираз перетворення 
					auto loopLambda = [&](T&& Value, SizeT Index) mutable {
						targetIQueryable.TransferTo(MoveRValue(Value), Index);
					};
					//Виклик обходу старої колекції з лямбда виразом
					ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&, SizeT>(MoveRValue(loopLambda)));
					//Повернення нової колекції
					return target;
				}

				///<summary>Переміщує елементи які задовільняють умову в нову колекцію. Стара колекція стає пустою</summary> 
				///<param name = "lambda">Лямбда вираз перетворення</param>
				///<param name = "filter">Лямбда вираз фільтрування</param>
				///<param name = "U">Тип елементів нової колекціп</param>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<param name = "Lambda">Тип класу лямбди перетворення</param>
				///<param name = "FilterLambda">Тип класу лямбди фільтрування</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename U> typename NewContainer,
					typename FilterLambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<FilterLambda, bool, const T&>::State ||
					TypeTraits::IsLambdaConvertible<FilterLambda, bool, const T&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<T>>::State>,
					bool
					> = true
				>
					NewContainer<T> CopyToIf(FilterLambda filter) {
					using namespace TypeTraits;
					using TargetType = NewContainer<T>;
					using TargetIQueryableType = IQueryable<NewContainer<T>, T>;
					//Створення об'єкта нової колекції
					NewContainer<T> target;
					//Отрмання інтерфейса IQueryable нової колекції
					TargetIQueryableType& targetIQueryable = target;
					//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
					if (!TargetType::IsDynamicSize)
						target.PrepareCopyToOrMove(this->GetCountIf(filter));
					//Змінна для індексування номерів
					SizeT Index = 0;
					//Проміжна лямбда для переміщення елмементів
					auto loopLambda = [&](T&& Value, SizeT OldCollectionIndex) {
						//Провірка типу фільтруючої лямбди
						if constexpr (IsLambdaConvertible<FilterLambda, U, const T&>) {
							//Виклик з значення елемента
							if (filter(Value)) {
								T t(Value);
								targer.TransferTo(MoveRValue(t), Index);
								//Інкремет індекса елемента
								Index++;
							}
						}
						//Виклик з значенням елемента і його індексом в старій колекції
						else {
							if (filter(Value, OldCollectionIndex)) {
								T t(Value);
								targer.TransferTo(MoveRValue(t), Index);
								//Інкремет індекса елемента
								Index++;
							}
						}
					};
					//Виклик проміжної лямбди для кожного елемента старої колекції
					this->RemoveAllImpl(LambdaHolder<decltype(loopLambda), void, T&&>(loopLambda));
					//Повернення нової колекції
					return target;
				}

				///<summary>Переміщує елементи які задовільняють умову в нову колекцію. Стара колекція стає пустою</summary> 
				///<param name = "lambda">Лямбда вираз перетворення</param>
				///<param name = "filter">Лямбда вираз фільтрування</param>
				///<param name = "U">Тип елементів нової колекціп</param>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<param name = "Lambda">Тип класу лямбди перетворення</param>
				///<param name = "FilterLambda">Тип класу лямбди фільтрування</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename U> typename NewContainer,
					typename FilterLambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<FilterLambda, bool, const T&>::State ||
					TypeTraits::IsLambdaConvertible<FilterLambda, bool, const T&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<T>>::State>,
					bool
					> = true
				>
					NewContainer<T> MoveToIf(FilterLambda filter) {
					using namespace TypeTraits;
					using TargetType = NewContainer<T>;
					using TargetIQueryableType = IQueryable<NewContainer<T>, T>;
					//Створення об'єкта нової колекції
					NewContainer<T> target;
					//Отрмання інтерфейса IQueryable нової колекції
					TargetIQueryableType& targetIQueryable = target;
					//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
					if (!TargetType::IsDynamicSize)
						target.PrepareCopyToOrMove(this->GetCountIf(filter));
					//Змінна для індексування номерів
					SizeT Index = 0;
					//Проміжна лямбда для переміщення елмементів
					auto loopLambda = [&](T&& Value, SizeT OldCollectionIndex) {
						//Провірка типу фільтруючої лямбди
						if constexpr (IsLambdaConvertible<FilterLambda, U, const T&>) {
							//Виклик з значення елемента
							if (filter(Value)) {
								targer.TransferTo(MoveRValue(Value), Index);
								//Інкремет індекса елемента
								Index++;
							}
						}
						//Виклик з значенням елемента і його індексом в старій колекції
						else {
							if (filter(Value, OldCollectionIndex)) {
								T t(Value);
								targer.TransferTo(MoveRValue(Value), Index);
								//Інкремет індекса елемента
								Index++;
							}
						}
					};
					//Виклик проміжної лямбди для кожного елемента старої колекції
					this->RemoveAllImpl(LambdaHolder<decltype(loopLambda), void, T&&>(loopLambda));
					//Повернення нової колекції
					return target;
				}
#pragma endregion
#pragma region Copy and Move to IAssociable
				///<summary>Переміщує елементи які задовільняють умову в нову колекцію. Стара колекція стає пустою</summary> 
				///<param name = "lambda">Лямбда вираз перетворення</param>
				///<param name = "filter">Лямбда вираз фільтрування</param>
				///<param name = "U">Тип елементів нової колекціп</param>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<param name = "Lambda">Тип класу лямбди перетворення</param>
				///<param name = "FilterLambda">Тип класу лямбди фільтрування</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename TKey, typename TValue> typename NewContainer,
					typename Lambda,
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
					typename TKey = typename TypeTraits::PairTypes<U>::Key,
					typename TValue = typename TypeTraits::PairTypes<U>::Value,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::PairTypes<U>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, U, const T&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, U, const T&, SizeT>::State ||
					TypeTraits::IsBasedOn<IAssociableBase, NewContainer<TKey, TValue>>::State >,
					bool
					> = true
				>
					auto CopyToAssociable(Lambda lambda) -> NewContainer<TKey, TValue> {
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, U, const T&>::State) {
						NewContainer<TKey, TValue> newContainer;
						IAssociable<NewContainer<TKey, TValue>, U, TKey, TValue>& newContainerIAssociable = newContainer;
						auto loopLambda = [&](const T& Item) { newContainerIAssociable.TransferToAsPair(MoveRValue(lambda(Item))); };
						this->ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&>(MoveRValue(loopLambda)));
						return newContainer;
					}
					else {
						NewContainer<TKey, TValue> newContainer;
						IAssociable<NewContainer<TKey, TValue>, U, TKey, TValue>& newContainerIAssociable = newContainer;
						auto loopLambda = [&](const T& Item, SizeT Index) { newContainerIAssociable.TransferToAsPair(MoveRValue(lambda(Item, Index))); };
						this->ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&, SizeT>(MoveRValue(loopLambda)));
						return newContainer;
					}
					return NewContainer<TKey, TValue>();
				}
#pragma endregion
#pragma region Miscenalious
				///<summary>Підраховує кількість елементів які підходять по умові</summary>
				///<param name = "lambda">Лямбда вираз фільтр який вказує які елемент враховувати</param>
				///<param name = "Lambda">Тип лямбда виразу</param>
				///<returns>Кількість елементів</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, bool, const T&, SizeT>::State>,
					bool
					> = true
				>
				SizeT GetCountIf(Lambda filter) {
					//Провірка чи лямбда вираз по аргумента підходить
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State) {
						//Ініціалізовуєм лічильник
						SizeT Count = 0;
						//Проміжна лямбда для підрахунку
						auto lambda = [&](const T& Value) {
							//якщо фільтруюча лямбда вертає true то інкрементем лічильник який захоплений ззовні котекстом
							if (filter(Value))
								Count++;
						};
						//Викликаєм проміжну лямбду для кожного елемента
						this->ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&>(MoveRValue(lambda)));
						//Вертаєм інкрементоване значення так як кількість рахувалась від 0
						return ++Count;
					}
					//Якщо лямбда приймає ще й номер елемента то відповідно будем і його передавати
					else if constexpr (TypeTraits::IsLambdaConvertible<Lambda, bool, const T&, SizeT>::State) {
						SizeT Count = 0;
						//Ініціалізовуєм лічильник
						auto lambda = [&](const T& Value, SizeT Index) {
							//якщо фільтруюча лямбда вертає true то інкрементем лічильник який захоплений ззовні котекстом
							if (filter(Value, Index))
								Count++;
						};
						//Викликаєм проміжну лямбду для кожного елемента
						this->ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&, SizeT>(MoveRValue(lambda)));
						//Вертаєм інкрементоване значення так як кількість рахувалась від 0
						return ++Count;
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::GetCountIf const specific");
					}
				}

				///<summary>Підраховує кількість елементів які підходять по умові</summary>
				///<param name = "lambda">Лямбда вираз фільтр який вказує які елемент враховувати</param>
				///<param name = "Lambda">Тип лямбда виразу</param>
				///<returns>Кількість елементів</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, bool, const T&, SizeT>::State>,
					bool
					> = true
				>
				SizeT GetCountIf(Lambda filter) const {
					//Провірка чи лямбда вираз по аргумента підходить
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>) {
						//Ініціалізовуєм лічильник
						SizeT Count = 0;
						//Проміжна лямбда для підрахунку
						auto lambda = [&](const T& Value) {
							//якщо фільтруюча лямбда вертає true то інкрементем лічильник який захоплений ззовні котекстом
							if (filter(Value))
								Count++;
						};
						//Викликаєм проміжну лямбду для кожного елемента
						this->ForEachImpl(LambdaHolderImpl<decltype(filter), void, const T&>(filter));
						//Вертаєм інкрементоване значення так як кількість рахувалась від 0
						return ++Count;
					}
					//Якщо лямбда приймає ще й номер елемента то відповідно будем і його передавати
					else if constexpr (TypeTraits::IsLambdaConvertible<Lambda, bool, const T&, SizeT>) {
						SizeT Count = 0;
						//Ініціалізовуєм лічильник
						auto lambda = [&](const T& Value, SizeT Index) {
							//якщо фільтруюча лямбда вертає true то інкрементем лічильник який захоплений ззовні котекстом
							if (filter(Value, Index))
								Count++;
						};
						//Викликаєм проміжну лямбду для кожного елемента
						this->ForEachImpl(LambdaHolderImpl<decltype(filter), void, const T&, SizeT>(filter));
						//Вертаєм інкрементоване значення так як кількість рахувалась від 0
						return ++Count;
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::GetCountIf const specific");
					}
				}

				//TODO: Implement IsContaine
				///<summary>Вертає true якщо такий ітем є</summary>
				//bool IsContaine(const T& Item) = 0;

				///<summary>Видалія всі елементи з колекції. Але перед видаленням викликає лямбду для кожного</summary>
				///<param name = "lambda">Лямбда вираз який отрмає кожен елемент перед видаленням</param>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, void, T&&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, void, T&&, SizeT>::State>,
					bool
					> = true
				>
				Container& RemoveAllWith(Lambda lambda) {
					//Провірка аргументів лямбди 
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, void, T&&>::State) {
						//Виклик імплементації
						return this->RemoveAllImpl(LambdaHolderImpl<Lambda, void, T&&>(MoveRValue(lambda)));
					}
					//Якщо потрібно ще індекс передавати
					else if constexpr (TypeTraits::IsLambdaConvertible<Lambda, void, T&&, SizeT>::State) {
						//Виклик імплементації
						return this->RemoveAllImpl(LambdaHolderImpl<Lambda, void, T&&, SizeT>(MoveRValue(lambda)));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::RemoveAll specific");
					}
				}
#pragma endregion
				///<summary>Видаляє всі елементи з колекції</summary>
				virtual Container& RemoveAll() = 0;
			public:
				///Вертає кількість елементів в колекції</summary>
				virtual SizeT GetCount() const = 0;
			public:
				///<summary>Вертає true якщо колекція підтримує зміну розміру</summary>
				static inline constexpr Boolean IsResizable = TypeTraits::TIsResizable<Container>::State;
				///<summary>Вертає true якщо колекція динамічно змінює розмір</summary>
				static inline constexpr Boolean IsDynamicSize = TypeTraits::TIsDynamicSize<Container>::State;
			public:
				//Віртуальний деструктор
				virtual ~IQueryable() = 0 {}
			private:
				//Всі IQueryable бачуть один одного і завдяки цьому можна робити трансфер об'єктів
				template<typename OtherT, typename OtherEnumerable>
				friend class IQueryable;
				//Всі IAssociable бачуть один одного і завдяки цьому можна робити трансфер об'єктів до асоціативних колекцій
				template<typename Container, typename Pair, typename TKey, typename TValue>
				friend class IAssociable;
			};

			///<summary>Інтерфейс впорядкованої обробки елементів</summary>
			///<param name = "Container">Колекція для якої інтерфейс імплементовується</param>
			///<param name = "T">Тип елементів</param>
			template<typename Container, typename T>
			class SeedKernelCall IOrdeable : public IQueryable<Container, T>
			{
			protected:
				///<summary>Реалізація сортування</summary>
				///<param name = "lambda">Лямбда вираз порівняння елементів</param>
				///<param name = "IsAscendingOrder">Якщо true то сортування в порядку спадання</param>
				///<returns>Силка на колекцію</returns>
				virtual Container& SortImpl(LambdaHolder<bool, const T&, const T&>& lambda, bool IsAscendingOrder) = 0;
				///<summary>Реалізація наповнення колекції елементами</summary>
				///<param name = "lambda">Лямбда вираз який вертає значення елементів</param>
				///<param name = "Count">Кількість елементів які будуть створенні</param>
				///<returns>Силка на колекцію</returns>
				virtual void FillImpl(LambdaHolder<T>& lambda, SizeT Count) = 0;
				///<summary>Реалізація наповнення колекції елементами</summary>
				///<param name = "lambda">Лямбда вираз який вертає значення елементів</param>
				///<param name = "Count">Кількість елементів які будуть створенні</param>
				///<returns>Силка на колекцію</returns>
				virtual void FillImpl(LambdaHolder<T, SizeT>& lambda, SizeT Count) = 0;
			public:
				///<summary>Виставить всі елементи в зворотньому порядку</summary>
				///<returns>Силка на колекцію</returns>
				virtual Container& Reverse() = 0;
				///<summary>Відсортує всі елементи</summary>
				///<param name = "IsAscendingOrder">Якщо true то сортування в порядку спадання</param>
				///<returns>Силка на колекцію</returns>
				Container& Sort(bool IsAscendingOrder = true) {
					auto lambda = [](const T& A, const T& B) -> bool {return A >= B; };
					return SortImpl(LambdaHolderImpl<decltype(lambda), bool, const T&, const T&>(lambda), IsAscendingOrder);
				}
				///<summary>Відсортує всі елементи використовуючи лямбду як компаратор</summary>
				///<param name = "lambda">Лямбда вираз порівняння елементів</param>
				///<param name = "IsAscendingOrder">Якщо true то сортування в порядку спадання</param>
				///<returns>Силка на колекцію</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, bool, const T&, const T&>::State>,
					bool
				> = true>
				Container& Sort(Lambda lambda, bool IsAscendingOrder = true) {
					//Провірка чи підходить лямбда вираз по аргументам і результату 
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, bool, const T&, const T&>) {
						//якщо так то викликаєм реалізацію
						return SortImpl(LambdaHolderImpl<Lambda, bool, const T&, const T&>(lambda), IsAscendingOrder);
					}
					else {
						static_assert(false, "Invalid lambda in IOrdeable::Sort specific");
					}
				}
				///<summary>Заповнить Count елментів результатом Lambda функції</summary>
				///<param name = "lambda">Лямбда вираз який вертає значення елементів</param>
				///<param name = "Count">Кількість елементів які будуть створенні</param>
				///<returns>Силка на колекцію</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, T>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, T, SizeT>::State>,
					bool
					> = true
				>
				static Container Fill(Lambda lambda, SizeT Count) {
					//Провірка аргументів і результату лямбда виразу
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, T>::State == true) {
						//Створення колекції
						Container target;
						//Отримання інтерфейса IOrdeable цільового контейнера
						IOrdeable& targetIOrdeable = target;
						//Виклик імплементації
						targetIOrdeable.FillImpl(LambdaHolderImpl<Lambda, T>(MoveRValue(lambda)), Count);
						//Поверення нового об'єкта
						return target;
					}
					//Якщо потрібно передавати індекс елемента то передаєм і його
					else if constexpr (TypeTraits::IsLambdaConvertible<Lambda, T, SizeT>::State) {
						//Створення колекції
						Container target;
						//Отримання інтерфейса IOrdeable цільового контейнера
						IOrdeable& targetIOrdeable = target;
						//Виклик імплементації
						targetIOrdeable.FillImpl(LambdaHolderImpl<Lambda, T, SizeT>(MoveRValue(lambda)), Count);
						//Поверення нового об'єкта
						return target;
					}
					else {
						static_assert(false, "Invalid lambda in IOrdeable::Fill specific");
					}
				}
			public:
				//Деструктор
				virtual ~IOrdeable() = 0 {}
			private:
				//Всі IOrdeable бачуть один одного і завдяки цьому можна робити трансфер об'єктів
				template<typename OtherT, typename OtherEnumerable>
				friend class IOrdeable;
			};

			///<summary>Інтерфейс індексованих колекцій таких як масиви</summary>
			template<typename Container, typename T>
			class SeedKernelCall IIndexable : public IOrdeable<Container, T> {
			public:
				///<summary>Отримати колекцію яка міститиме всі елементи від елемента з номером StartIndex до останього включно</summary>
				///<param name = "StartIndex">Номер першого елемента який включається в нову колекцію. Індексація з нуля</param>
				///<returns>Нову колекцію з відповідними елементами</returns>
				virtual Container Slice(SizeT StartIndex) = 0;
				///<summary>Отримати колекцію яка міститиме Count елементів від елемента з номером StartIndex</summary>
				///<param name = "StartIndex">Номер першого елемента який включається в нову колекцію. Індексація з нуля</param>
				///<param name = "Count">Кількість елементів які потрібно скопіювати в нову колекцію</param>
				///<returns>Нову колекцію з відповідними елементами</returns>
				virtual Container Slice(SizeT StartIndex, SizeT Count) = 0;
				///<summary>Оператор доступу до елемента по номеру</summary>
				///<param name = "Index">Індекс елемента</param>
				///<returns>Силку на елемент в масиву</returns>
				virtual T& operator[](SizeT Index) = 0;
				///<summary>Константний оператор доступу до елемента по номеру</summary>
				///<param name = "Index">Індекс елемента</param>
				///<returns>Константна cилка на елемент в масиву</returns>
				virtual const T& operator[](SizeT Index) const = 0;
			public:
				///<summary>Деструктор</summary>
				virtual ~IIndexable() = 0 {}
			};

			///<summary>Інтерфейс перелічуваних колекцій таких як списки</summary>
			template<typename Container, typename T>
			class SeedKernelCall IEnumerable : public IOrdeable<Container, T> {
			protected:
				///<summary>Видаляє елементи які задовільняють умову лямбда виразу</summary>
				///<param name = "lambda">Лямбда вираз фільтр який вказує які елементи видалити</param>
				///<returns>Силка на колекцію</returns>
				virtual Container& RemoveIfImpl(LambdaHolder<bool, const T&>& lambda) = 0;
				///<summary>Видаляє елементи які задовільняють умову лямбда виразу</summary>
				///<param name = "lambda">Лямбда вираз фільтр який вказує які елементи видалити</param>
				///<returns>Силка на колекцію</returns>
				virtual Container& RemoveIfImpl(LambdaHolder<bool, const T&, SizeT>& lambda) = 0;
			public:
				///<summary>Видалить всі елементи які підпадають під умову<summary>
				///<param name = "lambda">Лямбда вираз фільтр який вказує які елементи видалити</param>
				///<param name = "Lambda">Тип лямбда виразу</param>
				///<returns>Силка на колекцію</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, bool, const T&, SizeT>::State>,
					bool
					> = true
				>
				Container& RemoveIf(Lambda lambda) {
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>) {
						return RemoveIfImpl(LambdaHolderImpl<Lambda, bool, const T&>(Lambda));
					}
					else if constexpr (TypeTraits::IsLambdaConvertible<Lambda, bool, const T&, SizeT>) {
						return RemoveIfImpl(LambdaHolderImpl<Lambda, bool, const T&, SizeT>(Lambda));
					}
					else {
						static_assert(false, "Invalid lambda in IEnumerble::RemoveIf specific");
					}
				}
			public:
				///<summary>Деструктор</summary>
				virtual ~IEnumerable() = 0 {}
			};

			template<typename Container, typename Pair, typename TKey, typename TValue>
			class SeedKernelCall IAssociable : public IAssociableBase {
			protected:
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& ForEachImpl(LambdaHolder<void, Pair&>& lambda) = 0;
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& ForEachImpl(LambdaHolder<void, const Pair&>& lambda) = 0;
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual const Container& ForEachImpl(LambdaHolder<void, const Pair&>& lambda) const = 0;
			protected:
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& ForEachKeyImpl(LambdaHolder<void, const TKey&>& lambda) = 0;
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual const Container& ForEachKeyImpl(LambdaHolder<void, const TKey&>& lambda) const = 0;
			protected:
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& ForEachValueImpl(LambdaHolder<void, TValue&>& lambda) = 0;
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& ForEachValueImpl(LambdaHolder<void, const TValue&>& lambda) = 0;
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual const Container& ForEachValueImpl(LambdaHolder<void, const TValue&>& lambda) const = 0;
			protected:
				///<summary>Функція перенесення значення в асоціативний масив як пари ключ <-> значення</summary>
				///<param name = "pair">Пара ключ <-> значення</param>
				virtual void TransferToAsPair(Pair&& pair) = 0;
			protected:
				///<summary>Реалізація знищенння всіх елементів з викликом лямбди</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& RemoveAllWithImpl(LambdaHolder<void, TKey &&, TValue &&>& lambda) = 0;
				///<summary>Реалізація знищенння всіх елементів з викликом лямбди</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& RemoveAllWithValueImpl(LambdaHolder<void, TValue &&>& lambda) = 0;
				///<summary>Реалізація знищенння всіх елементів з викликом лямбди</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Силку на поточний контейнер</returns>
				virtual Container& RemoveAllWithKeyImpl(LambdaHolder<void, TKey &&>& lambda) = 0;
			public:
				virtual ~IAssociable() = 0 {}
			public:
				///<summary>Копіює всі елементи в нову колекцію</summary>
				///<param name = "lambda">Лямбда вираз яка описує правило перетворення елемента</param>
				///<param name = "U">Тип елементів нової колекціп</param>
				///<param name = "NewContainer">Тип нової колекції</param>
				///<param name = "Lambda">Тип класу лямбди</param>
				///<returns>Нова колекція з перетвореними елементами</returns>
				template<
					template<typename U> typename NewContainer,
					typename Lambda,
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, U, const TKey&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, U, const TKey&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<U>>::State>,
					bool
					> = true
				>
					NewContainer<U> CopyKeysTo(Lambda lambda) const {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					using TargetIQueryableType = IQueryable<NewContainer<U>, U>;
					//Створюєм об'єкт нової колекції
					NewContainer<U> target = NewContainer<U>();
					//Інтерфейс IQueryable об'єкта нової колекції
					TargetIQueryableType& targetIQueryable = static_cast<TargetIQueryableType&>(target);
					//Інтерфейс IQueryable об'єкт поточної колекції
					const IQueryable<Container, T>& This = *this;
					//Отримуєм кількість елементів поточної колекції
					SizeT count = This.GetCount();
					//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
					if (!TargetType::IsDynamicSize)
						targetIQueryable.PrepareCopyToOrMove(this->GetCount());
					//Провірка чи лямбда вирах перетворення приймає індекс елемента
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, U, const TKey&>::State) {
						//Створення проміжного лямбда виразу який отримуватиме в циклі елементи старої колекції і копіюватиму їх в ному викликаючи лямба вираз перетворення 
						auto loopLambda = [&](const T& Value, SizeT Index) mutable {
							targetIQueryable.TransferTo(MoveRValue(lambda(Value)), Index);
						};
						//Виклик обходу старої колекції з лямбда виразом
						ForEachKeyImpl(LambdaHolderImpl<decltype(loopLambda), void, const TKey&, SizeT>(MoveRValue(loopLambda)));
					}
					else {
						//Створення проміжного лямбда виразу який отримуватиме в циклі елементи старої колекції і копіюватиму їх в ному викликаючи лямба вираз перетворення 
						auto loopLambda = [&](const TKey& Value, SizeT Index) mutable {
							targetIQueryable.TransferTo(MoveRValue(lambda(Value, Index)), Index);
						};
						//Виклик обходу старої колекції з лямбда виразом
						ForEachKeyImpl(LambdaHolderImpl<decltype(loopLambda), void, const TKey&, SizeT>(MoveRValue(loopLambda)));
					}
					//Повернення нової колекції
					return target;
				}

				template<
					template<typename U> typename NewContainer,
					typename Lambda,
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, U, const TValue&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, U, const TValue&, SizeT>::State>,
					bool
					> = true,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsBasedOn<IQueryableBase, NewContainer<U>>::State>,
					bool
					> = true
				>
					NewContainer<U> CopyValueTo(Lambda lambda) const {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					using TargetIQueryableType = IQueryable<NewContainer<U>, U>;
					//Створюєм об'єкт нової колекції
					NewContainer<U> target = NewContainer<U>();
					//Інтерфейс IQueryable об'єкта нової колекції
					TargetIQueryableType& targetIQueryable = static_cast<TargetIQueryableType&>(target);
					//Інтерфейс IQueryable об'єкт поточної колекції
					const IQueryable<Container, TValueT>& This = *this;
					//Отримуєм кількість елементів поточної колекції
					SizeT count = This.GetCount();
					//Якщо колекції не підтримує гарячу зміну розміру то інформує її про кількість елементів
					if (!TargetType::IsDynamicSize)
						targetIQueryable.PrepareCopyToOrMove(this->GetCount());
					//Провірка чи лямбда вирах перетворення приймає індекс елемента
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, U, const TValue&>::State) {
						//Створення проміжного лямбда виразу який отримуватиме в циклі елементи старої колекції і копіюватиму їх в ному викликаючи лямба вираз перетворення 
						auto loopLambda = [&](const TValue& Value, SizeT Index) mutable {
							targetIQueryable.TransferTo(MoveRValue(lambda(Value)), Index);
						};
						//Виклик обходу старої колекції з лямбда виразом
						ForEachKeyImpl(LambdaHolderImpl<decltype(loopLambda), void, const TValue&, SizeT>(MoveRValue(loopLambda)));
					}
					else {
						//Створення проміжного лямбда виразу який отримуватиме в циклі елементи старої колекції і копіюватиму їх в ному викликаючи лямба вираз перетворення 
						auto loopLambda = [&](const TValue& Value, SizeT Index) mutable {
							targetIQueryable.TransferTo(MoveRValue(lambda(Value, Index)), Index);
						};
						//Виклик обходу старої колекції з лямбда виразом
						ForEachKeyImpl(LambdaHolderImpl<decltype(loopLambda), void, const TValue&, SizeT>(MoveRValue(loopLambda)));
					}
					//Повернення нової колекції
					return target;
				}
#pragma region ForEach

				///<summary>Виконує дію для кожного елемента</summary>
				///<param name = "lambda">Лямбда вираз який виконається для кожного елемента</param>
				///<param name = "Lambda">Тип класу лямбди</param>
				///<returns>Силку на поточний контейнер</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, void, Pair&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, void, const Pair&>::State>,
					bool
					> = true
				>
					Container & ForEach(Lambda lambda) {
					using namespace TypeTraits;

					if constexpr (IsLambdaConvertible<Lambda, void, Pair&>::State) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, Pair&>(MoveRValue(lambda)));
					}
					else if constexpr (IsLambdaConvertible<Lambda, void, const Pair&>::State) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const Pair&>(MoveRValue(lambda)));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::ForEach specific");
					}
				}
				///<summary>Виконує дію для кожного елемента в константній колекції</summary>
				///<param name = "lambda">Лямбда вираз який виконається для кожного елемента</param>
				///<param name = "Lambda">Тип класу лямбди</param>
				///<returns>Константна силка на поточний контейнер</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, void, const Pair&>::State>,
					bool
					> = true
				>
					const Container & ForEach(Lambda lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, void, const Pair&>::State) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const Pair&>(MoveRValue(lambda)));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::ForEach const specific");
					}
				}

				//ForEach keys

				///<summary>Виконує дію для кожного елемента</summary>
				///<param name = "lambda">Лямбда вираз який виконається для кожного елемента</param>
				///<param name = "Lambda">Тип класу лямбди</param>
				///<returns>Силку на поточний контейнер</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, void, TKey&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, void, const TKey&>::State>,
					bool
					> = true
				>
					Container & ForEachKey(Lambda lambda) {
					using namespace TypeTraits;

					if constexpr (IsLambdaConvertible<Lambda, void, const TKey&>::State) {
						return ForEachKeyImpl(LambdaHolderImpl<decltype(lambda), void, const TKey&>(MoveRValue(lambda)));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::ForEach specific");
					}
				}
				///<summary>Виконує дію для кожного елемента в константній колекції</summary>
				///<param name = "lambda">Лямбда вираз який виконається для кожного елемента</param>
				///<param name = "Lambda">Тип класу лямбди</param>
				///<returns>Константна силка на поточний контейнер</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, void, const TKey&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, void, const TKey&, SizeT>::State>,
					bool
					> = true
				>
					const Container & ForEachKey(Lambda lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, void, const TKey&>) {
						return ForEachKeyImpl(LambdaHolderImpl<decltype(lambda), void, const TKey&>(lambda));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::ForEach const specific");
					}
				}

				//ForEachValue

				///<summary>Виконує дію для кожного елемента</summary>
				///<param name = "lambda">Лямбда вираз який виконається для кожного елемента</param>
				///<param name = "Lambda">Тип класу лямбди</param>
				///<returns>Силку на поточний контейнер</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, void, TValue&>::State ||
					TypeTraits::IsLambdaConvertible<Lambda, void, const TValue&>::State>,
					bool
					> = true
				>
					Container & ForEachValue(Lambda lambda) {
					using namespace TypeTraits;

					if constexpr (IsLambdaConvertible<Lambda, void, TValue&>::State) {
						return ForEachValueImpl(LambdaHolderImpl<decltype(lambda), void, TValue&>(MoveRValue(lambda)));
					}
					else if constexpr (IsLambdaConvertible<Lambda, void, const TValue&>::State) {
						return ForEachValueImpl(LambdaHolderImpl<decltype(lambda), void, const TValue&>(MoveRValue(lambda)));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::ForEach specific");
					}
				}
				///<summary>Виконує дію для кожного елемента в константній колекції</summary>
				///<param name = "lambda">Лямбда вираз який виконається для кожного елемента</param>
				///<param name = "Lambda">Тип класу лямбди</param>
				///<returns>Константна силка на поточний контейнер</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertible<Lambda, void, const TValue&>::State>,
					bool
					> = true
				>
					const Container & ForEachValue(Lambda lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const TValue&>) {
						return ForEachValueImpl(LambdaHolderImpl<decltype(lambda), void, const TValue&>(lambda));
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, const TValue&, SizeT>) {
						return ForEachValueImpl(LambdaHolderImpl<decltype(lambda), void, const TValue&, SizeT>(lambda));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::ForEach const specific");
					}
				}

#pragma endregion
#pragma region Miscenelious methods
				///<summary>Вертає чи є елемент з заданим ключом</summary>
				///<param name = "key">Шуканий ключ</param>
				virtual bool IsContaine(const TKey& key) const = 0;

				///<summary>Видаляє всі елементи</summary>
				virtual Container& RemoveAll() = 0;

				///<summary>Видаляє всі елементи. Перед видаленням викликає Lambda для кожного елемента</summary>
				///<param name = "lambda">Лямбда вираз який може приймати RValue силку на ключ і значення або на щось одне</param>
				///<returns>Силку на даний контейнер</returns>
				template<typename Lambda,
					TypeTraits::EnableIfTV<TypeTraits::LogicConst<
					TypeTraits::IsLambdaConvertibleV<Lambda, void, TValue&&, TKey&&> ||
					TypeTraits::IsLambdaConvertibleV<Lambda, void, TValue&&> ||
					TypeTraits::IsLambdaConvertibleV<Lambda, void, TKey&&>
					>, bool> = true>
				Container& RemoveAllWith(Lambda lambda) {
					if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, TValue&&, TKey&&>) {
						return RemoveAllWithImpl(LambdaHolderImpl<decltype(lambda), void, TValue&&, TKey&&>(MoveRValue(lambda)));
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<Lambda, void, TValue&&>) {
						return RemoveAllWithValueImpl(LambdaHolderImpl<decltype(lambda), void, TValue&&>(MoveRValue(lambda)));
					}
					else {
						return RemoveAllWithKeyImpl(LambdaHolderImpl<decltype(lambda), void,  TKey&&>(MoveRValue(lambda)));
					}
				}

#pragma endregion

				//Всі IQueryable бачуть один одного і завдяки цьому можна робити трансфер об'єктів
				template<typename OtherT, typename OtherEnumerable>
				friend class IQueryable;

				
			};
		}	
	}
}
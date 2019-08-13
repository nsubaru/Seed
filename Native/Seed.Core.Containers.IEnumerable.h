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
		}

		namespace Queryable {
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

			///<summary>Батьківський клас для всіх IQueryable</summary>
			class SeedKernelCall IQueryableBase {
			protected:
				virtual ~IQueryableBase() = 0 {};
				//Всі IQueryable бачать це й класс, це зроблено для провірки чи тип є IQueryable
				template<typename Container, typename T>
				friend class IQueryable;
			};
			///<summary>Інтерфейс функціональної обробки елементів колекцій</summary>
			///<param name = "Container">Колекція, яка використовує інтерфейс</param>
			///<param name = "T">Тип елементів в колекції</param>
			template<typename Container, typename T>
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
				///<summary>Імплементація обходу колекція</summary>
				///<param name = "lambda">Лямбда вираз який викличеться для кожного елемента</param>
				///<returns>Константа силка на поточний контейнер</returns>
			protected:
				///<summary>Підготовка кінцевого контейнера до копіювання\переміщення в нього елементів якщо він не уміє змінювати розмір динамічно</summary>
				virtual void PrepareCopyToOrMove(SizeT Count) = 0;
				///<summary>Переміщення елемента в новий контенер</summary>
				virtual void TransferTo(T&& Value, SizeT ItemIndex) = 0;
			protected:
				virtual void RemoveAllImpl(LambdaHolder<void, T&&>& lambda) = 0;
				virtual void RemoveAllImpl(LambdaHolder<void, T&&, SizeT>& lambda) = 0;
			protected:
				//virtual SizeT CountIfImpl(LambdaHolder<bool, const T&>& lambda) = 0;
			public:
				///<summary>Виконує дію для кожного елемента</summary>
				///<param name = "lambda">Лямбда вираз який виконається для кожного елемента, приймати має T&, верта void </param>
				///<returns>Силку на поточний контейнер</returns>
				template<typename Lambda>
				Container& ForEach(Lambda lambda) {
					using namespace TypeTraits;

					if constexpr (IsLambdaConvertible<Lambda, void, T&>::State) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, T&>(MoveRValue(lambda)));
					}
					else if constexpr (IsLambdaConvertible<Lambda, void, T&, SizeT>::State) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, T&, SizeT>(MoveRValue(lambda)));
					}
					else if constexpr (IsLambdaConvertible<Lambda, void, const T&>::State) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&, SizeT>(MoveRValue(lambda)));
					}
					else if constexpr (IsLambdaConvertible<Lambda, void, const T&, SizeT>::State) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&, SizeT>(MoveRValue(lambda)));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::ForEach specific");
					}
				}
				///<summary>Виконує дію для кожного елемента в константній колекції</summary>
				///<param name = "lambda">Лямбда вираз який виконається для кожного елемента, приймати має T&, верта void </param>
				///<returns>Константна силка на поточний контейнер</returns>
				template<typename Lambda>
				const Container& ForEach(Lambda lambda) const {
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, void, const T&>) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&, SizeT>(lambda));
					}
					else if constexpr (TypeTraits::IsLambdaConvertible<Lambda, void, const T&, SizeT>) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&, SizeT>(lambda));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::ForEach const specific");
					}
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
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType
				>
				NewContainer<U> CopyTo(Lambda lambda) {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					//Провірка чи нова колекція наслідується від IQueryableBase і відповідно імплементовує інтерфейс прийому елементів
					if constexpr (!IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::CopyTo const specific");
					}
					//Провірка чи колекція підтримує зміну розміру
					else if constexpr (!TargetType::IsResizable){
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					//Провірка чи лямбда вираз валідні аргументи приймає
					else if constexpr (Or<IsLambdaConvertible<Lambda, U, const T&>, IsLambdaConvertible<Lambda, U, const T&, SizeT>>) {
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
					else {
						static_assert(false, "Invalid lambda in IQueryable::CopyTo specific");
					}
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
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType
				>
				NewContainer<U> CopyTo(Lambda lambda) const {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					//Провірка чи нова колекція наслідується від IQueryableBase і відповідно імплементовує інтерфейс прийому елементів
					if constexpr (!IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::CopyTo const specific");
					}
					//Провірка чи колекція підтримує зміну розміру
					else if constexpr (!TargetType::IsResizable) {
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					//Провірка чи лямбда вираз валідні аргументи приймає
					else if constexpr (Or<IsLambdaConvertible<Lambda, U, const T&>, IsLambdaConvertible<Lambda, U, const T&, SizeT>>) {
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
					else {
						static_assert(false, "Invalid lambda in IQueryable::CopyTo specific");
					}
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
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType
				>
				NewContainer<U> CopyToIf(Lambda lambda, FilterLambda filter) {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					//Провірка чи нова колекція наслідується від IQueryableBase і відповідно імплементовує інтерфейс прийому елементів
					if constexpr (!IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::CopyTo const specific");
					}
					//Провірка чи колекція підтримує зміну розміру
					else if constexpr (!TargetType::IsResizable) {
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					//Провірка чи лямбда вираз фільтрування валідні аргументи приймає
					else if constexpr (!(Or<IsLambdaConvertible<FilterLambda, bool, const T&>, IsLambdaConvertible<FilterLambda, bool, const T&, SizeT>)) {
						static_assert(false, "Invalid filter lambda in IQueryable::CopyTo specific")
					}
					//Провірка чи лямбда вираз перетворення валідні аргументи приймає
					else if constexpr (Or<IsLambdaConvertible<Lambda, void, const T&>, IsLambdaConvertible<Lambda, void, const T&, SizeT>>) {
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
					else {
						static_assert(false, "Invalid lambda in IQueryable::CopyTo const specific");
					}
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
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType
				>
				NewContainer<U> CopyToIf(Lambda lambda, FilterLambda filter) const {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					//Провірка чи нова колекція наслідується від IQueryableBase і відповідно імплементовує інтерфейс прийому елементів
					if constexpr (!IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::CopyTo const specific");
					}
					//Провірка чи колекція підтримує зміну розміру
					else if constexpr (!TargetType::IsResizable) {
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					//Провірка чи лямбда вираз фільтрування валідні аргументи приймає
					else if constexpr (!(Or < IsLambdaConvertible<FilterLambda, bool, const T&>, IsLambdaConvertible<FilterLambda, bool, const T&, SizeT>)) {
						static_assert(false, "Invalid filter lambda in IQueryable::CopyTo specific")
					}
					//Провірка чи лямбда вираз перетворення валідні аргументи приймає
					else if constexpr (Or<IsLambdaConvertible<Lambda, void, const T&>, IsLambdaConvertible<Lambda, void, const T&, SizeT>>) {
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
					else {
						static_assert(false, "Invalid lambda in IQueryable::CopyTo const specific");
					}
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
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType
				>
				NewContainer<U> MoveTo(Lambda lambda) {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					//Провірка чи нова колекція наслідується від IQueryableBase і відповідно імплементовує інтерфейс прийому елементів
					if constexpr (!IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::MoveTo const specific");
					}
					//Провірка чи колекція підтримує зміну розміру
					else if constexpr (!TargetType::IsResizable) {
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					//Провірка чи лямбда вираз перетворення не потребує індекса елемента
					else if constexpr (IsLambdaConvertible<Lambda, U, T&&>::State) {
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
					else if constexpr(IsLambdaConvertible<Lambda, U, T&&, SizeT>::State){
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
					else {
						static_assert(false, "Invalid lambda in IQueryable::MoveTo const specific");
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
					typename U = TypeTraits::LambdaType<Lambda>::ReturnType
				>
				NewContainer<U> MoveToIf(Lambda lambda, FilterLambda filter) {
					using namespace TypeTraits;
					using TargetType = NewContainer<U>;
					//Провірка чи нова колекція наслідується від IQueryableBase і відповідно імплементовує інтерфейс прийому елементів
					if constexpr (!IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::MoveTo const specific");
					}
					//Провірка чи колекція підтримує зміну розміру
					else if constexpr (!TargetType::IsResizable) {
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					//Провірка чи лямбда вираз фільтрування валідні аргументи приймає
					else if constexpr (!(Or<IsLambdaConvertible<FilterLambda, bool, T&>, IsLambdaConvertible<FilterLambda, bool, T&, SizeT>>)) {
						static_assert(false, "Invalid filter lambda in IQueryable::MoveTo specific")
					}
					//Провірка чи лямбда вираз перетворення валідні аргументи приймає
					else if constexpr (Or<IsLambdaConvertible<Lambda, T, T&&>, IsLambdaConvertible<Lambda, T, T&&, SizeT>>) {
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
					else {
						static_assert(false, "Invalid lambda in IQueryable::MoveTo const specific");
					}
				}

				///<summary>Підраховує кількість елементів які підходять по умові</summary>
				///<param name = "lambda">Лямбда вираз фільтр який вказує які елемент враховувати</param>
				///<param name = "Lambda">Тип лямбда виразу</param>
				///<returns>Кількість елементів</returns>
				template<typename Lambda>
				SizeT GetCountIf(Lambda filter) {
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
						this->ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&>(lambda));
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
						this->ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&, SizeT>(lambda));
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
				template<typename Lambda>
				SizeT GetCountIf(Lambda lambda) const {
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
						this->ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&>(lambda));
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
						this->ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&, SizeT>(lambda));
						//Вертаєм інкрементоване значення так як кількість рахувалась від 0
						return ++Count;
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::GetCountIf const specific");
					}
				}

				///<summary>Видалія всі елементи з колекції. Але перед видаленням викликає лямбду для кожного</summary>
				///<param name = "lambda">Лямбда вираз який отрмає кожен елемент перед видаленням</param>
				template<typename Lambda>
				Container& RemoveAll(Lambda lambda) {
					//Провірка аргументів лямбди 
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, void, T&&>) {
						//Виклик імплементації
						return this->RemoveAllImpl(LambdaHolderImpl<Lambda, void, T&&>(lambda));
					}
					//Якщо потрібно ще індекс передавати
					else if constexpr (TypeTraits::IsLambdaConvertible<Lambda, void, T&&, SizeT>) {
						//Виклик імплементації
						return this->RemoveAllImpl(LambdaHolderImpl<Lambda, void, T&&, SizeT>(lambda));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::RemoveAll specific");
					}
				}

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
			};

			template<typename Container, typename T>
			class SeedKernelCall IOrdeable : public IQueryable<Container, T>
			{
			protected:
				virtual Container& SortImpl(LambdaHolder<bool, const T&, const T&>& lambda, bool IsAscendingOrder) = 0;
				virtual Container& FillImpl(LambdaHolder<T>& lambda, SizeT Count) = 0;
				virtual Container& FillImpl(LambdaHolder<T, SizeT>& lambda, SizeT Count) = 0;
			public:
				///<summary>Виставить всі елементи в зворотньому порядку</summary>
				virtual Container& Reverse() = 0;
				///<summary>Відсортує всі елементи</summary>
				virtual Container& Sort(bool IsAscendingOrder = true) = 0;
				///<summary>Відсортує всі елементи використовуючи лямбду як компаратор</summary>
				template<typename Lambda>
				Container& Sort(Lambda lambda, bool IsAscendingOrder = true) {
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, bool, const T&, const T&>) {
						return SortImpl(LambdaHolderImpl<Lambda, bool, const T&, const T&>(lambda), IsAscendingOrder);
					}
					else {
						static_assert(false, "Invalid lambda in IOrdeable::Sort specific");
					}
				}
				///<summary>Заповнить Count елментів результатом Lambda функції</summary>
				template<typename Lambda>
				static Container Fill(Lambda lambda, SizeT Count) {
					if constexpr (TypeTraits::IsLambdaConvertible<Lambda, T>::State == true) {
						Container target;
						IOrdeable& targetIOrdeable = target;
						targetIOrdeable.FillImpl(LambdaHolderImpl<Lambda, T>(MoveRValue(lambda)), Count);
						return target;
					}
					else if constexpr (TypeTraits::IsLambdaConvertible<Lambda, T, SizeT>::State) {
						Container target;
						IOrdeable& targetIOrdeable = target;
						targetIOrdeable.FillImpl(LambdaHolderImpl<Lambda, T, SizeT>(MoveRValue(lambda)), Count);
						return target;
					}
					else {
						static_assert(false, "Invalid lambda in IOrdeable::Fill specific");
					}
				}
			public:
				virtual ~IOrdeable() = 0 {}
			private:
				template<typename OtherT, typename OtherEnumerable>
				friend class IOrdeable;
			};

			template<typename Container, typename T>
			class SeedKernelCall IIndexable : public IOrdeable<Container, T> {
			public:
				///<summary>Отримати колекцію яка міститиме всі елементи від елемента з номером StartIndex до останього включно</summary>
				virtual Container Slice(SizeT StartIndex) = 0;
				///<summary>Отримати колекцію яка міститиме Count елементів від елемента з номером StartIndex</summary>
				virtual Container Slice(SizeT StartIndex, SizeT Count) = 0;
				///<summary>Оператор доступу до елемента по номеру</summary>
				virtual T& operator[](SizeT Index) = 0;
				///<summary>Константний оператор доступу до елемента по номеру</summary>
				virtual const T& operator[](SizeT Index) const = 0;
			};

			template<typename Container, typename T>
			class SeedKernelCall IEnumerable : public IOrdeable<Container, T> {
			protected:
				virtual Container& RemoveIfImpl(LambdaHolder<bool, const T&>& lambda) = 0;
				virtual Container& RemoveIfImpl(LambdaHolder<bool, const T&, SizeT>& lambda) = 0;
			public:
				///<summary>Видалить всі елементи які підпадають під умову<summary>
				template<typename Lambda>
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
				virtual ~IEnumerable() = 0 {}
			};

			template<typename Container, typename Pair, typename TKey, typename TValue>
			class IAssociable : public IQueryable<Container, Pair> {
			public:
				virtual ~IAssociable() = 0 {}
			};
		}	
	}
}
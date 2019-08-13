#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {


		//template<typename T, typename Enumerable>
		//class IEnumerable {
		//private:

		//public:
		//protected:
		//	virtual Enumerable & ForEachImpl(LambdaHolder<void, T&>& lambda) = 0;
		//	virtual Enumerable & ForEachImpl(LambdaHolder<void, T&, SizeT>& lambda) = 0;
		//	virtual Enumerable & FillImpl(LambdaHolder<T>& lambda, SizeT Count) = 0;
		//	virtual void TransformTo(T&& Value, SizeT Index) = 0;
		//	virtual void PrepareTransformTo(SizeT Count) = 0;
		//	virtual void ExecuteInScope(LambdaHolder<void>& lambda) = 0;
		//public:
		//	template<typename Lambda>
		//	__forceinline Enumerable & ForEach(Lambda& lambda) {
		//		if constexpr (StaticAsserts::And<
		//			StaticAsserts::IsVoidT<StaticAsserts::LambdaType<Lambda>::ReturnType>,
		//			StaticAsserts::IsSameT<StaticAsserts::LambdaType<Lambda>::Args, StaticAsserts::Pack<T&>>
		//		>)
		//			return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, T&>(lambda));
		//		else if constexpr (StaticAsserts::And<
		//			StaticAsserts::IsVoidT<StaticAsserts::LambdaType<Lambda>::ReturnType>,
		//			StaticAsserts::IsSameT<StaticAsserts::LambdaType<Lambda>::Args, StaticAsserts::Pack<T&, SizeT>>
		//		>)
		//			return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, T&, SizeT>(lambda));
		//		else
		//			static_assert(false, "Invalid lambda passed");
		//	}

		//	template<typename Lambda>
		//	__forceinline Enumerable & Fill(Lambda& lambda, SizeT Count) {
		//		return FillImpl(LambdaHolderImpl<decltype(lambda), T>(lambda), Count);
		//	}
		//	template<template<typename U> typename NewEnumerable, typename Lambda, typename U = StaticAsserts::LambdaType<Lambda>::ReturnType>
		//	__forceinline auto Transform(Lambda lambda) -> NewEnumerable<U> {
		//		NewEnumerable<U> result;
		//		IEnumerable<U, NewEnumerable<U>> & iResult = (IEnumerable<U, NewEnumerable<U>>&)result;
		//		iResult.PrepareTransformTo(this->GetCount());
		//		if constexpr (
		//			StaticAsserts::IsSameV<StaticAsserts::LambdaType<Lambda>::Args, StaticAsserts::Pack<T&>>
		//		)
		//		{
		//			auto OtherScope = [&]() {
		//				this->ForEach([&](T& Value, SizeT Index) {
		//					iResult.TransformTo(lambda(Value), Index);
		//					});
		//			};
		//			iResult.ExecuteInScope(LambdaHolderImpl<decltype(OtherScope), void>(OtherScope));
		//		}
		//		else if constexpr (
		//			StaticAsserts::IsSameV<StaticAsserts::LambdaType<Lambda>::Args, StaticAsserts::Pack<T&, SizeT>>
		//		) {
		//			auto OtherScope = [&]() {
		//				this->ForEach([&](T& Value, SizeT Index) {
		//					iResult.TransformTo(MoveRValue(lambda(Value, Index)), Index);
		//					});
		//			};
		//			iResult.ExecuteInScope(LambdaHolderImpl<decltype(OtherScope), void>(OtherScope));
		//		}
		//		else {
		//			static_assert(false, "Invalid lambda passed to Transform");
		//		}
		//		return result;
		//	}

		//	virtual SizeT GetCount() const = 0;

		//	virtual ~IEnumerable() = 0 {};

		//	template<typename OtherT, typename OtherEnumerable>
		//	friend class IEnumerable;
		//};

		//template<typename T, typename Enumerable>
		//class ISortable : public IEnumerable<T, Enumerable>{
		//protected:
		//	virtual Enumerable Sort(LambdaHolder<bool, const T&, const T&>& lambda) = 0;
		//public:
		//	template<typename Lambda>
		//	virtual Enumerable Sort(Lambda& lambda) {
		//		return Sort(LambdaHolderImpl<lambda, bool, const T&, const T&>(lambda))
		//	}
		//	virtual ~ISortable() = 0 {}
		//};
		namespace StaticAsserts {
			template<typename Collection>
			struct TIsResizable : StaticAsserts::LogicFalse {};

			template<typename Collection>
			struct TIsDynamicSize : StaticAsserts::LogicFalse {};
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
					if constexpr (StaticAsserts::IsVoidV<RetType>)
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

			class SeedKernelCall IQueryableBase {
			protected:
				constexpr IQueryableBase() {};
				virtual ~IQueryableBase() = 0 {};
				
				template<typename Container, typename T>
				friend class IQueryable;
			};
			template<typename Container, typename T>
			class SeedKernelCall IQueryable : IQueryableBase {
			protected:
				virtual void ExecuteInScope(LambdaHolder<void>& lambda) = 0;
			protected:
				virtual Container& ForEachImpl(LambdaHolder<void, T&>& lambda) = 0;
				virtual Container& ForEachImpl(LambdaHolder<void, T&, SizeT>& lambda) = 0;
				virtual Container& ForEachImpl(LambdaHolder<void, const T&>& lambda) = 0;
				virtual Container& ForEachImpl(LambdaHolder<void, const T&, SizeT>& lambda) = 0;
				virtual const Container& ForEachImpl(LambdaHolder<void, const T&>& lambda) const = 0;
				virtual const Container& ForEachImpl(LambdaHolder<void, const T&, SizeT>& lambda) const = 0;
			protected:
				virtual void PrepareCopyToOrMove(SizeT Count) = 0;
				virtual void TransferTo(T&& Value, SizeT ItemIndex) = 0;
			protected:
				virtual void RemoveAllImpl(LambdaHolder<void, T&&>& lambda) = 0;
				virtual void RemoveAllImpl(LambdaHolder<void, T&&, SizeT>& lambda) = 0;
			protected:
				//virtual SizeT CountIfImpl(LambdaHolder<bool, const T&>& lambda) = 0;
			public:
				///<summary>Виконує дію для кожного елемента</summary>
				template<typename Lambda>
				Container& ForEach(Lambda lambda) {
					using namespace StaticAsserts;
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
				template<typename Lambda>
				const Container& ForEach(Lambda lambda) const {
					if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, void, const T&>) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&, SizeT>(lambda));
					}
					else if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, void, const T&, SizeT>) {
						return ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&, SizeT>(lambda));
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::ForEach const specific");
					}
				}

				///<summary>Копіює всі елементи в нову колекцію</summary>
				template<template<typename U> typename NewContainer, typename Lambda, typename U = StaticAsserts::LambdaType<Lambda>::ReturnType>
				NewContainer<U> CopyTo(Lambda lambda) {
					using namespace StaticAsserts;
					using TargetType = NewContainer<U>;
					if constexpr (!IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::CopyTo const specific");
					} else if constexpr (!TargetType::IsResizable){
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					else if constexpr (Or<IsLambdaConvertible<Lambda, U, const T&>, IsLambdaConvertible<Lambda, U, const T&, SizeT>>) {
						using TargetIQueryableType = IQueryable<NewContainer<U>, U>;
						NewContainer<U> target = NewContainer<U>();
						TargetIQueryableType& targetIQueryable = static_cast<TargetIQueryableType&>(target);
						IQueryable<Container, T>& This = *this;
						SizeT count = This.GetCount();
						targetIQueryable.PrepareCopyToOrMove(count);
						if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, U, const T&>::State) {
							auto loopLambda = [&](const T& Value, SizeT Index) mutable {
								targetIQueryable.TransferTo(MoveRValue(lambda(Value)), Index);
							};
							ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&, SizeT>(MoveRValue(loopLambda)));
						}
						else {
							auto loopLambda = [&](const T& Value, SizeT Index) mutable {
								targetIQueryable.TransferTo(MoveRValue(lambda(Value, Index)), Index);
							};
							ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&, SizeT>(MoveRValue(loopLambda)));
						}
						
						return target;
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::CopyTo specific");
					}
				}
				///<summary>Копіює всі елементи в нову колекцію</summary>
				template<template<typename U> typename NewContainer, typename Lambda, typename U = StaticAsserts::LambdaType<Lambda>::ReturnType>
				NewContainer<U> CopyTo(Lambda lambda) const {
					using TargetType = NewContainer<U>;
					if constexpr (!StaticAsserts::IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::CopyTo const specific");
					}
					else if constexpr (!TargetType::IsResizable) {
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					else if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, U, const T&> || StaticAsserts::IsLambdaConvertible<Lambda, U, const T&, SizeT>) {
						using TargetIQueryableType = IQueryable<NewContainer<U>, U, TargetType::IsResizable, TargetType::IsDynamicSize>;
						NewContainer<U> target;
						TargetIQueryableType& targetIQueryable = target;
						target.PrepareCopyToOrMove(this->GetCount());
						auto loopLambda = [&](const T& Value, SizeT Index) {
							if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, U, const T&>)
								target.TransferTo(MoveRValue(lambda(Value)), Index);
							else
								targer.TransferTo(MoveRValue(lambda(Value, Index)), Index);
						};
						this->ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&, SizeT>(loopLambda));
						return target;
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::CopyTo const specific");
					}
				}

				///<summary>Копіює елементи які задовільняють умову в нову колекцію</summary>
				template<template<typename U> typename NewContainer, typename Lambda, typename FilterLambda, typename U = StaticAsserts::LambdaType<Lambda>::ReturnType>
				NewContainer<U> CopyToIf(Lambda lambda, FilterLambda filter) {
					using namespace StaticAsserts;
					using TargetType = NewContainer<U>;
					if constexpr (!IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::CopyTo const specific");
					}
					else if constexpr (!TargetType::IsResizable) {
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					else if constexpr (!(Or<IsLambdaConvertible<FilterLambda, bool, const T&>, IsLambdaConvertible<FilterLambda, bool, const T&, SizeT>)) {
						static_assert(false, "Invalid filter lambda in IQueryable::CopyTo specific")
					}
					else if constexpr (Or<IsLambdaConvertible<Lambda, void, const T&>, IsLambdaConvertible<Lambda, void, const T&, SizeT>>) {
						using TargetIQueryableType = IQueryable<NewContainer<U>, U>;

						NewContainer<U> target;
						TargetIQueryableType& targetIQueryable = target;

						if(!TargetType::IsDynamicSize)
							target.PrepareCopyToOrMove(this->GetCountIf(filter));

						SizeT Index = 0;
						auto loopLambda = [&](const T& Value) {
							if (filter(Value)) {
								if constexpr (IsLambdaConvertible<Lambda, U, const T&>)
									target.TransferTo(MoveRValue(lambda(Value)), Index);
								else
									targer.TransferTo(MoveRValue(lambda(Value, Index)), Index);
								Index++;
							}
						};

						this->ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&>(loopLambda));
						return target;
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::CopyTo const specific");
					}
				}
				///<summary>Копіює елементи які задовільняють умову в нову колекцію</summary>
				template<template<typename U> typename NewContainer, typename Lambda, typename FilterLambda, typename U = StaticAsserts::LambdaType<Lambda>::ReturnType>
				NewContainer<U> CopyToIf(Lambda lambda, FilterLambda filter) const {
					using TargetType = NewContainer<U>;
					if constexpr (!IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::CopyTo const specific");
					}
					else if constexpr (!TargetType::IsResizable) {
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					else if constexpr (!(Or<IsLambdaConvertible<FilterLambda, bool, const T&>, IsLambdaConvertible<FilterLambda, bool, const T&, SizeT>>)) {
						static_assert(false, "Invalid filter lambda in IQueryable::CopyTo specific")
					}
					else if constexpr (Or<IsLambdaConvertible<Lambda, void, const T&>, IsLambdaConvertible<Lambda, void, const T&, SizeT>>) {
						using TargetIQueryableType = IQueryable<NewContainer<U>, U>;

						NewContainer<U> target;
						TargetIQueryableType& targetIQueryable = target;

						if (!TargetType::IsDynamicSize)
							target.PrepareCopyToOrMove(this->GetCountIf(filter));

						SizeT Index = 0;
						auto loopLambda = [&](const T& Value) {
							if (filter(Value)) {
								if constexpr (IsLambdaConvertible<Lambda, U, const T&>)
									target.TransferTo(MoveRValue(lambda(Value)), Index);
								else
									targer.TransferTo(MoveRValue(lambda(Value, Index)), Index);
								Index++;
							}
						};

						this->ForEachImpl(LambdaHolderImpl<decltype(loopLambda), void, const T&>(loopLambda));
						return target;
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::CopyTo const specific");
					}
				}

				///<summary>Переміщує всі елементи в нову колекцію. Стара колекція стає пустою</summary> 
				template<template<typename U> typename NewContainer, typename Lambda, typename U = StaticAsserts::LambdaType<Lambda>::ReturnType>
				NewContainer<U> MoveTo(Lambda lambda) {
					using namespace StaticAsserts;
					using TargetType = NewContainer<U>;
					if constexpr (!StaticAsserts::IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::MoveTo const specific");
					}
					else if constexpr (!TargetType::IsResizable) {
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					else if constexpr (IsLambdaConvertible<Lambda, U, T&&>::State) {
						using TargetIQueryableType = IQueryable<NewContainer<U>, U>;

						NewContainer<U> target;
						TargetIQueryableType& targetIQueryable = target;

						if (!TargetType::IsDynamicSize)
							targetIQueryable.PrepareCopyToOrMove(this->GetCount());

						auto loopLambda = [&](T&& Value, SizeT Index) {
							targetIQueryable.TransferTo(MoveRValue(lambda(MoveRValue(Value))), Index);
						};

						RemoveAllImpl(LambdaHolderImpl<decltype(loopLambda), void, T&&, SizeT>(MoveRValue(loopLambda)));

						return target;
					}
					else if constexpr(IsLambdaConvertible<Lambda, U, T&&, SizeT>::State){
						using TargetIQueryableType = IQueryable<NewContainer<U>, U>;

						NewContainer<U> target;
						TargetIQueryableType& targetIQueryable = target;

						if (!TargetType::IsDynamicSize)
							targetIQueryable.PrepareCopyToOrMove(this->GetCountIf(filter));

						auto loopLambda = [&](T&& Value, SizeT Index) mutable {
							targetIQueryable.TransferTo(MoveRValue(lambda(MoveRValue(Value), Index)), Index);
						};

						this->RemoveAllImpl(LambdaHolder<decltype(loopLambda), void, T&&, SizeT>(loopLambda));

						return target;
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::MoveTo const specific");
					}
				}
				///<summary>Переміщує елементи які задовільняють умову в нову колекцію. Стара колекція стає пустою</summary> 
				template<template<typename U> typename NewContainer, typename Lambda, typename FilterLambda, typename U = StaticAsserts::LambdaType<Lambda>::ReturnType>
				NewContainer<U> MoveToIf(Lambda lambda, FilterLambda filter) {
					using namespace StaticAsserts;
					using TargetType = NewContainer<U>;
					if constexpr (!IsBasedOn<IQueryableBase, NewContainer<U>>::State) {
						static_assert(false, "Invalid target collection type in IQueryable::MoveTo const specific");
					}
					else if constexpr (!TargetType::IsResizable) {
						static_assert(false, "Can not convert dynamic sized collection to not resizable");
					}
					else if constexpr (!(Or<StaticAsserts::IsLambdaConvertible<FilterLambda, bool, T&>, IsLambdaConvertible<FilterLambda, bool, T&, SizeT>>)) {
						static_assert(false, "Invalid filter lambda in IQueryable::MoveTo specific")
					}
					else if constexpr (Or<StaticAsserts::IsLambdaConvertible<Lambda, T, T&&>, IsLambdaConvertible<Lambda, T, T&&, SizeT>>) {
						using TargetIQueryableType = IQueryable<NewContainer<U>, U>;

						NewContainer<U> target;
						TargetIQueryableType& targetIQueryable = target;

						if (!TargetType::IsDynamicSize)
							target.PrepareCopyToOrMove(this->GetCountIf(filter));

						SizeT Index = 0;
						auto loopLambda = [&](T&& Value) {
							if (filter(Value)) {
								if constexpr (IsLambdaConvertible<Lambda, void, T&&>)
									target.TransferTo(MoveRValue(lambda(MoveRValue(Value))), Index);
								else
									targer.TransferTo(MoveRValue(lambda(MoveRValue(Value), Index)), Index);
								Index++;
							}
						};

						this->RemoveAllImpl(LambdaHolder<decltype(loopLambda), void, T&&>(loopLambda));
						return target;
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::MoveTo const specific");
					}
				}
				///<summary>Підраховує кількість елементів які підходять по умові</summary>
				template<typename Lambda>
				SizeT GetCountIf(Lambda filter) {
					if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, bool, const T&>) {
						SizeT Count = 0;
						auto lambda = [](const T& Value) {
							if (filter(Value))
								Count++;
						};

						this->ForEachImpl(LambdaHolderImpl<decltype(lambda), void, Value>(lambda));
						return ++Count;
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::GetCountIf const specific");
					}
				}
				///<summary>Підраховує кількість елементів які підходять по умові</summary>
				template<typename Lambda>
				SizeT GetCountIf(Lambda lambda) const {
					if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, bool, const T&>) {
						SizeT Count = 0;
						auto lambda = [](const T& Value) {
							if (filter(Value))
								Count++;
						};

						this->ForEachImpl(LambdaHolderImpl<decltype(lambda), void, const T&>(lambda));
						return ++Count;
					}
					else {
						static_assert(false, "Invalid lambda in IQueryable::GetCountIf const specific");
					}
				}

				///<summary>Видалія всі елементи з колекції. Але перед видаленням викликає лямбду для кожного</summary>
				template<typename Lambda>
				Container& RemoveAll(Lambda lambda) {
					if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, void, const T&>) {
						return this->RemoveAllImpl(LambdaHolderImpl<Lambda, void, const T&>(lambda));
					}
					else if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, void, const T&, SizeT>) {
						return this->RemoveAllImpl(LambdaHolderImpl<Lambda, void, const T&, SizeT>(lambda));
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
				static inline constexpr Boolean IsResizable = StaticAsserts::TIsResizable<Container>::State;
				///<summary>Вертає true якщо колекція динамічно змінює розмір</summary>
				static inline constexpr Boolean IsDynamicSize = StaticAsserts::TIsDynamicSize<Container>::State;
			public:
				virtual ~IQueryable() = 0 {}
			private:
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
					if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, bool, const T&, const T&>) {
						return SortImpl(LambdaHolderImpl<Lambda, bool, const T&, const T&>(lambda), IsAscendingOrder);
					}
					else {
						static_assert(false, "Invalid lambda in IOrdeable::Sort specific");
					}
				}
				///<summary>Заповнить Count елментів результатом Lambda функції</summary>
				template<typename Lambda>
				static Container Fill(Lambda lambda, SizeT Count) {
					if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, T>::State == true) {
						Container target;
						IOrdeable& targetIOrdeable = target;
						targetIOrdeable.FillImpl(LambdaHolderImpl<Lambda, T>(MoveRValue(lambda)), Count);
						return target;
					}
					else if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, T, SizeT>::State) {
						Container target;
						IOrdeable& targetIOrdeable = target;
						targetIOrdeable.FillImpl(LambdaHolderImpl<Lambda, T, SizeT>(MoveRValue(lambda)), Count);
						return target;
					}
					else {
						static_assert(false, "Invalid lambda in IOrdeable::Fill specific");
					}
				}
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
					if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, bool, const T&>) {
						return RemoveIfImpl(LambdaHolderImpl<Lambda, bool, const T&>(Lambda));
					}
					else if constexpr (StaticAsserts::IsLambdaConvertible<Lambda, bool, const T&, SizeT>) {
						return RemoveIfImpl(LambdaHolderImpl<Lambda, bool, const T&, SizeT>(Lambda));
					}
					else {
						static_assert(false, "Invalid lambda in IEnumerble::RemoveIf specific");
					}
				}
			};
		}
	}
}
#pragma once
#include "Seed.Core.Containers.API.LINQ.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				class SeedKernelCall ICollection :
					public ICollectionBase< TCollection, THeap, T, TIsDynamic, TIsConcurencySuport>
				{
				protected:
					using ThisCollection = TCollection<THeap, T>;
				public:
					template<template <typename> typename NewCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<T> CopyTo() {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<T>::IsDynamic == false) {
							NewCollection<T> Instance = NewCollection<T>(ThisCollectionRef.Count);
							ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
								Instance.UnsafeSet(Item, Index);
								});
							return Instance;
						}
						else {
							NewCollection<T> Instance = NewCollection<T>();
							ThisCollectionRef.ForEach([&](const T& Item) {
								Instance.UnsafeAdd(Item);
								});
							return Instance;
						}
					}

					template<template <typename> typename NewCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<T> CopyTo() const {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<T>::IsDynamic == false) {
							NewCollection<T> Instance = NewCollection<T>(ThisCollectionRef.Count);
							ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
								Instance.UnsafeSet(Item, Index);
								});
							return Instance;
						}
						else {
							NewCollection<T> Instance = NewCollection<T>();
							ThisCollectionRef.ForEach([&](const T& Item) {
								Instance.UnsafeAdd(Item);
								});
							return Instance;
						}
					}

					template<template <typename> typename NewCollection,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsBasedOnV<ICollectionGenericBase, NewCollection<T>>
						>,
						bool
					> = true>
						NewCollection<T> MoveTo() const {
						auto ThisCollectionRef = static_cast<const ThisCollection&>(*this);
						if constexpr (NewCollection<T>::IsDynamic == false) {
							NewCollection<T> Instance = NewCollection<T>(ThisCollectionRef.Count);
							ThisCollectionRef.ForEach([&](const T& Item, SizeT Index) {
								Instance.UnsafeSet(Item, Index);
								});
							return Instance;
						}
						else {
							NewCollection<T> Instance = NewCollection<T>();
							ThisCollectionRef.ForEach([&](const T& Item) {
								Instance.UnsafeAdd(Item);
								});
							return Instance;
						}
					}
				public:
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, T&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, T&, SizeT>::State
						>,
						bool
					> = true>
						ThisCollection& ForEach(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					template<
						typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, const T&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, const T&, SizeT>::State
						>,
						bool
					> = true>
						__forceinline const ThisCollection& ForEach(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, T&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, T&, SizeT>::State
						>,
						bool
					> = true>
						ThisCollection& ForEach(Lambda&& lambda, SizeT Count) {
						return static_cast<ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), Count);
					}

					template<
						typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, const T&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, const T&, SizeT>::State
						>,
						bool
					> = true>
						__forceinline const ThisCollection& ForEach(Lambda&& lambda, SizeT Count) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), Count);
					}
				public:
					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertible<Lambda, void, T&, const T&>::State>, bool> = true>
					__forceinline T Aggregate(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl(MoveRValue(lambda));
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertible<Lambda, void, T&, const T&>::State>, bool> = true>
					__forceinline T Aggregate(Lambda&& lambda, const T& StartValue) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl(MoveRValue(lambda), StartValue);
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertible<Lambda, void, T&, const T&>::State>, bool> = true>
					__forceinline T Aggregate(Lambda&& lambda, T&& StartValue) const {
						return static_cast<const ThisCollection&>(*this).AggregateImpl(MoveRValue(lambda), MoveRValue(StartValue));
					}

					__forceinline bool Any() const {
						return static_cast<const ThisCollection&>(*this).AnyImpl();
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State>, bool> = true>
					__forceinline bool Any(Lambda&& lambda) {
						return static_cast<const ThisCollection&>(*this).AnyImpl(MoveRValue(lambda));
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State>, bool> = true>
					__forceinline bool All(Lambda&& lambda) {
						return static_cast<const ThisCollection&>(*this).AllImpl(MoveRValue(lambda));
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State>, bool> = true>
					__forceinline T& First(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).FirstImpl(MoveRValue(lambda));
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State>, bool> = true>
					__forceinline T& Last(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).LastImpl(MoveRValue(lambda));
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State>, bool> = true>
					__forceinline const T& First(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).FirstImpl(MoveRValue(lambda));
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State>, bool> = true>
					__forceinline const T& Last(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).LastImpl(MoveRValue(lambda));
					}

					__forceinline ThisCollection& RemoveAll() {
						return static_cast<ThisCollection&>(*this).RemoveAllImpl();
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertible<Lambda, void, T&&>::State>, bool> = true>
					__forceinline ThisCollection& RemoveAll(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).RemoveAllImpl(MoveRValue(lambda));
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertible<Lambda, bool, const T&>::State>, bool> = true>
					__forceinline SizeT GetCountIf(Lambda && lambda) {
						return static_cast<ThisCollection&>(*this).GetCountIfImpl(MoveRValue(lambda));
					}
				public:


				public:
					virtual ~ICollection() = 0 {}
				};
			}
		}
	}
}
#pragma once
#include "Seed.Core.Containers.API.LINQ.Base.h"
namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				class SeedKernelCall IFixedCollection :
					public IFixedCollectionBase<TCollection, THeap, T, TSize, TIsConcurencySuport>
				{
				private:
					using ThisCollection = TCollection<THeap, TSize, T>;
				public:
					template<template<SizeT, typename> typename NewCollection>
					NewCollection<TSize, T> CopyToFixed() {
						NewCollection<TSize, T> newCollection;
						ThisCollection& thisCollection = static_cast<ThisCollection&>(*this);
						thisCollection.ForEach([&](const T& Value, SizeT Index) {
							newCollection.UnsafeSet(Value, Index);
							});
						return newCollection;
					}

					template<template<SizeT, typename> typename NewCollection>
					NewCollection<TSize, T> CopyToFixed() const {
						NewCollection<TSize, T> newCollection;
						const ThisCollection& thisCollection = static_cast<const ThisCollection&>(*this);
						thisCollection.ForEach([&](const T& Value, SizeT Index) {
							newCollection.UnsafeSet(Value, Index);
							});
						return newCollection;
					}

					template<template<SizeT, typename> typename NewCollection>
					NewCollection<TSize, T> MoveToFixed() {
						NewCollection<TSize, T> newCollection;
						ThisCollection& thisCollection = static_cast<ThisCollection&>(*this);
						SizeT Index = 0;
						thisCollection.RemoveAll([&](T&& Value) -> void {
							newCollection.UnsafeSet(MoveRValue(Value), Index);
							Index++;
							});
						return newCollection;
					}

					template<template<SizeT, typename> typename NewCollection, SizeT TNewSize>
					NewCollection<TNewSize, T> CopyToFixed() {
						if constexpr (TNewSize == TSize) {
							NewCollection<TNewSize, T> newCollection;
							ThisCollection& thisCollection = static_cast<ThisCollection&>(*this);
							thisCollection.ForEach([&](const T& Value, SizeT Index) {
								newCollection.UnsafeSet(Value, Index);
								});
							return newCollection;
						}
						else if constexpr (TNewSize < TSize) {

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

					__forceinline ThisCollection& Reverse() {
						return static_cast<ThisCollection&>(*this).ReverseImpl();
					}
				public:
					virtual ~IFixedCollection() = 0 {}
				};
			}
		}
	}
}
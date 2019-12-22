#pragma once
#include "Seed.Core.Containers.API.LINQ.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<template<typename THeap> typename TString, typename THeap, typename TChar, Boolean TIsConcurencySuport>
				class IString : public IStringCollectionBaseLinq<TString, THeap, TChar, TIsConcurencySuport> {
				private:
					using ThisCollection = TString<THeap>;
				public:
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

					template<
						typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TChar&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TChar&, SizeT>::State
						>,
						bool
					> = true>
						__forceinline const ThisCollection& ForEach(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}

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

					template<
						typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TChar&>::State ||
						TypeTraits::IsLambdaConvertibleT<Lambda, void, const TChar&, SizeT>::State
						>,
						bool
					> = true>
						__forceinline const ThisCollection& ForEach(Lambda&& lambda, SizeT Count) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda), Count);
					}
				public:
					__forceinline bool Any() const {
						return static_cast<const ThisCollection&>(*this).AnyImpl();
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const TChar&>::State>, bool> = true>
					__forceinline bool Any(Lambda&& lambda) {
						return static_cast<const ThisCollection&>(*this).AnyImpl(MoveRValue(lambda));
					}

					template<typename Lambda, TypeTraits::EnableIfTV<TypeTraits::LogicConst<TypeTraits::IsLambdaConvertibleT<Lambda, bool, const TChar&>::State>, bool> = true>
					__forceinline bool All(Lambda&& lambda) {
						return static_cast<const ThisCollection&>(*this).AllImpl(MoveRValue(lambda));
					}

					__forceinline ThisCollection& Reverse() {
						return static_cast<ThisCollection&>(*this).ReverseImpl();
					}
				public:
					virtual ~IString() = 0 {}
				};
			}
		}
	}
}
#pragma once
#include "Seed.Core.Containers.API.LINQ.IOrderable.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<template<typename THeap, typename T>typename TList, typename THeap, typename T, bool TIsConcurencySuport>
				class IList : public IOrderable<TList, THeap, T, true, TIsConcurencySuport> {
				protected:
					using ListType = TList<THeap, T>;
				public:
					template<
						typename Lambda,
						typename FilteringLambda,
						TypeTraits::EnableIfTV<TypeTraits::LogicConst<
						(TypeTraits::IsLambdaConvertibleT<Lambda, void, T&&>::State ||
							TypeTraits::IsLambdaConvertibleT<Lambda, void, T&&, SizeT>::State) &&
							(TypeTraits::IsLambdaConvertibleT<FilteringLambda, Boolean, const T&>::State ||
								TypeTraits::IsLambdaConvertibleT<FilteringLambda, Boolean, const T&, SizeT>::State)
					>, bool> = true>
						__forceinline ListType& RemoveIf(FilteringLambda filter, Lambda&& lambda) {
						return static_cast<ListType&>(*this).RemoveIfImpl(MoveRValue(lambda));
					}
				public:
					virtual ~IList() = 0 {}
				};
			}
		}
	}
}
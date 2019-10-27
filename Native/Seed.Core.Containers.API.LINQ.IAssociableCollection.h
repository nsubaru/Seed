#pragma once
#include "Seed.Core.Containers.API.LINQ.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<template<typename THeap, typename TKey, typename TValue> typename TCollection, typename THeap, typename TKey, typename TValue, typename TPair, bool IsConcurencySuportT>
				class SeedKernelCall IAssociableCollection :
					public IAssociableCollectionBase<TCollection, THeap, TKey, TValue, TPair, IsConcurencySuportT>
				{
				private:
					using ThisCollection = TCollection<THeap, TKey, TValue>;
				public:
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, TPair&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, TPair&, SizeT>::State
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
						TypeTraits::IsLambdaConvertible<Lambda, void, const TPair&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, const TPair&, SizeT>::State
						>,
						bool
					> = true>
						__forceinline const ThisCollection& ForEach(Lambda&& lambda) const {
						return static_cast<const ThisCollection&>(*this).ForEachImpl(MoveRValue(lambda));
					}
				public:
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, const TKey&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, const TKey&, SizeT>::State
						>,
						bool
					> = true>
						ThisCollection& ForEachKey(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachKeyImpl(MoveRValue(lambda));
					}

					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, const TKey&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, const TKey&, SizeT>::State
						>,
						bool
					> = true>
						const ThisCollection& ForEachKey(Lambda&& lambda) const {
						return static_cast<ThisCollection&>(*this).ForEachKeyImpl(MoveRValue(lambda));
					}
				public:
					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, const TValue&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, const TValue&, SizeT>::State
						>,
						bool
					> = true>
						ThisCollection& ForEachValue(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachValueImpl(MoveRValue(lambda));
					}

					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, TValue&>::State ||
						TypeTraits::IsLambdaConvertible<Lambda, void, TValue&, SizeT>::State
						>,
						bool
					> = true>
						ThisCollection& ForEachValue(Lambda&& lambda) {
						return static_cast<ThisCollection&>(*this).ForEachValueImpl(MoveRValue(lambda));
					}
				public:
					ThisCollection& RemoveAll() {
						return static_cast<ThisCollection&>(*this).RemoveAllImpl();
					}

					template<typename Lambda,
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
						TypeTraits::IsLambdaConvertible<Lambda, void, TPair&&>::State
						>,
						bool
					> = true>
						ThisCollection& RemoveAllWith(Lambda && lambda) {
						return static_cast<ThisCollection&>(*this).RemoveAllWithImpl(MoveRValue(lambda));
					}
				public:
					virtual ~IAssociableCollection() = 0 {};
				};
			}
		}
	}
}
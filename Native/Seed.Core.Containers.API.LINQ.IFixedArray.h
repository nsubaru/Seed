#pragma once
#include "Seed.Core.Containers.API.LINQ.IFixedCollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				class IFixedArray : public IFixedCollection<TCollection, THeap, T, TSize, TIsConcurencySuport> {
					using ThisCollection = TCollection<THeap, TSize, T>;
				public:
					ThisCollection& SortBy(bool IsDescending = true) {
						return static_cast<ThisCollection&>(*this).SortByImpl(IsDescending);
					}

					template<typename Lambda, 
						TypeTraits::EnableIfTV<
						TypeTraits::LogicConst<
							TypeTraits::IsLambdaConvertibleT<Lambda, T>::State ||
							TypeTraits::IsLambdaConvertibleT<Lambda, T, SizeT>::State
						>
						, bool> = true>
					static __forceinline ThisCollection Generate(Lambda generator) {
						if constexpr (TypeTraits::IsLambdaConvertibleT<Lambda, T>::State)
						{
							if constexpr (!ThisCollection::IsDynamic)
							{
								ThisCollection collection = ThisCollection();
								for (SizeT i = 0; i < TSize; i++)
								{
									collection.UnsafeSet(MoveRValue(generator()), i);
								}
								return collection;
							}
							else
							{
								ThisCollection collection = ThisCollection();
								for (SizeT i = 0; i < TSize; i++)
								{
									collection.UnsafeAdd(MoveRValue(generator()));
								}
								return collection;
							}
						}
						else
						{
							if constexpr (!ThisCollection::IsDynamic)
							{
								ThisCollection collection = ThisCollection();
								for (SizeT i = 0; i < TSize; i++)
								{
									collection.UnsafeSet(generator(i), i);
								}
								return collection;
							}
							else
							{
								ThisCollection collection = ThisCollection();
								for (SizeT i = 0; i < TSize; i++)
								{
									collection.UnsafeAdd(generator(i));
								}
								return collection;
							}
						}
					}
				public:
					virtual ~IFixedArray() = 0 {}
				};
			}
		}
	}
}
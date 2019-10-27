#pragma once
#include "Seed.Core.Containers.API.LINQ.IFixedCollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT TSize, Boolean TIsConcurencySuport>
				class SeedKernelCall IFixedArray : public IFixedCollection<TCollection, THeap, T, TSize, TIsConcurencySuport> {
					using ThisCollection = TCollection<THeap, TSize, T>;
				public:
					ThisCollection& SortBy(bool IsDescending = true) {
						return static_cast<ThisCollection&>(*this).SortByImpl(IsDescending);
					}
				public:
					virtual ~IFixedArray() = 0 {}
				};
			}
		}
	}
}
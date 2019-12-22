#pragma once
#include "Seed.Core.Containers.API.LINQ.ICollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<template<typename THeap, typename T>typename TList, typename THeap, typename T, bool TIsDynamic, bool TIsConcurencySuport>
				class IOrderable :
					public ICollection<TList, THeap, T, TIsDynamic, TIsConcurencySuport> {
				protected:
					using ThisCollection = TList<THeap, T>;
				public:
					__forceinline ThisCollection& SortBy(bool IsDescending = true) {
						return static_cast<ThisCollection&>(*this).SortByImpl(IsDescending);
					}

					__forceinline ThisCollection& Reverse() {
						return static_cast<ThisCollection&>(*this).ReverseImpl();
					}
				public:
					virtual ~IOrderable() = 0 {}
				};
			}
		}
	}
}
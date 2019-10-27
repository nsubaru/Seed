#pragma once
#include "Seed.Core.Containers.API.LINQ.IOrderable.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<template<typename THeap, typename T>typename TList, typename THeap, typename T, bool TIsConcurencySuport>
				class SeedKernelCall IArray :
					public IOrderable<TList, THeap, T, false, TIsConcurencySuport> {
				public:
					virtual ~IArray() = 0 {}
				};
			}
		}
	}
}
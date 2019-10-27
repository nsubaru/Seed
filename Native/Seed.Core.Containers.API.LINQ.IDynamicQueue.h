#pragma once
#include "Seed.Core.Containers.API.LINQ.ICollection.h"

namespace Seed {
	inline namespace Core {
		namespace Containers {
			namespace LINQ {
				template<template<typename THeap, typename T>typename TList, typename THeap, typename T, bool TIsConcurencySuport>
				class SeedKernelCall IDynamicQueue :
					public ICollection<TList, THeap, T, false, TIsConcurencySuport> {
				public:
				public:
					virtual ~IDynamicQueue() = 0 {}
				};
			}
		}
	}
}